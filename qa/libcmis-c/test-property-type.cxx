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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

#include "internals.hxx"
#include "property-type.h"
#include "test-dummies.hxx"

using namespace std;

class PropertyTypeTest : public CppUnit::TestFixture
{
    private:
        libcmis_PropertyTypePtr getTested( string id, string xmlType );

    public:
        void getIdTest( );
        void getLocalNameTest( );
        void getLocalNamespaceTest( );
        void getDisplayNameTest( );
        void getQueryNameTest( );
        void getTypeTest( );
        void isMultiValuedTest( );
        void isUpdatableTest( );
        void isInheritedTest( );
        void isRequiredTest( );
        void isQueryableTest( );
        void isOrderableTest( );
        void isOpenChoiceTest( );

        CPPUNIT_TEST_SUITE( PropertyTypeTest );
        CPPUNIT_TEST( getIdTest );
        CPPUNIT_TEST( getLocalNameTest );
        CPPUNIT_TEST( getLocalNamespaceTest );
        CPPUNIT_TEST( getDisplayNameTest );
        CPPUNIT_TEST( getQueryNameTest );
        CPPUNIT_TEST( getTypeTest );
        CPPUNIT_TEST( isMultiValuedTest );
        CPPUNIT_TEST( isUpdatableTest );
        CPPUNIT_TEST( isInheritedTest );
        CPPUNIT_TEST( isRequiredTest );
        CPPUNIT_TEST( isQueryableTest );
        CPPUNIT_TEST( isOrderableTest );
        CPPUNIT_TEST( isOpenChoiceTest );
        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( PropertyTypeTest );

libcmis_PropertyTypePtr PropertyTypeTest::getTested( string id, string xmlType )
{
    libcmis_PropertyTypePtr result = new libcmis_property_type( );
    libcmis::PropertyTypePtr handle( new dummies::PropertyType( id, xmlType ) );
    result->handle = handle;

    return result;
}

void PropertyTypeTest::getIdTest( )
{
    string id( "Id" );
    libcmis_PropertyTypePtr tested = getTested( id, "string" );
    char* actual = libcmis_property_type_getId( tested );
    CPPUNIT_ASSERT_EQUAL( id, string( actual ) );

    free( actual );
    libcmis_property_type_free( tested );
}

void PropertyTypeTest::getLocalNameTest( )
{
    libcmis_PropertyTypePtr tested = getTested( "id", "string" );
    char* actual = libcmis_property_type_getLocalName( tested );
    CPPUNIT_ASSERT_EQUAL( string( "PropertyType::LocalName" ), string( actual ) );

    free( actual );
    libcmis_property_type_free( tested );
}

void PropertyTypeTest::getLocalNamespaceTest( )
{
    libcmis_PropertyTypePtr tested = getTested( "id", "string" );
    char* actual = libcmis_property_type_getLocalNamespace( tested );
    CPPUNIT_ASSERT_EQUAL( string( "PropertyType::LocalNamespace" ), string( actual ) );

    free( actual );
    libcmis_property_type_free( tested );
}

void PropertyTypeTest::getDisplayNameTest( )
{
    libcmis_PropertyTypePtr tested = getTested( "id", "string" );
    char* actual = libcmis_property_type_getDisplayName( tested );
    CPPUNIT_ASSERT_EQUAL( string( "PropertyType::DisplayName" ), string( actual ) );

    free( actual );
    libcmis_property_type_free( tested );
}

void PropertyTypeTest::getQueryNameTest( )
{
    libcmis_PropertyTypePtr tested = getTested( "id", "string" );
    char* actual = libcmis_property_type_getQueryName( tested );
    CPPUNIT_ASSERT_EQUAL( string( "PropertyType::QueryName" ), string( actual ) );

    free( actual );
    libcmis_property_type_free( tested );
}

void PropertyTypeTest::getTypeTest( )
{
    // String
    {
        libcmis_PropertyTypePtr tested = getTested( "id", "string" );
        libcmis_property_type_Type actualType = libcmis_property_type_getType( tested );
        CPPUNIT_ASSERT_EQUAL( libcmis_String, actualType );
        char* actualXml = libcmis_property_type_getXmlType( tested );
        CPPUNIT_ASSERT_EQUAL( string( "String" ), string( actualXml ) );

        free( actualXml );
        libcmis_property_type_free( tested );
    }
    
    // DateTime
    {
        libcmis_PropertyTypePtr tested = getTested( "id", "datetime" );
        libcmis_property_type_Type actualType = libcmis_property_type_getType( tested );
        CPPUNIT_ASSERT_EQUAL( libcmis_DateTime, actualType );
        char* actualXml = libcmis_property_type_getXmlType( tested );
        CPPUNIT_ASSERT_EQUAL( string( "DateTime" ), string( actualXml ) );

        free( actualXml );
        libcmis_property_type_free( tested );
    }
    
    // Integer
    {
        libcmis_PropertyTypePtr tested = getTested( "id", "integer" );
        libcmis_property_type_Type actualType = libcmis_property_type_getType( tested );
        CPPUNIT_ASSERT_EQUAL( libcmis_Integer, actualType );
        char* actualXml = libcmis_property_type_getXmlType( tested );
        CPPUNIT_ASSERT_EQUAL( string( "Integer" ), string( actualXml ) );

        free( actualXml );
        libcmis_property_type_free( tested );
    }
    
    // Html
    {
        libcmis_PropertyTypePtr tested = getTested( "id", "html" );
        libcmis_property_type_Type actualType = libcmis_property_type_getType( tested );
        CPPUNIT_ASSERT_EQUAL( libcmis_String, actualType );
        char* actualXml = libcmis_property_type_getXmlType( tested );
        CPPUNIT_ASSERT_EQUAL( string( "Html" ), string( actualXml ) );

        free( actualXml );
        libcmis_property_type_free( tested );
    }
}

void PropertyTypeTest::isMultiValuedTest( )
{
    libcmis_PropertyTypePtr tested = getTested( "id", "string" );
    bool actual = libcmis_property_type_isMultiValued( tested );
    CPPUNIT_ASSERT_EQUAL( true , actual );

    libcmis_property_type_free( tested );
}

void PropertyTypeTest::isUpdatableTest( )
{
    libcmis_PropertyTypePtr tested = getTested( "id", "string" );
    bool actual = libcmis_property_type_isUpdatable( tested );
    CPPUNIT_ASSERT_EQUAL( true , actual );

    libcmis_property_type_free( tested );
}

void PropertyTypeTest::isInheritedTest( )
{
    libcmis_PropertyTypePtr tested = getTested( "id", "string" );
    bool actual = libcmis_property_type_isInherited( tested );
    CPPUNIT_ASSERT_EQUAL( true , actual );

    libcmis_property_type_free( tested );
}

void PropertyTypeTest::isRequiredTest( )
{
    libcmis_PropertyTypePtr tested = getTested( "id", "string" );
    bool actual = libcmis_property_type_isRequired( tested );
    CPPUNIT_ASSERT_EQUAL( true , actual );

    libcmis_property_type_free( tested );
}

void PropertyTypeTest::isQueryableTest( )
{
    libcmis_PropertyTypePtr tested = getTested( "id", "string" );
    bool actual = libcmis_property_type_isQueryable( tested );
    CPPUNIT_ASSERT_EQUAL( true , actual );

    libcmis_property_type_free( tested );
}

void PropertyTypeTest::isOrderableTest( )
{
    libcmis_PropertyTypePtr tested = getTested( "id", "string" );
    bool actual = libcmis_property_type_isOrderable( tested );
    CPPUNIT_ASSERT_EQUAL( true , actual );

    libcmis_property_type_free( tested );
}

void PropertyTypeTest::isOpenChoiceTest( )
{
    libcmis_PropertyTypePtr tested = getTested( "id", "string" );
    bool actual = libcmis_property_type_isOpenChoice( tested );
    CPPUNIT_ASSERT_EQUAL( true , actual );

    libcmis_property_type_free( tested );
}
