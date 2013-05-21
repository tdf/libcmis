/* libcmis
 * Version: MPL 1.1 / GPLv2+ / LGPLv2+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * Copyright (C) 2011 SUSE <cbosdonnat@suse.com>
 *
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPLv2+"), or
 * the GNU Lesser General Public License Version 2 or later (the "LGPLv2+"),
 * in which case the provisions of the GPLv2+ or the LGPLv2+ are applicable
 * instead of those above.
 */
#include <cctype>
#include <string>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "base-session.hxx"
#include "oauth2-handler.hxx"
#include "session-factory.hxx"
#include "xml-utils.hxx"

using namespace std;

namespace
{
    size_t lcl_getHeaders( void *ptr, size_t size, size_t nmemb, void *userdata )
    {
        libcmis::HttpResponse* response = static_cast< libcmis::HttpResponse* >( userdata );

        string buf( ( const char* ) ptr, size * nmemb );

        size_t sepPos = buf.find( ':' );
        if ( sepPos != string::npos )
        {
            string name( buf, 0, sepPos );
            string value = buf.substr( sepPos + 1 );
            value = libcmis::trim( value );

            response->getHeaders()[name] = value;

            if ( "Content-Transfer-Encoding" == name )
                response->getData( )->setEncoding( value );
        }
        
        return nmemb;
    }

    size_t lcl_bufferData( void* buffer, size_t size, size_t nmemb, void* data )
    {
        libcmis::EncodedData* encoded = static_cast< libcmis::EncodedData* >( data );
        encoded->decode( buffer, size, nmemb );
        return nmemb;
    }

    size_t lcl_readStream( void* buffer, size_t size, size_t nmemb, void* data )
    {
        istream& is = *( static_cast< istream* >( data ) );
        char* out = ( char * ) buffer;
        is.read( out, size * nmemb );

        return is.gcount( ) / size;
    }

    curlioerr lcl_ioctlStream( CURL* /*handle*/, int cmd, void* data )
    {
        curlioerr errCode = CURLIOE_OK;

        switch ( cmd )
        {
            case CURLIOCMD_RESTARTREAD:
                {
                    istream& is = *( static_cast< istream* >( data ) );
                    is.clear( );
                    is.seekg( 0, ios::beg );

                    if ( !is.good() )
                    {
                        fprintf ( stderr, "rewind failed\n" );
                        errCode = CURLIOE_FAILRESTART;
                    }
                }
                break;
            case CURLIOCMD_NOP:
                break;
            default:
                errCode = CURLIOE_UNKNOWNCMD;
        }
        return errCode;
    }
}

BaseSession::BaseSession( string bindingUrl, string repositoryId, string username,
        string password, libcmis::OAuth2DataPtr /*oauth2*/, bool verbose ) throw ( libcmis::Exception ) :
    Session( ),
    m_curlHandle( NULL ),
    m_no100Continue( false ),
    m_oauth2Handler( NULL ),
    m_bindingUrl( bindingUrl ),
    m_repositoryId( repositoryId ),
    m_username( username ),
    m_password( password ),
    m_authProvided( false ),
    m_repositories( ),
    m_verbose( verbose ),
    m_noHttpErrors( false ),
    m_refreshedToken( false )
{
    curl_global_init( CURL_GLOBAL_ALL );
    m_curlHandle = curl_easy_init( );
}

BaseSession::BaseSession( const BaseSession& copy ) :
    Session( ),
    m_curlHandle( NULL ),
    m_no100Continue( copy.m_no100Continue ),
    m_oauth2Handler( copy.m_oauth2Handler ),
    m_bindingUrl( copy.m_bindingUrl ),
    m_repositoryId( copy.m_repositoryId ),
    m_username( copy.m_username ),
    m_password( copy.m_password ),
    m_authProvided( copy.m_authProvided ),
    m_repositories( copy.m_repositories ),
    m_verbose( copy.m_verbose ),
    m_noHttpErrors( copy.m_noHttpErrors ),
    m_refreshedToken( false )
{
    // Not sure how sharing curl handles is safe.
    curl_global_init( CURL_GLOBAL_ALL );
    m_curlHandle = curl_easy_init( );
}

