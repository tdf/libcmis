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
#include "gdrive-utils.hxx"

using namespace std;
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
    string convertedKey = GdriveUtils::toCmisKey( key );
    propertyType->setId( convertedKey );
    propertyType->setLocalName( convertedKey );
    propertyType->setLocalNamespace( convertedKey );
    propertyType->setQueryName( convertedKey );
    propertyType->setDisplayName( convertedKey );
    propertyType->setTypeFromJsonType( json.getStrType( ) );
    propertyType->setUpdatable( GdriveUtils::checkUpdatable( key ) );
    setPropertyType( propertyType );    
    
    vector< string > values;
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

