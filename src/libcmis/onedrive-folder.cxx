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

#include "onedrive-document.hxx"
#include "onedrive-folder.hxx"
#include "onedrive-session.hxx"
#include "onedrive-property.hxx"
#include "onedrive-utils.hxx"

using namespace std;
using namespace libcmis;

OneDriveFolder::OneDriveFolder( OneDriveSession* session ):
    libcmis::Object( session ),
    libcmis::Folder( session ),
    OneDriveObject( session )
{
}

OneDriveFolder::OneDriveFolder( OneDriveSession* session, Json json ):
    libcmis::Object( session ),
    libcmis::Folder( session ),
    OneDriveObject( session, json )
{
}

OneDriveFolder::~OneDriveFolder( )
{
}

vector< libcmis::ObjectPtr > OneDriveFolder::getChildren( ) 
    throw ( libcmis::Exception )
{
    vector< libcmis::ObjectPtr > children;
    string query = getSession( )->getBindingUrl( ) + "/" + getId( ) + "/files";

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
    Json::JsonVector objs = jsonRes["data"].getList( );
    
    // Create children objects from Json objects
    for(unsigned int i = 0; i < objs.size(); i++)
    {   
        children.push_back( getSession( )->getObjectFromJson( objs[i] ) );
    }   
    
    return children;
}

libcmis::FolderPtr OneDriveFolder::createFolder( 
    const PropertyPtrMap& properties ) 
        throw( libcmis::Exception )
{
    Json propsJson = OneDriveUtils::toOneDriveJson( properties );

    string uploadUrl = getSession( )->getBindingUrl( ) + "/" + getId( );
    
    std::istringstream is( propsJson.toString( ) );
    string response;
    try
    {
        response = getSession()->httpPostRequest( uploadUrl, is, "application/json" )
                                    ->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    Json jsonRes = Json::parse( response );
    libcmis::FolderPtr folderPtr( new OneDriveFolder( getSession( ), jsonRes ) );

    return folderPtr;
}

libcmis::DocumentPtr OneDriveFolder::createDocument( 
    const PropertyPtrMap& /*properties*/, 
    boost::shared_ptr< ostream > /*os*/, 
    string /*contentType*/, string /*fileName*/ ) 
    throw ( libcmis::Exception )
{    
    ::boost::shared_ptr<libcmis::Document> doc;
    return doc;
}

vector< string > OneDriveFolder::removeTree( 
    bool /*allVersions*/, 
    libcmis::UnfileObjects::Type /*unfile*/, 
    bool /*continueOnError*/ ) 
        throw ( libcmis::Exception )
{
    remove( ); 
    // Nothing to return here
    return vector< string >( );
}
