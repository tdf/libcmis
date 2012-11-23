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

#include "allowable-actions.h"
#include "error.h"
#include "folder.h"
#include "internals.hxx"
#include "object.h"
#include "object-type.h"
#include "property.h"
#include "property-type.h"
#include "test-dummies.hxx"
#include "vectors.h"

using namespace std;

class ObjectTest : public CppUnit::TestFixture
{
    private:
        libcmis_ObjectPtr getTested( bool triggersFaults );
        libcmis_FolderPtr getTestFolder( );

    public:
        void getIdTest( );
        void getNameTest( );
        void getPathsTest( );
        void getBaseTypeTest( );
        void getTypeTest( );
        void getCreatedByTest( );
        void getCreationDateTest( );
        void getLastModifiedByTest( );
        void getLastModificationDateTest( );
        void getChangeTokenTest( );
        void isImmutableTest( );
        void getPropertiesTest( );
        void getPropertyTest( );
        void getPropertyMissingTest( );
        void updatePropertiesTest( );
        void updatePropertiesErrorTest( );
        void getTypeDescriptionTest( );
        void getAllowableActionsTest( );
        void refreshTest( );
        void refreshErrorTest( );
        void removeTest( );
        void removeErrorTest( );
        void moveTest( );
        void moveErrorTest( );
        void toStringTest( );

