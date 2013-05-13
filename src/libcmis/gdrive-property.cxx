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

#include "gdrive-property.hxx"
#include "property-type.hxx"

using std::string;
using namespace libcmis;

GDriveProperty::GDriveProperty( )
{
}

GDriveProperty::~GDriveProperty( )
{
}

GDriveProperty::GDriveProperty( const string& key, Json json ):
    Property( )
{
    PropertyTypePtr propertyType( new PropertyType( ) );
    string convertedKey = convertToCmisKey( key );
    propertyType->setId( convertedKey );
    propertyType->setLocalName( convertedKey );
    propertyType->setLocalNamespace( convertedKey );
    propertyType->setQueryName( convertedKey );
    propertyType->setDisplayName( convertedKey );
    propertyType->setTypeFromJsonType( json.getDataType( ) );
   
    // for some reason fileSize is recognized as string
    if ( key=="fileSize" ) 
        propertyType->setType( PropertyType::Integer );

    setPropertyType( propertyType );    
    vector< string > values;
    
    // take the parentId
    if (key == "parents" )
        values.push_back( json[0]["id"].toString( ) );
    else 
        values.push_back( json.toString( ) );

    setValues( values );
}

GDriveProperty::GDriveProperty( const GDriveProperty& copy ) :
     libcmis::Property( copy )
{
}
 
GDriveProperty& GDriveProperty::operator=( const GDriveProperty& copy )
{
    if ( this != &copy )
    {
        libcmis::Property::operator=( copy );
    }
    return *this;
}

string GDriveProperty::convertToCmisKey( const string& key )
{
    string convertedKey;
    if ( key == "id")
        convertedKey = "cmis:objectId";
    else if ( key == "ownerNames" )
        convertedKey = "cmis:createdBy";
    else if ( key == "createdDate" )
        convertedKey = "cmis:creationDate";
    else if ( key == "lastModifyingUserName" )
        convertedKey = "cmis:lastModifiedBy";
    else if ( key == "modifiedDate" )
        convertedKey = "cmis:lastModificationDate";
    else if ( key == "title" )
        convertedKey = "cmis:contentStreamFileName";
    else if ( key == "mimeType" )
        convertedKey = "cmis:contentStreamMimeType";
    else if ( key == "fileSize" )
        convertedKey = "cmis:contentStreamLength";
    else if ( key == "editable" )
        convertedKey = "cmis:isImmutable";
    else if ( key == "parents" )
        convertedKey = "cmis:parentId";
    else convertedKey = key;
    return convertedKey;
}

string GDriveProperty::convertToGDriveKey( const string& key )
{
    string convertedKey;
    if ( key == "cmis:objectId")
        convertedKey = "id";
    else if ( key == "cmis:createBy" )
        convertedKey = "ownerNames";
    else if ( key == "cmis:creationDate" )
        convertedKey = "createdDate";
    else if ( key == "cmis:lastModifiedBy" )
        convertedKey = "lastModifyingUserNam";
    else if ( key == "cmis:lastModificationDate" )
        convertedKey = "modifiedDate";
    else if ( key == "cmis:contentStreamFileName" )
        convertedKey = "title";
    else if ( key == "cmis:contentStreamMimeType" )
        convertedKey = "mimeType";
    else if ( key == "cmis:contentStreamLength" )
        convertedKey = "fileSize";
    else if ( key == "cmis:isImmutable" )
        convertedKey = "editable";
    else if ( key == "cmis:parentId" )
        convertedKey = "parents";
    else convertedKey = key;
    return convertedKey;
}

