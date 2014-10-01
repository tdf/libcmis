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

#include "ws-versioningservice.hxx"
#include "ws-requests.hxx"
#include "ws-session.hxx"

using namespace std;
using libcmis::PropertyPtrMap;

VersioningService::VersioningService( ) :
    m_session( NULL ),
    m_url( "" )
{
}

VersioningService::VersioningService( WSSession* session ) :
    m_session( session ),
    m_url( session->getServiceUrl( "VersioningService" ) )
{
}

VersioningService::VersioningService( const VersioningService& copy ) :
    m_session( copy.m_session ),
    m_url( copy.m_url )
{
}

VersioningService::~VersioningService( )
{
}

VersioningService& VersioningService::operator=( const VersioningService& copy )
{
    if ( this != &copy )
    {
        m_session = copy.m_session;
        m_url = copy.m_url;
    }

    return *this;
}

libcmis::DocumentPtr VersioningService::checkOut( string repoId, string documentId ) throw ( libcmis::Exception )
{
    libcmis::DocumentPtr pwc;

    CheckOut request( repoId, documentId );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        CheckOutResponse* response = dynamic_cast< CheckOutResponse* >( resp );
        if ( response != NULL )
        {
            string pwcId = response->getObjectId( );
            libcmis::ObjectPtr object = m_session->getObject( pwcId );
            pwc = boost::dynamic_pointer_cast< libcmis::Document >( object );
        }
    }

    return pwc;
}

void VersioningService::cancelCheckOut( string repoId, string documentId ) throw ( libcmis::Exception )
{
    CancelCheckOut request( repoId, documentId );
    m_session->soapRequest( m_url, request );
}

libcmis::DocumentPtr VersioningService::checkIn( string repoId, string objectId, bool isMajor,
        const PropertyPtrMap& properties,
        boost::shared_ptr< ostream > stream, string contentType, string fileName,
        string comment ) throw ( libcmis::Exception )
{
    libcmis::DocumentPtr newVersion;

    CheckIn request( repoId, objectId, isMajor, properties, stream, contentType, fileName, comment );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        CheckInResponse* response = dynamic_cast< CheckInResponse* >( resp );
        if ( response != NULL )
        {
            string newId = response->getObjectId( );
            libcmis::ObjectPtr object = m_session->getObject( newId );
            newVersion = boost::dynamic_pointer_cast< libcmis::Document >( object );
        }
    }

    return newVersion;
}

vector< libcmis::DocumentPtr > VersioningService::getAllVersions( string repoId, string objectId ) throw ( libcmis::Exception )
{
    vector< libcmis::DocumentPtr > versions;

    GetAllVersions request( repoId, objectId );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        GetAllVersionsResponse* response = dynamic_cast< GetAllVersionsResponse* >( resp );
        if ( response != NULL )
        {
            versions = response->getObjects( );
        }
    }

    return versions;
}