BaseSession::BaseSession( ) :
    Session( ),
    m_curlHandle( NULL ),
    m_no100Continue( false ),
    m_oauth2Handler( NULL ),
    m_bindingUrl( ),
    m_repositoryId( ),
    m_username( ),
    m_password( ),
    m_authProvided( false ),
    m_repositories( ),
    m_verbose( false ),
    m_noHttpErrors( false ),
    m_refreshedToken( false )
{
    curl_global_init( CURL_GLOBAL_ALL );
    m_curlHandle = curl_easy_init( );
}

BaseSession& BaseSession::operator=( const BaseSession& copy )
{
    if ( this != &copy )
    {
        m_curlHandle = NULL;
        m_no100Continue = copy.m_no100Continue;
        m_oauth2Handler = copy.m_oauth2Handler;
        m_bindingUrl = copy.m_bindingUrl;
        m_repositoryId = copy.m_repositoryId;
        m_username = copy.m_username;
        m_password = copy.m_password;
        m_authProvided = copy.m_authProvided;
        m_repositories = copy.m_repositories;
        m_verbose = copy.m_verbose;
        m_noHttpErrors = copy.m_noHttpErrors;
        m_refreshedToken = copy.m_refreshedToken;

        // Not sure how sharing curl handles is safe.
        curl_global_init( CURL_GLOBAL_ALL );
        m_curlHandle = curl_easy_init( );
    }

    return *this;
}

BaseSession::~BaseSession( )
{
    if ( NULL != m_curlHandle )
        curl_easy_cleanup( m_curlHandle );
    delete( m_oauth2Handler );
}

string& BaseSession::getUsername( ) throw ( CurlException )
{
    checkCredentials( );
    return m_username;
}

string& BaseSession::getPassword( ) throw ( CurlException )
{
    checkCredentials( );
    return m_password;
}

string BaseSession::createUrl( const string& pattern, map< string, string > variables )
{
    string url( pattern );

    // Decompose the pattern and replace the variables by their values
    map< string, string >::iterator it = variables.begin( );
    while ( it != variables.end( ) )
    {
        string name = "{";
        name += it->first;
        name += "}";
        string value = it->second;

        // Search and replace the variable
        size_t pos = url.find( name );
        if ( pos != string::npos )
        {
            // Escape the URL by chunks
            url = url.replace( pos, name.size(), libcmis::escape( value ) );
        }

        ++it;
    }

    // Cleanup the remaining unset variables
    size_t pos1 = url.find( '{' );
    while ( pos1 != string::npos )
    {
        // look for the closing bracket
        size_t pos2 = url.find( '}', pos1 );
        if ( pos2 != string::npos )
            url.erase( pos1, pos2 - pos1 + 1 );

        pos1 = url.find( '{', pos1 - 1 );
    }
    
    return url;
}

libcmis::HttpResponsePtr BaseSession::httpGetRequest( string url ) throw ( CurlException )
{
    // Reset the handle for the request
    curl_easy_reset( m_curlHandle );
    initProtocols( );

    libcmis::HttpResponsePtr response( new libcmis::HttpResponse( ) );

    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, lcl_bufferData );
    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, response->getData( ).get( ) );
    
    curl_easy_setopt( m_curlHandle, CURLOPT_HEADERFUNCTION, &lcl_getHeaders );
    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEHEADER, response.get() );

    // fix Cloudoku too many redirects error
    // note: though curl doc says -1 is the default for MAXREDIRS, the error i got
    // said it was 0
    curl_easy_setopt( m_curlHandle, CURLOPT_MAXREDIRS, 100);

    try
    {
        httpRunRequest( url );
        response->getData( )->finish( );
    }
    catch ( const CurlException& e )
    {
        // If the access token is expired, we get 401 error,
        // Need to use the refresh token to get a new one.
        if ( getHttpStatus( ) == 401 && !getRefreshToken( ).empty( ) && !m_refreshedToken )
        {        
            // Refresh the token
            m_oauth2Handler->refresh( );
            
            // Resend the query
            try
            {
                // Avoid infinite recursive call
                m_refreshedToken = true;
                response = httpGetRequest( url );
                m_refreshedToken = false;
            } 
            catch (const CurlException& )
            {
               
                throw;
            }
            m_refreshedToken = false;
        }
        else throw;
    }
    m_refreshedToken = false;

    return response;
}

