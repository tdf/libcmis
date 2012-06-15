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

#include <ctime>
#include <sstream>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <libxml/tree.h>

#include "object-type.hxx"
#include "property.hxx"
#include "property-type.hxx"
#include "xml-utils.hxx"
#include "test-helpers.hxx"

using namespace boost;
using namespace std;
using namespace test;

class XmlTest : public CppUnit::TestFixture
{
    public:

        // Parser tests
        void parseDateTimeTest( );
        void parseBoolTest( );
        void parseIntegerTest( );
        void parseDoubleTest( );
        
        void parsePropertyStringTest( );
        void parsePropertyIntegerTest( );
        void parsePropertyDateTimeTest( );
        void parsePropertyBoolTest( );

        // Writer tests
        void propertyStringAsXmlTest( ); 
        void propertyIntegerAsXmlTest( ); 

        CPPUNIT_TEST_SUITE( XmlTest );
        CPPUNIT_TEST( parseDateTimeTest );
        CPPUNIT_TEST( parseBoolTest );
        CPPUNIT_TEST( parseIntegerTest );
        CPPUNIT_TEST( parseDoubleTest );
        CPPUNIT_TEST( parsePropertyStringTest );
        CPPUNIT_TEST( parsePropertyIntegerTest );
        CPPUNIT_TEST( parsePropertyDateTimeTest );
        CPPUNIT_TEST( parsePropertyBoolTest );
        CPPUNIT_TEST( propertyStringAsXmlTest );
        CPPUNIT_TEST( propertyIntegerAsXmlTest );
        CPPUNIT_TEST_SUITE_END( );
};

/** Dummy ObjectType implementation used as a PropertyType factory.
  */
class ObjectTypeDummy : public libcmis::ObjectType
{
    private:
        map< string, libcmis::PropertyTypePtr > m_properties;

    public:
        ObjectTypeDummy( );
        virtual ~ObjectTypeDummy() { };

        virtual string getId( ) { return string( ); }
        virtual string getLocalName( ) { return string( ); }
        virtual string getLocalNamespace( ) { return string( ); }
        virtual string getDisplayName( ) { return string( ); }
        virtual string getQueryName( ) { return string( ); }
        virtual string getDescription( ) { return string( ); }

        virtual libcmis::ObjectTypePtr  getParentType( ) throw ( libcmis::Exception )
        {
            libcmis::ObjectTypePtr empty;
            return empty;
        }

        virtual libcmis::ObjectTypePtr  getBaseType( ) throw ( libcmis::Exception )
        {
            libcmis::ObjectTypePtr empty;
            return empty;
        }

        virtual std::vector< libcmis::ObjectTypePtr > getChildren( ) throw ( libcmis::Exception )
        {
            vector< libcmis::ObjectTypePtr > empty;
            return empty;
        }
        
        virtual bool isCreatable( ) { return false; }
        virtual bool isFileable( ) { return false; }
        virtual bool isQueryable( ) { return false; }
        virtual bool isFulltextIndexed( ) { return false; }
        virtual bool isIncludedInSupertypeQuery( ) { return false; }
        virtual bool isControllablePolicy( ) { return false; }
        virtual bool isControllableACL( ) { return false; }
        virtual bool isVersionable( ) { return false; }
        virtual libcmis::ObjectType::ContentStreamAllowed getContentStreamAllowed( )
        {
            return libcmis::ObjectType::Allowed;
        }

        virtual map< string, libcmis::PropertyTypePtr >& getPropertiesTypes( ) { return m_properties; }

        virtual string toString( ) { return string( ); }
};

