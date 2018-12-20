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

#include <libcmis/session-factory.hxx>
#include <libcmis/xml-utils.hxx>

#include "oauth2-handler.hxx"

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

    template<typename T>
    class ScopeGuard
    {
    public:
        ScopeGuard(T &var, T newValue)
            : m_var(var)
            , m_origValue(var)
        {
            m_var = newValue;
        }

        ~ScopeGuard()
        {
            m_var = m_origValue;
        }

    private:
        T& m_var;
        const T m_origValue;
    };
}

HttpSession::HttpSession( string username, string password, bool noSslCheck,
                          libcmis::OAuth2DataPtr oauth2, bool verbose ) :
    m_curlHandle( NULL ),
    m_no100Continue( false ),
    m_oauth2Handler( NULL ),
    m_username( username ),
    m_password( password ),
    m_authProvided( false ),
    m_verbose( verbose ),
    m_noHttpErrors( false ),
    m_noSSLCheck( noSslCheck ),
    m_refreshedToken( false ),
    m_inOAuth2Authentication( false ),
    m_authMethod( CURLAUTH_ANY )
{
    curl_global_init( CURL_GLOBAL_ALL );
    m_curlHandle = curl_easy_init( );

    if ( oauth2 && oauth2->isComplete( ) ){
        setOAuth2Data( oauth2 );
    }
}

HttpSession::HttpSession( const HttpSession& copy ) :
    m_curlHandle( NULL ),
    m_no100Continue( copy.m_no100Continue ),
    m_oauth2Handler( copy.m_oauth2Handler ),
    m_username( copy.m_username ),
    m_password( copy.m_password ),
    m_authProvided( copy.m_authProvided ),
    m_verbose( copy.m_verbose ),
    m_noHttpErrors( copy.m_noHttpErrors ),
    m_noSSLCheck( copy.m_noSSLCheck ),
    m_refreshedToken( false ),
    m_inOAuth2Authentication( false ),
    m_authMethod( copy.m_authMethod )
{
    // Not sure how sharing curl handles is safe.
    curl_global_init( CURL_GLOBAL_ALL );
    m_curlHandle = curl_easy_init( );
}

HttpSession::HttpSession( ) :
    m_curlHandle( NULL ),
    m_no100Continue( false ),
    m_oauth2Handler( NULL ),
    m_username( ),
    m_password( ),
    m_authProvided( false ),
    m_verbose( false ),
    m_noHttpErrors( false ),
    m_noSSLCheck( false ),
    m_refreshedToken( false ),
    m_inOAuth2Authentication( false ),
    m_authMethod( CURLAUTH_ANY )
{
    curl_global_init( CURL_GLOBAL_ALL );
    m_curlHandle = curl_easy_init( );
}

HttpSession& HttpSession::operator=( const HttpSession& copy )
{
    if ( this != &copy )
    {
        curl_easy_cleanup( m_curlHandle );
        m_curlHandle = NULL;
        m_no100Continue = copy.m_no100Continue;
        m_oauth2Handler = copy.m_oauth2Handler;
        m_username = copy.m_username;
        m_password = copy.m_password;
        m_authProvided = copy.m_authProvided;
        m_verbose = copy.m_verbose;
        m_noHttpErrors = copy.m_noHttpErrors;
        m_noSSLCheck = copy.m_noSSLCheck;
        m_refreshedToken = copy.m_refreshedToken;
        m_inOAuth2Authentication = copy.m_inOAuth2Authentication;
        m_authMethod = copy.m_authMethod;

        // Not sure how sharing curl handles is safe.
        curl_global_init( CURL_GLOBAL_ALL );
        m_curlHandle = curl_easy_init( );
    }

    return *this;
}

HttpSession::~HttpSession( )
{
    if ( NULL != m_curlHandle )
        curl_easy_cleanup( m_curlHandle );
    delete( m_oauth2Handler );
}

string& HttpSession::getUsername( )
{
    checkCredentials( );
    return m_username;
}

string& HttpSession::getPassword( )
{
    checkCredentials( );
    return m_password;
}

