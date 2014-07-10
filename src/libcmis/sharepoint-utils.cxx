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

#include "sharepoint-utils.hxx"
#include "json-utils.hxx"
#include "xml-utils.hxx"

using namespace std;
using libcmis::PropertyPtrMap;

string SharePointUtils::toCmisKey( const string& key )
{
    string convertedKey;
    if ( key == "__metadata")
        convertedKey = "cmis:objectId";
    else if ( key == "CheckInComment" )
        convertedKey = "cmis:checkinComment";
    else if ( key == "TimeCreated" )
        convertedKey = "cmis:creationDate";
    else if ( key == "TileLastModified" )
        convertedKey = "cmis:lastModificationDate";
    else if ( key == "Name" )
        convertedKey = "cmis:name";
    else if ( key == "MajorVersion" )
        convertedKey = "cmis:versionLabel";
    else if ( key == "Length" )
        convertedKey = "cmis:contentStreamLength";
    else convertedKey = key;
    return convertedKey;
}

vector< string > SharePointUtils::parseSharePointProperty( string key, Json json )
{
    vector< string > values;
    if ( key == "__metadata" )
    {
        string id = json["uri"].toString( );
        size_t pos = id.find( "_api/Web/" );
        id = id.substr( pos + strlen( "_api/Web/" ) );
        values.push_back( id );
    }
    if ( key == "Author" ||
         key == "CheckedOutByUser" ||
         key == "ListItemAllFields" ||
         key == "LockedByUser" ||
         key == "ModifiedBy" ||
         key == "Versions" )
    {
        string propertyUri = json["__deferred"]["uri"].toString( );
        values.push_back( propertyUri );
    }
    else values.push_back( json.toString( ) );
    return values;
}

bool SharePointUtils::isSharePoint( string response )
{
    xmlDocPtr doc = xmlReadMemory( response.c_str( ), response.size( ), "noname.xml", NULL, 0 );
    xmlXPathContextPtr xpath = xmlXPathNewContext( doc );
    return "SP.Web" == libcmis::getXPathValue( xpath, "//@term" );
}
