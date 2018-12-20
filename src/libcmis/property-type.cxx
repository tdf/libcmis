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
 * Copyright (C) 2011 SUSE <cbosdonnat@suse.com>
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

#include <libcmis/property-type.hxx>

#include <libcmis/object-type.hxx>
#include <libcmis/xml-utils.hxx>

using namespace std;

namespace libcmis
{
    PropertyType::PropertyType( ) :
        m_id( ),
        m_localName( ),
        m_localNamespace( ),
        m_displayName( ),
        m_queryName( ),
        m_type( String ),
        m_xmlType( "String" ),
        m_multiValued( false ),
        m_updatable( false ),
        m_inherited( false ),
        m_required( false ),
        m_queryable( false ),
        m_orderable( false ),
        m_openChoice( false ),
        m_temporary( false )
    {
    }

    PropertyType::PropertyType( xmlNodePtr node ) :
        m_id( ),
        m_localName( ),
        m_localNamespace( ),
        m_displayName( ),
        m_queryName( ),
        m_type( String ),
        m_xmlType( "String" ),
        m_multiValued( false ),
        m_updatable( false ),
        m_inherited( false ),
        m_required( false ),
        m_queryable( false ),
        m_orderable( false ),
        m_openChoice( false ),
        m_temporary( false )
    {
        for ( xmlNodePtr child = node->children; child; child = child->next )
        {
            xmlChar* content = xmlNodeGetContent( child );
            string value( ( const char * ) content );
            xmlFree( content );

            if ( xmlStrEqual( child->name, BAD_CAST( "id" ) ) )
                setId( value );
            else if ( xmlStrEqual( child->name, BAD_CAST( "localName" ) ) )
                setLocalName( value );
            else if ( xmlStrEqual( child->name, BAD_CAST( "localNamespace" ) ) )
                setLocalNamespace( value );
            else if ( xmlStrEqual( child->name, BAD_CAST( "displayName" ) ) )
                setDisplayName( value );
            else if ( xmlStrEqual( child->name, BAD_CAST( "queryName" ) ) )
                setQueryName( value );
            else if ( xmlStrEqual( child->name, BAD_CAST( "propertyType" ) ) )
                setTypeFromXml( value );
            else if ( xmlStrEqual( child->name, BAD_CAST( "cardinality" ) ) )
                setMultiValued( value == "multi" );
            else if ( xmlStrEqual( child->name, BAD_CAST( "updatability" ) ) )
                setUpdatable( value == "readwrite" );
            else if ( xmlStrEqual( child->name, BAD_CAST( "inherited" ) ) )
                setInherited( libcmis::parseBool( value ) );
            else if ( xmlStrEqual( child->name, BAD_CAST( "required" ) ) )
                setRequired( libcmis::parseBool( value ) );
            else if ( xmlStrEqual( child->name, BAD_CAST( "queryable" ) ) )
                setQueryable( libcmis::parseBool( value ) );
            else if ( xmlStrEqual( child->name, BAD_CAST( "orderable" ) ) )
                setOrderable( libcmis::parseBool( value ) );
            else if ( xmlStrEqual( child->name, BAD_CAST( "openChoice" ) ) )
                setOpenChoice( libcmis::parseBool( value ) );
        }
    }

    PropertyType::PropertyType( const PropertyType& copy ) :
        m_id ( copy.m_id ),
        m_localName ( copy.m_localName ),
        m_localNamespace ( copy.m_localNamespace ),
        m_displayName ( copy.m_displayName ),
        m_queryName ( copy.m_queryName ),
        m_type ( copy.m_type ),
        m_xmlType( copy.m_xmlType ),
        m_multiValued ( copy.m_multiValued ),
        m_updatable ( copy.m_updatable ),
        m_inherited ( copy.m_inherited ),
        m_required ( copy.m_required ),
        m_queryable ( copy.m_queryable ),
        m_orderable ( copy.m_orderable ),
        m_openChoice ( copy.m_openChoice ),
        m_temporary( copy.m_temporary )
    {
    }

