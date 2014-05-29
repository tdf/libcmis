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

#include "onedrive-utils.hxx"
#include "json-utils.hxx"
#include "xml-utils.hxx"

using namespace std;
using libcmis::PropertyPtrMap;

string OneDriveUtils::toCmisKey( const string& key )
{
    string convertedKey;
    if ( key == "id")
        convertedKey = "cmis:objectId";
    else if ( key == "from" )
        convertedKey = "cmis:createdBy";
    else if ( key == "description" )
        convertedKey = "cmis:description";
    else if ( key == "created_time" )
        convertedKey = "cmis:creationDate";
    else if ( key == "updated_time" )
        convertedKey = "cmis:lastModificationDate";
    else if ( key == "name" )
        convertedKey = "cmis:contentStreamFileName";
    else if ( key == "size" )
        convertedKey = "cmis:contentStreamLength";
    else if ( key == "parent_id" )
        convertedKey = "cmis:parentId";
    else convertedKey = key;
    return convertedKey;
}

string OneDriveUtils::toOneDriveKey( const string& key )
{
    string convertedKey;
    if ( key == "cmis:objectId")
        convertedKey = "id";
    else if ( key == "cmis:createdBy" )
        convertedKey = "from";
    else if ( key == "cmis:creationDate" )
        convertedKey = "created_time";
    else if ( key == "cmis:description" )
        convertedKey = "description";
    else if ( key == "cmis:lastModificationDate" )
        convertedKey = "updated_time";
    else if ( key == "cmis:contentStreamFileName" )
        convertedKey = "name";
    else if ( key == "cmis:name" )
        convertedKey = "name";
    else if ( key == "cmis:contentStreamLength" )
        convertedKey = "file_size";
    else if ( key == "cmis:parentId" )
        convertedKey = "parent_id";
    else convertedKey = key;
    return convertedKey;
}

bool OneDriveUtils::checkUpdatable( const std::string& key)
{
    bool updatable = ( key == "name" ||
                       key == "description" );
    return updatable;
}

bool OneDriveUtils::checkMultiValued( const string& key )
{
    bool bMultiValued = ( key == "from" ||
                          key == "shared_with" );
    return bMultiValued;    
}

vector< string > OneDriveUtils::parseOneDriveProperty( string key, Json json )
{
    vector< string > values;
    if ( key == "from" )
    {
        string ownerName = json["name"].toString( );
        values.push_back( ownerName);
    }
    else if ( key == "shared_with" )
    {
        string sharedWith = json["access"].toString( );
        values.push_back( sharedWith );
    }
    else values.push_back( json.toString( ) );
    return values;
}
