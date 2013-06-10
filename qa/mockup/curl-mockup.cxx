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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

#include "curl/curl.h"
#include "internals.hxx"

using namespace std;

namespace mockup
{
    extern Configuration* config;
}

struct curl_slist *curl_slist_append( struct curl_slist *, const char * )
{
    /* TODO Implement me */
    return NULL;
}

void curl_slist_free_all( struct curl_slist * )
{
    /* TODO Implement me */
}

void curl_free( void * /*p*/ )
{
    /* TODO Implement me */
}

CURLcode curl_global_init( long )
{
    return CURLE_OK;
}

CURL *curl_easy_init( void )
{
    return new CurlHandle();
}

void curl_easy_cleanup( CURL * curl )
{
    CurlHandle* handle = static_cast< CurlHandle* >( curl );
    delete( handle );
}

void curl_easy_reset( CURL * curl )
{
    CurlHandle* handle = static_cast< CurlHandle * >( curl );
    handle->reset( );
}

char *curl_easy_escape( CURL *, const char *string, int length )
{
    return strndup( string, length );
}

char *curl_unescape( const char *string, int length )
{
    return strndup( string, length );
}

char *curl_easy_unescape( CURL *, const char *string, int length, int * )
{
    return curl_unescape( string, length );
}

CURLcode curl_easy_setopt( CURL * curl, CURLoption option, ... )
{
    CurlHandle* handle = static_cast< CurlHandle * >( curl );

    va_list arg;
    va_start( arg, option );
    switch ( option )
    {
        /* TODO Add support for more options */
        case CURLOPT_POST:
        {
            if ( va_arg( arg, long ) )
                handle->m_method = string( "POST" );
            break;
        }
        case CURLOPT_UPLOAD:
        {
            if ( 0 != va_arg( arg, long ) )
                handle->m_method = string( "PUT" );
            break;
        }
        case CURLOPT_CUSTOMREQUEST:
            handle->m_method = string( va_arg( arg, char* ) );
            break;
        case CURLOPT_URL:
        {
            handle->m_url = string( va_arg( arg, char* ) );
            break;
        }
        case CURLOPT_WRITEFUNCTION:
        {
            handle->m_writeFn = va_arg( arg, write_callback ); 
            break;
        }
        case CURLOPT_WRITEDATA:
        {
            handle->m_writeData = va_arg( arg, void* );
            break;
        }
        case CURLOPT_READFUNCTION:
        {
            handle->m_readFn = va_arg( arg, read_callback ); 
            break;
        }
        case CURLOPT_READDATA:
        {
            handle->m_readData = va_arg( arg, void* );
            break;
        }
        case CURLOPT_INFILESIZE:
        {
            handle->m_readSize = va_arg( arg, long );
            break;
        }
        case CURLOPT_HEADERFUNCTION:
        {
            handle->m_headersFn = va_arg( arg, headers_callback ); 
            break;
        }
        case CURLOPT_WRITEHEADER:
        {
            handle->m_headersData = va_arg( arg, void* );
            break;
        }
        case CURLOPT_USERNAME:
        {
            handle->m_username = string( va_arg( arg, char* ) );
            break;
        }
        case CURLOPT_PASSWORD:
        {
            handle->m_password = string( va_arg( arg, char* ) );
            break;
        }
        case CURLOPT_USERPWD:
        {
            string userpwd( va_arg( arg, char* ) );
            size_t pos = userpwd.find( ':' );
            if ( pos != string::npos )
            {
                handle->m_username = userpwd.substr( 0, pos );
                handle->m_password = userpwd.substr( pos + 1 );
            }
            break;
        }
        case CURLOPT_PROXY:
        {
            // FIXME curl does some more complex things with port and type
            handle->m_proxy = string( va_arg( arg, char* ) );
            break;
        }
        case CURLOPT_NOPROXY:
        {
            handle->m_noProxy = string( va_arg( arg, char* ) );
            break;
        }
        case CURLOPT_PROXYUSERNAME:
        {
            handle->m_proxyUser = string( va_arg( arg, char* ) );
            break;
        }
        case CURLOPT_PROXYPASSWORD:
        {
            handle->m_proxyPass = string( va_arg( arg, char* ) );
            break;
        }
        case CURLOPT_PROXYUSERPWD:
        {
            string userpwd( va_arg( arg, char* ) );
            size_t pos = userpwd.find( ':' );
            if ( pos != string::npos )
            {
                handle->m_proxyUser = userpwd.substr( 0, pos );
                handle->m_proxyPass = userpwd.substr( pos + 1 );
            }
            break;
        }
       default:
        {
            // We surely don't want to break the test for that.
        }
    }
    va_end( arg );

    return CURLE_OK;
}

