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
 * Copyright (C) 2014 Mihai Varga <mihai.mv13@gmail.com>
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

#include "onedrive-session.hxx"

#include "oauth2-handler.hxx"
#include "onedrive-object-type.hxx"
#include "onedrive-document.hxx"
#include "onedrive-folder.hxx"
#include "onedrive-object.hxx"
#include "onedrive-repository.hxx"

using namespace std;

OneDriveSession::OneDriveSession ( string baseUrl,
                               string username,
                               string password,
                               libcmis::OAuth2DataPtr oauth2,
                               bool verbose ) :
    BaseSession( baseUrl, string(), username, password, false,
                 libcmis::OAuth2DataPtr(), verbose )

{
    // Add the dummy repository
    m_repositories.push_back( getRepository( ) );

    if ( oauth2 && oauth2->isComplete( ) ){
        setOAuth2Data( oauth2 );
    }
}

OneDriveSession::OneDriveSession() :
    BaseSession()
{
}

OneDriveSession::~OneDriveSession()
{
}

libcmis::RepositoryPtr OneDriveSession::getRepository( )
{
    // Return a dummy repository since OneDrive doesn't have that notion
    libcmis::RepositoryPtr repo( new OneDriveRepository( ) );
    return repo;
}

libcmis::ObjectPtr OneDriveSession::getObject( string objectId )
{
    // Run the http request to get the properties definition
    string res;
    string objectLink = m_bindingUrl + "/me/drive/items/" + objectId;
    if (objectId == getRootId())
        objectLink = m_bindingUrl + objectId;
    try
    {
        res = httpGetRequest( objectLink )->getStream()->str();
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    Json jsonRes = Json::parse( res );
    return getObjectFromJson( jsonRes );
}

libcmis::ObjectPtr OneDriveSession::getObjectFromJson( Json& jsonRes ) 
{
    libcmis::ObjectPtr object;
    if ( jsonRes["folder"].toString() != "" )
    {
        object.reset( new OneDriveFolder( this, jsonRes ) );
    }
    else if ( jsonRes["file"].toString() != "" )
    {
        object.reset( new OneDriveDocument( this, jsonRes ) );
    }
    else
    {
        object.reset( new OneDriveObject( this, jsonRes ) );
    }
    return object;
}

libcmis::ObjectPtr OneDriveSession::getObjectByPath( string path )
{
    string res;
    string objectQuery = m_bindingUrl + "/me/drive/root:" + libcmis::escape( path );
    try
    {
        res = httpGetRequest( objectQuery )->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw libcmis::Exception( "No file could be found for path " + path + ": " + e.what() );
    }
    Json jsonRes = Json::parse( res );
    return getObjectFromJson( jsonRes );
}

bool OneDriveSession::isAPathMatch( Json objectJson, string path )
{
    string parentId = objectJson["parent_id"].toString( );
    string objectName = objectJson["name"].toString( );
    size_t pos = path.rfind("/");
    string pathName = path.substr( pos + 1, path.size( ) );
    string truncatedPath = path.substr( 0, pos );

    if ( truncatedPath.empty( ) && parentId == "null" && objectName == pathName )
    {
        // match
        return true;
    }
    if ( truncatedPath.empty( ) || parentId == "null" || objectName != pathName )
    {
        return false;
    }

    string res;
    string parentUrl = m_bindingUrl + "/" + parentId;
    try
    {
        res = httpGetRequest( parentUrl )->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    Json jsonRes = Json::parse( res );
    return isAPathMatch( jsonRes, truncatedPath );
}

libcmis::ObjectTypePtr OneDriveSession::getType( string id )
{
    libcmis::ObjectTypePtr type( new OneDriveObjectType( id ) );
    return type;
}

vector< libcmis::ObjectTypePtr > OneDriveSession::getBaseTypes( )
{
    vector< libcmis::ObjectTypePtr > types;
    return types;
}