libcmis::HttpResponsePtr BaseSession::httpPutRequest( string url, istream& is, vector< string > headers ) throw ( CurlException )
{
    // Duplicate istream in case we need to retry
    string isStr( static_cast< stringstream const&>( stringstream( ) << is.rdbuf( ) ).str( ) );
    
    istringstream isOriginal( isStr ), isBackup( isStr );    

    // Reset the handle for the request
    curl_easy_reset( m_curlHandle );
    initProtocols( );

    libcmis::HttpResponsePtr response( new libcmis::HttpResponse( ) );

    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, lcl_bufferData );
    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, response->getData( ).get( ) );
    
    curl_easy_setopt( m_curlHandle, CURLOPT_HEADERFUNCTION, &lcl_getHeaders );
    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEHEADER, response.get() );

    // Get the stream length
    is.seekg( 0, ios::end );
    long size = is.tellg( );
    is.seekg( 0, ios::beg );
    curl_easy_setopt( m_curlHandle, CURLOPT_INFILESIZE, size );
    curl_easy_setopt( m_curlHandle, CURLOPT_READDATA, &isOriginal );
    curl_easy_setopt( m_curlHandle, CURLOPT_READFUNCTION, lcl_readStream );
    curl_easy_setopt( m_curlHandle, CURLOPT_UPLOAD, 1 );
    curl_easy_setopt( m_curlHandle, CURLOPT_IOCTLFUNCTION, lcl_ioctlStream );
    curl_easy_setopt( m_curlHandle, CURLOPT_IOCTLDATA, &isOriginal );

    // If we know for sure that 100-Continue won't be accepted,
    // don't even try with it to save one HTTP request.
    if ( m_no100Continue )
        headers.push_back( "Expect:" );
    try
    {
        httpRunRequest( url, headers );
        response->getData( )->finish();
    }
    catch ( const CurlException& e )
    {
        long status = getHttpStatus( );
        /** If we had a HTTP 417 response, this is likely to be due to some 
            HTTP 1.0 proxy / server not accepting the "Expect: 100-continue"
            header. Try to disable this header and try again.
        */
        if ( status == 417 && !m_no100Continue)
        {
            // Remember that we don't want 100-Continue for the future requests
            m_no100Continue = true;
            response = httpPutRequest( url, isBackup, headers );
        }

        // If the access token is expired, we get 401 error,
        // Need to use the refresh token to get a new one.
        if ( status == 401 && !getRefreshToken( ).empty( ) && !m_refreshedToken )
        {
            
            // Refresh the token
            m_oauth2Handler->refresh( );
            
            // Resend the query
            try
            {
                // Avoid infinite recursive call
                m_refreshedToken = true;
                response = httpPutRequest( url, isBackup, headers );
                m_refreshedToken = false;
            } 
            catch (const CurlException& )
            {
                m_refreshedToken = false;
                throw;
            }
        }
        // Has tried but failed
        if ( ( status != 417 || m_no100Continue ) && 
             ( status != 401 || getRefreshToken( ).empty( ) || m_refreshedToken ) ) throw;
    }
    m_refreshedToken = false;
    return response;
}

