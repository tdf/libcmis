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

BaseSession::BaseSession( string atomPubUrl, string repositoryId, 
        string username, string password, bool verbose ) throw ( libcmis::Exception ) :
    Session( ),
    m_authProvider( ),
    m_curlHandle( NULL ),
    m_bindingUrl( atomPubUrl ),
    m_repositoryId( repositoryId ),
    m_username( username ),
    m_password( password ),
    m_authProvided( false ),
    m_repositories( ),
    m_verbose( verbose ),
    m_noHttpErrors( false )
{
    curl_global_init( CURL_GLOBAL_ALL );
    m_curlHandle = curl_easy_init( );
}

BaseSession::BaseSession( const BaseSession& copy ) :
    Session( ),
    m_authProvider( copy.m_authProvider ),
    m_curlHandle( NULL ),
    m_bindingUrl( copy.m_bindingUrl ),
    m_repositoryId( copy.m_repositoryId ),
    m_username( copy.m_username ),
    m_password( copy.m_password ),
    m_authProvided( copy.m_authProvided ),
    m_repositories( copy.m_repositories ),
    m_verbose( copy.m_verbose ),
    m_noHttpErrors( copy.m_noHttpErrors )
{
    // Not sure how sharing curl handles is safe.
    curl_global_init( CURL_GLOBAL_ALL );
    m_curlHandle = curl_easy_init( );
}


BaseSession& BaseSession::operator=( const BaseSession& copy )
{
    if ( this != &copy )
    {
        m_authProvider = copy.m_authProvider;
        m_curlHandle = NULL;
        m_bindingUrl = copy.m_bindingUrl;
        m_repositoryId = copy.m_repositoryId;
        m_username = copy.m_username;
        m_password = copy.m_password;
        m_authProvided = copy.m_authProvided;
        m_repositories = copy.m_repositories;
        m_verbose = copy.m_verbose;
        m_noHttpErrors = copy.m_noHttpErrors;
        
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
#if LIBCURL_VERSION_VALUE >= 0x071504
            char* escaped = curl_easy_escape( m_curlHandle, value.c_str(), value.length() );
#else
            char* escaped = curl_escape( value.c_str(), value.length() );
#endif
            url = url.replace( pos, name.size(), escaped );
            curl_free( escaped );
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
    libcmis::HttpResponsePtr response( new libcmis::HttpResponse( ) );

    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, lcl_bufferData );
    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, response->getData( ).get( ) );
    
    curl_easy_setopt( m_curlHandle, CURLOPT_HEADERFUNCTION, &lcl_getHeaders );
    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEHEADER, response.get() );

    // fix Cloudoku too many redirects error
    // note: though curl doc says -1 is the default for MAXREDIRS, the error i got
    // said it was 0
    curl_easy_setopt( m_curlHandle, CURLOPT_FOLLOWLOCATION, 1);
    curl_easy_setopt( m_curlHandle, CURLOPT_MAXREDIRS, 100);

    try
    {
        httpRunRequest( url );
        response->getData( )->finish();
    }
    catch ( const CurlException& e )
    {
        throw;
    }

    return response;
}

libcmis::HttpResponsePtr BaseSession::httpPutRequest( string url, istream& is, vector< string > headers ) throw ( CurlException )
{
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
    curl_easy_setopt( m_curlHandle, CURLOPT_READDATA, &is );
    curl_easy_setopt( m_curlHandle, CURLOPT_READFUNCTION, lcl_readStream );
    curl_easy_setopt( m_curlHandle, CURLOPT_UPLOAD, 1 );
    curl_easy_setopt( m_curlHandle, CURLOPT_IOCTLFUNCTION, lcl_ioctlStream );
    curl_easy_setopt( m_curlHandle, CURLOPT_IOCTLDATA, &is );

    struct curl_slist *headers_slist = NULL;
    for ( vector< string >::iterator it = headers.begin( ); it != headers.end( ); ++it )
        headers_slist = curl_slist_append( headers_slist, it->c_str( ) );
    curl_easy_setopt( m_curlHandle, CURLOPT_HTTPHEADER, headers_slist );

    try
    {
        httpRunRequest( url );
        response->getData( )->finish();
    }
    catch ( CurlException& e )
    {
        curl_slist_free_all( headers_slist );
        throw;
    }

    curl_slist_free_all( headers_slist );

    return response;
}

libcmis::HttpResponsePtr BaseSession::httpPostRequest( string url, istream& is, string contentType ) throw ( CurlException )
{
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
    curl_easy_setopt( m_curlHandle, CURLOPT_READDATA, &is );
    curl_easy_setopt( m_curlHandle, CURLOPT_READFUNCTION, lcl_readStream );
    curl_easy_setopt( m_curlHandle, CURLOPT_POST, 1 );
    curl_easy_setopt( m_curlHandle, CURLOPT_IOCTLFUNCTION, lcl_ioctlStream );
    curl_easy_setopt( m_curlHandle, CURLOPT_IOCTLDATA, &is );

    struct curl_slist *headers_slist = NULL;
    string contentTypeHeader = string( "Content-Type:" ) + contentType;
    headers_slist = curl_slist_append( headers_slist, contentTypeHeader.c_str( ) );
    curl_easy_setopt( m_curlHandle, CURLOPT_HTTPHEADER, headers_slist );

    try
    {
        httpRunRequest( url );
        response->getData( )->finish();
    }
    catch ( const CurlException& e )
    {
        curl_slist_free_all( headers_slist );
        throw;
    }

    curl_slist_free_all( headers_slist );

    return response;
}

void BaseSession::httpDeleteRequest( string url ) throw ( CurlException )
{
    curl_easy_setopt( m_curlHandle, CURLOPT_CUSTOMREQUEST, "DELETE" );
    httpRunRequest( url );
}

void BaseSession::httpRunRequest( string url ) throw ( CurlException )
{
    // Grab something from the web
    curl_easy_setopt( m_curlHandle, CURLOPT_URL, url.c_str() );

    // Set the credentials
    if ( m_authProvider.get() && !m_authProvided && ( m_username.empty() || m_password.empty() ) )
    {
        m_authProvided = m_authProvider->authenticationQuery( m_username, m_password );
        if ( !m_authProvided )
        {
            throw CurlException( "User cancelled authentication request" );
        }
    }

    if ( !m_username.empty() && !m_password.empty() )
    {
        curl_easy_setopt( m_curlHandle, CURLOPT_HTTPAUTH, CURLAUTH_ANY );

#if LIBCURL_VERSION_VALUE >= 0x071901
        curl_easy_setopt( m_curlHandle, CURLOPT_USERNAME, m_username.c_str() );
        curl_easy_setopt( m_curlHandle, CURLOPT_PASSWORD, m_password.c_str() );
#else
        string userpwd = m_username + ":" + m_password;
        curl_easy_setopt( m_curlHandle, CURLOPT_USERPWD, userpwd.c_str( ) );
#endif
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
    
    // Reset the handle for the next request
    curl_easy_reset( m_curlHandle );

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
