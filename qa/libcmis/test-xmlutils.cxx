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

#define private public

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

        void parseEmptyPropertyTest( );
        
        void parseRenditionTest( );
        void parseRepositoryCapabilitiesTest( );

        // Writer tests
        void propertyStringAsXmlTest( ); 
        void propertyIntegerAsXmlTest( ); 

        // Other utilities tests
        void sha1Test( );

        CPPUNIT_TEST_SUITE( XmlTest );
        CPPUNIT_TEST( parseDateTimeTest );
        CPPUNIT_TEST( parseBoolTest );
        CPPUNIT_TEST( parseIntegerTest );
        CPPUNIT_TEST( parseDoubleTest );
        CPPUNIT_TEST( parsePropertyStringTest );
        CPPUNIT_TEST( parsePropertyIntegerTest );
        CPPUNIT_TEST( parsePropertyDateTimeTest );
        CPPUNIT_TEST( parsePropertyBoolTest );
        CPPUNIT_TEST( parseEmptyPropertyTest );
        CPPUNIT_TEST( parseRenditionTest );
        CPPUNIT_TEST( parseRepositoryCapabilitiesTest );
        CPPUNIT_TEST( propertyStringAsXmlTest );
        CPPUNIT_TEST( propertyIntegerAsXmlTest );
        CPPUNIT_TEST( sha1Test );
        CPPUNIT_TEST_SUITE_END( );
};

/** Dummy ObjectType implementation used as a PropertyType factory.
  */
class ObjectTypeDummy : public libcmis::ObjectType
{
    public:
        ObjectTypeDummy( );
        virtual ~ObjectTypeDummy() { };
};

