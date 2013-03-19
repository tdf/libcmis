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

#include <json/json_tokener.h>
#include <json/linkhash.h>

#include "json-utils.hxx"

template <>
Json::Json( const std::vector<Json>& arr ) :
    m_json( ::json_object_new_array( ) )
{
    for ( std::vector<Json>::const_iterator i = arr.begin() ; i != arr.end() ; ++i )
        add( *i ) ;
}

Json::Json( const char *str ) :
    m_json( ::json_object_new_string( str ) )
{
}

Json::Json( struct json_object *json ) :
    m_json( json )
{
    ::json_object_get( m_json ) ;
}

Json::Json( const Json& copy ) :
    m_json( copy.m_json )
{
    ::json_object_get( m_json ) ;
}

Json::~Json( )
{
    if ( m_json != 0 )
        ::json_object_put( m_json ) ;
}

Json& Json::operator=( const Json& rhs )
{
    Json tmp( rhs ) ;
    swap( tmp ) ;
    return *this ;
}

void Json::swap( Json& other )
{
    std::swap( m_json, other.m_json ) ;
}

Json Json::operator[]( string key ) const
{
    struct json_object *j = ::json_object_object_get( m_json, key.c_str() ) ;

    return Json( j ) ;
}

Json Json::operator[]( const std::size_t& index ) const
{
    struct json_object *json = ::json_object_array_get_idx( m_json, index ) ;

    return Json( json ) ;
}

std::ostream& operator<<( std::ostream& os, const Json& json )
{
    return os << ::json_object_to_json_string( json.m_json ) ;
}

void Json::add( const Json& json )
{
    ::json_object_get( json.m_json ) ;
    ::json_object_array_add( m_json, json.m_json ) ;
}

Json Json::parse( string str )
{
    struct json_object *json = ::json_tokener_parse( str.c_str() ) ;
    return Json( json) ;
}

Json::JsonObject Json::getObjects( )
{
    JsonObject objs;

    
    for(struct lh_entry *entry = json_object_get_object(m_json)->head; entry; entry = entry->next )
    {
        if ( entry ) 
        {     
            char* key = (char*)entry->k; 
            struct json_object* val = (struct json_object*)entry->v; 
            objs.insert( JsonObject::value_type(key, Json( val ) ) );
        }
        
    }
    return objs ;
}
Json::Type Json::getDataType() const
{
    return static_cast<Type>( ::json_object_get_type( m_json ) ) ;
}

int Json::getLength( ) const
{
    return ::json_object_array_length( m_json );
}

string Json::toString()
{

    return ::json_object_get_string( m_json ) ;
}