libcmis::HttpResponsePtr BaseSession::httpPostRequest( const string& url, istream& is, 
    const string& contentType, bool redirect ) throw ( CurlException )
{
    // Duplicate istream in case we need to retry
    string isStr( static_cast< stringstream const&>( stringstream( ) << is.rdbuf( ) ).str( ) );
    
    istringstream isOriginal( isStr ), isBackup( isStr );
    
    // Reset the handle for the request
    curl_easy_reset( m_curlHandle );
    initProtocols( );

    libcmis::HttpResponsePtr response( new libcmis::HttpResponse( ) );

    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, lcl_bufferData );
    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, response->getData( ).get( ) );
    
    curl_easy_setopt( m_curlHandle, CURLOPT_HEADERFUNCTION, &lcl_getHeaders );
    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEHEADER, response.get() );

    // Get the stream length
    is.seekg( 0, ios::end );
    long size = is.tellg( );
    is.seekg( 0, ios::beg );
    curl_easy_setopt( m_curlHandle, CURLOPT_POSTFIELDSIZE, size );
    curl_easy_setopt( m_curlHandle, CURLOPT_READDATA, &isOriginal );
    curl_easy_setopt( m_curlHandle, CURLOPT_READFUNCTION, lcl_readStream );
    curl_easy_setopt( m_curlHandle, CURLOPT_POST, 1 );
    curl_easy_setopt( m_curlHandle, CURLOPT_IOCTLFUNCTION, lcl_ioctlStream );
    curl_easy_setopt( m_curlHandle, CURLOPT_IOCTLDATA, &isOriginal );

    vector< string > headers;
    headers.push_back( string( "Content-Type:" ) + contentType );

    // If we know for sure that 100-Continue won't be accepted,
    // don't even try with it to save one HTTP request.
    if ( m_no100Continue )
        headers.push_back( "Expect:" );
    try 
    {
        httpRunRequest( url, headers, redirect );
        response->getData( )->finish();    
    }
    catch ( const CurlException& e )
    {
        
        long status = getHttpStatus( );
        /** If we had a HTTP 417 response, this is likely to be due to some 
            HTTP 1.0 proxy / server not accepting the "Expect: 100-continue"
            header. Try to disable this header and try again.
        */
        if ( status == 417 && !m_no100Continue )
        {       
            // Remember that we don't want 100-Continue for the future requests
            m_no100Continue = true;
            response = httpPostRequest( url, isBackup, contentType, redirect );
        }

        // If the access token is expired, we get 401 error,
        // Need to use the refresh token to get a new one.
        if ( status == 401 && !getRefreshToken( ).empty( ) && !m_refreshedToken )            
        {          
            // Refresh the token
            m_oauth2Handler->refresh( );
            
            // Resend the query
            try
            {
                // Avoid infinite recursive call
                m_refreshedToken = true;
                response = httpPostRequest( url, isBackup, contentType, redirect );
                m_refreshedToken = false;
            } 
            catch (const CurlException& )
            {
                m_refreshedToken = false;
                throw;
            }
        }
        
        // Has tried but failed
        if ( ( status != 417 || m_no100Continue ) && 
             ( status != 401 || getRefreshToken( ).empty( ) || m_refreshedToken ) ) throw;
    }
    m_refreshedToken = false;

    return response;
}

void BaseSession::httpDeleteRequest( string url ) throw ( CurlException )
{
    // Reset the handle for the request
    curl_easy_reset( m_curlHandle );
    initProtocols( );

    curl_easy_setopt( m_curlHandle, CURLOPT_CUSTOMREQUEST, "DELETE" );
    try
    {
        httpRunRequest( url );
    }
    catch ( const CurlException& e )
    {
        // If the access token is expired, we get 401 error,
        // Need to use the refresh token to get a new one.
        if ( getHttpStatus( ) == 401 && !getRefreshToken( ).empty( ) && !m_refreshedToken )            
        {
            
            // Refresh the token
            m_oauth2Handler->refresh( );
            // Resend the query
            try
            {
                // Avoid infinite recursive call
                m_refreshedToken = true;
                httpDeleteRequest( url );
                m_refreshedToken = false;
            } 
            catch (const CurlException& )
            {  
                m_refreshedToken = false;
                throw;
            }
        }
        else throw;
    }
    m_refreshedToken = false;
}

void BaseSession::checkCredentials( ) throw ( CurlException )
{
    // Check that we have the complete credentials
    libcmis::AuthProviderPtr authProvider = libcmis::SessionFactory::getAuthenticationProvider();
    if ( authProvider && !m_authProvided && ( m_username.empty() || m_password.empty() ) )
    {
        m_authProvided = authProvider->authenticationQuery( m_username, m_password );
        if ( !m_authProvided )
        {
            throw CurlException( "User cancelled authentication request" );
        }
    }
}

