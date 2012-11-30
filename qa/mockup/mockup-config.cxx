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

using namespace std;

namespace mockup
{
    Response::Response( string filepath, string matchParam ) :
        m_filepath( filepath ),
        m_matchParam( matchParam )
    {
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

    string Configuration::getResponse( CurlHandle* handle )
    {
        string filepath;
        const string& url = handle->m_url;

        size_t pos = url.find( "?" );
        string urlBase = url;
        string params;
        if ( pos != string::npos )
        {
            urlBase = url.substr( 0, pos );
            params = url.substr( pos + 1 );
        }

        for ( map< string, Response >::iterator it = m_responses.begin( );
                it != m_responses.end( ) && filepath.empty( ); ++it )
        {
            string& paramFind = it->second.m_matchParam;
            bool matchBaseUrl = it->first.empty() || ( it->first.find( urlBase ) == 0 );
            bool matchParams = paramFind.empty( ) || ( params.find( paramFind ) != string::npos );

            if ( matchBaseUrl && matchParams )
            {
                filepath = it->second.m_filepath;
            }
            else if ( matchBaseUrl )
            {
                handle->m_httpError = 404;
            }
        }
        return filepath;
    }

    Configuration* config = new Configuration( );
}

void curl_mockup_reset( )
{
    if ( mockup::config != NULL )
        delete mockup::config;
    mockup::config = new mockup::Configuration( );
}

void curl_mockup_addResponse( const char* urlBase, const char* matchParam, const char* filepath )
{
    map< string, mockup::Response >::iterator it = mockup::config->m_responses.find( urlBase );
    if ( it != mockup::config->m_responses.end( ) )
        mockup::config->m_responses.erase( it );
    mockup::config->m_responses.insert( pair< string, mockup::Response >( urlBase, mockup::Response( filepath, matchParam ) ) );
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
