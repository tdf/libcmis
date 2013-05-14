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
    throw ( libcmis::Exception )
{
    vector< libcmis::ObjectPtr > children;
    
    string childrenUrl = getSession( )->getBaseUrl() + "/files/" + getId( ) + 
                                                                "/children";    
    string res;
    try
    {
        res = getSession( )->httpGetRequest( childrenUrl )->getStream()->str();
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    Json jsonRes = Json::parse( res );
    Json::JsonVector objs = jsonRes["items"].getList( );
    // Create children objects from Json objects
    for(unsigned int i = 0; i < objs.size(); i++)
    {   
        string childId = objs[i]["id"].toString( );
        libcmis::ObjectPtr child = getSession( )->getObject( childId );
        children.push_back( child );
    }   
    
    return children;
}

void addStringProperty( PropertyPtrMap& properties,
                string key,
                string value )
{
    Json json = Json::parse( "\""+ value + "\"" );
    PropertyPtr property( new GDriveProperty( key, json ) );
    properties[ key ] = property;
}

string GDriveFolder::uploadProperties( const PropertyPtrMap& properties )
{
    // URL for uploading meta data
    string metaUrl = getSession()->getBaseUrl() + "/files/";
    
    PropertyPtrMap addedProperties = properties;
    
    // Add parent to the properties
    string parentStr = "[{\"id\":\""+ getId( ) + "\"}]";
    addStringProperty( addedProperties, "parents", parentStr );

    // TODO doesn't work correctly at the moment.
    Json uploadJson( addedProperties );
    
    std::istringstream is( uploadJson.toString( ) );
    string title = uploadJson["title"].toString();
    string parents = uploadJson["parents"].toString();
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
        throw( libcmis::Exception )
{
    PropertyPtrMap addedProperties = properties;
    // GDrive folder is a file with a different mime type.
    string mimeType = "application/vnd.google-apps.folder";
    addStringProperty( addedProperties, "mimeType", mimeType);
    
    // Upload the properties definition
    string response = uploadProperties( (const PropertyPtrMap&) addedProperties);
    Json jsonRes = Json::parse( response );
    libcmis::FolderPtr folderPtr( new GDriveFolder( getSession( ), jsonRes ) );

    return folderPtr;
}
    

libcmis::DocumentPtr GDriveFolder::createDocument( 
    const PropertyPtrMap& properties, 
    boost::shared_ptr< ostream > os, 
    string contentType, string fileName ) 
    throw ( libcmis::Exception )
{    
    static const string uploadUrl = 
        "https://www.googleapis.com/upload/drive/v2/files/";
    if ( !os.get( ) )
        throw libcmis::Exception( "Missing stream" );
    
    PropertyPtrMap addedProperties = properties;
    
    if ( !fileName.empty( ) )
        addStringProperty( addedProperties, "title", fileName );
    
    // Upload metadata    
    uploadProperties( (const PropertyPtrMap&) addedProperties );
    
    // Upload stream
    boost::shared_ptr< istream> is ( new istream ( os->rdbuf( ) ) );
    string response;
    try
    {
        response = getSession()->httpPostRequest( uploadUrl, *is, contentType )
                                    ->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    long httpStatus = getSession( )->getHttpStatus( );
    if ( httpStatus < 200 || httpStatus >= 300 )
        throw libcmis::Exception( "Document content wasn't set for"
                "some reason" );
    Json jsonResponse = Json::parse( response );
    DocumentPtr documentPtr( new GDriveDocument( getSession( ), jsonResponse ) );
    return documentPtr;
}

vector< string > GDriveFolder::removeTree( 
    bool /*allVersions*/, 
    libcmis::UnfileObjects::Type /*unfile*/, 
    bool /*continueOnError*/ ) 
        throw ( libcmis::Exception )
{
    //TODO implementation
    vector< string > result;
    return result;
}