void BaseSession::httpRunRequest( string url, vector< string > headers, bool redirect ) throw ( CurlException )
{
    // Redirect
    curl_easy_setopt( m_curlHandle, CURLOPT_FOLLOWLOCATION, redirect);
    
    // Activate the cookie engine
    curl_easy_setopt( m_curlHandle, CURLOPT_COOKIEFILE, "" );

    // Grab something from the web
    curl_easy_setopt( m_curlHandle, CURLOPT_URL, url.c_str() );

    // Set the headers
    struct curl_slist *headers_slist = NULL;
    for ( vector< string >::iterator it = headers.begin( ); it != headers.end( ); ++it )
        headers_slist = curl_slist_append( headers_slist, it->c_str( ) );

    // If we are using OAuth2, then add the proper header with token to authenticate
    // Otherwise, just set the credentials normally using in libcurl options
    if ( m_oauth2Handler != NULL && !m_oauth2Handler->getHttpHeader( ).empty() )
    {
        headers_slist = curl_slist_append( headers_slist, 
                                  m_oauth2Handler->getHttpHeader( ).c_str( ) );
    }
    else if ( !getUsername().empty() && !getPassword().empty() )
    {
        curl_easy_setopt( m_curlHandle, CURLOPT_HTTPAUTH, CURLAUTH_ANY );

#if LIBCURL_VERSION_VALUE >= 0x071301
        curl_easy_setopt( m_curlHandle, CURLOPT_USERNAME, getUsername().c_str() );
        curl_easy_setopt( m_curlHandle, CURLOPT_PASSWORD, getPassword().c_str() );
#else
        string userpwd = getUsername() + ":" + getPassword();
        curl_easy_setopt( m_curlHandle, CURLOPT_USERPWD, userpwd.c_str( ) );
#endif
    }
    
    curl_easy_setopt( m_curlHandle, CURLOPT_HTTPHEADER, headers_slist );

    // Set the proxy configuration if any
    if ( !libcmis::SessionFactory::getProxy( ).empty() )
    {
        curl_easy_setopt( m_curlHandle, CURLOPT_PROXY, libcmis::SessionFactory::getProxy( ).c_str() );
#if LIBCURL_VERSION_VALUE >= 0x071304
        curl_easy_setopt( m_curlHandle, CURLOPT_NOPROXY, libcmis::SessionFactory::getNoProxy( ).c_str() );
#endif
        const string& proxyUser = libcmis::SessionFactory::getProxyUser( );
        const string& proxyPass = libcmis::SessionFactory::getProxyPass( );
        if ( !proxyUser.empty( ) && !proxyPass.empty( ) )
        {
            curl_easy_setopt( m_curlHandle, CURLOPT_PROXYAUTH, CURLAUTH_ANY ); 
#if LIBCURL_VERSION_VALUE >= 0X071301
            curl_easy_setopt( m_curlHandle, CURLOPT_PROXYUSERNAME, proxyUser.c_str( ) );
            curl_easy_setopt( m_curlHandle, CURLOPT_PROXYPASSWORD, proxyPass.c_str( ) );
#else
            string userpwd = proxyUser + ":" + proxyPass;
            curl_easy_setopt( m_curlHandle, CURLOPT_PROXYUSERPWD, userpwd.c_str( ) );
#endif
        }
    }

    // Get some feedback when something wrong happens
    char errBuff[CURL_ERROR_SIZE];
    curl_easy_setopt( m_curlHandle, CURLOPT_ERRORBUFFER, errBuff );

    // We want to get the response even if there is an Http error
    if ( !m_noHttpErrors )
        curl_easy_setopt( m_curlHandle, CURLOPT_FAILONERROR, 1 );

    if ( m_verbose )
        curl_easy_setopt( m_curlHandle, CURLOPT_VERBOSE, 1 );

    // Perform the query
    CURLcode errCode = curl_easy_perform( m_curlHandle );
    
    // Free the headers list
    curl_slist_free_all( headers_slist );

    // Process the response
    bool isHttpError = errCode == CURLE_HTTP_RETURNED_ERROR;
    if ( CURLE_OK != errCode && !( m_noHttpErrors && isHttpError ) )
    {
        long httpError = 0;
        curl_easy_getinfo( m_curlHandle, CURLINFO_RESPONSE_CODE, &httpError );
        throw CurlException( string( errBuff ), errCode, url, httpError );
    }
}

