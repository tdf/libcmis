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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

#include <string>
#include <fstream>
#include <cerrno>

#define private public
#define protected public

#include "json-utils.hxx"
#include "property.hxx"
#include "property-type.hxx"

using namespace std;
using namespace libcmis;

class JsonTest : public CppUnit::TestFixture
{
    public:
        void parseTest( );
        void parseTypeTest( );
        void createFromPropertyTest( );
        void createFromPropertiesTest( );
        void badKeyTest( );
        void badIndexTest( );
        void addTest( );

        CPPUNIT_TEST_SUITE( JsonTest );
        CPPUNIT_TEST( parseTest );
        CPPUNIT_TEST( parseTypeTest );
        CPPUNIT_TEST( createFromPropertyTest );
        CPPUNIT_TEST( createFromPropertiesTest );  
        CPPUNIT_TEST( badKeyTest );
        CPPUNIT_TEST( badIndexTest );
        CPPUNIT_TEST( addTest );
        CPPUNIT_TEST_SUITE_END( );
};

string getFileContents( const char *filename)
{
    std::ifstream in( filename, std::ios::in | std::ios::binary );
    if (in)
    {
        std::string contents;
        in.seekg( 0, std::ios::end );
        contents.resize(in.tellg( ) );
        in.seekg( 0, std::ios::beg );
        in.read( &contents[0], contents.size( ) );
        in.close( );
        return contents;
    }
    throw ( errno );
}

Json parseFile( string fileName )
{
    Json json = Json::parse( getFileContents( fileName.c_str( ) ) );
    return json;
}

void JsonTest::parseTest( )
{
    Json json = parseFile( "data/gdrive/jsontest-good.json" );
    string kind = json["kind"].toString( );
    string id = json["id"].toString( );
    string mimeType = json["mimeType"].toString( );
    string createdDate = json["createdDate"].toString( );
    string intTest = json["intTest"].toString( );
    string doubleTest = json["doubleTest"].toString( );
    string editable = json["editable"].toString( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong kind", string( "drive#file" ), kind );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id", string( "aFileId"), id );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong mimeType", string( "application/vnd.google-apps.form"), mimeType );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong createdDate", string( "2010-04-28T14:53:23.141Z"), createdDate );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong intTest", string("-123"), intTest );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong doubleTest", string("-123.456"), doubleTest );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong editable", string( "true"), editable );
}

void JsonTest::parseTypeTest( )
{
    Json json = parseFile( "data/gdrive/jsontest-good.json" );
    Json::Type stringType = json["kind"].getDataType( );
    Json::Type boolType = json["editable"].getDataType( );
    Json::Type intType = json["intTest"].getDataType( );
    Json::Type doubleType = json["doubleTest"].getDataType( );
    Json::Type dateTimeType = json["createdDate"].getDataType( );
    Json::Type objectType = json["exportLinks"].getDataType( );
    Json::Type arrayType = json["parents"].getDataType( );
    
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong string type", Json::json_string, stringType );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong bool type", Json::json_bool, boolType );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong int type", Json::json_int, intType );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong double type", Json::json_double, doubleType );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong datetime type", Json::json_datetime, dateTimeType );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong object type", Json::json_object, objectType );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong array type", Json::json_array, arrayType );   
}

void JsonTest::createFromPropertyTest( )
{
    vector< string > values;
    string expected("Value 1" ); 
    values.push_back( expected );

    PropertyTypePtr propertyType( new PropertyType( ) );

    PropertyPtr property( new Property( propertyType, values ) );

    Json json( property );

    CPPUNIT_ASSERT_EQUAL( expected, json.toString( ) );  
}

void JsonTest::createFromPropertiesTest( )
{
    vector< string > values;
    string expected( "value" );
    values.push_back( "value" );

    PropertyTypePtr propertyType( new PropertyType( ) );

    PropertyPtr property( new libcmis::Property( propertyType, values ) );
    
    PropertyPtrMap properties;
    properties[ "key" ] = property;
    
    Json json( properties );

    CPPUNIT_ASSERT_EQUAL( expected, json["key"].toString( ) ); 
}

void JsonTest::badKeyTest( )
{
    Json json = parseFile( "data/gdrive/jsontest-good.json" );
    // just make sure it doesn't crash here
    string notExist = json["nonExistedKey"].toString( );
    CPPUNIT_ASSERT_EQUAL( string( ), notExist);
}

void JsonTest::badIndexTest( )
{
    Json json = parseFile( "data/gdrive/jsontest-good.json" );
    // just make sure it doesn't crash here
    string notExist = json[1000].toString( );
    CPPUNIT_ASSERT_EQUAL( string( ), notExist);
}

void JsonTest::addTest( )
{
    Json json = parseFile( "data/gdrive/jsontest-good.json" );
    Json addJson("added");
    json.add( "new", addJson);
    CPPUNIT_ASSERT_EQUAL( addJson.toString( ), json["new"].toString( ) );
}

CPPUNIT_TEST_SUITE_REGISTRATION( JsonTest );
