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

#include "mockup-config.h"
#include "internals.hxx"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

namespace
{
    void lcl_splitUrl( const string& url, string& urlBase, string& params )
    {
        size_t pos = url.find( "?" );
        urlBase = url;
        if ( pos != string::npos )
        {
            urlBase = url.substr( 0, pos );
            params = url.substr( pos + 1 );
        }
    }

    const char** lcl_toStringArray( vector< string > vect )
    {
        const char** array = new const char*[vect.size() + 1];
        for ( size_t i = 0; i < vect.size( ); i++ )
            array[i] = vect[i].c_str();
        array[vect.size()] = NULL;
        return array;
    }
}

namespace mockup
{
    Response::Response( string response, unsigned int status, bool isFilePath, string headers ) :
        m_response( response ),
        m_status( status ),
        m_isFilePath( isFilePath ),
        m_headers( headers )
    {
    }

    Request::Request( string url, string method, string body, vector< string > headers ) :
        m_url( url ),
        m_method( method ),
        m_body( body ),
        m_headers( headers )
    {
    }

    RequestMatcher::RequestMatcher( string baseUrl, string matchParam, string method, string matchBody ) :
        m_baseUrl( baseUrl ),
        m_matchParam( matchParam ),
        m_method( method ),
        m_matchBody( matchBody )
    {
    }

    bool RequestMatcher::operator< ( const RequestMatcher& compare ) const
    {
        int cmpBaseUrl = m_baseUrl.compare( compare.m_baseUrl ) ;
        if ( cmpBaseUrl != 0 )
            return cmpBaseUrl < 0;

        int cmpMatchParam = m_matchParam.compare( compare.m_matchParam );
        if ( cmpMatchParam != 0 )
            return cmpMatchParam < 0;

        int cmpMatchMethod = m_method.compare( compare.m_method );
        if ( cmpMatchMethod != 0 )
            return cmpMatchMethod < 0;

        int cmpMatchBody = m_matchBody.compare( compare.m_matchBody );
        return cmpMatchBody < 0;
    }

    Configuration::Configuration( ) :
        m_responses( ),
        m_requests( ),
        m_username( ),
        m_password( ),
        m_badSSLCertificate( )
    {
    }

    bool Configuration::hasCredentials( )
    {
        return !m_username.empty( ) && !m_password.empty( );
    }

    CURLcode Configuration::writeResponse( CurlHandle* handle )
    {
        CURLcode code = CURLE_OK;

        string headers;
        string response;
        bool foundResponse = false;
        bool isFilePath = true;
        const string& url = handle->m_url;

        string urlBase = url;
        string params;
        lcl_splitUrl( url, urlBase, params );
        string method = handle->m_method;
        string body = m_requests.back().m_body;

        for ( map< RequestMatcher, Response >::iterator it = m_responses.begin( );
                it != m_responses.end( ) && response.empty( ); ++it )
        {
            RequestMatcher matcher = it->first;
            string& paramFind = matcher.m_matchParam;
            bool matchBaseUrl = matcher.m_baseUrl.empty() || ( urlBase == matcher.m_baseUrl );
            bool matchParams = paramFind.empty( ) || ( params.find( paramFind ) != string::npos );
            bool matchMethod = it->first.m_method.empty( ) || ( it->first.m_method == method );
            bool matchBody = matcher.m_matchBody.empty( ) || ( body.find( matcher.m_matchBody ) != string::npos );

            if ( matchBaseUrl && matchParams && matchMethod && matchBody )
            {
                foundResponse = true;
                response = it->second.m_response;
                handle->m_httpError = it->second.m_status;
                isFilePath = it->second.m_isFilePath;
                headers = it->second.m_headers;
            }
        }

        // Output headers is any
        if ( !headers.empty() )
        {
            char* buf = strdup( headers.c_str() );
            handle->m_headersFn( buf, 1, headers.size( ), handle->m_headersData );
            free( buf );
        }

        // If nothing matched, then send a 404 HTTP error instead
        if ( !foundResponse || ( foundResponse && isFilePath && response.empty() ) )
            handle->m_httpError = 404;
        else
        {
            if ( isFilePath )
            {
                FILE* fd = fopen( response.c_str( ), "r" );

                size_t bufSize = 2048;
                char* buf = new char[bufSize];

                size_t read = 0;
                size_t written = 0;
                do
                {
                    read = fread( buf, 1, bufSize, fd );
                    written = handle->m_writeFn( buf, 1, read, handle->m_writeData );
                } while ( read == bufSize && written == read );

                fclose( fd );
                delete[] buf;
            }
            else
            {
                if ( !response.empty() )
                {
                    char* buf = strdup( response.c_str() );
                    handle->m_writeFn( buf, 1, response.size( ), handle->m_writeData );
                    free( buf );
                }
            }
        }

        // What curl error code to give?
        if ( handle->m_httpError == 0 )
            handle->m_httpError = 200;

        if ( handle->m_httpError < 200 || handle->m_httpError >= 300 )
            code = CURLE_HTTP_RETURNED_ERROR;

        return code;
    }

    Configuration* config = new Configuration( );
}

void curl_mockup_reset( )
{
    if ( mockup::config != NULL )
        delete mockup::config;
    mockup::config = new mockup::Configuration( );
}

