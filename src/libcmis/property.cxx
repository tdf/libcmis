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

#include "property.hxx"

#include "xml-utils.hxx"

using namespace std;

namespace
{
    string lcl_getAttributeValue( xmlNodePtr node, const char* attributeName )
    {
        xmlChar* xmlStr = xmlGetProp( node, BAD_CAST( attributeName ) );
        string value( ( char * ) xmlStr );
        xmlFree( xmlStr );
        return value;
    }
}

namespace libcmis
{
    Property::Property( std::string id, std::string localName,
                      std::string displayName, std::string queryName,
                      std::vector< std::string > strValues, Type type ) :
        m_id( id ),
        m_localName( localName ),
        m_displayName( displayName ),
        m_queryName( queryName ),
        m_type( type ),
        m_strValues( strValues )
    {
    }

    void Property::toXml( xmlTextWriterPtr writer )
    {
        string xmlType = string( "cmis:" ) + getXmlType( );
        xmlTextWriterStartElement( writer, BAD_CAST( xmlType.c_str( ) ) );
        
        // Write the attributes
        xmlTextWriterWriteFormatAttribute( writer, BAD_CAST( "propertyDefinitionId" ),
                "%s", BAD_CAST( getId( ).c_str( ) ) );
        xmlTextWriterWriteFormatAttribute( writer, BAD_CAST( "localName" ),
                "%s", BAD_CAST( getLocalName( ).c_str( ) ) );
        xmlTextWriterWriteFormatAttribute( writer, BAD_CAST( "displayName" ),
                "%s", BAD_CAST( getDisplayName( ).c_str( ) ) );
        xmlTextWriterWriteFormatAttribute( writer, BAD_CAST( "queryName" ),
                "%s", BAD_CAST( getQueryName( ).c_str( ) ) );
        
        // Write the values
        for ( vector< string >::iterator it = m_strValues.begin( ); it != m_strValues.end( ); ++it )
        {
            xmlTextWriterWriteElement( writer, BAD_CAST( "cmis:value" ), BAD_CAST( it->c_str( ) ) );
        }

        xmlTextWriterEndElement( writer );
    }

    IntegerProperty::IntegerProperty( std::string id, std::string localName,
            std::string displayName, std::string queryName, std::vector< std::string > values ) :
        Property( id, localName, displayName, queryName, values, Property::Integer ),
        m_values( )
    {
        // Parse the values into long
        for ( vector< string >::iterator it = values.begin(); it != values.end( ); ++it )
        {
            try
            {
                m_values.push_back( parseInteger( *it ) );
            }
            catch( const Exception& e )
            {
                // Just ignore the unparsable value
            }
        }
    }
            
    vector< boost::posix_time::ptime > IntegerProperty::getDateTimes( )
    {
        return vector< boost::posix_time::ptime >( );
    }

    vector< bool > IntegerProperty::getBools( )
    {
        return vector< bool > ( );
    }

    vector< double > IntegerProperty::getDoubles( )
    {
        return vector< double >( );
    }
    
    DecimalProperty::DecimalProperty( std::string id, std::string localName,
            std::string displayName, std::string queryName, std::vector< std::string > values ) :
        Property( id, localName, displayName, queryName, values, Property::Decimal ),
        m_values( )
    {
        // Parse the values into doubles
        for ( vector< string >::iterator it = values.begin(); it != values.end( ); ++it )
        {
            try
            {
                m_values.push_back( parseDouble( *it ) );
            }
            catch( const Exception& e )
            {
                // Just ignore the unparsable value
            }
        }
    }
            
    vector< boost::posix_time::ptime > DecimalProperty::getDateTimes( )
    {
        return vector< boost::posix_time::ptime >( );
    }

    vector< bool > DecimalProperty::getBools( )
    {
        return vector< bool > ( );
    }

    vector< long > DecimalProperty::getLongs( )
    {
        return vector< long > ( );
    }
    
    BoolProperty::BoolProperty( std::string id, std::string localName,
            std::string displayName, std::string queryName, std::vector< std::string > values ) :
        Property( id, localName, displayName, queryName, values, Property::Bool ),
        m_values( )
    {
        // Parse the values into bools
        for ( vector< string >::iterator it = values.begin(); it != values.end( ); ++it )
        {
            try
            {
                m_values.push_back( parseBool( *it ) );
            }
            catch( const Exception& e )
            {
                // Just ignore the unparsable value
            }
        }
    }
            
