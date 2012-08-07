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
#include "property.h"
#include "property-type.h"
#include "test-dummies.hxx"

using namespace std;

class PropertyTest : public CppUnit::TestFixture
{
    private:
        libcmis_PropertyTypePtr getTestType( string xmlType );

    public:
        void getDateTimesTest( );
        void getBoolsTest( );
        /*
        void getStringsTest( );
        void getLongsTest( );
        void getDoublesTest( );
        */

        CPPUNIT_TEST_SUITE( PropertyTest );
        CPPUNIT_TEST( getDateTimesTest );
        CPPUNIT_TEST( getBoolsTest );
        /*
        CPPUNIT_TEST( getStringsTest );
        CPPUNIT_TEST( getLongsTest );
        CPPUNIT_TEST( getDoublesTest );
        */
        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( PropertyTest );

libcmis_PropertyTypePtr PropertyTest::getTestType( string xmlType )
{
    libcmis_PropertyTypePtr result = new libcmis_property_type( );
    libcmis::PropertyTypePtr handle( new dummies::PropertyType( "Id", xmlType ) );
    result->handle = handle;

    return result;
}

void PropertyTest::getDateTimesTest( )
{
    libcmis_PropertyTypePtr type = getTestType( "datetime" );
    size_t size = 2;
    const char** values = new const char*[size];
    values[0] = "2012-01-19T09:06:57.388Z";
    values[1] = "2012-02-19T09:06:57.388Z";
    libcmis_PropertyPtr tested  = libcmis_property_create( type, values, size );

    libcmis_vector_time_t* times = libcmis_property_getDateTimes( tested );
    CPPUNIT_ASSERT_EQUAL( size, libcmis_vector_time_t_size( times ) ); 
    libcmis_vector_time_t_free( times );

    libcmis_vector_string* strings = libcmis_property_getStrings( tested );
    CPPUNIT_ASSERT_EQUAL( size, libcmis_vector_string_size( strings ) );
    CPPUNIT_ASSERT_EQUAL( string( values[1] ), string( libcmis_vector_string_get( strings, 1 ) ) );
    libcmis_vector_string_free( strings );
    delete[] values;

    libcmis_vector_bool* bools = libcmis_property_getBools( tested );
    CPPUNIT_ASSERT_EQUAL( size_t( 0 ), libcmis_vector_bool_size( bools ) );
    libcmis_vector_bool_free( bools );

    libcmis_property_free( tested );
    libcmis_property_type_free( type );
}

void PropertyTest::getBoolsTest( )
{
    libcmis_PropertyTypePtr type = getTestType( "boolean" );
    size_t size = 2;
    const char** values = new const char*[size];
    values[0] = "true";
    values[1] = "false";
    libcmis_PropertyPtr tested  = libcmis_property_create( type, values, size );

    libcmis_vector_bool* bools = libcmis_property_getBools( tested );
    CPPUNIT_ASSERT_EQUAL( size, libcmis_vector_bool_size( bools ) );
    CPPUNIT_ASSERT_EQUAL( true, libcmis_vector_bool_get( bools, 0 ) );
    CPPUNIT_ASSERT_EQUAL( false, libcmis_vector_bool_get( bools, 1 ) );
    libcmis_vector_bool_free( bools );

    libcmis_vector_string* strings = libcmis_property_getStrings( tested );
    CPPUNIT_ASSERT_EQUAL( size, libcmis_vector_string_size( strings ) );
    CPPUNIT_ASSERT_EQUAL( string( values[1] ), string( libcmis_vector_string_get( strings, 1 ) ) );
    libcmis_vector_string_free( strings );
    delete[] values;

    libcmis_vector_long* longs = libcmis_property_getLongs( tested );
    CPPUNIT_ASSERT_EQUAL( size_t( 0 ), libcmis_vector_long_size( longs ) );
    libcmis_vector_long_free( longs );

    libcmis_property_free( tested );
    libcmis_property_type_free( type );
}
