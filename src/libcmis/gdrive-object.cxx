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
 * Copyright (C) 2013 SUSE <cbosdonnat@suse.com>
 *               2013 Cao Cuong Ngo <cao.cuong.ngo@gmail.com>   
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

#include "gdrive-object.hxx"

#include "gdrive-property.hxx"
#include "gdrive-allowable-actions.hxx"
#include "gdrive-repository.hxx"
#include "gdrive-utils.hxx"

using namespace std;
using namespace libcmis;

GDriveObject::GDriveObject( GDriveSession* session ) :
    libcmis::Object( session )
{
}

GDriveObject::GDriveObject( GDriveSession* session, Json json, string id, string name ) :
    libcmis::Object( session )
{
   initializeFromJson( json, id, name ); 
}

GDriveObject::GDriveObject( const GDriveObject& copy ) :
    libcmis::Object( copy )
{
}

GDriveObject& GDriveObject::operator=( const GDriveObject& copy )
{
    if ( this != &copy )
    {
        libcmis::Object::operator=( copy );
    }
    return *this;
}

void GDriveObject::initializeFromJson ( Json json, string id, string name )
{
    Json::JsonObject objs = json.getObjects( );
    Json::JsonObject::iterator it;
    for ( it = objs.begin( ); it != objs.end( ); ++it)
    {
        PropertyPtr property;
 
        // in case of a revision, get the ID and name of the original file
        if ( !id.empty( ) && it->first == "id" )
        {
            Json idJson( id.c_str( ) );
            property.reset( new GDriveProperty( "id", idJson ) );
            m_properties[ property->getPropertyType( )->getId()] = property;
            property.reset( new GDriveProperty( "revisionId", it->second ) );
            m_properties[ property->getPropertyType( )->getId()] = property;

            Json nameJson( name.c_str( ) );
            property.reset( new GDriveProperty( "cmis:name", nameJson) );
            m_properties[ property->getPropertyType( )->getId()] = property;
            property.reset( new GDriveProperty( "cmis:contentStreamFileName", nameJson) );
            m_properties[ property->getPropertyType( )->getId()] = property;
        }
        else 
        {
            property.reset( new GDriveProperty( it->first, it->second ) );
            m_properties[ property->getPropertyType( )->getId()] = property;
           
            // we map "name" to both "cmis:name" and "cmis:getContentStreamFileName"
            if ( it->first == "name" )
            {
                property.reset( new GDriveProperty( "cmis:name", it->second) );
                m_properties[ property->getPropertyType( )->getId()] = property;
            }
            
            // some revision keep the original file name.
            if ( it->first == "originalFilename" )
            {
                property.reset( new GDriveProperty( "cmis:name", it->second ) );
                m_properties[ property->getPropertyType( )->getId()] = property;
                property.reset( new GDriveProperty( "cmis:contentStreamFileName", it->second) );
                m_properties[ property->getPropertyType( )->getId()] = property;
            }

            // In case of a revision, get the modified date as creation date
            if ( it->first == "modifiedDate" && !id.empty( ) )
            {
                property.reset( new GDriveProperty( "cmis:creationDate", it->second) );
                m_properties[ property->getPropertyType( )->getId()] = property;
            }
            // In case of a revision, get the last modifying user and the creator
            if ( it->first == "lastModifyingUserName" && !id.empty( ) )
            {
                property.reset( new GDriveProperty( "cmis:createdBy", it->second) );
                m_properties[ property->getPropertyType( )->getId()] = property;
            }
        }
    }
    m_refreshTimestamp = time( NULL );
    
    // Create AllowableActions
    bool isFolder = json["mimeType"].toString( ) == GDRIVE_FOLDER_MIME_TYPE;
    m_allowableActions.reset( new GdriveAllowableActions( isFolder ) );
}

GDriveSession* GDriveObject::getSession( )
{
    return dynamic_cast< GDriveSession* > ( m_session );
}

void GDriveObject::refreshImpl( Json json )
{
    m_typeDescription.reset( );
    m_properties.clear( );
    initializeFromJson( json );
}