ObjectTypeDummy::ObjectTypeDummy( ) : m_properties( )
{
    // String Property
    {
        libcmis::PropertyTypePtr prop ( new libcmis::PropertyType( ) );
        prop->setId( string( "STR-ID" ) );
        prop->setLocalName( string( "LOCAL" ) );
        prop->setDisplayName( string( "DISPLAY" ) );
        prop->setQueryName( string( "QUERY" ) );
        prop->setTypeFromXml( "string" );

        m_properties.insert( pair< string, libcmis::PropertyTypePtr >( prop->getId( ), prop ) );
    }
    
    // Integer Property
    {
        libcmis::PropertyTypePtr prop ( new libcmis::PropertyType( ) );
        prop->setId( string( "INT-ID" ) );
        prop->setLocalName( string( "LOCAL" ) );
        prop->setDisplayName( string( "DISPLAY" ) );
        prop->setQueryName( string( "QUERY" ) );
        prop->setTypeFromXml( "integer" );

        m_properties.insert( pair< string, libcmis::PropertyTypePtr >( prop->getId( ), prop ) );
    }
    
    // DateTime Property
    {
        libcmis::PropertyTypePtr prop ( new libcmis::PropertyType( ) );
        prop->setId( string( "DATE-ID" ) );
        prop->setLocalName( string( "LOCAL" ) );
        prop->setDisplayName( string( "DISPLAY" ) );
        prop->setQueryName( string( "QUERY" ) );
        prop->setTypeFromXml( "datetime" );

        m_properties.insert( pair< string, libcmis::PropertyTypePtr >( prop->getId( ), prop ) );
    }
    
    // Boolean Property
    {
        libcmis::PropertyTypePtr prop ( new libcmis::PropertyType( ) );
        prop->setId( string( "BOOL-ID" ) );
        prop->setLocalName( string( "LOCAL" ) );
        prop->setDisplayName( string( "DISPLAY" ) );
        prop->setQueryName( string( "QUERY" ) );
        prop->setTypeFromXml( "boolean" );

        m_properties.insert( pair< string, libcmis::PropertyTypePtr >( prop->getId( ), prop ) );
    }
}

void XmlTest::parseDateTimeTest( )
{
    tm basis;
    basis.tm_year = 2011 - 1900;
    basis.tm_mon = 8; // Months are in 0..11 range
    basis.tm_mday = 28;
    basis.tm_hour = 12;
    basis.tm_min = 44;
    basis.tm_sec = 28;

    // No time zone test
    {
        char toParse[50];
        strftime( toParse, sizeof( toParse ), "%FT%T", &basis );
        posix_time::ptime t = libcmis::parseDateTime( string( toParse ) );

        gregorian::date expDate( basis.tm_year + 1900, basis.tm_mon + 1, basis.tm_mday );
        posix_time::time_duration expTime( basis.tm_hour, basis.tm_min, basis.tm_sec );
        posix_time::ptime expected( expDate, expTime );

        CPPUNIT_ASSERT_EQUAL_MESSAGE( "No time zone case failed", expected, t );
    }
    
    // Z time zone test
    {
        char toParse[50];
        strftime( toParse, sizeof( toParse ), "%FT%TZ", &basis );
        posix_time::ptime t = libcmis::parseDateTime( string( toParse ) );
        
        gregorian::date expDate( basis.tm_year + 1900, basis.tm_mon + 1, basis.tm_mday );
        posix_time::time_duration expTime( basis.tm_hour, basis.tm_min, basis.tm_sec );
        posix_time::ptime expected( expDate, expTime );

        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Z time zone case failed", expected, t );
    }

    // +XX:XX time zone test
    {
        char toParse[50];
        strftime( toParse, sizeof( toParse ), "%FT%T+02:00", &basis );
        posix_time::ptime t = libcmis::parseDateTime( string( toParse ) );
        
        gregorian::date expDate( basis.tm_year + 1900, basis.tm_mon + 1, basis.tm_mday );
        posix_time::time_duration expTime( basis.tm_hour + 2, basis.tm_min, basis.tm_sec );
        posix_time::ptime expected( expDate, expTime );

        CPPUNIT_ASSERT_EQUAL_MESSAGE( "+XX:XX time zone case failed", expected, t );
    }

    // -XX:XX time zone test
    {
        char toParse[50];
        strftime( toParse, sizeof( toParse ), "%FT%T-02:00", &basis );
        posix_time::ptime t = libcmis::parseDateTime( string( toParse ) );
        
        gregorian::date expDate( basis.tm_year + 1900, basis.tm_mon + 1, basis.tm_mday );
        posix_time::time_duration expTime( basis.tm_hour - 2, basis.tm_min, basis.tm_sec );
        posix_time::ptime expected( expDate, expTime );

        CPPUNIT_ASSERT_EQUAL_MESSAGE( "+XX:XX time zone case failed", expected, t );
    }
}

