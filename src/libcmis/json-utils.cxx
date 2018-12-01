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

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>

#include "json-utils.hxx"
#include "exception.hxx"
#include "xml-utils.hxx"

using namespace std;
using namespace libcmis;
using namespace boost;
using boost::property_tree::ptree;
using boost::property_tree::json_parser::read_json;
using boost::property_tree::json_parser::write_json;

Json::Json( ) :
	m_tJson( ptree( ) ),
    m_type( json_object )
{
}

Json::Json( const char *str ) :
    m_tJson( ptree( ) ),
    m_type( json_string )
{
    m_tJson.put_value( str );
    m_type = parseType( );
}

Json::Json( ptree tJson ) :
    m_tJson( tJson ),
    m_type( json_object )
{
    m_type = parseType( );
}

Json::Json( const PropertyPtr& property ):
    m_tJson( ),
    m_type( json_object )
{
    string str = property->toString( );
    m_tJson.put("", str );
}

Json::Json( const PropertyPtrMap& properties ) :
    m_tJson( ptree( ) ),
    m_type( json_array )
{
    for ( PropertyPtrMap::const_iterator it = properties.begin() ; 
            it != properties.end() ; ++it )
        {
            string key = it->first;
            string value = it->second->toString( );
            m_tJson.put( key, value );
        }
}

Json::Json( const Json& copy ) :
    m_tJson( copy.m_tJson ),
    m_type( copy.m_type )
{
}

Json::Json( const JsonObject& obj ) :
    m_tJson( ptree( ) ),
    m_type( json_array )
{
    for ( JsonObject::const_iterator i = obj.begin() ; i != obj.end() ; ++i )
        add( i->first, i->second ) ;
}

Json::Json( const JsonVector& arr ) :
    m_tJson( ptree( ) ),
    m_type( json_array )
{
    for ( std::vector<Json>::const_iterator i = arr.begin(); i != arr.end(); ++i )
        add( *i ) ;
}


Json::~Json( )
{
}

Json& Json::operator=( const Json& rhs )
{
    if ( this != &rhs )
    {
        m_tJson = rhs.m_tJson;
        m_type = rhs.m_type;
    }
    return *this ;
}

void Json::swap( Json& rhs )
{
    std::swap( m_tJson, rhs.m_tJson );
    std::swap( m_type, rhs.m_type );
}

Json Json::operator[]( string key ) const 
{
    ptree tJson;
    try
    {    
        tJson = m_tJson.get_child( key );
    }
    catch ( boost::exception const& )
    {
        return Json( "" );
    }

    Json childJson( tJson );

    return childJson ;
}

void Json::add( const std::string& key, const Json& json ) 
{
    try
    {
        m_tJson.add_child( key, json.getTree( ) );
    }
    catch ( boost::exception const& )
    {
        throw libcmis::Exception( "Couldn't add Json object" );
    }
}

Json::JsonVector Json::getList()
{
    JsonVector list;
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, m_tJson.get_child(""))
    {
        list.push_back( Json( v.second  ) );
    }
	return list ;
}

void Json::add( const Json& json )
{
    try
    {
        m_tJson.push_back( ptree::value_type( "", json.getTree( )) );
    }
    catch ( boost::exception const& )
    {
        throw libcmis::Exception( "Couldn't add Json object" );
    }
}

Json Json::parse( const string& str )
{
    ptree pTree;
    std::stringstream ss( str ); 
    if ( ss.good( ) )
    {
        try 
        {
            property_tree::json_parser::read_json( ss, pTree );
        }
        catch ( boost::exception const& )
        {
            return Json( str.c_str( ) );
        }
    }
    return Json( pTree );
}

Json::JsonObject Json::getObjects( )
{
    JsonObject objs;
    BOOST_FOREACH(boost::property_tree::ptree::value_type &v, m_tJson.get_child(""))
    {
        Json jsonValue( v.second ); 
        objs.insert( JsonObject::value_type(v.first, jsonValue ) );
    }
    return objs ;
}

Json::Type Json::parseType( )
{
    Type type = json_string;
    string str = toString( );
    if ( str.empty( ) )
        return type;
    try 
    {
        boost::posix_time::ptime time = libcmis::parseDateTime( str );
        if ( !time.is_not_a_date_time( ) )
            return json_datetime;
    }
    catch (...)
    {
        // Try other types
    }
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
    return type;
}

Json::Type Json::getDataType( ) const
{
    return m_type;
}

string Json::getStrType( ) const
{
    switch ( m_type )
    {
        case json_null: return "json_null";
        case json_bool: return "json_bool";
        case json_int:  return "json_int";
        case json_double: return "json_double";
        case json_string: return "json_string";
        case json_datetime: return "json_datetime";
        case json_object: return "json_object";
        case json_array: return "json_array";
    }
    return "json_string";   
}

string Json::toString( ) const
{
    string str;
    try 
    {
        stringstream ss;
        write_json( ss, m_tJson );
        str = ss.str( );
    }
    catch ( boost::exception const& )
    {
        str = m_tJson.get_value<string>( );
    }
    // empty json
    if ( str == "{\n}\n" ) str = "";
    return str;
}