libcmis::HttpResponsePtr HttpSession::httpGetRequest( string url )
{
    checkOAuth2( url );

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
    curl_easy_setopt( m_curlHandle, CURLOPT_MAXREDIRS, 20);

    try
    {
        httpRunRequest( url );
        response->getData( )->finish( );
    }
    catch ( const CurlException& )
    {
        // If the access token is expired, we get 401 error,
        // Need to use the refresh token to get a new one.
        if ( getHttpStatus( ) == 401 && !getRefreshToken( ).empty( ) && !m_refreshedToken )
        {
            // Refresh the token
            oauth2Refresh();

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

libcmis::HttpResponsePtr HttpSession::httpPutRequest( string url, istream& is, vector< string > headers )
{
    checkOAuth2( url );

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

    curl_easy_setopt( m_curlHandle, CURLOPT_MAXREDIRS, 20);

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
    catch ( const CurlException& )
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
            oauth2Refresh();

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

libcmis::HttpResponsePtr HttpSession::httpPostRequest( const string& url, istream& is,
    const string& contentType, bool redirect )
{
    checkOAuth2( url );

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

    curl_easy_setopt( m_curlHandle, CURLOPT_MAXREDIRS, 20);

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
    catch ( const CurlException& )
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
            oauth2Refresh();

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

void HttpSession::httpDeleteRequest( string url )
{
    checkOAuth2( url );

    // Reset the handle for the request
    curl_easy_reset( m_curlHandle );
    initProtocols( );

    curl_easy_setopt( m_curlHandle, CURLOPT_CUSTOMREQUEST, "DELETE" );
    try
    {
        httpRunRequest( url );
    }
    catch ( const CurlException& )
    {
        // If the access token is expired, we get 401 error,
        // Need to use the refresh token to get a new one.
        if ( getHttpStatus( ) == 401 && !getRefreshToken( ).empty( ) && !m_refreshedToken )
        {

            // Refresh the token
            oauth2Refresh();

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

void HttpSession::checkCredentials( )
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

void HttpSession::httpRunRequest( string url, vector< string > headers, bool redirect )
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
    else if ( !getUsername().empty() )
    {
        curl_easy_setopt( m_curlHandle, CURLOPT_HTTPAUTH, m_authMethod );
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
    errBuff[0] = 0;
    curl_easy_setopt( m_curlHandle, CURLOPT_ERRORBUFFER, errBuff );

    // We want to get the response even if there is an Http error
    if ( !m_noHttpErrors )
        curl_easy_setopt( m_curlHandle, CURLOPT_FAILONERROR, 1 );

    if ( m_verbose )
        curl_easy_setopt( m_curlHandle, CURLOPT_VERBOSE, 1 );

    // We want to get the certificate infos in error cases
#if LIBCURL_VERSION_VALUE >= 0X071301
    curl_easy_setopt( m_curlHandle, CURLOPT_CERTINFO, 1 );
#endif

    if ( m_noSSLCheck )
    {
#if LIBCURL_VERSION_VALUE >= 0x070801
        curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYHOST, 0);
#endif
#if LIBCURL_VERSION_VALUE >= 0x070402
        curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYPEER, 0);
#endif
    }

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

        bool errorFixed = false;
#if LIBCURL_VERSION_VALUE >= 0X071301
        // If we had a bad certificate, then try to get more details
        if ( CURLE_SSL_CACERT == errCode )
        {
            vector< string > certificates;

            // We somehow need to rerun the request to get the certificate
            curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYPEER, 0);
            errCode = curl_easy_perform( m_curlHandle );

            union {
                struct curl_slist    *to_info;
                struct curl_certinfo *to_certinfo;
            } ptr;

            ptr.to_info = NULL;

            CURLcode res = curl_easy_getinfo(m_curlHandle, CURLINFO_CERTINFO, &ptr.to_info);

            if ( !res && ptr.to_info )
            {
                // We need the first certificate in the chain only
                if ( ptr.to_certinfo->num_of_certs > 0 )
                {
                    struct curl_slist *slist;

                    string certStart( "Cert:" );
                    for ( slist = ptr.to_certinfo->certinfo[0]; slist; slist = slist->next )
                    {
                        string data( slist->data );
                        size_t startPos = data.find( certStart );
                        if ( startPos == 0 )
                        {
                            startPos = certStart.length();
                            data = data.substr( startPos );
                            certificates.push_back( data );
                        }
                    }
                }
            }

            if ( !certificates.empty() )
            {
                libcmis::CertValidationHandlerPtr validationHandler =
                    libcmis::SessionFactory::getCertificateValidationHandler( );
                bool ignoreCert = validationHandler && validationHandler->validateCertificate( certificates );
                if ( ignoreCert )
                {
                    m_noSSLCheck = true;

                    isHttpError = errCode == CURLE_HTTP_RETURNED_ERROR;
                    errorFixed = ( CURLE_OK == errCode || ( m_noHttpErrors && isHttpError ) );
                    if ( !errorFixed )
                        curl_easy_getinfo( m_curlHandle, CURLINFO_RESPONSE_CODE, &httpError );
                }
                else
                {
                    throw CurlException( "Invalid SSL certificate" );
                }
            }
        }
#endif

        if ( !errorFixed )
            throw CurlException( string( errBuff ), errCode, url, httpError );
    }
}


void HttpSession::checkOAuth2( string url )
try
{
    if ( m_oauth2Handler )
    {
        m_oauth2Handler->setOAuth2Parser( OAuth2Providers::getOAuth2Parser( url ) );
        if ( m_oauth2Handler->getAccessToken().empty() && !m_inOAuth2Authentication )
            oauth2Authenticate( );
    }
}
catch ( const libcmis::Exception& e )
{
    throw CurlException( e.what( ) );
}

long HttpSession::getHttpStatus( )
{
    long status = 0;
    curl_easy_getinfo( m_curlHandle, CURLINFO_RESPONSE_CODE, &status );

    return status;
}

void HttpSession::setOAuth2Data( libcmis::OAuth2DataPtr oauth2 )
{
    m_oauth2Handler = new OAuth2Handler( this, oauth2 );
}

void HttpSession::oauth2Authenticate( )
{
    string authCode;

    const ScopeGuard<bool> inOauth2Guard(m_inOAuth2Authentication, true);

    try
    {
        // Try to get the authentication code using the given provider.
        authCode = m_oauth2Handler->oauth2Authenticate( );

        // If that didn't work, call the fallback provider from SessionFactory
        if ( authCode.empty( ) )
        {
            libcmis::OAuth2AuthCodeProvider fallbackProvider = libcmis::SessionFactory::getOAuth2AuthCodeProvider( );
            if ( fallbackProvider != NULL )
            {
                char * code = fallbackProvider( m_oauth2Handler->getAuthURL().c_str(), getUsername().c_str(), getPassword().c_str() );
                if ( code != NULL )
                {
                    authCode = string( code );
                    free( code );
                }
            }
        }
    }
    catch ( const CurlException& e )
    {
        // Thrown by getUsername() and getPassword() if user cancels the credentials request
        throw e.getCmisException( );
    }

    // If still no auth code, then raise an exception
    if ( authCode.empty( ) )
        throw libcmis::Exception( "Couldn't get OAuth authentication code", "permissionDenied" );

    m_oauth2Handler->fetchTokens( string( authCode ) );
}

void HttpSession::setNoSSLCertificateCheck( bool noCheck )
{
    m_noSSLCheck = noCheck;
}

string HttpSession::getRefreshToken( )
{
    string refreshToken;
    if ( m_oauth2Handler )
        refreshToken = m_oauth2Handler->getRefreshToken( );
    return refreshToken;
}

void HttpSession::oauth2Refresh( )
try
{
    const ScopeGuard<bool> inOauth2Guard(m_inOAuth2Authentication, true);
    m_oauth2Handler->refresh( );
}
catch ( const libcmis::Exception& e )
{
    throw CurlException( e.what() );
}

void HttpSession::initProtocols( )
{
    const unsigned long protocols = CURLPROTO_HTTP | CURLPROTO_HTTPS;
    curl_easy_setopt(m_curlHandle, CURLOPT_PROTOCOLS, protocols);
    curl_easy_setopt(m_curlHandle, CURLOPT_REDIR_PROTOCOLS, protocols);
}

const char* CurlException::what( ) const noexcept
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
            else
                type = "permissionDenied";
            break;
    }

    return libcmis::Exception( msg, type );
}