    vector< boost::posix_time::ptime > BoolProperty::getDateTimes( )
    {
        return vector< boost::posix_time::ptime >( );
    }

    vector< long > BoolProperty::getLongs( )
    {
        return vector< long >( );
    }

    vector< double > BoolProperty::getDoubles( )
    {
        return vector< double >( );
    }
    
    DateTimeProperty::DateTimeProperty( std::string id, std::string localName,
            std::string displayName, std::string queryName, std::vector< std::string > values ) :
        Property( id, localName, displayName, queryName, values, Property::DateTime ),
        m_values( )
    {
        // Parse the values into ptime
        for ( vector< string >::iterator it = values.begin(); it != values.end( ); ++it )
            m_values.push_back( parseDateTime( *it ) );
    }
    
    vector< bool > DateTimeProperty::getBools( )
    {
        return vector< bool > ( );
    }
    
    vector< long > DateTimeProperty::getLongs( )
    {
        return vector< long >( );
    }

    vector< double > DateTimeProperty::getDoubles( )
    {
        return vector< double >( );
    }
    
    StringProperty::StringProperty( std::string id, std::string localName,
            std::string displayName, std::string queryName, std::vector< std::string > values ) :
        Property( id, localName, displayName, queryName, values, Property::String )
    {
    }
            
    vector< boost::posix_time::ptime > StringProperty::getDateTimes( )
    {
        return vector< boost::posix_time::ptime >( );
    }
    
    vector< bool > StringProperty::getBools( )
    {
        return vector< bool >( );
    }
    
    vector< long > StringProperty::getLongs( )
    {
        return vector< long >( );
    }

    vector< double > StringProperty::getDoubles( )
    {
        return vector< double >( );
    }

    IdProperty::IdProperty( std::string id, std::string localName,
            std::string displayName, std::string queryName, std::vector< std::string > values ) :
        StringProperty( id, localName, displayName, queryName, values )
    {
    }
    
    HtmlProperty::HtmlProperty( std::string id, std::string localName,
            std::string displayName, std::string queryName, std::vector< std::string > values ) :
        StringProperty( id, localName, displayName, queryName, values )
    {
    }

    UriProperty::UriProperty( std::string id, std::string localName,
            std::string displayName, std::string queryName, std::vector< std::string > values ) :
        StringProperty( id, localName, displayName, queryName, values )
    {
    }

    PropertyPtr parseProperty( xmlNodePtr node )
    {
        string id = lcl_getAttributeValue( node, "propertyDefinitionId" );
        string localName = lcl_getAttributeValue( node, "localName" );
        string displayName = lcl_getAttributeValue( node, "displayName" );
        string queryName = lcl_getAttributeValue( node, "queryName" );

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

        PropertyPtr property;
        if ( xmlStrEqual( node->name, BAD_CAST( "propertyInteger" ) ) )
        {
            property.reset( new IntegerProperty ( id, localName, displayName, queryName, values ) );
        }
        else if( xmlStrEqual( node->name, BAD_CAST( "propertyDateTime" ) ) )
        {
            property.reset( new DateTimeProperty ( id, localName, displayName, queryName, values ) );
        }
        else if( xmlStrEqual( node->name, BAD_CAST( "propertyBoolean" ) ) )
        {
            property.reset( new BoolProperty ( id, localName, displayName, queryName, values ) );
        }
        else if( xmlStrEqual( node->name, BAD_CAST( "propertyDecimal" ) ) )
        {
            property.reset( new DecimalProperty ( id, localName, displayName, queryName, values ) );
        }
        else if( xmlStrEqual( node->name, BAD_CAST( "propertyId" ) ) )
        {
            property.reset( new IdProperty ( id, localName, displayName, queryName, values ) );
        }
        else if( xmlStrEqual( node->name, BAD_CAST( "propertyUri" ) ) )
        {
            property.reset( new UriProperty ( id, localName, displayName, queryName, values ) );
        }
        else if( xmlStrEqual( node->name, BAD_CAST( "propertyHtml" ) ) )
        {
            property.reset( new HtmlProperty ( id, localName, displayName, queryName, values ) );
        }
        else
        {
            property.reset( new StringProperty ( id, localName, displayName, queryName, values ) );
        }
        return property;
    }
}