vector< RenditionPtr> GDriveObject::getRenditions( string /* filter */ )
{
    if ( m_renditions.empty( ) )
    {
        string downloadUrl = GDRIVE_METADATA_LINK + getId( ) + "?alt=media";
        string mimeType = getStringProperty( "cmis:contentStreamMimeType" );
        if ( !mimeType.empty( ) )
        {
            RenditionPtr rendition(
                new Rendition( mimeType, mimeType, mimeType, downloadUrl ));
            m_renditions.push_back( rendition );
        }

        vector< string > exportLinks = getMultiStringProperty( "exportLinks" );
        for ( vector<string>::iterator it = exportLinks.begin( ); it != exportLinks.end( ); ++it)
        { 
            int pos = (*it).find(":\"");
            if ( pos == -1 ) continue;
            mimeType = (*it).substr( 0, pos );
            string url = (*it).substr( pos + 2, (*it).length( ) - pos - 3 );
            RenditionPtr rendition(
                new Rendition( mimeType, mimeType, mimeType, url ) );
            m_renditions.push_back( rendition );
        }

        // thumbnail link        
        string thumbnailLink = getStringProperty( "thumbnailLink" );
        if ( !thumbnailLink.empty( ) )
        {
            mimeType = "cmis:thumbnail";
            RenditionPtr rendition( 
                new Rendition( mimeType, mimeType, mimeType, thumbnailLink ));
            m_renditions.push_back( rendition );
        }
    }
    return m_renditions;
}

libcmis::ObjectPtr GDriveObject::updateProperties(
        const PropertyPtrMap& properties )
{
    // Make Json object from properties
    Json json = GdriveUtils::toGdriveJson( properties );

    istringstream is( json.toString( ));

    libcmis::HttpResponsePtr response;
    try 
    {   
        vector< string > headers;
        headers.push_back( "Content-Type: application/json" );
        response = getSession( )->httpPatchRequest( getUrl( ), is, headers );
    }
    catch ( const CurlException& e )
    {   
        throw e.getCmisException( );
    }
    
    string res = response->getStream( )->str( );
    Json jsonRes = Json::parse( res );
    libcmis::ObjectPtr updated( new GDriveObject ( getSession( ), jsonRes ) );

    if ( updated->getId( ) == getId( ) )
         refreshImpl( jsonRes );

    return updated;
}

void GDriveObject::refresh( )
{
    string res;
    try
    {
        res  = getSession()->httpGetRequest( getUrl( ) )->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    Json json = Json::parse( res );
    refreshImpl( json );
}

void GDriveObject::remove( bool /*allVersions*/ )
{
    try
    {
        getSession( )->httpDeleteRequest( GDRIVE_METADATA_LINK + getId( ) );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
}

void GDriveObject::move( FolderPtr /*source*/, FolderPtr destination ) 
{  
    Json parentsJson;
    parentsJson.add( "addParents", Json(destination->getId( ).c_str()) );
    parentsJson.add( "removeParents", Json(getStringProperty( "cmis:parentId" ).c_str()) );
    
    istringstream is( parentsJson.toString( ) );
    libcmis::HttpResponsePtr response;
    try 
    {   
        vector< string > headers;
        headers.push_back( "Content-Type: application/json" );
        response = getSession( )->httpPatchRequest( getUrl( ), is, headers );
    }
    catch ( const CurlException& e )
    {   
        throw e.getCmisException( );
    }
    string res = response->getStream( )->str( );
    Json jsonRes = Json::parse( res );

    refreshImpl( jsonRes );
}

string GDriveObject::getUrl( )
{
    // thumbnailLink causes some operations to fail with internal server error,
    // see https://issuetracker.google.com/issues/36760667
    return GDRIVE_METADATA_LINK + getId( ) +
                "?fields=kind,id,name,parents,mimeType,createdTime,modifiedTime,size";
}

vector< string> GDriveObject::getMultiStringProperty( const string& propertyName )
{
    vector< string > values;
    PropertyPtrMap::const_iterator it = getProperties( ).find( string( propertyName ) );
    if ( it != getProperties( ).end( ) && it->second != NULL && !it->second->getStrings( ).empty( ) )
        values = it->second->getStrings( );
    return values; 
}

