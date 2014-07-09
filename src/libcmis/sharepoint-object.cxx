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

SharePointObject::SharePointObject( SharePointSession* session, Json json, string id, string name ) :
    libcmis::Object( session )
{
   initializeFromJson( json, id, name ); 
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

void SharePointObject::initializeFromJson ( Json json, string /*id*/, string /*name*/ )
{
    Json::JsonObject objs = json["d"].getObjects( );
    Json::JsonObject::iterator it;
    bool isFolder = json["d"]["__metadata"]["type"].toString( ) == "SP.Folder";
    for ( it = objs.begin( ); it != objs.end( ); ++it)
    {
        PropertyPtr property;
 
        property.reset( new SharePointProperty( it->first, it->second ) );
        m_properties[ property->getPropertyType( )->getId()] = property;
        if ( it->first == "Name" && !isFolder )
        {
            PropertyPtr contentStreamProp;
            contentStreamProp.reset( new SharePointProperty( "cmis:contentStreamFileName", it->second ) );
            m_properties[ contentStreamProp->getPropertyType( )->getId()] = contentStreamProp;
        }
    }

    if ( !isFolder )
    {
        Json authorJson = getSession( )->getJsonFromUrl( getStringProperty( "Author" ) );
        PropertyPtr property;
        property.reset( new SharePointProperty( "cmis:createdBy", 
                    authorJson["d"]["Title"] ) );
        m_properties[ property->getPropertyType( )->getId( ) ] = property;
    }


    m_refreshTimestamp = time( NULL );
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
        res  = getSession()->httpGetRequest( getUrl( ) )->getStream( )->str( );
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
        getSession( )->httpDeleteRequest( getUrl( ) );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
}

string SharePointObject::getUrl( )
{
    return getSession( )->getBindingUrl( ) + "/" + getId( );
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
    libcmis::ObjectPtr updated;
    return updated;
}

void SharePointObject::move( FolderPtr /*source*/, FolderPtr /*destination*/ ) 
                                        throw ( libcmis::Exception )
{  
}
