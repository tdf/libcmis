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

#include "gdrive-session.hxx"

#include <libcmis/object-type.hxx>

#include "oauth2-handler.hxx"
#include "gdrive-document.hxx"
#include "gdrive-folder.hxx"
#include "gdrive-repository.hxx"
#include "gdrive-object-type.hxx"
#include "gdrive-utils.hxx"

using namespace std;

GDriveSession::GDriveSession ( string baseUrl,
                               string username,
                               string password,
                               libcmis::OAuth2DataPtr oauth2,
                               bool verbose ) :
    BaseSession( baseUrl, string(), username, password, false,
                 libcmis::OAuth2DataPtr(), verbose )

{
    // Add the dummy repository, even if we don't have OAuth2
    m_repositories.push_back( getRepository( ) );

    if ( oauth2 && oauth2->isComplete( ) ){
        setOAuth2Data( oauth2 );
    }
}

GDriveSession::GDriveSession() :
    BaseSession()
{
}

GDriveSession::~GDriveSession()
{
}


void GDriveSession::setOAuth2Data( libcmis::OAuth2DataPtr oauth2 )
{
    m_oauth2Handler = new OAuth2Handler( this, oauth2 );
    m_oauth2Handler->setOAuth2Parser( OAuth2Providers::getOAuth2Parser( getBindingUrl( ) ) );

    oauth2Authenticate( );
}

void GDriveSession::oauth2Authenticate()
{
    // treat the supplied password as refresh token
    if (!m_password.empty())
    {
        try
        {
            m_inOAuth2Authentication = true;

            m_oauth2Handler->setRefreshToken(m_password);
            // Try to get new access tokens using the stored refreshtoken
            m_oauth2Handler->refresh();
            m_inOAuth2Authentication = false;
        }
        catch (const CurlException &e)
        {
            m_inOAuth2Authentication = false;
            // refresh token expired or invalid, trigger initial auth (that in turn will hit the fallback with copy'n'paste method)
            BaseSession::oauth2Authenticate();
        }
    }
    else
    {
        BaseSession::oauth2Authenticate();
    }
}

string GDriveSession::getRefreshToken() {
    return HttpSession::getRefreshToken();
}

libcmis::RepositoryPtr GDriveSession::getRepository( )
{
    // Return a dummy repository since GDrive doesn't have that notion
    libcmis::RepositoryPtr repo( new GdriveRepository( ) );
    return repo;
}

bool GDriveSession::setRepository( std::string )
{
    return true;
}

libcmis::ObjectPtr GDriveSession::getObject( string objectId )
{
    if(objectId == "root") {
        return getRootFolder();
    }
    // Run the http request to get the properties definition
    string res;
    string objectLink = GDRIVE_METADATA_LINK + objectId +
         "?fields=kind,id,name,parents,mimeType,createdTime,modifiedTime,thumbnailLink,size";
    try
    {
        res = httpGetRequest( objectLink )->getStream()->str();
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    Json jsonRes = Json::parse( res );

    // If we have a folder, then convert the object
    // into a GDriveFolder otherwise, convert it
    // into a GDriveDocument
    libcmis::ObjectPtr object;
    string kind = jsonRes["kind"].toString( );
    if ( kind == "drive#file" )
    {
        string mimeType = jsonRes["mimeType"].toString( );

        // Folder is a file with a special mimeType
        if ( mimeType == GDRIVE_FOLDER_MIME_TYPE )
            object.reset( new GDriveFolder( this, jsonRes ) );
        else
            object.reset( new GDriveDocument( this, jsonRes ) );
    } else if ( kind == "drive#revision" ) // A revision is a document too
    {
        object.reset( new GDriveDocument( this, jsonRes ) );
    }
    else // not a folder nor file, maybe a permission or changes,...
        object.reset( new GDriveObject( this, jsonRes ) );

    return object;
}

libcmis::ObjectPtr GDriveSession::getObjectByPath( string path )
{
    size_t pos = 0;
    size_t endpos = 0;
    string objectId;
    libcmis::ObjectPtr object;

    do
    {
        endpos = path.find( "/", pos );
        size_t len = path.length( ) - pos;
        if ( endpos != string::npos )
            len = endpos - pos;

        string segment = path.substr( pos, len );
        if ( segment.empty( ) )
        {
            // Root case or ignore double slashes
            if ( pos == 0 )
                objectId = "root";
            else
                continue;
        }
        else
        {
            // Normal child case
            // Ask for the ID of the child if there is any
            // somewhat flawed as names are not necessarily unique in GDrive...
            string query = libcmis::escape("'" + objectId + "' in parents and trashed = false and name='" + segment + "'");

            string childIdUrl = m_bindingUrl + "/files/?q=" + query + "&fields=files(id)";

            string res;
            try
            {
                res = httpGetRequest( childIdUrl )->getStream()->str();
            }
            catch ( const CurlException& e )
            {
                throw e.getCmisException( );
            }
            Json jsonRes = Json::parse( res );

            // Did we get an id?
            Json::JsonVector items = jsonRes["files"].getList();
            if ( items.empty( ) )
                throw libcmis::Exception( "Object not found: " + path, "objectNotFound" );

            objectId = items[0]["id"].toString( );
            if ( objectId.empty( ) )
                throw libcmis::Exception( "Object not found: " + path, "objectNotFound" );
        }

        pos = endpos + 1;
    } while ( endpos != string::npos );

    return getObject( objectId );
}

libcmis::FolderPtr GDriveSession::getRootFolder()
{
    // permissions/scope with just drive.file don't allow to get it with the "root" alias/by its actual object-ID
    Json propsJson;

    // GDrive folder is a file with a different mime type.
    string mimeType = GDRIVE_FOLDER_MIME_TYPE;

    // Add mimetype to the propsJson
    Json jsonMimeType( mimeType.c_str( ) );
    propsJson.add( "mimeType", jsonMimeType );
    propsJson.add( "id", "root" );

    // Upload meta-datas
    propsJson.add("cmis:name", "VirtualRoot");

    libcmis::FolderPtr folderPtr( new GDriveFolder( this, propsJson ) );

    return folderPtr;
}

libcmis::ObjectTypePtr GDriveSession::getType( string id )
{
    libcmis::ObjectTypePtr type( new GdriveObjectType( id ) );
    return type;
}

vector< libcmis::ObjectTypePtr > GDriveSession::getBaseTypes( )
{
    vector< libcmis::ObjectTypePtr > types;
    // TODO Implement me
    return types;
}