    PropertyType::PropertyType( string type,
                                string id,
                                string localName,
                                string displayName,
                                string queryName ) :
        m_id ( id ),
        m_localName ( localName ),
        m_localNamespace ( ),
        m_displayName ( displayName ),
        m_queryName ( queryName ),
        m_type ( ),
        m_xmlType( type ),
        m_multiValued( false ),
        m_updatable( false ),
        m_inherited( false ),
        m_required( false ),
        m_queryable( false ),
        m_orderable( false ),
        m_openChoice( false ),
        m_temporary( true )
    {
        setTypeFromXml( m_xmlType );
    }

    PropertyType& PropertyType::operator=( const PropertyType& copy )
    {
        if ( this != &copy )
        {
            m_id = copy.m_id;
            m_localName = copy.m_localName;
            m_localNamespace = copy.m_localNamespace;
            m_displayName = copy.m_displayName;
            m_queryName = copy.m_queryName;
            m_type = copy.m_type;
            m_xmlType = copy.m_xmlType;
            m_multiValued = copy.m_multiValued;
            m_updatable = copy.m_updatable;
            m_inherited = copy.m_inherited;
            m_required = copy.m_required;
            m_queryable = copy.m_queryable;
            m_orderable = copy.m_orderable;
            m_openChoice = copy.m_openChoice;
            m_temporary = copy.m_temporary;
        }

        return *this;
    }

    void PropertyType::setTypeFromJsonType( string jsonType )
    {
        if ( jsonType == "json_bool" )
            m_type = Bool;
        else if ( jsonType == "json_double" )
            m_type = Decimal;
        else if ( jsonType == "json_int" )
            m_type = Integer;
        else if ( jsonType == "json_datetime" )
            m_type = DateTime;
        else m_type = String;
    }

    void PropertyType::setTypeFromXml( string typeStr )
    {
        // Default to string
        m_xmlType = string( "String" );
        m_type = String;

        if ( typeStr == "datetime" )
        {
            m_xmlType = string( "DateTime" );
            m_type = DateTime;
        }
        else if ( typeStr == "integer" )
        {
            m_xmlType = string( "Integer" );
            m_type = Integer;
        }
        else if ( typeStr == "decimal" )
        {
            m_xmlType = string( "Decimal" );
            m_type = Decimal;
        }
        else if ( typeStr == "boolean" )
        {
            m_xmlType = string( "Boolean" );
            m_type = Bool;
        }
        // Special kinds of String
        else if ( typeStr == "html" )
            m_xmlType = string( "Html" );
        else if ( typeStr == "id" )
            m_xmlType = string( "Id" );
        else if ( typeStr == "uri" )
            m_xmlType = string( "Uri" );
    }

    void PropertyType::update( vector< ObjectTypePtr > typesDefs )
    {
        for ( vector< ObjectTypePtr >::iterator it = typesDefs.begin();
                it != typesDefs.end( ) && m_temporary; ++it )
        {
            map< string, PropertyTypePtr >& propertyTypes =
                ( *it )->getPropertiesTypes( );
            map< string, PropertyTypePtr >::iterator propIt =
                propertyTypes.find( getId( ) );
            if ( propIt != propertyTypes.end() )
            {
                PropertyTypePtr complete = propIt->second;

                m_localName = complete->m_localName;
                m_localNamespace = complete->m_localNamespace;
                m_displayName = complete->m_displayName;
                m_queryName = complete->m_queryName;
                m_type = complete->m_type;
                m_xmlType = complete->m_xmlType;
                m_multiValued = complete->m_multiValued;
                m_updatable = complete->m_updatable;
                m_inherited = complete->m_inherited;
                m_required = complete->m_required;
                m_queryable = complete->m_queryable;
                m_orderable = complete->m_orderable;
                m_openChoice = complete->m_openChoice;
                m_temporary = false;
            }
        }
    }
}