        CPPUNIT_TEST_SUITE( ObjectTest );
        CPPUNIT_TEST( getIdTest );
        CPPUNIT_TEST( getNameTest );
        CPPUNIT_TEST( getPathsTest );
        CPPUNIT_TEST( getBaseTypeTest );
        CPPUNIT_TEST( getTypeTest );
        CPPUNIT_TEST( getCreatedByTest );
        CPPUNIT_TEST( getCreationDateTest );
        CPPUNIT_TEST( getLastModifiedByTest );
        CPPUNIT_TEST( getLastModificationDateTest );
        CPPUNIT_TEST( getChangeTokenTest );
        CPPUNIT_TEST( isImmutableTest );
        CPPUNIT_TEST( getPropertiesTest );
        CPPUNIT_TEST( getPropertyTest );
        CPPUNIT_TEST( getPropertyMissingTest );
        CPPUNIT_TEST( updatePropertiesTest );
        CPPUNIT_TEST( updatePropertiesErrorTest );
        CPPUNIT_TEST( getTypeDescriptionTest );
        CPPUNIT_TEST( getAllowableActionsTest );
        CPPUNIT_TEST( refreshTest );
        CPPUNIT_TEST( refreshErrorTest );
        CPPUNIT_TEST( removeTest );
        CPPUNIT_TEST( removeErrorTest );
        CPPUNIT_TEST( moveTest );
        CPPUNIT_TEST( moveErrorTest );
        CPPUNIT_TEST( toStringTest );
        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( ObjectTest );

libcmis_ObjectPtr ObjectTest::getTested( bool triggersFaults )
{
    libcmis_ObjectPtr result = new libcmis_object( );
    libcmis::ObjectPtr handle( new dummies::Object( triggersFaults ) );
    result->handle = handle;

    return result;
}

libcmis_FolderPtr ObjectTest::getTestFolder( )
{
    libcmis_FolderPtr result = new libcmis_folder( );
    libcmis::FolderPtr handle( new dummies::Folder( false, false ) );
    result->setHandle( handle );

    return result;
}

void ObjectTest::getIdTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    char* actual = libcmis_object_getId( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Object::Id" ), string( actual ) );
    free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::getNameTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    char* actual = libcmis_object_getName( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Object::Name" ), string( actual ) );
    free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::getPathsTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    libcmis_vector_string_Ptr actual = libcmis_object_getPaths( tested );
    CPPUNIT_ASSERT_EQUAL( size_t( 2 ), libcmis_vector_string_size( actual ) );
    CPPUNIT_ASSERT_EQUAL(
            string( "/Path1/" ),
            string( libcmis_vector_string_get( actual, 0 ) ) );
    libcmis_vector_string_free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::getBaseTypeTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    char* actual = libcmis_object_getBaseType( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Object::BaseType" ), string( actual ) );
    free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::getTypeTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    char* actual = libcmis_object_getType( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Object::Type" ), string( actual ) );
    free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::getCreatedByTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    char* actual = libcmis_object_getCreatedBy( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Object::CreatedBy" ), string( actual ) );
    free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::getCreationDateTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    time_t actual = libcmis_object_getCreationDate( tested );
    CPPUNIT_ASSERT( 0 != actual );
    libcmis_object_free( tested );
}

void ObjectTest::getLastModifiedByTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    char* actual = libcmis_object_getLastModifiedBy( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Object::LastModifiedBy" ), string( actual ) );
    free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::getLastModificationDateTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    time_t actual = libcmis_object_getLastModificationDate( tested );
    CPPUNIT_ASSERT( 0 != actual );
    libcmis_object_free( tested );
}

void ObjectTest::getChangeTokenTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    char* actual = libcmis_object_getChangeToken( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Object::ChangeToken" ), string( actual ) );
    free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::isImmutableTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    CPPUNIT_ASSERT( libcmis_object_isImmutable( tested ) );
    libcmis_object_free( tested );
}

void ObjectTest::getPropertiesTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    libcmis_vector_property_Ptr actual = libcmis_object_getProperties( tested );
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), libcmis_vector_property_size( actual ) );
    libcmis_vector_property_free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::getPropertyTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    const char* id = "Property1";
    libcmis_PropertyPtr actual = libcmis_object_getProperty( tested, id );
    CPPUNIT_ASSERT( NULL != actual );
    libcmis_property_free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::getPropertyMissingTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    const char* id = "MissingProperty";
    libcmis_PropertyPtr actual = libcmis_object_getProperty( tested, id );
    CPPUNIT_ASSERT( NULL == actual );
    libcmis_property_free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::updatePropertiesTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not set to 0 initially", 0 == libcmis_object_getRefreshTimestamp( tested ) );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // Create the changed properties map
    libcmis_vector_property_Ptr newProperties = libcmis_vector_property_create( );
    libcmis_ObjectTypePtr objectType = libcmis_object_getTypeDescription( tested );
    libcmis_PropertyTypePtr propertyType = libcmis_object_type_getPropertyType( objectType, "cmis:Property2" );
    size_t size = 2;
    const char** values = new const char*[size];
    values[0] = "Value 1";
    values[1] = "Value 2";
    libcmis_PropertyPtr newProperty = libcmis_property_create( propertyType, values, size );
    delete[ ] values;
    libcmis_vector_property_append( newProperties, newProperty );
    
    // Update the properties (method under test)
    libcmis_ObjectPtr updated = libcmis_object_updateProperties( tested, newProperties, error );

    // Checks
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not updated", 0 != libcmis_object_getRefreshTimestamp( tested ) );
    CPPUNIT_ASSERT( updated != NULL );

    // Free it all
    libcmis_object_free( updated );
    libcmis_property_free( newProperty );
    libcmis_property_type_free( propertyType );
    libcmis_object_type_free( objectType );
    libcmis_vector_property_free( newProperties );
    libcmis_error_free( error );
    libcmis_object_free( tested );
}

void ObjectTest::updatePropertiesErrorTest( )
{
    libcmis_ObjectPtr tested = getTested( true );
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not set to 0 initially", 0 == libcmis_object_getRefreshTimestamp( tested ) );
    libcmis_ErrorPtr error = libcmis_error_create( );
    
    // Create the changed properties map
    libcmis_vector_property_Ptr newProperties = libcmis_vector_property_create( );
    libcmis_ObjectTypePtr objectType = libcmis_object_getTypeDescription( tested );
    libcmis_PropertyTypePtr propertyType = libcmis_object_type_getPropertyType( objectType, "cmis:Property2" );
    size_t size = 2;
    const char** values = new const char*[size];
    values[0] = "Value 1";
    values[1] = "Value 2";
    libcmis_PropertyPtr newProperty = libcmis_property_create( propertyType, values, size );
    delete[ ] values;
    libcmis_vector_property_append( newProperties, newProperty );
    
    // Update the properties (method under test)
    libcmis_ObjectPtr updated = libcmis_object_updateProperties( tested, newProperties, error );

    // Checks
    CPPUNIT_ASSERT( updated == NULL );
    const char* actualMessage = libcmis_error_getMessage( error );
    CPPUNIT_ASSERT( !string( actualMessage ).empty( ) );

    // Free it all
    libcmis_object_free( updated );
    libcmis_property_free( newProperty );
    libcmis_property_type_free( propertyType );
    libcmis_object_type_free( objectType );
    libcmis_vector_property_free( newProperties );
    libcmis_error_free( error );
    libcmis_object_free( tested );
}

void ObjectTest::getTypeDescriptionTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    libcmis_ObjectTypePtr actual = libcmis_object_getTypeDescription( tested );
    char* actualId = libcmis_object_type_getId( actual );
    CPPUNIT_ASSERT( !string( actualId ).empty( ) );
    free( actualId );
    libcmis_object_type_free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::getAllowableActionsTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    libcmis_AllowableActionsPtr actual = libcmis_object_getAllowableActions( tested );
    CPPUNIT_ASSERT( libcmis_allowable_actions_isDefined( actual, libcmis_GetFolderParent ) );
    libcmis_allowable_actions_free( actual );
    libcmis_object_free( tested );
}

void ObjectTest::refreshTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not set to 0 initially", 0 == libcmis_object_getRefreshTimestamp( tested ) );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_object_refresh( tested, error );
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not updated", 0 != libcmis_object_getRefreshTimestamp( tested ) );
    libcmis_error_free( error );
    libcmis_object_free( tested );
}