ObjectTypeDummy::ObjectTypeDummy( )
{
    // String Property
    {
        libcmis::PropertyTypePtr prop ( new libcmis::PropertyType( ) );
        prop->setId( string( "STR-ID" ) );
        prop->setLocalName( string( "LOCAL" ) );
        prop->setDisplayName( string( "DISPLAY" ) );
        prop->setQueryName( string( "QUERY" ) );
        prop->setTypeFromXml( "string" );

        m_propertiesTypes.insert( pair< string, libcmis::PropertyTypePtr >( prop->getId( ), prop ) );
    }
    
    // Integer Property
    {
        libcmis::PropertyTypePtr prop ( new libcmis::PropertyType( ) );
        prop->setId( string( "INT-ID" ) );
        prop->setLocalName( string( "LOCAL" ) );
        prop->setDisplayName( string( "DISPLAY" ) );
        prop->setQueryName( string( "QUERY" ) );
        prop->setTypeFromXml( "integer" );

        m_propertiesTypes.insert( pair< string, libcmis::PropertyTypePtr >( prop->getId( ), prop ) );
    }
    
    // DateTime Property
    {
        libcmis::PropertyTypePtr prop ( new libcmis::PropertyType( ) );
        prop->setId( string( "DATE-ID" ) );
        prop->setLocalName( string( "LOCAL" ) );
        prop->setDisplayName( string( "DISPLAY" ) );
        prop->setQueryName( string( "QUERY" ) );
        prop->setTypeFromXml( "datetime" );

        m_propertiesTypes.insert( pair< string, libcmis::PropertyTypePtr >( prop->getId( ), prop ) );
    }
    
    // Boolean Property
    {
        libcmis::PropertyTypePtr prop ( new libcmis::PropertyType( ) );
        prop->setId( string( "BOOL-ID" ) );
        prop->setLocalName( string( "LOCAL" ) );
        prop->setDisplayName( string( "DISPLAY" ) );
        prop->setQueryName( string( "QUERY" ) );
        prop->setTypeFromXml( "boolean" );

        m_propertiesTypes.insert( pair< string, libcmis::PropertyTypePtr >( prop->getId( ), prop ) );
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

    // Error test
    {
        posix_time::ptime t = libcmis::parseDateTime( string( "Nothing interesting 9990" ) );
        CPPUNIT_ASSERT_MESSAGE( "Error case failed", t.is_not_a_date_time( ) );
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

void XmlTest::parseEmptyPropertyTest( )
{
    stringstream buf;
    buf << "<cmis:propertyId " << getXmlns( ) << "propertyDefinitionId=\"STR-ID\" />";
    libcmis::ObjectTypePtr dummy( new ObjectTypeDummy( ) );
    libcmis::PropertyPtr actual = libcmis::parseProperty( getXmlNode( buf.str( ) ), dummy );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id parsed", string( "STR-ID" ), actual->getPropertyType()->getId( ) );
    CPPUNIT_ASSERT_MESSAGE( "Should have no value", actual->getStrings( ).empty( ) );
}

void XmlTest::parseRenditionTest( )
{
    stringstream buf;
    buf << "<cmis:rendition " << getXmlns( ) << ">"
        <<     "<cmis:streamId>STREAM-ID</cmis:streamId>"
        <<     "<cmis:mimetype>MIME</cmis:mimetype>"
        <<     "<cmis:length>123456</cmis:length>"
        <<     "<cmis:kind>KIND</cmis:kind>"
        <<     "<cmis:title>TITLE</cmis:title>"
        <<     "<cmis:height>123</cmis:height>"
        <<     "<cmis:width>456</cmis:width>"
        <<     "<cmis:renditionDocumentId>DOC-ID</cmis:renditionDocumentId>"
        << "</cmis:rendition>";
    libcmis::RenditionPtr actual( new libcmis::Rendition( getXmlNode( buf.str( ) ) ) );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong stream id parsed", string( "STREAM-ID" ), actual->getStreamId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong mime type parsed", string( "MIME" ), actual->getMimeType( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong length parsed", long( 123456 ), actual->getLength( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong kind parsed", string( "KIND" ), actual->getKind( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong title parsed", string( "TITLE" ), actual->getTitle( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong height parsed", long( 123 ), actual->getHeight( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong width parsed", long( 456 ), actual->getWidth( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong rendition doc id parsed", string( "DOC-ID" ), actual->getRenditionDocumentId( ) );
}

string lcl_findCapability( map< libcmis::Repository::Capability, string > store, libcmis::Repository::Capability capability )
{
    string result;
    map< libcmis::Repository::Capability, string >::iterator it = store.find( capability );
    if ( it != store.end( ) )
        result = it->second;

    return result;
}

void XmlTest::parseRepositoryCapabilitiesTest( )
{
    stringstream buf;
    buf << "<cmis:capabilities " << getXmlns( ) << ">"
        <<    "<cmis:capabilityACL>manage</cmis:capabilityACL>"
        <<    "<cmis:capabilityAllVersionsSearchable>false</cmis:capabilityAllVersionsSearchable>"
        <<    "<cmis:capabilityChanges>none</cmis:capabilityChanges>"
        <<    "<cmis:capabilityContentStreamUpdatability>anytime</cmis:capabilityContentStreamUpdatability>"
        <<    "<cmis:capabilityGetDescendants>true</cmis:capabilityGetDescendants>"
        <<    "<cmis:capabilityGetFolderTree>true</cmis:capabilityGetFolderTree>"
        <<    "<cmis:capabilityOrderBy>common</cmis:capabilityOrderBy>"
        <<    "<cmis:capabilityMultifiling>true</cmis:capabilityMultifiling>"
        <<    "<cmis:capabilityPWCSearchable>false</cmis:capabilityPWCSearchable>"
        <<    "<cmis:capabilityPWCUpdatable>true</cmis:capabilityPWCUpdatable>"
        <<    "<cmis:capabilityQuery>bothcombined</cmis:capabilityQuery>"
        <<    "<cmis:capabilityRenditions>read</cmis:capabilityRenditions>"
        <<    "<cmis:capabilityUnfiling>false</cmis:capabilityUnfiling>"
        <<    "<cmis:capabilityVersionSpecificFiling>false</cmis:capabilityVersionSpecificFiling>"
        <<    "<cmis:capabilityJoin>none</cmis:capabilityJoin>"
        << "</cmis:capabilities>";

    map< libcmis::Repository::Capability, string > capabilities = libcmis::Repository::parseCapabilities( getXmlNode( buf.str( ) ) );

    CPPUNIT_ASSERT_EQUAL( string( "manage" ), lcl_findCapability( capabilities, libcmis::Repository::ACL ) );
    CPPUNIT_ASSERT_EQUAL( string( "false" ), lcl_findCapability( capabilities, libcmis::Repository::AllVersionsSearchable ) );
    CPPUNIT_ASSERT_EQUAL( string( "none" ), lcl_findCapability( capabilities, libcmis::Repository::Changes ) );
    CPPUNIT_ASSERT_EQUAL( string( "anytime" ), lcl_findCapability( capabilities, libcmis::Repository::ContentStreamUpdatability ) );
    CPPUNIT_ASSERT_EQUAL( string( "true" ), lcl_findCapability( capabilities, libcmis::Repository::GetDescendants ) );
    CPPUNIT_ASSERT_EQUAL( string( "true" ), lcl_findCapability( capabilities, libcmis::Repository::GetFolderTree ) );
    CPPUNIT_ASSERT_EQUAL( string( "common" ), lcl_findCapability( capabilities, libcmis::Repository::OrderBy ) );
    CPPUNIT_ASSERT_EQUAL( string( "true" ), lcl_findCapability( capabilities, libcmis::Repository::Multifiling ) );
    CPPUNIT_ASSERT_EQUAL( string( "false" ), lcl_findCapability( capabilities, libcmis::Repository::PWCSearchable ) );
    CPPUNIT_ASSERT_EQUAL( string( "true" ), lcl_findCapability( capabilities, libcmis::Repository::PWCUpdatable ) );
    CPPUNIT_ASSERT_EQUAL( string( "bothcombined" ), lcl_findCapability( capabilities, libcmis::Repository::Query ) );
    CPPUNIT_ASSERT_EQUAL( string( "read" ), lcl_findCapability( capabilities, libcmis::Repository::Renditions ) );
    CPPUNIT_ASSERT_EQUAL( string( "false" ), lcl_findCapability( capabilities, libcmis::Repository::Unfiling ) );
    CPPUNIT_ASSERT_EQUAL( string( "false" ), lcl_findCapability( capabilities, libcmis::Repository::VersionSpecificFiling ) );
    CPPUNIT_ASSERT_EQUAL( string( "none" ), lcl_findCapability( capabilities, libcmis::Repository::Join ) );
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

void XmlTest::sha1Test( )
{
    {
        string actual = libcmis::sha1( "Hello" );
        CPPUNIT_ASSERT_EQUAL( string( "f7ff9e8b7bb2e09b70935a5d785e0cc5d9d0abf0" ), actual );
    }

    {
        // check correct width
        string actual = libcmis::sha1( "35969137" );
        CPPUNIT_ASSERT_EQUAL( string( "0d93546909cfeb5c00089202104df3980000ec9f" ), actual );
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION( XmlTest );
