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

#include "object-type.hxx"
#include "property.hxx"
#include "xml-utils.hxx"

using namespace std;

namespace libcmis
{
    Property::Property( PropertyTypePtr propertyType, std::vector< std::string > strValues ) :
        m_propertyType( propertyType ),
        m_strValues( ),
        m_boolValues( ),
        m_longValues( ),
        m_doubleValues( ),
        m_dateTimeValues( )
    {
        setValues( strValues );
    }

    void Property::setValues( vector< string > strValues )
    {
        m_strValues = strValues;
        m_boolValues.clear( );
        m_longValues.clear( );
        m_doubleValues.clear( );
        m_dateTimeValues.clear( );

        for ( vector< string >::iterator it = strValues.begin(); it != strValues.end( ); ++it )
        {
            try
            {
                switch ( getPropertyType( )->getType( ) )
                {
                    case PropertyType::Integer:
                        m_longValues.push_back( parseInteger( *it ) );
                        break;
                    case PropertyType::Decimal:
                        m_doubleValues.push_back( parseDouble( *it ) );
                        break;
                    case PropertyType::Bool:
                        m_boolValues.push_back( parseBool( *it ) );
                        break;
                    case PropertyType::DateTime:
                        m_dateTimeValues.push_back( parseDateTime( *it ) );
                        break;
                    default:
                    case PropertyType::String:
                        // Nothing to convert for strings
                        break;
                }
            }
            catch( const Exception& )
            {
                // Just ignore the unparsable values
            }
        }
    }

    void Property::toXml( xmlTextWriterPtr writer )
    {
        string xmlType = string( "cmis:property" ) + getPropertyType()->getXmlType( );
        xmlTextWriterStartElement( writer, BAD_CAST( xmlType.c_str( ) ) );
        
        // Write the attributes
        xmlTextWriterWriteFormatAttribute( writer, BAD_CAST( "propertyDefinitionId" ),
                "%s", BAD_CAST( getPropertyType()->getId( ).c_str( ) ) );
        xmlTextWriterWriteFormatAttribute( writer, BAD_CAST( "localName" ),
                "%s", BAD_CAST( getPropertyType()->getLocalName( ).c_str( ) ) );
        xmlTextWriterWriteFormatAttribute( writer, BAD_CAST( "displayName" ),
                "%s", BAD_CAST( getPropertyType()->getDisplayName( ).c_str( ) ) );
        xmlTextWriterWriteFormatAttribute( writer, BAD_CAST( "queryName" ),
                "%s", BAD_CAST( getPropertyType()->getQueryName( ).c_str( ) ) );
        
        // Write the values
        for ( vector< string >::iterator it = m_strValues.begin( ); it != m_strValues.end( ); ++it )
        {
            xmlTextWriterWriteElement( writer, BAD_CAST( "cmis:value" ), BAD_CAST( it->c_str( ) ) );
        }

        xmlTextWriterEndElement( writer );
    }
    
    PropertyPtr parseProperty( xmlNodePtr node, ObjectTypePtr objectType )
    {
        PropertyPtr property;

        if ( node != NULL && objectType != NULL )
        {
            try
            {
                string id = getXmlNodeAttributeValue( node, "propertyDefinitionId" );

                // Find the value nodes
                vector< string > values;
                for ( xmlNodePtr child = node->children; child; child = child->next )
                {
                    if ( xmlStrEqual( child->name, BAD_CAST( "value" ) ) )
                    {
                        xmlChar* content = xmlNodeGetContent( child );
                        values.push_back( string( ( char * ) content ) );
                        xmlFree( content );
                    }
                }

                map< string, PropertyTypePtr >::iterator it = objectType->getPropertiesTypes( ).find( id );
                if ( it != objectType->getPropertiesTypes().end( ) )
                {
                    property.reset( new Property( it->second, values ) );
                }
            }
            catch ( const Exception& )
            {
                // Ignore that non-property node
            }
        }

        return property;
    }
}