void curl_mockup_addResponse( const char* urlBase, const char* matchParam, const char* method,
                              const char* response, unsigned int status, bool isFilePath,
                              const char* headers, const char* matchBody )
{
    string matchBodyStr;
    if ( matchBody )
        matchBodyStr = matchBody;
    mockup::RequestMatcher matcher( urlBase, matchParam, method, matchBodyStr );
    map< mockup::RequestMatcher, mockup::Response >::iterator it = mockup::config->m_responses.find( matcher );
    if ( it != mockup::config->m_responses.end( ) )
        mockup::config->m_responses.erase( it );

    string headersStr;
    if ( headers != NULL )
        headersStr = headers;
    mockup::Response responseDesc( response, status, isFilePath, headersStr );
    mockup::config->m_responses.insert( pair< mockup::RequestMatcher, mockup::Response >( matcher, responseDesc ) );
}

void curl_mockup_setResponse( const char* filepath )
{
    mockup::config->m_responses.clear( );
    curl_mockup_addResponse( "", "", "", filepath );
}

void curl_mockup_setCredentials( const char* username, const char* password )
{
    mockup::config->m_username = string( username );
    mockup::config->m_password = string( password );
}

const struct HttpRequest* curl_mockup_getRequest( const char* urlBase,
                                                  const char* matchParam,
                                                  const char* method,
                                                  const char* matchBody )
{
    struct HttpRequest* request = NULL;

    string urlBaseString( urlBase );
    string matchParamString( matchParam );

    string matchBodyStr;
    if ( matchBody )
        matchBodyStr = matchBody;

    for ( vector< mockup::Request >::iterator it = mockup::config->m_requests.begin( );
            it != mockup::config->m_requests.end( ) && request == NULL; ++it )
    {
        string url;
        string params;
        if ( it->m_method == string( method ) )
        {
            lcl_splitUrl( it->m_url, url, params );

            bool matchBaseUrl = urlBaseString.empty() || ( url.find( urlBaseString ) == 0 );
            bool matchParams = matchParamString.empty( ) || ( params.find( matchParamString ) != string::npos );
            bool matchBodyPart = !matchBody || ( it->m_body.find( matchBodyStr ) != string::npos );

            if ( matchBaseUrl && matchParams && matchBodyPart )
            {
                request = new HttpRequest;
                request->url = it->m_url.c_str();
                request->body = it->m_body.c_str();
                request->headers = lcl_toStringArray( it->m_headers );
            }
        }
    }

    return request;
}

const char* curl_mockup_getRequestBody( const char* urlBase,
                                        const char* matchParam,
                                        const char* method,
                                        const char* matchBody )
{
    const struct HttpRequest* request = curl_mockup_getRequest( urlBase, matchParam, method, matchBody );
    if ( request )
    {
        const char* body = request->body;
        curl_mockup_HttpRequest_free( request );
        return body;
    }
    return NULL;
}

int curl_mockup_getRequestsCount( const char* urlBase,
                                  const char* matchParam,
                                  const char* method,
                                  const char* matchBody )
{
    int count = 0;

    string urlBaseString( urlBase );
    string matchParamString( matchParam );
    string matchBodyStr( matchBody );

    for ( vector< mockup::Request >::iterator it = mockup::config->m_requests.begin( );
            it != mockup::config->m_requests.end( ); ++it )
    {
        string url;
        string params;
        if ( it->m_method == string( method ) )
        {
            lcl_splitUrl( it->m_url, url, params );

            bool matchBaseUrl = urlBaseString.empty() ||
                                  ( url.find( urlBaseString ) == 0 );
            bool matchParams = matchParamString.empty( ) ||
                                  ( params.find( matchParamString ) != string::npos );
            bool matchBodyPart = !matchBody ||
                                  ( it->m_body.find( matchBodyStr ) != string::npos );

            if ( matchBaseUrl && matchParams && matchBodyPart )
            {
                count++;
            }
        }
    }
    return count;
}

void curl_mockup_HttpRequest_free( const struct HttpRequest* request )
{
    delete[] request->headers;
    delete request;
}

char* curl_mockup_HttpRequest_getHeader( const struct HttpRequest* request, const char* name )
{
    char* value = NULL;
    size_t i = 0;
    while ( request->headers[i] != NULL )
    {
        string header = request->headers[i];
        size_t pos = header.find( string( name ) + ":" );
        if ( pos == 0 )
        {
            value = strdup( header.substr( pos + 1 ).c_str() );
        }
        ++i;
    }
    return value;
}

const char* curl_mockup_getProxy( CURL* curl )
{
    CurlHandle* handle = static_cast< CurlHandle* >( curl );
    if ( NULL != handle )
        return handle->m_proxy.c_str();
    return NULL;
}

const char* curl_mockup_getNoProxy( CURL* curl )
{
    CurlHandle* handle = static_cast< CurlHandle* >( curl );
    if ( NULL != handle )
        return handle->m_noProxy.c_str();
    return NULL;
}

const char* curl_mockup_getProxyUser( CURL* curl )
{
    CurlHandle* handle = static_cast< CurlHandle* >( curl );
    if ( NULL != handle )
        return handle->m_proxyUser.c_str();
    return NULL;
}

const char* curl_mockup_getProxyPass( CURL* curl )
{
    CurlHandle* handle = static_cast< CurlHandle* >( curl );
    if ( NULL != handle )
        return handle->m_proxyPass.c_str();
    return NULL;
}

void curl_mockup_setSSLBadCertificate( const char* certificate )
{
    mockup::config->m_badSSLCertificate = string( certificate );
}
