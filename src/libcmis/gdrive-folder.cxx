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
 * Copyright (C) 2013 Cao Cuong Ngo <cao.cuong.ngo@gmail.com>
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

#include "gdrive-folder.hxx"

#include "gdrive-session.hxx"
#include "gdrive-document.hxx"
#include "gdrive-property.hxx"
#include "gdrive-utils.hxx"

using namespace std;
using namespace libcmis;

GDriveFolder::GDriveFolder( GDriveSession* session ):
    libcmis::Object( session ),
    libcmis::Folder( session ),
    GDriveObject( session )
{
}

GDriveFolder::GDriveFolder( GDriveSession* session, Json json ):
    libcmis::Object( session ),
    libcmis::Folder( session ),
    GDriveObject( session, json )
{
}

GDriveFolder::~GDriveFolder( )
{
}

vector< libcmis::ObjectPtr > GDriveFolder::getChildren( ) 
{
    vector< libcmis::ObjectPtr > children;
    
    // GDrive doesn't support fetch all the children in one query.
    // Instead of sending multiple queries for children,
    // we send a single query to search for objects where parents
    // include the folderID.
    string query = GDRIVE_METADATA_LINK + "?q=\"" + getId( ) + "\"+in+parents+and+trashed+=+false" +
        "&fields=files(kind,id,name,parents,mimeType,createdTime,modifiedTime,thumbnailLink,size)";

    string res;
    try
    {
        res = getSession( )->httpGetRequest( query )->getStream()->str();
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    Json jsonRes = Json::parse( res );
    Json::JsonVector objs = jsonRes["files"].getList( );
    
    // Create children objects from Json objects
    for(unsigned int i = 0; i < objs.size(); i++)
    {   
        ObjectPtr child;
        if ( objs[i]["mimeType"].toString( ) == GDRIVE_FOLDER_MIME_TYPE )
            child.reset( new GDriveFolder( getSession( ), objs[i] ) );
        else
            child.reset( new GDriveDocument( getSession( ), objs[i] ) );
        children.push_back( child );
    }   
    
    return children;
}

string GDriveFolder::uploadProperties( Json properties )
{
    // URL for uploading meta data
    string metaUrl =  GDRIVE_METADATA_LINK + "?fields=kind,id,name,parents,mimeType,createdTime,modifiedTime";

    // add parents to the properties    
    properties.add( "parents", GdriveUtils::createJsonFromParentId( getId( ) ) );
    
    //upload metadata
    std::istringstream is( properties.toString( ) );
    string response;
    try
    {
        response = getSession()->httpPostRequest( metaUrl, is, "application/json" )
                                    ->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }    

    return response;
}
                             
libcmis::FolderPtr GDriveFolder::createFolder( 
    const PropertyPtrMap& properties ) 
{
    Json propsJson = GdriveUtils::toGdriveJson( properties );
 
    // GDrive folder is a file with a different mime type.
    string mimeType = GDRIVE_FOLDER_MIME_TYPE;
  
    // Add mimetype to the propsJson
    Json jsonMimeType( mimeType.c_str( ) );
    propsJson.add( "mimeType", jsonMimeType );
    
    // Upload meta-datas
    string response = uploadProperties( propsJson );
    
    Json jsonRes = Json::parse( response );
    libcmis::FolderPtr folderPtr( new GDriveFolder( getSession( ), jsonRes ) );

    return folderPtr;
}

libcmis::DocumentPtr GDriveFolder::createDocument( 
    const PropertyPtrMap& properties, 
    boost::shared_ptr< ostream > os, 
    string contentType, string fileName ) 
{    
    if ( !os.get( ) )
        throw libcmis::Exception( "Missing stream" );
    
    Json propsJson = GdriveUtils::toGdriveJson( properties );

    if(!fileName.empty()) {
        // use provided filename
        Json jsonFilename( fileName.c_str( ) );

        propsJson.add( "name", jsonFilename );
    }
    if(!contentType.empty()) {
        propsJson.add( "mimeType", Json(contentType.c_str()));
    }
    
    // Upload meta-datas
    string res = uploadProperties( propsJson);

    // parse the document
    Json jsonRes = Json::parse( res );

    boost::shared_ptr< GDriveDocument >
      gDocument( new GDriveDocument( getSession( ), jsonRes ) );

    // Upload stream
    gDocument->uploadStream( os, contentType);    

    return gDocument;
}

vector< string > GDriveFolder::removeTree( 
    bool /*allVersions*/, 
    libcmis::UnfileObjects::Type /*unfile*/, 
    bool /*continueOnError*/ ) 
{
    try
    {   
        getSession( )->httpDeleteRequest( GDRIVE_METADATA_LINK + getId( ) );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    
    // Nothing to return here
    return vector< string >( );
}

