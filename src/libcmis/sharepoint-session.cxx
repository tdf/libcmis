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

#include "sharepoint-object.hxx"
#include "sharepoint-repository.hxx"
#include "sharepoint-session.hxx"

using namespace std;

SharePointSession::SharePointSession ( string baseUrl,
                               string username,
                               string password,
                               bool verbose )
                                    throw ( libcmis::Exception ) :
    BaseSession( baseUrl, string(), username, password, false,
                 libcmis::OAuth2DataPtr(), verbose )

{
    setNTLMAuthentication( true );

    libcmis::HttpResponsePtr response;
    try
    {
        response = httpGetRequest( baseUrl + "/currentuser" );
    }
    catch ( const CurlException& e )
    {
        // It's not SharePoint or wrong username/passwd provided
        throw e.getCmisException( );
    }

    // Add the dummy repository
    m_repositories.push_back( getRepository( ) );
}

SharePointSession::SharePointSession( const SharePointSession& copy ) :
    BaseSession( copy )
{
}

SharePointSession::SharePointSession() :
    BaseSession()
{
}

SharePointSession::~SharePointSession()
{
}

libcmis::RepositoryPtr SharePointSession::getRepository( )
    throw ( libcmis::Exception )
{
    // Return a dummy repository since SharePoint doesn't have that notion
    libcmis::RepositoryPtr repo( new SharePointRepository( ) );
    return repo;
}

libcmis::ObjectPtr SharePointSession::getObject( string objectId )
    throw ( libcmis::Exception )
{
    string res;
    string objectLink = m_bindingUrl + "/" + objectId;
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

libcmis::ObjectPtr SharePointSession::getObjectFromJson( Json& jsonRes ) 
            throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object;
    string kind = jsonRes["d"]["__metadata"]["type"].toString( );
    // only SharePointObject available for now
    if ( kind == "SP.Folder" )
    {
        object.reset( new SharePointObject( this, jsonRes ) );
    }
    else if ( kind == "SP.File" )
    {
        object.reset( new SharePointObject( this, jsonRes ) );
    }
    else
    {
        object.reset( new SharePointObject( this, jsonRes ) );
    }
    return object;
}

libcmis::ObjectPtr SharePointSession::getObjectByPath( string path )
    throw ( libcmis::Exception )
{
    return getObject( path );
}

libcmis::ObjectTypePtr SharePointSession::getType( string /*id*/ )
    throw ( libcmis::Exception )
{
    libcmis::ObjectTypePtr type;
    return type;
}

vector< libcmis::ObjectTypePtr > SharePointSession::getBaseTypes( )
    throw ( libcmis::Exception )
{
    vector< libcmis::ObjectTypePtr > types;
    return types;
}

Json SharePointSession::getJsonFromUrl( string url )
    throw ( libcmis::Exception )
{
    string response;
    try
    {
        response = httpGetRequest( url )->getStream()->str();
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    return Json::parse( response );
}

