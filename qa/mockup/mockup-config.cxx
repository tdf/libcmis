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
#include <string.h>

using namespace std;

namespace mockup
{
    Response::Response( string response, unsigned int status, bool isFilePath ) :
        m_response( response ),
        m_status( status ),
        m_isFilePath( isFilePath )
    {
    }

    RequestMatcher::RequestMatcher( string baseUrl, string matchParam ) :
        m_baseUrl( baseUrl ),
        m_matchParam( matchParam )
    {
    }

    bool RequestMatcher::operator< ( const RequestMatcher& compare ) const
    {
        int cmpBaseUrl = m_baseUrl.compare( compare.m_baseUrl ) ;
        bool result = cmpBaseUrl;
        if ( cmpBaseUrl == 0 )
        {
            int cmpMatchParam = m_matchParam.compare( compare.m_matchParam );
            result = cmpMatchParam < 0;
        }
        return result;
    }

    Configuration::Configuration( ) :
        m_responses( ),
        m_username( ),
        m_password( )
    {
    }

    bool Configuration::hasCredentials( )
    {
        return !m_username.empty( ) && !m_password.empty( );
    }

    CURLcode Configuration::writeResponse( CurlHandle* handle )
    {
        CURLcode code = CURLE_OK;

        string response;
        bool isFilePath = true;
        const string& url = handle->m_url;

        size_t pos = url.find( "?" );
        string urlBase = url;
        string params;
        if ( pos != string::npos )
        {
            urlBase = url.substr( 0, pos );
            params = url.substr( pos + 1 );
        }

        for ( map< RequestMatcher, Response >::iterator it = m_responses.begin( );
                it != m_responses.end( ) && response.empty( ); ++it )
        {
            RequestMatcher matcher = it->first;
            string& paramFind = matcher.m_matchParam;
            bool matchBaseUrl = matcher.m_baseUrl.empty() || ( matcher.m_baseUrl.find( urlBase ) == 0 );
            bool matchParams = paramFind.empty( ) || ( params.find( paramFind ) != string::npos );

            if ( matchBaseUrl && matchParams )
            {
                response = it->second.m_response;
                handle->m_httpError = it->second.m_status;
                isFilePath = it->second.m_isFilePath;
            }
            else if ( matchBaseUrl )
            {
                handle->m_httpError = 404;
            }
        }

        // If nothing matched, then send a 404 HTTP error instead
        if ( response.empty( ) )
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
               char* buf = strdup( response.c_str() );
               handle->m_writeFn( buf, 1, response.size( ), handle->m_writeData );
               delete( buf );
            }
        }

        // What curl error code to give?
        if ( handle->m_httpError != 0 )
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

void curl_mockup_addResponse( const char* urlBase, const char* matchParam, const char* response, unsigned int status, bool isFilePath )
{
    mockup::RequestMatcher matcher( urlBase, matchParam );
    map< mockup::RequestMatcher, mockup::Response >::iterator it = mockup::config->m_responses.find( matcher );
    if ( it != mockup::config->m_responses.end( ) )
        mockup::config->m_responses.erase( it );
    mockup::config->m_responses.insert( pair< mockup::RequestMatcher, mockup::Response >( matcher, mockup::Response( response, status, isFilePath ) ) );
}

void curl_mockup_setResponse( const char* filepath )
{
    mockup::config->m_responses.clear( );
    curl_mockup_addResponse( "", "", filepath );
}

void curl_mockup_setCredentials( const char* username, const char* password )
{
    mockup::config->m_username = string( username );
    mockup::config->m_password = string( password );
}

const char* curl_mockup_getProxy( CURL* curl )
{
    CurlHandle* handle = ( CurlHandle * )curl;
    if ( NULL != handle )
        return handle->m_proxy.c_str();
    return NULL;
}

const char* curl_mockup_getNoProxy( CURL* curl )
{
    CurlHandle* handle = ( CurlHandle * )curl;
    if ( NULL != handle )
        return handle->m_noProxy.c_str();
    return NULL;
}

const char* curl_mockup_getProxyUser( CURL* curl )
{
    CurlHandle* handle = ( CurlHandle * )curl;
    if ( NULL != handle )
        return handle->m_proxyUser.c_str();
    return NULL;
}

const char* curl_mockup_getProxyPass( CURL* curl )
{
    CurlHandle* handle = ( CurlHandle * )curl;
    if ( NULL != handle )
        return handle->m_proxyPass.c_str();
    return NULL;
}