CURLcode curl_easy_perform( CURL * curl )
{
    CurlHandle* handle = static_cast< CurlHandle * >( curl );

    /* Check the credentials */
    if ( mockup::config->hasCredentials( ) &&
            ( handle->m_username != mockup::config->m_username ||
              handle->m_password != mockup::config->m_password ) )
    {
        // Send HTTP 401
        handle->m_httpError = 401;
        return CURLE_HTTP_RETURNED_ERROR;
    }
    
    // Store the requests for later verifications
    stringstream body;

    if ( handle->m_readFn && handle->m_readData )
    {
        size_t bufSize = 2048;
        char* buf = new char[bufSize];

        size_t read = 0;
        do
        {
            read = handle->m_readFn( buf, 1, bufSize, handle->m_readData );
            body.write( buf, read );
        } while ( read == bufSize );

        delete[] buf;
    }
    mockup::config->m_requests.push_back( mockup::Request( handle->m_url, handle->m_method, body.str( ) ) );
    

    return mockup::config->writeResponse( handle );
}

CURLcode curl_easy_getinfo( CURL * curl, CURLINFO info, ... )
{
    CurlHandle* handle = static_cast< CurlHandle * >( curl );

    va_list arg;
    va_start( arg, info );
    switch ( info )
    {
        case CURLINFO_RESPONSE_CODE:
        {
            long* buf = va_arg( arg, long* ); 
            *buf = handle->m_httpError;
            break;
        }
        default:
        {
            // We surely don't want to break the test for that.
        }
    }
    va_end( arg );

    return CURLE_OK;
}

CurlHandle::CurlHandle( ) :
    m_url( ),
    m_writeFn( NULL ),
    m_writeData( NULL ),
    m_readFn( NULL ),
    m_readData( NULL ),
    m_readSize( 0 ),
    m_headersFn( NULL ),
    m_headersData( NULL ),
    m_username( ),
    m_password( ),
    m_proxy( ),
    m_noProxy( ),
    m_proxyUser( ),
    m_proxyPass( ),
    m_httpError( 0 ),
    m_method( "GET" )
{
}

CurlHandle::CurlHandle( const CurlHandle& copy ) :
    m_url( copy.m_url ),
    m_writeFn( copy.m_writeFn ),
    m_writeData( copy.m_writeData ),
    m_readFn( copy.m_readFn ),
    m_readData( copy.m_readData ),
    m_readSize( copy.m_readSize ),
    m_headersFn( copy.m_headersFn ),
    m_headersData( copy.m_headersData ),
    m_username( copy.m_username ),
    m_password( copy.m_password ),
    m_proxy( copy.m_proxy ),
    m_noProxy( copy.m_noProxy ),
    m_proxyUser( copy.m_proxyUser ),
    m_proxyPass( copy.m_proxyPass ),
    m_httpError( copy.m_httpError ),
    m_method( copy.m_method )
{
}

CurlHandle& CurlHandle::operator=( const CurlHandle& copy )
{
    if ( this != &copy )
    {
        m_url = copy.m_url;
        m_writeFn = copy.m_writeFn;
        m_writeData = copy.m_writeData;
        m_readFn = copy.m_readFn;
        m_readData = copy.m_readData;
        m_readSize = copy.m_readSize;
        m_headersFn = copy.m_headersFn;
        m_headersData = copy.m_headersData;
        m_username = copy.m_username;
        m_password = copy.m_password;
        m_httpError = copy.m_httpError;
        m_method = copy.m_method;
    }
    return *this;
}

void CurlHandle::reset( )
{
    m_url = string( );
    m_writeFn = NULL;
    m_writeData = NULL;
    m_readFn = NULL;
    m_readData = NULL;
    m_readSize = 0;
    m_username = string( );
    m_password = string( );
    m_method = "GET";
}