void XmlTest::parseBoolTest( )
{
    // 'true' test
    {
        bool result = libcmis::parseBool( string( "true" ) );
        CPPUNIT_ASSERT_MESSAGE( "'true' can't be parsed properly", result );
    }
    
    // '1' test
    {
        bool result = libcmis::parseBool( string( "1" ) );
        CPPUNIT_ASSERT_MESSAGE( "'1' can't be parsed properly", result );
    }

    // 'false' test
    {
        bool result = libcmis::parseBool( string( "false" ) );
        CPPUNIT_ASSERT_MESSAGE( "'false' can't be parsed properly", !result );
    }
    
    // '0' test
    {
        bool result = libcmis::parseBool( string( "0" ) );
        CPPUNIT_ASSERT_MESSAGE( "'0' can't be parsed properly", !result );
    }

    // Error test
    {
        try
        {
            libcmis::parseBool( string( "boolcheat" ) );
            CPPUNIT_FAIL( "Exception should be thrown" );
        }
        catch ( const libcmis::Exception& e )
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad exception message",
                   string( "Invalid xsd:boolean input: boolcheat" ), string( e.what() ) );
        }
    }
}

void XmlTest::parseIntegerTest( )
{
    // Positive value test
    {
        long result = libcmis::parseInteger( string( "123" ) );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Positive integer can't be parsed properly", 123L, result );
    }
    
    // Negative value test
    {
        long result = libcmis::parseInteger( string( "-123" ) );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Negative integer can't be parsed properly", -123L, result );
    }
    
    // Overflow error test
    {
        try
        {
            libcmis::parseInteger( string( "9999999999999999999" ) );
            CPPUNIT_FAIL( "Exception should be thrown" );
        }
        catch ( const libcmis::Exception& e )
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad exception message",
                   string( "xsd:integer input can't fit to long: 9999999999999999999" ), string( e.what() ) );
        }
    }

    // Non integer test
    {
        try
        {
            libcmis::parseInteger( string( "123bad" ) );
            CPPUNIT_FAIL( "Exception should be thrown" );
        }
        catch ( const libcmis::Exception& e )
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad exception message",
                   string( "Invalid xsd:integer input: 123bad" ), string( e.what() ) );
        }
    }
}

void XmlTest::parseDoubleTest( )
{
    // Positive value test
    {
        double result = libcmis::parseDouble( string( "123.456" ) );
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE( "Positive decimal can't be parsed properly", 123.456, result, 0.000001 );
    }
    
    // Negative value test
    {
        double result = libcmis::parseDouble( string( "-123.456" ) );
        CPPUNIT_ASSERT_DOUBLES_EQUAL_MESSAGE( "Negative decimal can't be parsed properly", -123.456, result, 0.000001 );
    }

    // Non integer test
    {
        try
        {
            libcmis::parseDouble( string( "123.456bad" ) );
            CPPUNIT_FAIL( "Exception should be thrown" );
        }
        catch ( const libcmis::Exception& e )
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad exception message",
                   string( "Invalid xsd:decimal input: 123.456bad" ), string( e.what() ) );
        }
    }
}

void XmlTest::parsePropertyStringTest( )
{
    stringstream buf;
    buf << "<cmis:propertyString " << getXmlns( )
        <<            "propertyDefinitionId=\"STR-ID\">"
        <<      "<cmis:value>VALUE 1</cmis:value>"
        <<      "<cmis:value>VALUE 2</cmis:value>"
        << "</cmis:propertyString>";
    libcmis::ObjectTypePtr dummy( new ObjectTypeDummy( ) );
    libcmis::PropertyPtr actual = libcmis::parseProperty( getXmlNode( buf.str( ) ), dummy );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id parsed", string( "STR-ID" ), actual->getPropertyType( )->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of values parsed", vector<string>::size_type( 2 ), actual->getStrings( ).size( ) );
}