void ObjectTest::refreshErrorTest( )
{
    libcmis_ObjectPtr tested = getTested( true );
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not set to 0 initially", 0 == libcmis_object_getRefreshTimestamp( tested ) );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_object_refresh( tested, error );
    const char* actualMessage = libcmis_error_getMessage( error );
    CPPUNIT_ASSERT( !string( actualMessage ).empty( ) );
    libcmis_error_free( error );
    libcmis_object_free( tested );
}

void ObjectTest::removeTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not set to 0 initially", 0 == libcmis_object_getRefreshTimestamp( tested ) );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_object_remove( tested, true, error );
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not updated", 0 != libcmis_object_getRefreshTimestamp( tested ) );
    libcmis_error_free( error );
    libcmis_object_free( tested );
}

void ObjectTest::removeErrorTest( )
{
    libcmis_ObjectPtr tested = getTested( true );
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not set to 0 initially", 0 == libcmis_object_getRefreshTimestamp( tested ) );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_object_remove( tested, true, error );
    const char* actualMessage = libcmis_error_getMessage( error );
    CPPUNIT_ASSERT( !string( actualMessage ).empty( ) );
    libcmis_error_free( error );
    libcmis_object_free( tested );
}

void ObjectTest::moveTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not set to 0 initially", 0 == libcmis_object_getRefreshTimestamp( tested ) );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // Move the object from source to dest (tested method)
    libcmis_FolderPtr source = getTestFolder( );
    libcmis_FolderPtr dest = getTestFolder( );
    libcmis_object_move( tested, source, dest, error );

    // Check
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not updated", 0 != libcmis_object_getRefreshTimestamp( tested ) );

    // Free it all
    libcmis_folder_free( dest );
    libcmis_folder_free( source );
    libcmis_error_free( error );
    libcmis_object_free( tested );
}

void ObjectTest::moveErrorTest( )
{
}

void ObjectTest::toStringTest( )
{
    libcmis_ObjectPtr tested = getTested( false );
    char* actual = libcmis_object_toString( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Object::toString" ), string( actual ) );
    free( actual );
    libcmis_object_free( tested );
}
