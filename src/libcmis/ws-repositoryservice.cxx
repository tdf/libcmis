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

#include "ws-repositoryservice.hxx"
#include "ws-requests.hxx"
#include "ws-session.hxx"

using namespace std;

RepositoryService::RepositoryService( WSSession* session ) :
    m_session( session ),
    m_url( )
{
    m_url = session->getServiceUrl( "RepositoryService" );
}

RepositoryService::RepositoryService( const RepositoryService& copy ) :
    m_session( copy.m_session ),
    m_url( copy.m_url )
{
}

RepositoryService::~RepositoryService( )
{
}

RepositoryService& RepositoryService::operator=( const RepositoryService& copy )
{
    m_session = copy.m_session;
    m_url = copy.m_url;

    return *this;
}

map< string, string > RepositoryService::getRepositories( ) throw ( SoapFault, CurlException )
{
    map< string, string > repositories;

    GetRepositories request;
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size() == 1 )
    {
       GetRepositoriesResponse* response = dynamic_cast< GetRepositoriesResponse* >( responses.front( ).get( ) );
       if ( response != NULL )
       {
           repositories = response->getRepositories();
       }
    }
    return repositories;
}
