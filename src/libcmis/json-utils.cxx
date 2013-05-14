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

#include <cassert>

#include <json/json_tokener.h>
#include <json/linkhash.h>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "json-utils.hxx"
#include "exception.hxx"
#include "xml-utils.hxx"

using namespace std;
using namespace libcmis;

template <>
Json::Json( const std::vector<Json>& arr ) :
    m_json( ::json_object_new_array( ) ),
    m_type( json_array )
{
    for ( std::vector<Json>::const_iterator i = arr.begin() ; 
                                        i != arr.end() ; ++i )
        add( *i ) ;
}

Json::Json( const char *str ) :
    m_json( ::json_object_new_string( str ) ),
    m_type( json_string )
{
    if ( m_json == 0 )
        throw libcmis::Exception(" Can not create json object from string " );
    m_type = parseType( );
}

Json::Json( struct json_object *json ) :
    m_json( json ),
    m_type( json_object )
{
    try
    {
        ::json_object_get( m_json ) ;
    }
    catch (...)
    {
        throw libcmis::Exception(" Can not create Json object" );
    }
    m_type = parseType( );
}

Json::Json( const Json& copy ) :
    m_json( copy.m_json ),
    m_type( copy.m_type )
{
    assert( m_json != 0 ) ;
    ::json_object_get( m_json ) ;
}

template <>
Json::Json( const JsonObject& obj ) :
    m_json( ::json_object_new_object() ),
    m_type( json_array )
{
    if ( m_json == 0 )
        throw libcmis::Exception( string( "cannot create json object" ) ) ;

    for ( JsonObject::const_iterator i = obj.begin() ; i != obj.end() ; ++i )
        add( i->first, i->second ) ;
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

void Json::add( const std::string& key, const Json& json )
{
    assert( m_json != 0 ) ; 
    assert( json.m_json != 0 ) ; 

    ::json_object_get( json.m_json ) ; 
    ::json_object_object_add( m_json, key.c_str(), json.m_json ) ; 
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

Json::JsonVector Json::getList()
{
	std::size_t len = ::json_object_array_length( m_json ) ;
	JsonVector list ;
	
	for ( std::size_t i = 0 ; i < len ; ++i )
		list.push_back( Json( ::json_object_array_get_idx( m_json, i ) ) ) ;
	
	return list ;
}

void Json::add( const Json& json )
{
    ::json_object_get( json.m_json ) ;
    ::json_object_array_add( m_json, json.m_json ) ;
}

Json Json::parse( const string& str )
{
    struct json_object *json = ::json_tokener_parse( str.c_str() ) ;
    return Json( json) ;
}

Json::JsonObject Json::getObjects( )
{
    JsonObject objs;
    for(struct lh_entry *entry = json_object_get_object(m_json)->head; entry; 
                                                        entry = entry->next )
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

Json::Type Json::parseType( )
{
    if ( m_json == NULL ) 
        return json_null;
    Type type =static_cast<Type>( ::json_object_get_type( m_json ) );
    string str = toString( );
    if ( type == json_string )
    {
        boost::posix_time::ptime time = libcmis::parseDateTime( 
                                        json_object_get_string( m_json ) );
        if ( !time.is_not_a_date_time( ) )
            type = json_datetime;
        else
        {
            Type backupType = type;
            type = json_bool;
            try
            {
                parseBool( str );
            }
            catch (...)
            {
                type = backupType;
            }
            if ( type != json_bool )
            {
                if ( str.find('.') == string::npos )
                {
                    backupType = type;
                    type = json_int;
                    try
                    {
                        parseInteger( str );
                    }
                    catch(...) 
                    { 
                        type = backupType;
                    }
                }
                else
                {
                    backupType = type;
                    type = json_double;
                    try
                    {
                        parseDouble( str );
                    }
                    catch(...) 
                    { 
                        type = backupType;
                    }
                }
            }
        }   
    }
    return type;
}

Json::Type Json::getDataType( ) const
{
    return m_type;
}

int Json::getLength( ) const
{
    return ::json_object_array_length( m_json );
}

string Json::toString( ) const
{
    // Return an empty string if the object doesn't exist
    if ( m_json == NULL ) return string( );
    return ::json_object_get_string( m_json ) ;
}

