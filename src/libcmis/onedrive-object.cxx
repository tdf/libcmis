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

#include "onedrive-allowable-actions.hxx"
#include "onedrive-object.hxx"
#include "onedrive-property.hxx"
#include "onedrive-repository.hxx"
#include "onedrive-utils.hxx"

using namespace std;
using namespace libcmis;

OneDriveObject::OneDriveObject( OneDriveSession* session ) :
    libcmis::Object( session )
{
}

OneDriveObject::OneDriveObject( OneDriveSession* session, Json json, string id, string name ) :
    libcmis::Object( session )
{
   initializeFromJson( json, id, name ); 
}

OneDriveObject::OneDriveObject( const OneDriveObject& copy ) :
    libcmis::Object( copy )
{
}

OneDriveObject& OneDriveObject::operator=( const OneDriveObject& copy )
{
    if ( this != &copy )
    {
        libcmis::Object::operator=( copy );
    }
    return *this;
}

void OneDriveObject::initializeFromJson ( Json json, string /*id*/, string /*name*/ )
{
    Json::JsonObject objs = json.getObjects( );
    Json::JsonObject::iterator it;
    bool isFolder = json["type"].toString( ) == "folder";
    for ( it = objs.begin( ); it != objs.end( ); ++it)
    {
        PropertyPtr property;
 
        property.reset( new OneDriveProperty( it->first, it->second ) );
        m_properties[ property->getPropertyType( )->getId()] = property;
        if ( it->first == "name" && !isFolder )
        {
            PropertyPtr contentStreamProp;
            contentStreamProp.reset( new OneDriveProperty( "cmis:contentStreamFileName", it->second ) );
            m_properties[ contentStreamProp->getPropertyType( )->getId()] = contentStreamProp;
        }
    }

    m_refreshTimestamp = time( NULL );
    m_allowableActions.reset( new OneDriveAllowableActions( isFolder ) );
}

OneDriveSession* OneDriveObject::getSession( )
{
    return dynamic_cast< OneDriveSession* > ( m_session );
}

void OneDriveObject::refreshImpl( Json json )
{
    m_typeDescription.reset( );
    m_properties.clear( );
    initializeFromJson( json );
}

void OneDriveObject::refresh( ) throw ( libcmis::Exception )
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

void OneDriveObject::remove( bool /*allVersions*/ ) throw ( libcmis::Exception )
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

string OneDriveObject::getUrl( )
{
    return getSession( )->getBindingUrl( ) + "/" + getId( );
}

string OneDriveObject::getUploadUrl( )
{
    return getUrl( ) + "/files";
}

vector< string> OneDriveObject::getMultiStringProperty( const string& propertyName )
{
    vector< string > values;
    PropertyPtrMap::const_iterator it = getProperties( ).find( string( propertyName ) );
    if ( it != getProperties( ).end( ) && it->second != NULL && !it->second->getStrings( ).empty( ) )
        values = it->second->getStrings( );
    return values; 
}

libcmis::ObjectPtr OneDriveObject::updateProperties(
        const PropertyPtrMap& properties ) throw ( libcmis::Exception )
{
    // Make Json object from properties
    Json json = OneDriveUtils::toOneDriveJson( properties );

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
    libcmis::ObjectPtr updated = getSession()->getObjectFromJson( jsonRes );

    if ( updated->getId( ) == getId( ) )
         refreshImpl( jsonRes );

    return updated;
}

void OneDriveObject::move( FolderPtr /*source*/, FolderPtr destination ) 
                                        throw ( libcmis::Exception )
{  
    Json destJson;
    Json destId( destination->getId( ).c_str( ) );
    destJson.add( "destination", destId );
    
    istringstream is( destJson.toString( ) );
    libcmis::HttpResponsePtr response;
    try 
    {   
        string url = getUrl( ) + "?method=MOVE";
        response = getSession( )->httpPostRequest( url, is, "application/json" );
    }
    catch ( const CurlException& e )
    {   
        throw e.getCmisException( );
    }
    string res = response->getStream( )->str( );
    Json jsonRes = Json::parse( res );

    refreshImpl( jsonRes );
}
