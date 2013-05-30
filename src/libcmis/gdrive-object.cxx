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

GDriveObject::GDriveObject( GDriveSession* session, Json json ) :
    libcmis::Object( session )
{
   initializeFromJson( json ); 
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

void GDriveObject::initializeFromJson ( Json json )
{
    Json::JsonObject objs = json.getObjects( );
    Json::JsonObject::iterator it;
    for ( it = objs.begin( ); it != objs.end( ); it++)
    {
        PropertyPtr property(new GDriveProperty( it->first,it->second) );
        if ( property != NULL ){
            m_properties[ property->getPropertyType( )->getId()] = property;
           
            // we map "title" to both "cmis:name" and 
            // "cmis:getContentStreamFileName"
            if ( it->first == "title" )
            {
                property.reset( new GDriveProperty( "cmis:name", it->second) );
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

vector< RenditionPtr> GDriveObject::getRenditions( )
{
    if ( m_renditions.empty( ) )
    {
        string downloadUrl = getStringProperty( "downloadUrl" );
        if ( !downloadUrl.empty( ) )
        {
            string mimeType = getStringProperty( "cmis:contentStreamMimeType" );
            if ( !mimeType.empty( ) )
            { 
                RenditionPtr rendition( 
                    new Rendition( mimeType, mimeType, mimeType, downloadUrl ));
                m_renditions.push_back( rendition );
            }
        }

        string exportLinks = getStringProperty( "exportLinks" );
        if ( !exportLinks.empty( ) )
        {
            Json renditionJson = Json::parse( exportLinks );
            Json::JsonObject objs = renditionJson.getObjects( );
            Json::JsonObject::iterator it; 
            for ( it = objs.begin( ); it != objs.end( ); it++)
            { 
                string mimeType = it->first;
                string url = it->second.toString( );
                RenditionPtr rendition(
                    new Rendition( mimeType, mimeType, mimeType, url ) );
                m_renditions.push_back( rendition );
            }
        }

        // thumbnail link        
        string thumbnailLink = getStringProperty( "thumbnailLink" );
        if ( !thumbnailLink.empty( ) )
        {
            string mimeType = "cmis:thumbnail";   
            RenditionPtr rendition( 
                new Rendition( mimeType, mimeType, mimeType, thumbnailLink ));
            m_renditions.push_back( rendition );
        }
    }
    return m_renditions;
}

libcmis::ObjectPtr GDriveObject::updateProperties(
        const PropertyPtrMap& properties ) throw ( libcmis::Exception )
{
    // Make Json object from properties
    Json json = GdriveUtils::toGdriveJson( properties );

    istringstream is( json.toString( ));

    libcmis::HttpResponsePtr response;
    try 
    {   
        vector< string > headers;
        headers.push_back( "Content-Type: application/json" );
        response = getSession( )->httpPutRequest( getUrl( ), is, headers );
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

void GDriveObject::refresh( ) throw ( libcmis::Exception )
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

void GDriveObject::remove( bool /*allVersions*/ ) throw ( libcmis::Exception )
{
    try
    {
        getSession( )->httpDeleteRequest( getUrl( ) );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
}

void GDriveObject::move( FolderPtr source, FolderPtr destination ) 
                                        throw ( libcmis::Exception )
{  
    string parents = getStringProperty( "cmis:parentId" );
    string sourceId = source->getId( );
    string desId = destination->getId( );

    // Change the parentId from source Id to destination Id
    int sourcePos = parents.find( sourceId );
    int notFound = (signed int) string::npos;
    if ( sourcePos != notFound )
    {
        // Remove source Id
        parents.erase( sourcePos, sourceId.length( ) );
        // Add destination Id
        parents.insert( sourcePos, desId.c_str( ) );
    }

    // Create Json string from the parents string
    Json parentsJson;
    Json parentsValue = Json::parse( parents );
    parentsJson.add( "parents", parentsValue );
    
    istringstream is( parentsJson.toString( ) );
    libcmis::HttpResponsePtr response;
    try 
    {   
        vector< string > headers;
        headers.push_back( "Content-Type: application/json" );
        response = getSession( )->httpPutRequest( getUrl( ), is, headers );
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
    return getSession( )->getBindingUrl( ) + "/files/" + getId( );
}

string GDriveObject::getUploadUrl( )
{
    return GDRIVE_UPLOAD_LINKS;
}

