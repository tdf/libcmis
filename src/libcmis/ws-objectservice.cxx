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

#include "ws-objectservice.hxx"

#include "ws-requests.hxx"
#include "ws-session.hxx"

using namespace std;
using libcmis::PropertyPtrMap;

ObjectService::ObjectService( ) :
    m_session( NULL ),
    m_url( "" )
{
}

ObjectService::ObjectService( WSSession* session ) :
    m_session( session ),
    m_url( session->getServiceUrl( "ObjectService" ) )
{
}

ObjectService::ObjectService( const ObjectService& copy ) :
    m_session( copy.m_session ),
    m_url( copy.m_url )
{
}

ObjectService::~ObjectService( )
{
}

ObjectService& ObjectService::operator=( const ObjectService& copy )
{
    if ( this != &copy )
    {
        m_session = copy.m_session;
        m_url = copy.m_url;
    }

    return *this;
}

libcmis::ObjectPtr ObjectService::getObject( string repoId, string id )
{
    libcmis::ObjectPtr object;

    class GetObject request( repoId, id );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        GetObjectResponse* response = dynamic_cast< GetObjectResponse* >( resp );
        if ( response != NULL )
            object = response->getObject( );
    }

    return object;
}

libcmis::ObjectPtr ObjectService::getObjectByPath( string repoId, string path )
{
    libcmis::ObjectPtr object;

    GetObjectByPath request( repoId, path );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        GetObjectResponse* response = dynamic_cast< GetObjectResponse* >( resp );
        if ( response != NULL )
            object = response->getObject( );
    }

    return object;
}

vector< libcmis::RenditionPtr > ObjectService::getRenditions(
        string repoId, string objectId, string filter )
{
    vector< libcmis::RenditionPtr > renditions;

    GetRenditions request( repoId, objectId, filter );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        GetRenditionsResponse* response = dynamic_cast< GetRenditionsResponse* >( resp );
        if ( response != NULL )
        {
            renditions = response->getRenditions( );
        }
    }

    return renditions;
}

libcmis::ObjectPtr ObjectService::updateProperties(
        string repoId, string objectId,
        const PropertyPtrMap& properties,
        string changeToken )
{
    libcmis::ObjectPtr object;

    UpdateProperties request( repoId, objectId, properties, changeToken );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        UpdatePropertiesResponse* response = dynamic_cast< UpdatePropertiesResponse* >( resp );
        if ( response != NULL )
        {
            string id = response->getObjectId( );
            object = getObject( repoId, id );
        }
    }

    return object;
}

void ObjectService::deleteObject( string repoId, string id, bool allVersions )
{
    class DeleteObject request( repoId, id, allVersions );
    m_session->soapRequest( m_url, request );
}
        
vector< string > ObjectService::deleteTree( std::string repoId, std::string folderId, bool allVersions,
        libcmis::UnfileObjects::Type unfile, bool continueOnFailure )
{
    vector< string > failedIds;

    DeleteTree request( repoId, folderId, allVersions, unfile, continueOnFailure );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        DeleteTreeResponse* response = dynamic_cast< DeleteTreeResponse* >( resp );
        if ( response != NULL )
            failedIds = response->getFailedIds( );
    }

    return failedIds;
}

void ObjectService::move( string repoId, string objectId, string destId, string srcId )
{
    MoveObject request( repoId, objectId, destId, srcId );
    m_session->soapRequest( m_url, request );
}

boost::shared_ptr< istream > ObjectService::getContentStream( string repoId, string objectId )
{
    boost::shared_ptr< istream > stream;

    GetContentStream request( repoId, objectId );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        GetContentStreamResponse* response = dynamic_cast< GetContentStreamResponse* >( resp );
        if ( response != NULL )
            stream = response->getStream( );
    }

    return stream;
}

void ObjectService::setContentStream( std::string repoId, std::string objectId, bool overwrite, std::string changeToken,
        boost::shared_ptr< std::ostream > stream, std::string contentType, std::string fileName )
{
    SetContentStream request( repoId, objectId, overwrite, changeToken, stream, contentType, fileName );
    m_session->soapRequest( m_url, request );
}

libcmis::FolderPtr ObjectService::createFolder( string repoId, const PropertyPtrMap& properties,
        string folderId )
{
    libcmis::FolderPtr folder;

    CreateFolder request( repoId, properties, folderId );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        CreateFolderResponse* response = dynamic_cast< CreateFolderResponse* >( resp );
        if ( response != NULL )
        {
            string id = response->getObjectId( );
            folder = m_session->getFolder( id );
        }
    }

    return folder;
}

libcmis::DocumentPtr ObjectService::createDocument( string repoId, const PropertyPtrMap& properties,
        string folderId, boost::shared_ptr< ostream > stream, string contentType, string fileName )
{
    libcmis::DocumentPtr document;

    CreateDocument request( repoId, properties, folderId, stream, contentType, fileName );
    vector< SoapResponsePtr > responses = m_session->soapRequest( m_url, request );
    if ( responses.size( ) == 1 )
    {
        SoapResponse* resp = responses.front( ).get( );
        CreateFolderResponse* response = dynamic_cast< CreateFolderResponse* >( resp );
        if ( response != NULL )
        {
            string id = response->getObjectId( );
            libcmis::ObjectPtr object = m_session->getObject( id );
            document = boost::dynamic_pointer_cast< libcmis::Document >( object );
        }
    }

    return document;
}
