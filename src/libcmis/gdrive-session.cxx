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

#include "oauth2-handler.hxx"
#include "gdrive-session.hxx"
#include "gdrive-document.hxx"
#include "gdrive-folder.hxx"
#include "object-type.hxx"
#include "gdrive-repository.hxx"
#include "gdrive-object-type.hxx"
#include "gdrive-utils.hxx"

using std::string;
using std::istringstream;

GDriveSession::GDriveSession ( string baseUrl, 
                               string username, 
                               string password, 
                               libcmis::OAuth2DataPtr oauth2, 
                               bool verbose )
                                    throw ( libcmis::Exception ) :
    BaseSession( baseUrl, string(), username, password, false, 
                 libcmis::OAuth2DataPtr(), verbose )

{
    if ( oauth2 && oauth2->isComplete( ) ){
        setOAuth2Data( oauth2 );
        // Add the repository
        m_repositories.push_back( getRepository( ) );
    }
}

GDriveSession::GDriveSession( const GDriveSession& copy ) :
    BaseSession( copy )
{
}

GDriveSession::GDriveSession() :
    BaseSession()
{
}

GDriveSession::~GDriveSession()
{
}

libcmis::RepositoryPtr GDriveSession::getRepository( ) 
    throw ( libcmis::Exception )
{
    // Return a dummy repository since GDrive doesn't have that notion
    libcmis::RepositoryPtr repo( new GdriveRepository( ) );
    return repo;
}

libcmis::ObjectPtr GDriveSession::getObject( string objectId )
    throw ( libcmis::Exception )
{
    // Run the http request to get the properties definition
    string res;
    string objectLink = m_bindingUrl + "/files/" + objectId;
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
    throw ( libcmis::Exception )
{
    // Google Drive doesn't support get Object by path.
    // The path here serve as an object ID.
    if ( path == "/" )
        path = "root";
    return getObject( path );
}

libcmis::ObjectTypePtr GDriveSession::getType( string id ) 
    throw ( libcmis::Exception )
{
    libcmis::ObjectTypePtr type( new GdriveObjectType( id ) );
    return type;
}