void XmlTest::parsePropertyIntegerTest( )
{
    stringstream buf;
    buf << "<cmis:propertyInteger " << getXmlns( )
        <<            "propertyDefinitionId=\"INT-ID\">"
        <<      "<cmis:value>12345</cmis:value>"
        <<      "<cmis:value>67890</cmis:value>"
        << "</cmis:propertyInteger>";
    libcmis::ObjectTypePtr dummy( new ObjectTypeDummy( ) );
    libcmis::PropertyPtr actual = libcmis::parseProperty( getXmlNode( buf.str( ) ), dummy );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id parsed", string( "INT-ID" ), actual->getPropertyType()->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of values parsed", vector<string>::size_type( 2 ), actual->getLongs( ).size( ) );
}

void XmlTest::parsePropertyDateTimeTest( )
{
    stringstream buf;
    buf << "<cmis:propertyDateTime " << getXmlns( )
        <<            "propertyDefinitionId=\"DATE-ID\">"
        <<      "<cmis:value>2012-01-19T09:06:57.388Z</cmis:value>"
        <<      "<cmis:value>2011-01-19T09:06:57.388Z</cmis:value>"
        << "</cmis:propertyDateTime>";
    libcmis::ObjectTypePtr dummy( new ObjectTypeDummy( ) );
    libcmis::PropertyPtr actual = libcmis::parseProperty( getXmlNode( buf.str( ) ), dummy );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id parsed", string( "DATE-ID" ), actual->getPropertyType()->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of values parsed", vector<string>::size_type( 2 ), actual->getDateTimes( ).size( ) );
}

void XmlTest::parsePropertyBoolTest( )
{
    stringstream buf;
    buf << "<cmis:propertyBoolean " << getXmlns( )
        <<            "propertyDefinitionId=\"BOOL-ID\">"
        <<      "<cmis:value>true</cmis:value>"
        << "</cmis:propertyBoolean>";
    libcmis::ObjectTypePtr dummy( new ObjectTypeDummy( ) );
    libcmis::PropertyPtr actual = libcmis::parseProperty( getXmlNode( buf.str( ) ), dummy );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id parsed", string( "BOOL-ID" ), actual->getPropertyType()->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of values parsed", vector<string>::size_type( 1 ), actual->getBools( ).size( ) );
}

void XmlTest::propertyStringAsXmlTest( )
{
    vector< string > values;
    values.push_back( string( "Value 1" ) );
    values.push_back( string( "Value 2" ) );

    ObjectTypeDummy factory;
    map< string, libcmis::PropertyTypePtr >::iterator it = factory.getPropertiesTypes( ).find( "STR-ID" );
    CPPUNIT_ASSERT_MESSAGE( "Missing property type to setup fixture", it != factory.getPropertiesTypes( ).end() );
    libcmis::PropertyPtr property( new libcmis::Property( it->second, values ) );

    string actual = writeXml( property );

    stringstream expected;
    expected << "<?xml version=\"1.0\"?>\n"
             << "<cmis:propertyString propertyDefinitionId=\"STR-ID\" localName=\"LOCAL\" displayName=\"DISPLAY\" queryName=\"QUERY\">"
             << "<cmis:value>Value 1</cmis:value>"
             << "<cmis:value>Value 2</cmis:value>"
             << "</cmis:propertyString>\n";

    CPPUNIT_ASSERT_EQUAL( expected.str( ), actual );
}

void XmlTest::propertyIntegerAsXmlTest( )
{
    vector< string > values;
    values.push_back( string( "123" ) );
    values.push_back( string( "456" ) );

    ObjectTypeDummy factory;
    map< string, libcmis::PropertyTypePtr >::iterator it = factory.getPropertiesTypes( ).find( "INT-ID" );
    CPPUNIT_ASSERT_MESSAGE( "Missing property type to setup fixture", it != factory.getPropertiesTypes( ).end() );
    libcmis::PropertyPtr property( new libcmis::Property( it->second, values ) );

    string actual = writeXml( property );

    stringstream expected;
    expected << "<?xml version=\"1.0\"?>\n"
             << "<cmis:propertyInteger propertyDefinitionId=\"INT-ID\" localName=\"LOCAL\" displayName=\"DISPLAY\" queryName=\"QUERY\">"
             << "<cmis:value>123</cmis:value>"
             << "<cmis:value>456</cmis:value>"
             << "</cmis:propertyInteger>\n";

    CPPUNIT_ASSERT_EQUAL( expected.str( ), actual );
}

CPPUNIT_TEST_SUITE_REGISTRATION( XmlTest );
