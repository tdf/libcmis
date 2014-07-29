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

#include "sharepoint-document.hxx"
#include "sharepoint-folder.hxx"
#include "sharepoint-session.hxx"
#include "sharepoint-property.hxx"
#include "sharepoint-utils.hxx"

using namespace std;
using namespace libcmis;

SharePointFolder::SharePointFolder( SharePointSession* session ):
    libcmis::Object( session ),
    libcmis::Folder( session ),
    SharePointObject( session )
{
}

SharePointFolder::SharePointFolder( SharePointSession* session, Json json, string parentId ):
    libcmis::Object( session ),
    libcmis::Folder( session ),
    SharePointObject( session, json, parentId )
{
}

SharePointFolder::~SharePointFolder( )
{
}

string SharePointFolder::getParentId( )
{
    string parentId = getStringProperty( "cmis:parentId" );
    if ( parentId.empty( ) )
    {
        string parentUrl = getStringProperty( "ParentFolder" );
        string res;
        try
        {
            res = getSession( )->httpGetRequest( parentUrl )->getStream( )->str( );
        }
        catch ( const CurlException& e )
        {
            throw e.getCmisException( );
        }

        Json jsonRes = Json::parse( res );
        parentId = jsonRes["d"]["__metadata"]["uri"].toString( );
        PropertyPtr property;
        property.reset( new SharePointProperty( "cmis:parentId",
                        Json( parentId.c_str( ) ) ) );
        m_properties[ property->getPropertyType( )->getId()] = property;
    }
    return parentId;
}

vector< libcmis::ObjectPtr > SharePointFolder::getChildren( ) 
    throw ( libcmis::Exception )
{
    vector< libcmis::ObjectPtr > children;
    string filesUrl = getStringProperty( "Files" );
    string foldersUrl = getStringProperty( "Folders" );

    Json::JsonVector objs = getChildrenImpl( filesUrl );
    Json::JsonVector folders = getChildrenImpl( foldersUrl );
    objs.insert( objs.begin( ), folders.begin( ), folders.end( ) );

    for ( unsigned int i = 0; i < objs.size( ); i++)
    {
        children.push_back( getSession( )->getObjectFromJson( objs[i], getId( ) ) );
    }
    return children;
}

Json::JsonVector SharePointFolder::getChildrenImpl( string url )
    throw ( libcmis::Exception )
{
    string res;
    try
    {
        res = getSession( )->httpGetRequest( url )->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    Json jsonRes = Json::parse( res );
    Json::JsonVector objs = jsonRes["d"]["results"].getList( );
    return objs;
}

libcmis::FolderPtr SharePointFolder::createFolder( const PropertyPtrMap& properties ) 
    throw( libcmis::Exception )
{
    string folderName;
    for ( PropertyPtrMap::const_iterator it = properties.begin() ; 
            it != properties.end() ; ++it )
    {
       if ( it->first == "cmis:name" )
       {
           folderName = it->second->toString( );
       }
    }
    // bindingUrl/folders/add('/path/to/folder')
    string relativeUrl;
    if ( getStringProperty( "ServerRelativeUrl" ) == "/" )
    {
        relativeUrl = "/" + folderName;
    }
    else
    {
        relativeUrl = getStringProperty( "ServerRelativeUrl" ) + "/" + folderName;
    }
    relativeUrl = libcmis::escape( relativeUrl );
    string folderUrl = getSession( )->getBindingUrl( );
    folderUrl += "/folders/add('" + relativeUrl + "')";

    istringstream is( "empty" );
    string res;
    try 
    {   
        res = getSession( )->httpPostRequest( folderUrl, is, "" )->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {   
        throw e.getCmisException( );
    }
    Json jsonRes = Json::parse( res );

    libcmis::FolderPtr newFolder( new SharePointFolder( getSession( ), jsonRes, getId( ) ) );
    return newFolder;
}

libcmis::DocumentPtr SharePointFolder::createDocument( const PropertyPtrMap& properties, 
                                                       boost::shared_ptr< ostream > os, 
                                                       string contentType, 
                                                       string fileName ) 
    throw ( libcmis::Exception )
{    
    if ( !os.get( ) )
        throw libcmis::Exception( "Missing stream" );

    if ( fileName.empty( ) )
    {
        for ( PropertyPtrMap::const_iterator it = properties.begin() ; 
                it != properties.end() ; ++it )
        {
           if ( it->first == "cmis:name" ||
                it->first == "cmis:contentStreamFileName" )
           {
               fileName = it->second->toString( );
           }
        }
    }
    fileName = libcmis::escape( fileName );
    string url = getId( ) + "/files/add(overwrite=true,";
    url += "url='" + fileName + "')";

    // Upload stream
    boost::shared_ptr< istream> is ( new istream ( os->rdbuf( ) ) );
    string res;
    try
    {
        res = getSession( )->httpPostRequest( url, *is, contentType )->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    Json jsonRes = Json::parse( res );
    DocumentPtr document( new SharePointDocument( getSession( ), jsonRes, getId( ) ) );
    return document;
}

vector< string > SharePointFolder::removeTree( bool /*allVersions*/, 
                                               libcmis::UnfileObjects::Type /*unfile*/, 
                                               bool /*continueOnError*/ ) 
    throw ( libcmis::Exception )
{
    remove( ); 
    // Nothing to return here
    return vector< string >( );
}
