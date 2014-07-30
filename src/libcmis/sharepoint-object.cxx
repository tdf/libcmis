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

#include "sharepoint-allowable-actions.hxx"
#include "sharepoint-object.hxx"
#include "sharepoint-property.hxx"
#include "sharepoint-repository.hxx"
#include "sharepoint-utils.hxx"

using namespace std;
using namespace libcmis;

SharePointObject::SharePointObject( SharePointSession* session ) :
    libcmis::Object( session )
{
}

SharePointObject::SharePointObject( SharePointSession* session, Json json, string parentId, string name ) :
    libcmis::Object( session )
{
   initializeFromJson( json, parentId, name ); 
}

SharePointObject::SharePointObject( const SharePointObject& copy ) :
    libcmis::Object( copy )
{
}

SharePointObject& SharePointObject::operator=( const SharePointObject& copy )
{
    if ( this != &copy )
    {
        libcmis::Object::operator=( copy );
    }
    return *this;
}

void SharePointObject::initializeFromJson ( Json json, string parentId, string /*name*/ )
{
    if ( !json["d"].toString( ).empty( ) ) {
        // Basic GET requests receive the data inside a "d" object, 
        // but child listing doesn't, so this unifies the representation
        json = json["d"];
    }
    Json::JsonObject objs = json.getObjects( );
    Json::JsonObject::iterator it;
    PropertyPtr property;
    bool isFolder = json["__metadata"]["type"].toString( ) == "SP.Folder";
    for ( it = objs.begin( ); it != objs.end( ); ++it)
    {
        property.reset( new SharePointProperty( it->first, it->second ) );
        m_properties[ property->getPropertyType( )->getId()] = property;
        if ( it->first == "Name" && !isFolder )
        {
            property.reset( new SharePointProperty( "cmis:contentStreamFileName", it->second ) );
            m_properties[ property->getPropertyType( )->getId()] = property;
        }
    }

    if ( !parentId.empty( ) )
    {
        // ParentId is not provided in the response
        property.reset( new SharePointProperty( "cmis:parentId", Json( parentId.c_str( ) ) ) );
        m_properties[ property->getPropertyType( )->getId()] = property;
    }

    if ( !isFolder )
    {
        Json authorJson = getSession( )->getJsonFromUrl( getStringProperty( "Author" ) );
        property.reset( new SharePointProperty( "cmis:createdBy", 
                    authorJson["d"]["Title"] ) );
        m_properties[ property->getPropertyType( )->getId( ) ] = property;
    }
    else
    {
        // we need to get the creation and lastUpdate time which aren't
        // provided in the response
        Json propJson = getSession( )->getJsonFromUrl( getStringProperty( "Properties" ) );
        property.reset( new SharePointProperty( "cmis:creationDate", 
                    propJson["d"]["vti_x005f_timecreated"] ) );
        m_properties[ property->getPropertyType( )->getId( ) ] = property;

        property.reset( new SharePointProperty( "cmis:lastModificationDate", 
                    propJson["d"]["vti_x005f_timelastmodified"] ) );
        m_properties[ property->getPropertyType( )->getId( ) ] = property;
    }

    m_refreshTimestamp = time( NULL );
    m_allowableActions.reset( new SharePointAllowableActions( isFolder ) );
}

SharePointSession* SharePointObject::getSession( )
{
    return dynamic_cast< SharePointSession* > ( m_session );
}

void SharePointObject::refreshImpl( Json json )
{
    m_typeDescription.reset( );
    m_properties.clear( );
    initializeFromJson( json );
}

void SharePointObject::refresh( ) throw ( libcmis::Exception )
{
    string res;
    try
    {
        res  = getSession( )->httpGetRequest( getId( ) )->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    Json json = Json::parse( res );
    refreshImpl( json );
}

void SharePointObject::remove( bool /*allVersions*/ ) throw ( libcmis::Exception )
{
    try
    {
        getSession( )->httpDeleteRequest( getId( ) );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
}

vector< string> SharePointObject::getMultiStringProperty( const string& propertyName )
{
    vector< string > values;
    PropertyPtrMap::const_iterator it = getProperties( ).find( string( propertyName ) );
    if ( it != getProperties( ).end( ) && it->second != NULL && !it->second->getStrings( ).empty( ) )
        values = it->second->getStrings( );
    return values; 
}

libcmis::ObjectPtr SharePointObject::updateProperties(
        const PropertyPtrMap& /*properties*/ ) throw ( libcmis::Exception )
{
    // there are no updateable properties so just return the same object
    libcmis::ObjectPtr updated = getSession( )->getObject( getId( ) );
    return updated;
}

void SharePointObject::move( FolderPtr /*source*/, FolderPtr destination ) 
                                        throw ( libcmis::Exception )
{  
    if ( !getStringProperty( "cmis:checkinComment" ).empty( ) )
    {
        // only documents can be moved and only documents have this property
        string url = getId( ) + "/moveto(newurl='";
        url +=  libcmis::escape( destination->getStringProperty( "ServerRelativeUrl" ) );
        url += "/" + getStringProperty( "cmis:name" ) + "'";
        // overwrite flag
        url += ",flags=1)"; 
        istringstream is( "empty" );
        try 
        {   
            getSession( )->httpPostRequest( url, is, "" );
        }
        catch ( const CurlException& e )
        {   
            throw e.getCmisException( );
        }
        refresh( );
    }
}
