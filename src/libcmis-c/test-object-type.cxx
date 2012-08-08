
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
#include "object-type.h"
#include "test-dummies.hxx"

using namespace std;

class ObjectTypeTest : public CppUnit::TestFixture
{
    private:
        libcmis_ObjectTypePtr getTested( bool rootType, bool triggersFaults );

    public:
        void getIdTest( );
        void getLocalNameTest( );
        void getLocalNamespaceTest( );
        void getQueryNameTest( );
        void getDisplayNameTest( );
        void getDescriptionTest( );
        void getParentTypeTest( );
        void getParentTypeRootTest( );
        void getParentTypeErrorTest( );
        void getBaseTypeTest( );
        void getBaseTypeErrorTest( );
        void getChildrenTest( );
        void getChildrenErrorTest( );
        void isCreatableTest( );
        void isFileableTest( );
        void isQueryableTest( );
        void isFulltextIndexedTest( );
        void isIncludedInSupertypeQueryTest( );
        void isControllablePolicyTest( );
        void isControllableACLTest( );
        void isVersionableTest( );
        void getContentStreamAllowedTest( );
        void getPropertiesTypesTest( );
        void getPropertyTypeTest( );
        void toStringTest( );

        // TODO Add more tests

        CPPUNIT_TEST_SUITE( ObjectTypeTest );
        CPPUNIT_TEST( getIdTest );
        CPPUNIT_TEST( getLocalNameTest );
        CPPUNIT_TEST( getLocalNamespaceTest );
        CPPUNIT_TEST( getQueryNameTest );
        CPPUNIT_TEST( getDisplayNameTest );
        CPPUNIT_TEST( getDescriptionTest );
        CPPUNIT_TEST( getParentTypeTest );
        CPPUNIT_TEST( getParentTypeRootTest );
        CPPUNIT_TEST( getParentTypeErrorTest );
        CPPUNIT_TEST( getBaseTypeTest );
        CPPUNIT_TEST( getBaseTypeErrorTest );
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST( getChildrenErrorTest );
        CPPUNIT_TEST( isCreatableTest );
        CPPUNIT_TEST( isFileableTest );
        CPPUNIT_TEST( isQueryableTest );
        CPPUNIT_TEST( isFulltextIndexedTest );
        CPPUNIT_TEST( isIncludedInSupertypeQueryTest );
        CPPUNIT_TEST( isControllablePolicyTest );
        CPPUNIT_TEST( isControllableACLTest );
        CPPUNIT_TEST( isVersionableTest );
        CPPUNIT_TEST( getContentStreamAllowedTest );
        CPPUNIT_TEST( getPropertiesTypesTest );
        CPPUNIT_TEST( getPropertyTypeTest );
        CPPUNIT_TEST( toStringTest );
        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( ObjectTypeTest );

libcmis_ObjectTypePtr ObjectTypeTest::getTested( bool rootType, bool triggersFaults )
{
    libcmis_ObjectTypePtr result = new libcmis_object_type( );
    libcmis::ObjectTypePtr handle( new dummies::ObjectType( rootType, triggersFaults ) );
    result->handle = handle;

    return result;
}

void ObjectTypeTest::getIdTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    char* id = libcmis_object_type_getId( tested );
    CPPUNIT_ASSERT_EQUAL(
            string( "ObjectType::Id" ),
            string( id ) );
    free( id );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getLocalNameTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    char* actual = libcmis_object_type_getLocalName( tested );
    CPPUNIT_ASSERT_EQUAL(
            string( "ObjectType::LocalName" ),
            string( actual ) );
    free( actual );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getLocalNamespaceTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    char* actual = libcmis_object_type_getLocalNamespace( tested );
    CPPUNIT_ASSERT_EQUAL(
            string( "ObjectType::LocalNamespace" ),
            string( actual ) );
    free( actual );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getQueryNameTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    char* actual = libcmis_object_type_getQueryName( tested );
    CPPUNIT_ASSERT_EQUAL(
            string( "ObjectType::QueryName" ),
            string( actual ) );
    free( actual );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getDisplayNameTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    char* actual = libcmis_object_type_getDisplayName( tested );
    CPPUNIT_ASSERT_EQUAL(
            string( "ObjectType::DisplayName" ),
            string( actual ) );
    free( actual );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getDescriptionTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    char* actual = libcmis_object_type_getDescription( tested );
    CPPUNIT_ASSERT_EQUAL(
            string( "ObjectType::Description" ),
            string( actual ) );
    free( actual );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getParentTypeTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_ObjectTypePtr parent = libcmis_object_type_getParentType( tested, error );

    CPPUNIT_ASSERT( string( libcmis_error_getMessage( error ) ).empty( ) );
    char* id = libcmis_object_type_getId( parent );
    CPPUNIT_ASSERT_EQUAL( string( "ParentType::Id" ), string( id ) );
    free( id );

