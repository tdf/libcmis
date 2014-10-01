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

#include "ws-navigationservice.hxx"
#include "ws-requests.hxx"
#include "ws-session.hxx"

using namespace std;

NavigationService::NavigationService( ) :
    m_session( NULL ),
    m_url( "" )
{
}

NavigationService::NavigationService( WSSession* session ) :
    m_session( session ),
    m_url( session->getServiceUrl( "NavigationService" ) )
{
}

NavigationService::NavigationService( const NavigationService& copy ) :
    m_session( copy.m_session ),
    m_url( copy.m_url )
{
}

NavigationService::~NavigationService( )
{
}

NavigationService& NavigationService::operator=( const NavigationService& copy )
{
    if ( this != &copy )
    {
        m_session = copy.m_session;
        m_url = copy.m_url;
    }

    return *this;
}

vector< libcmis::FolderPtr > NavigationService::getObjectParents( std::string repoId, std::string objectId ) throw ( libcmis::Exception )
{
    vector< libcmis::FolderPtr > parents;

    GetObjectParents request( repoId, objectId );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        GetObjectParentsResponse* response = dynamic_cast< GetObjectParentsResponse* >( resp );
        if ( response != NULL )
            parents = response->getParents( );
    }

    return parents;
}

vector< libcmis::ObjectPtr > NavigationService::getChildren( string repoId, string folderId ) throw ( libcmis::Exception )
{
    vector< libcmis::ObjectPtr > children;

    GetChildren request( repoId, folderId );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        GetChildrenResponse* response = dynamic_cast< GetChildrenResponse* >( resp );
        if ( response != NULL )
            children = response->getChildren( );
    }

    return children;
}
