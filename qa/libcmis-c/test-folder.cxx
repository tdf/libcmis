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

#include <libcmis-c/document.h>
#include <libcmis-c/error.h>
#include <libcmis-c/folder.h>
#include <libcmis-c/object.h>
#include <libcmis-c/object-type.h>
#include <libcmis-c/property.h>
#include <libcmis-c/property-type.h>
#include <libcmis-c/vectors.h>

#include "internals.hxx"
#include "test-dummies.hxx"

using namespace std;

class FolderTest : public CppUnit::TestFixture
{
    private:
        libcmis_FolderPtr getTested( bool isRoot, bool triggersFaults );
        dummies::Document* getDocumentImplementation( libcmis_DocumentPtr document );

    public:
        void objectCastTest( );
        void objectCastFailureTest( );
        void objectFunctionsTest( );
        void getParentTest( );
        void getParentRootTest( );
        void getParentErrorTest( );
        void getChildrenTest( );
        void getChildrenErrorTest( );
        void getPathTest( );
        void createFolderTest( );
        void createFolderErrorTest( );
        void createDocumentTest( );
        void createDocumentErrorTest( );
        void removeTreeTest( );
        void removeTreeErrorTest( );

        CPPUNIT_TEST_SUITE( FolderTest );
        CPPUNIT_TEST( objectCastTest );
        CPPUNIT_TEST( objectCastFailureTest );
        CPPUNIT_TEST( objectFunctionsTest );
        CPPUNIT_TEST( getParentTest );
        CPPUNIT_TEST( getParentRootTest );
        CPPUNIT_TEST( getParentErrorTest );
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST( getChildrenErrorTest );
        CPPUNIT_TEST( getPathTest );
        CPPUNIT_TEST( createFolderTest );
        CPPUNIT_TEST( createFolderErrorTest );
        CPPUNIT_TEST( createDocumentTest );
        CPPUNIT_TEST( createDocumentErrorTest );
        CPPUNIT_TEST( removeTreeTest );
        CPPUNIT_TEST( removeTreeErrorTest );
        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( FolderTest );

libcmis_FolderPtr FolderTest::getTested( bool isRoot, bool triggersFaults )
{
    libcmis_FolderPtr result = new libcmis_folder( );
    libcmis::FolderPtr handle( new dummies::Folder( isRoot, triggersFaults ) );
    result->handle = handle;

    return result;
}

dummies::Document* FolderTest::getDocumentImplementation( libcmis_DocumentPtr document )
{
    dummies::Document* impl = dynamic_cast< dummies::Document* >( document->handle.get( ) );
    return impl;
}

void FolderTest::objectCastTest( )
{
    // Create the test object to cast
    libcmis_ObjectPtr tested = new libcmis_object( );
    libcmis::FolderPtr handle( new dummies::Folder( false, false ) );
    tested->handle = handle;

    // Test libcmis_is_folder
    CPPUNIT_ASSERT( libcmis_is_folder( tested ) );

    // Actually cast to a folder
    libcmis_FolderPtr actual = libcmis_folder_cast( tested );

    // Check the result
    CPPUNIT_ASSERT( NULL != actual );

    // Check that the libcmis_object-* functions are working with the cast result
    char* actualId = libcmis_object_getId( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Folder::Id" ), string( actualId ) );
    free( actualId );

    // Free it all
    libcmis_folder_free( actual );
    libcmis_object_free( tested );
}

void FolderTest::objectCastFailureTest( )
{
    // Create the test object to cast
    libcmis_ObjectPtr tested = new libcmis_object( );
    libcmis::DocumentPtr handle( new dummies::Document( true, false ) );
    tested->handle = handle;

    // Test libcmis_is_folder
    CPPUNIT_ASSERT( !libcmis_is_folder( tested ) );

    // Actually cast to a folder
    libcmis_FolderPtr actual = libcmis_folder_cast( tested );

    // Check the result
    CPPUNIT_ASSERT( NULL == actual );

    libcmis_object_free( tested );
}

void FolderTest::objectFunctionsTest( )
{
    libcmis_FolderPtr tested = getTested( false, false );
    char* actual = libcmis_object_getId( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Folder::Id" ), string( actual ) );
    free( actual );
    libcmis_folder_free( tested );
}

void FolderTest::getParentTest( )
{
    libcmis_FolderPtr tested = getTested( false, false );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_FolderPtr parent  = libcmis_folder_getParent( tested, error );
    CPPUNIT_ASSERT( NULL != parent );
    CPPUNIT_ASSERT( !libcmis_folder_isRootFolder( tested ) );
    libcmis_folder_free( parent );
    libcmis_error_free( error );
    libcmis_folder_free( tested );
}

void FolderTest::getParentRootTest( )
{
    libcmis_FolderPtr tested = getTested( true, false );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_FolderPtr parent  = libcmis_folder_getParent( tested, error );
    CPPUNIT_ASSERT( NULL == parent );
    CPPUNIT_ASSERT( libcmis_folder_isRootFolder( tested ) );
    libcmis_error_free( error );
    libcmis_folder_free( tested );
}

void FolderTest::getParentErrorTest( )
{
    libcmis_FolderPtr tested = getTested( false, true );
    libcmis_ErrorPtr error = libcmis_error_create( );
    libcmis_FolderPtr parent = libcmis_folder_getParent( tested, error );
    CPPUNIT_ASSERT( NULL == parent );
    const char* actualMessage = libcmis_error_getMessage( error );
    CPPUNIT_ASSERT( !string( actualMessage ).empty( ) );
    libcmis_error_free( error );
    libcmis_folder_free( tested );
}

void FolderTest::getChildrenTest( )
{
    libcmis_FolderPtr tested = getTested( false, false );
    libcmis_ErrorPtr error = libcmis_error_create( );

    libcmis_vector_object_Ptr children = libcmis_folder_getChildren( tested, error );
    CPPUNIT_ASSERT_EQUAL( size_t( 2 ), libcmis_vector_object_size( children ) );
    libcmis_vector_object_free( children );
    libcmis_error_free( error );
    libcmis_folder_free( tested );
}

void FolderTest::getChildrenErrorTest( )
{
    libcmis_FolderPtr tested = getTested( false, true );
    libcmis_ErrorPtr error = libcmis_error_create( );

    libcmis_vector_object_Ptr children = libcmis_folder_getChildren( tested, error );
    CPPUNIT_ASSERT( NULL == children );
    const char* actualMessage = libcmis_error_getMessage( error );
    CPPUNIT_ASSERT( !string( actualMessage ).empty( ) );
    libcmis_error_free( error );
    libcmis_folder_free( tested );
}

void FolderTest::getPathTest( )
{
    libcmis_FolderPtr tested = getTested( false, false );
    char* actual = libcmis_folder_getPath( tested );
    CPPUNIT_ASSERT_EQUAL( string( "/Path/" ), string( actual ) );
    free( actual );
    libcmis_folder_free( tested );
}

void FolderTest::createFolderTest( )
{
    libcmis_FolderPtr tested = getTested( false, false );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // Create the properties for the new folder
    libcmis_vector_property_Ptr properties = libcmis_vector_property_create( );
    libcmis_ObjectTypePtr objectType = libcmis_object_getTypeDescription( tested );
    const char* id = "Property1";
    libcmis_PropertyTypePtr propertyType = libcmis_object_type_getPropertyType( objectType, id );
    size_t size = 2;
    const char** values = new const char*[size];
    values[0] = "Value 1";
    values[1] = "Value 2";
    libcmis_PropertyPtr property = libcmis_property_create( propertyType, values, size );
    delete[] values;
    libcmis_vector_property_append( properties, property );

    // Create the new folder (method to test)
    libcmis_FolderPtr created = libcmis_folder_createFolder( tested, properties, error );
    
    // Check
    CPPUNIT_ASSERT( NULL != created );

    // Free everything
    libcmis_folder_free( created );
    libcmis_property_free( property );
    libcmis_property_type_free( propertyType );
    libcmis_object_type_free( objectType );
    libcmis_vector_property_free( properties );
    libcmis_error_free( error );
    libcmis_folder_free( tested );
}

void FolderTest::createFolderErrorTest( )
{
    libcmis_FolderPtr tested = getTested( false, true );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // Create the properties for the new folder
    libcmis_vector_property_Ptr properties = libcmis_vector_property_create( );
    libcmis_ObjectTypePtr objectType = libcmis_object_getTypeDescription( tested );
    const char* id = "Property1";
    libcmis_PropertyTypePtr propertyType = libcmis_object_type_getPropertyType( objectType, id );
    size_t size = 2;
    const char** values = new const char*[size];
    values[0] = "Value 1";
    values[1] = "Value 2";
    libcmis_PropertyPtr property = libcmis_property_create( propertyType, values, size );
    delete[] values;
    libcmis_vector_property_append( properties, property );

    // Create the new folder (method to test)
    libcmis_FolderPtr created = libcmis_folder_createFolder( tested, properties, error );
    
    // Check
    CPPUNIT_ASSERT( NULL == created );
    
    const char* actualMessage = libcmis_error_getMessage( error );
    CPPUNIT_ASSERT( !string( actualMessage ).empty( ) );

    // Free everything
    libcmis_folder_free( created );
    libcmis_property_free( property );
    libcmis_property_type_free( propertyType );
    libcmis_object_type_free( objectType );
    libcmis_vector_property_free( properties );
    libcmis_error_free( error );
    libcmis_folder_free( tested );
}

void FolderTest::createDocumentTest( )
{
    libcmis_FolderPtr tested = getTested( true, false );
    libcmis_ErrorPtr error = libcmis_error_create( );
    
    // Prepare the content to set
    FILE* tmp = tmpfile( );
    string expectedStream( "New Content Stream" );
    fwrite( expectedStream.c_str( ), 1, expectedStream.size( ), tmp );
    rewind( tmp );

    // Create the properties for the new version
    libcmis_vector_property_Ptr properties = libcmis_vector_property_create( );
    libcmis_ObjectTypePtr objectType = libcmis_object_getTypeDescription( tested );
    const char* id = "Property1";
    libcmis_PropertyTypePtr propertyType = libcmis_object_type_getPropertyType( objectType, id );
    size_t size = 2;
    const char** values = new const char*[size];
    values[0] = "Value 1";
    values[1] = "Value 2";
    libcmis_PropertyPtr property = libcmis_property_create( propertyType, values, size );
    delete[] values;
    libcmis_vector_property_append( properties, property );

    // get the content into a temporary file (tested method)
    const char* contentType = "content/type";
    const char* filename = "name.txt";
    libcmis_DocumentPtr actual = libcmis_folder_createDocument( tested, properties, 
            ( libcmis_readFn )fread, tmp, contentType, filename, error );
    fclose( tmp );

    // Check
    string actualStream = getDocumentImplementation( actual )->getContentString( );
    CPPUNIT_ASSERT( NULL == libcmis_error_getMessage( error ) );
    CPPUNIT_ASSERT_EQUAL( expectedStream, actualStream );

    libcmis_PropertyPtr checkedProperty = libcmis_object_getProperty( actual, "Property1" );
    libcmis_vector_string_Ptr newValues = libcmis_property_getStrings( checkedProperty );
    CPPUNIT_ASSERT_EQUAL( size_t( 2 ), libcmis_vector_string_size( newValues ) );

    // Free it all
    libcmis_vector_string_free( newValues );
    libcmis_property_free( checkedProperty );
    libcmis_document_free( actual );
    libcmis_property_free( property );
    libcmis_property_type_free( propertyType );
    libcmis_object_type_free( objectType );
    libcmis_vector_property_free( properties );
    libcmis_error_free( error );
    libcmis_folder_free( tested );
}

void FolderTest::createDocumentErrorTest( )
{
    libcmis_FolderPtr tested = getTested( true, true );
    libcmis_ErrorPtr error = libcmis_error_create( );
    
    // Prepare the content to set
    FILE* tmp = tmpfile( );
    string newStream( "New Content Stream" );
    fwrite( newStream.c_str( ), 1, newStream.size( ), tmp );
    rewind( tmp );

    // Create the properties for the new version
    libcmis_vector_property_Ptr properties = libcmis_vector_property_create( );
    libcmis_ObjectTypePtr objectType = libcmis_object_getTypeDescription( tested );
    const char* id = "Property1";
    libcmis_PropertyTypePtr propertyType = libcmis_object_type_getPropertyType( objectType, id );
    size_t size = 2;
    const char** values = new const char*[size];
    values[0] = "Value 1";
    values[1] = "Value 2";
    libcmis_PropertyPtr property = libcmis_property_create( propertyType, values, size );
    delete[] values;
    libcmis_vector_property_append( properties, property );

    // get the content into a temporary file (tested method)
    const char* contentType = "content/type";
    const char* filename = "name.txt";
    libcmis_DocumentPtr actual = libcmis_folder_createDocument( tested, properties, 
            ( libcmis_readFn )fread, tmp, contentType, filename, error );
    fclose( tmp );

    // Check
    CPPUNIT_ASSERT( !string( libcmis_error_getMessage( error ) ).empty( ) );
    CPPUNIT_ASSERT( NULL == actual );

    // Free it all
    libcmis_property_free( property );
    libcmis_property_type_free( propertyType );
    libcmis_object_type_free( objectType );
    libcmis_vector_property_free( properties );
    libcmis_error_free( error );
    libcmis_folder_free( tested );
}

void FolderTest::removeTreeTest( )
{
    libcmis_FolderPtr tested = getTested( false, false );
    libcmis_ErrorPtr error = libcmis_error_create( );
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not set to 0 initially", 0 == libcmis_object_getRefreshTimestamp( tested ) );

    // Remove the tree (method to test)
    libcmis_vector_string_Ptr failed = libcmis_folder_removeTree( tested, true, libcmis_Delete, true, error );
    
    // Check
    CPPUNIT_ASSERT_MESSAGE( "Timestamp not updated", 0 != libcmis_object_getRefreshTimestamp( tested ) );
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), libcmis_vector_string_size( failed ) );
    CPPUNIT_ASSERT_EQUAL( string( "failed 1" ), string( libcmis_vector_string_get( failed, 0 ) ) );

    // Free everything
    libcmis_vector_string_free( failed );
    libcmis_error_free( error );
    libcmis_folder_free( tested );
}

void FolderTest::removeTreeErrorTest( )
{
    libcmis_FolderPtr tested = getTested( false, true );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // Remove the tree (method to test)
    libcmis_vector_string_Ptr failed = libcmis_folder_removeTree(
            tested, true, libcmis_Delete, true, error );
    
    // Check
    const char* actualMessage = libcmis_error_getMessage( error );
    CPPUNIT_ASSERT( !string( actualMessage ).empty( ) );

    // Free everything
    libcmis_vector_string_free( failed );
    libcmis_error_free( error );
    libcmis_folder_free( tested );
}