    libcmis_error_free( error );
    libcmis_object_type_free( parent );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getParentTypeRootTest( )
{
    libcmis_ObjectTypePtr tested = getTested( true, false );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_ObjectTypePtr parent = libcmis_object_type_getParentType( tested, error );

    CPPUNIT_ASSERT( string( libcmis_error_getMessage( error ) ).empty( ) );
    CPPUNIT_ASSERT( NULL == parent );

    libcmis_error_free( error );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getParentTypeErrorTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, true );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_ObjectTypePtr parent = libcmis_object_type_getParentType( tested, error );

    CPPUNIT_ASSERT_EQUAL( string( "Fault triggered" ), string( libcmis_error_getMessage( error ) ) );
    CPPUNIT_ASSERT( NULL == parent );

    libcmis_error_free( error );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getBaseTypeTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_ObjectTypePtr base = libcmis_object_type_getBaseType( tested, error );

    CPPUNIT_ASSERT( string( libcmis_error_getMessage( error ) ).empty( ) );
    char* id = libcmis_object_type_getId( base );
    CPPUNIT_ASSERT_EQUAL( string( "RootType::Id" ), string( id ) );
    free( id );

    libcmis_error_free( error );
    libcmis_object_type_free( base );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getBaseTypeErrorTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, true );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_ObjectTypePtr base = libcmis_object_type_getBaseType( tested, error );

    CPPUNIT_ASSERT_EQUAL( string( "Fault triggered" ), string( libcmis_error_getMessage( error ) ) );
    CPPUNIT_ASSERT( NULL == base );

    libcmis_error_free( error );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getChildrenTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_vector_ObjectTypePtr* children = libcmis_object_type_getChildren( tested, error );

    CPPUNIT_ASSERT( string( libcmis_error_getMessage( error ) ).empty( ) );
    size_t size = libcmis_vector_ObjectTypePtr_size( children );
    CPPUNIT_ASSERT_EQUAL( size_t( 2 ), size );

    libcmis_error_free( error );
    libcmis_vector_ObjectTypePtr_free( children );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getChildrenErrorTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, true );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_vector_ObjectTypePtr* children = libcmis_object_type_getChildren( tested, error );

    CPPUNIT_ASSERT_EQUAL( string( "Fault triggered" ), string( libcmis_error_getMessage( error ) ) );
    CPPUNIT_ASSERT( NULL == children );

    libcmis_error_free( error );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::isCreatableTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    CPPUNIT_ASSERT( libcmis_object_type_isCreatable( tested ) );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::isFileableTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    CPPUNIT_ASSERT( libcmis_object_type_isFileable( tested ) );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::isQueryableTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    CPPUNIT_ASSERT( libcmis_object_type_isQueryable( tested ) );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::isFulltextIndexedTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    CPPUNIT_ASSERT( libcmis_object_type_isFulltextIndexed( tested ) );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::isIncludedInSupertypeQueryTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    CPPUNIT_ASSERT( libcmis_object_type_isIncludedInSupertypeQuery( tested ) );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::isControllablePolicyTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    CPPUNIT_ASSERT( libcmis_object_type_isControllablePolicy( tested ) );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::isControllableACLTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    CPPUNIT_ASSERT( libcmis_object_type_isControllableACL( tested ) );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::isVersionableTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    CPPUNIT_ASSERT( libcmis_object_type_isVersionable( tested ) );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getContentStreamAllowedTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    CPPUNIT_ASSERT_EQUAL(
            libcmis_Allowed,
            libcmis_object_type_getContentStreamAllowed( tested ) );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getPropertiesTypesTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    libcmis_vector_PropertyTypePtr* propertiesTypes = libcmis_object_type_getPropertiesTypes( tested );
    CPPUNIT_ASSERT_EQUAL( size_t( 3 ), libcmis_vector_PropertyTypePtr_size( propertiesTypes ) );
    libcmis_vector_PropertyTypePtr_free( propertiesTypes );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::getPropertyTypeTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    string id( "Property2" );
    libcmis_PropertyTypePtr propertyType = libcmis_object_type_getPropertyType( tested, id.c_str( ) );
    char* propertyId = libcmis_property_type_getId( propertyType );
    CPPUNIT_ASSERT_EQUAL( id, string( propertyId ) );
    free( propertyId );
    libcmis_property_type_free( propertyType );
    libcmis_object_type_free( tested );
}

void ObjectTypeTest::toStringTest( )
{
    libcmis_ObjectTypePtr tested = getTested( false, false );
    char* actual = libcmis_object_type_toString( tested );
    CPPUNIT_ASSERT_EQUAL(
            string( "ObjectType::toString" ),
            string( actual ) );
    free( actual );
    libcmis_object_type_free( tested );
}