long BaseSession::getHttpStatus( )
{
    long status = 0;
    curl_easy_getinfo( m_curlHandle, CURLINFO_RESPONSE_CODE, &status );

    return status;
}

void BaseSession::setOAuth2Data( libcmis::OAuth2DataPtr oauth2 ) throw ( libcmis::Exception )
{
    m_oauth2Handler = new OAuth2Handler( this, oauth2 );
    
    // If oauth2 contains refresh token, we are done here
    if ( m_oauth2Handler->isAuthenticated( ) ) return;

    string authCode;

    try
    {
        // Try to get the authentication code using the given provider.
        try
        {
            authCode = oauth2Authenticate( );
        }
        catch (...)
        {
            // continue to fallback
        }

        // If that didn't work, call the fallback provider from SessionFactory
        if ( authCode.empty( ) )
        {
            libcmis::OAuth2AuthCodeProvider fallbackProvider = libcmis::SessionFactory::getOAuth2AuthCodeProvider( );
            if ( fallbackProvider != NULL )
                authCode = string( fallbackProvider( m_oauth2Handler->getAuthURL().c_str(), getUsername().c_str(), getPassword().c_str() ) ); 
        }
    }
    catch ( const CurlException& e )
    {
        // Thrown by getUsername() and getPassword() if user cancels the credentials request
        // or by oauth2Authenticate() if anything wrong happened when getting the auth code
        throw e.getCmisException( );
    }

    // If still no auth code, then raise an exception
    if ( authCode.empty( ) )
        throw libcmis::Exception( "Couldn't get OAuth authentication code", "permissionDenied" );

    m_oauth2Handler->fetchTokens( string( authCode ) );
}

vector< libcmis::RepositoryPtr > BaseSession::getRepositories( )
{
    return m_repositories;
}

libcmis::FolderPtr BaseSession::getRootFolder() throw ( libcmis::Exception )
{
    return getFolder( getRootId() );
}

libcmis::FolderPtr BaseSession::getFolder( string id ) throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object = getObject( id );
    libcmis::FolderPtr folder = boost::dynamic_pointer_cast< libcmis::Folder >( object );
    return folder;
}

string BaseSession::oauth2Authenticate( ) throw ( CurlException )
{
    return string( );
}

const char* CurlException::what( ) const throw ()
{
    if ( !isCancelled( ) )
    {
        stringstream buf;
        buf << "CURL error - " << ( unsigned int ) m_code << ": ";
        buf << m_message;
        m_errorMessage = buf.str( );

        return m_errorMessage.c_str( );
    }

    return m_message.c_str( );
}

libcmis::Exception CurlException::getCmisException( ) const
{
    string msg;
    string type( "runtime" );

    switch ( m_httpStatus )
    {
        case 400:
            msg = string( what() ) + string( ": " ) + m_url;
            type = "invalidArgument";
            break;
        case 401:
            msg = "Authentication failure";
            type = "permissionDenied";
            break;
        case 403:
            msg = "Invalid credentials";
            type = "permissionDenied";
            break;
        case 404:
            msg = "Invalid URL: " + m_url;
            type = "objectNotFound";
            break;
        case 405:
            msg = string( what() ) + string( ": " ) + m_url;
            type = "notSupported";
            break;
        case 409:
            msg = "Editing conflict error";
            type = "updateConflict";
            break;
        default:
            msg = what();
            if ( !isCancelled( ) )
                msg += ": " + m_url;
            break;
    }

    return libcmis::Exception( msg, type );
}

void BaseSession::initProtocols( )
{
    const unsigned long protocols = CURLPROTO_HTTP | CURLPROTO_HTTPS;
    curl_easy_setopt(m_curlHandle, CURLOPT_PROTOCOLS, protocols);
    curl_easy_setopt(m_curlHandle, CURLOPT_REDIR_PROTOCOLS, protocols);
}

string BaseSession::getRefreshToken( ) throw ( libcmis::Exception )
{
    string refreshToken;
    if ( m_oauth2Handler ) 
        refreshToken = m_oauth2Handler->getRefreshToken( );
    return refreshToken;
}

