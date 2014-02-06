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

#include "error.h"
#include "folder.h"
#include "internals.hxx"
#include "document.h"
#include "object.h"
#include "object-type.h"
#include "property.h"
#include "property-type.h"
#include "test-dummies.hxx"
#include "vectors.h"

using namespace std;

extern bool isOutOfMemory;

namespace
{
    string lcl_readFile( FILE* file )
    {
        // Get the size
        fseek( file, 0, SEEK_END );
        long size = ftell( file );
        rewind( file );

        char* buf = new char[size + 1];
        size_t readbytes = fread( buf, 1, size, file );
        buf[ readbytes ] = '\0';

        string result( buf );
        delete[] buf;

        return result;
    }
}

class DocumentTest : public CppUnit::TestFixture
{
    private:
        libcmis_DocumentPtr getTested( bool isFiled, bool triggersFaults );
        dummies::Document* getTestedImplementation( libcmis_DocumentPtr document );

    public:
        void objectCastTest( );
        void objectCastFailureTest( );
        void objectFunctionsTest( );
        void getParentsTest( );
        void getParentsUnfiledTest( );
        void getParentsErrorTest( );
        void getContentStreamTest( );
        void getContentStreamErrorTest( );
        void getContentStreamBadAllocTest( );
        void setContentStreamTest( );
        void setContentStreamErrorTest( );
        void getContentTypeTest( );
        void getContentFilenameTest( );
        void getContentLengthTest( );
        void checkOutTest( );
        void checkOutErrorTest( );
        void cancelCheckoutTest( );
        void cancelCheckoutErrorTest( );
        void checkInTest( );
        void checkInErrorTest( );
        void getAllVersionsTest( );
        void getAllVersionsErrorTest( );

        CPPUNIT_TEST_SUITE( DocumentTest );
        CPPUNIT_TEST( objectCastTest );
        CPPUNIT_TEST( objectCastFailureTest );
        CPPUNIT_TEST( objectFunctionsTest );
        CPPUNIT_TEST( getParentsTest );
        CPPUNIT_TEST( getParentsUnfiledTest );
        CPPUNIT_TEST( getParentsErrorTest );
        CPPUNIT_TEST( getContentStreamTest );
        CPPUNIT_TEST( getContentStreamErrorTest );
        CPPUNIT_TEST( getContentStreamBadAllocTest );
        CPPUNIT_TEST( setContentStreamTest );
        CPPUNIT_TEST( setContentStreamErrorTest );
        CPPUNIT_TEST( getContentTypeTest );
        CPPUNIT_TEST( getContentFilenameTest );
        CPPUNIT_TEST( getContentLengthTest );
        CPPUNIT_TEST( checkOutTest );
        CPPUNIT_TEST( checkOutErrorTest );
        CPPUNIT_TEST( cancelCheckoutTest );
        CPPUNIT_TEST( cancelCheckoutErrorTest );
        CPPUNIT_TEST( checkInTest );
        CPPUNIT_TEST( checkInErrorTest );
        CPPUNIT_TEST( getAllVersionsTest );
        CPPUNIT_TEST( getAllVersionsErrorTest );
        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( DocumentTest );

libcmis_DocumentPtr DocumentTest::getTested( bool isFiled, bool triggersFaults )
{
    // Create the document
    libcmis_DocumentPtr result = new libcmis_document( );
    libcmis::DocumentPtr handle( new dummies::Document( isFiled, triggersFaults ) );
    result->handle = handle;

    return result;
}

dummies::Document* DocumentTest::getTestedImplementation( libcmis_DocumentPtr document )
{
    dummies::Document* impl = dynamic_cast< dummies::Document* >( document->handle.get( ) );
    return impl;
}

void DocumentTest::objectCastTest( )
{
    // Create the test object to cast
    libcmis_ObjectPtr tested = new libcmis_object( );
    libcmis::DocumentPtr handle( new dummies::Document( true, false ) );
    tested->handle = handle;

    // Test libcmis_is_document
    CPPUNIT_ASSERT( libcmis_is_document( tested ) );

    // Actually cast to a document
    libcmis_DocumentPtr actual = libcmis_document_cast( tested );

    // Check the result
    CPPUNIT_ASSERT( NULL != actual );

    // Check that the libcmis_object-* functions are working with the cast result
    char* actualId = libcmis_object_getId( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Document::Id" ), string( actualId ) );
    free( actualId );

    // Free it all
    libcmis_document_free( actual );
    libcmis_object_free( tested );
}

void DocumentTest::objectCastFailureTest( )
{
    // Create the test object to cast
    libcmis_ObjectPtr tested = new libcmis_object( );
    libcmis::FolderPtr handle( new dummies::Folder( false, false ) );
    tested->handle = handle;

    // Test libcmis_is_document
    CPPUNIT_ASSERT( !libcmis_is_document( tested ) );

    // Actually cast to a document
    libcmis_DocumentPtr actual = libcmis_document_cast( tested );

    // Check the result
    CPPUNIT_ASSERT( NULL == actual );

    libcmis_object_free( tested );
}

void DocumentTest::objectFunctionsTest( )
{
    libcmis_DocumentPtr tested = getTested( true, false );
    char* actual = libcmis_object_getId( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Document::Id" ), string( actual ) );
    free( actual );
    libcmis_document_free( tested );
}

void DocumentTest::getParentsTest( )
{
    libcmis_DocumentPtr tested = getTested( true, false );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // get the parent folders (tested method)
    libcmis_vector_folder_Ptr actual = libcmis_document_getParents( tested, error );

    // Check
    CPPUNIT_ASSERT_EQUAL( size_t( 2 ), libcmis_vector_folder_size( actual ) );

    // Free it all
    libcmis_vector_folder_free( actual );
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::getParentsUnfiledTest( )
{
    libcmis_DocumentPtr tested = getTested( false, false );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // get the parent folders (tested method)
    libcmis_vector_folder_Ptr actual = libcmis_document_getParents( tested, error );

    // Check
    CPPUNIT_ASSERT_EQUAL( size_t( 0 ), libcmis_vector_folder_size( actual ) );

    // Free it all
    libcmis_vector_folder_free( actual );
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::getParentsErrorTest( )
{
    libcmis_DocumentPtr tested = getTested( true, true );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // get the parent folders (tested method)
    libcmis_vector_folder_Ptr actual = libcmis_document_getParents( tested, error );

    // Check
    CPPUNIT_ASSERT( NULL == actual );
    CPPUNIT_ASSERT( !string( libcmis_error_getMessage( error ) ).empty( ) );

    // Free it all
    libcmis_vector_folder_free( actual );
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::getContentStreamTest( )
{
    libcmis_DocumentPtr tested = getTested( true, false );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // get the content into a temporary file (tested method)
    FILE* tmp = tmpfile( );
    libcmis_document_getContentStream( tested, 
            ( libcmis_writeFn )fwrite, tmp, error );

    // Check
    string expected = getTestedImplementation( tested )->getContentString( );
    
    string actual = lcl_readFile( tmp );
    fclose( tmp );
    CPPUNIT_ASSERT( NULL == libcmis_error_getMessage( error ) );
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    // Free it all
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::getContentStreamErrorTest( )
{
    libcmis_DocumentPtr tested = getTested( true, true );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // get the content into a temporary file (tested method)
    FILE* tmp = tmpfile( );
    libcmis_document_getContentStream( tested, 
            ( libcmis_writeFn )fwrite, tmp, error );

    // Check
    string actual = lcl_readFile( tmp );
    fclose( tmp );
    CPPUNIT_ASSERT_EQUAL( string( ), actual );
    CPPUNIT_ASSERT( !string( libcmis_error_getMessage( error ) ).empty( ) );

    // Free it all
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::getContentStreamBadAllocTest( )
{
    libcmis_DocumentPtr tested = getTested( true, false );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // get the content into a temporary file (tested method)
    FILE* tmp = tmpfile( );

    isOutOfMemory= true;
    libcmis_document_getContentStream( tested, 
            ( libcmis_writeFn )fwrite, tmp, error );
    isOutOfMemory = false;

    // Check
    string actual = lcl_readFile( tmp );
    fclose( tmp );
    CPPUNIT_ASSERT( !string( libcmis_error_getMessage( error ) ).empty() );
    CPPUNIT_ASSERT_EQUAL( string( ), actual );

    // Free it all
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::setContentStreamTest( )
{
    libcmis_DocumentPtr tested = getTested( true, false );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // Prepare the content to set
    FILE* tmp = tmpfile( );
    string expected( "New Content Stream" );
    fwrite( expected.c_str( ), 1, expected.size( ), tmp );
    rewind( tmp );

    // get the content into a temporary file (tested method)
    const char* contentType = "content/type";
    const char* filename = "name.txt";
    libcmis_document_setContentStream( tested, 
            ( libcmis_readFn )fread, tmp, contentType, filename, true, error );
    fclose( tmp );

    // Check
    string actual = getTestedImplementation( tested )->getContentString( );
    CPPUNIT_ASSERT( NULL == libcmis_error_getMessage( error ) );
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    // Free it all
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::setContentStreamErrorTest( )
{
    libcmis_DocumentPtr tested = getTested( true, true );
    libcmis_ErrorPtr error = libcmis_error_create( );
    
    string expected = getTestedImplementation( tested )->getContentString( );

    // Prepare the content to set
    FILE* tmp = tmpfile( );
    string newContent( "New Content Stream" );
    fwrite( newContent.c_str( ), 1, newContent.size( ), tmp );
    rewind( tmp );

    // get the content into a temporary file (tested method)
    const char* contentType = "content/type";
    const char* filename = "name.txt";
    libcmis_document_setContentStream( tested, 
            ( libcmis_readFn )fread, tmp, contentType, filename, true, error );
    fclose( tmp );

    // Check
    string actual = getTestedImplementation( tested )->getContentString( );
    CPPUNIT_ASSERT( !string( libcmis_error_getMessage( error ) ).empty( ) );
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    // Free it all
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::getContentTypeTest( )
{
    libcmis_DocumentPtr tested = getTested( true, false );
    char* actual = libcmis_document_getContentType( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Document::ContentType" ), string( actual ) );
    free( actual );
    libcmis_document_free( tested );
}

void DocumentTest::getContentFilenameTest( )
{
    libcmis_DocumentPtr tested = getTested( true, false );
    char* actual = libcmis_document_getContentFilename( tested );
    CPPUNIT_ASSERT_EQUAL( string( "Document::ContentFilename" ), string( actual ) );
    free( actual );
    libcmis_document_free( tested );
}

void DocumentTest::getContentLengthTest( )
{
    libcmis_DocumentPtr tested = getTested( true, false );
    long actual = libcmis_document_getContentLength( tested );
    CPPUNIT_ASSERT( 0 != actual );
    libcmis_document_free( tested );
}

void DocumentTest::checkOutTest( )
{
    libcmis_DocumentPtr tested = getTested( true, false );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // checkout a private working copy (tested method)
    libcmis_DocumentPtr actual = libcmis_document_checkOut( tested, error );

    // Check
    CPPUNIT_ASSERT( NULL != actual );

    // Free it all
    libcmis_document_free( actual );
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::checkOutErrorTest( )
{
    libcmis_DocumentPtr tested = getTested( true, true );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // checkout a private working copy (tested method)
    libcmis_DocumentPtr actual = libcmis_document_checkOut( tested, error );

    // Check
    CPPUNIT_ASSERT( NULL == actual );
    CPPUNIT_ASSERT( !string( libcmis_error_getMessage( error ) ).empty( ) );

    // Free it all
    libcmis_document_free( actual );
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::cancelCheckoutTest( )
{
    libcmis_DocumentPtr tested = getTested( true, false );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // checkout a private working copy (tested method)
    libcmis_document_cancelCheckout( tested, error );

    // Check
    CPPUNIT_ASSERT( 0 != libcmis_object_getRefreshTimestamp( tested ) );

    // Free it all
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::cancelCheckoutErrorTest( )
{
    libcmis_DocumentPtr tested = getTested( true, true );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // checkout a private working copy (tested method)
    libcmis_document_cancelCheckout( tested, error );

    // Check
    CPPUNIT_ASSERT( 0 == libcmis_object_getRefreshTimestamp( tested ) );
    CPPUNIT_ASSERT( !string( libcmis_error_getMessage( error ) ).empty( ) );

    // Free it all
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::checkInTest( )
{
    libcmis_DocumentPtr tested = getTested( true, false );
    libcmis_ErrorPtr error = libcmis_error_create( );
    
    // Prepare the content to set
    FILE* tmp = tmpfile( );
    string expected( "New Content Stream" );
    fwrite( expected.c_str( ), 1, expected.size( ), tmp );
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
    const char* comment = "Version comment";
    const char* filename = "filename.txt";
    libcmis_DocumentPtr newVersion = libcmis_document_checkIn(
            tested, true, comment, properties, 
            ( libcmis_readFn )fread, tmp, contentType, filename, error );
    fclose( tmp );

    // Check
    CPPUNIT_ASSERT( NULL != newVersion );
    
    string actual = getTestedImplementation( tested )->getContentString( );
    CPPUNIT_ASSERT( NULL == libcmis_error_getMessage( error ) );
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    libcmis_PropertyPtr checkedProperty = libcmis_object_getProperty( tested, "Property1" );
    libcmis_vector_string_Ptr newValues = libcmis_property_getStrings( checkedProperty );
    CPPUNIT_ASSERT_EQUAL( size_t( 2 ), libcmis_vector_string_size( newValues ) );

    // Free it all
    libcmis_vector_string_free( newValues );
    libcmis_property_free( checkedProperty );
    libcmis_document_free( newVersion );
    libcmis_property_free( property );
    libcmis_property_type_free( propertyType );
    libcmis_object_type_free( objectType );
    libcmis_vector_property_free( properties );
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::checkInErrorTest( )
{
    libcmis_DocumentPtr tested = getTested( true, true );
    libcmis_ErrorPtr error = libcmis_error_create( );
    
    string expected = getTestedImplementation( tested )->getContentString( );

    // Prepare the content to set
    FILE* tmp = tmpfile( );
    string newContent( "New Content Stream" );
    fwrite( newContent.c_str( ), 1, newContent.size( ), tmp );
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
    const char* comment = "Version comment";
    const char* filename = "filename.txt";
    libcmis_DocumentPtr newVersion = libcmis_document_checkIn(
            tested, true, comment, properties, 
            ( libcmis_readFn )fread, tmp, contentType, filename, error );
    fclose( tmp );

    // Check
    CPPUNIT_ASSERT( NULL == newVersion );

    string actual = getTestedImplementation( tested )->getContentString( );
    CPPUNIT_ASSERT( !string( libcmis_error_getMessage( error ) ).empty( ) );
    CPPUNIT_ASSERT_EQUAL( expected, actual );

    libcmis_PropertyPtr checkedProperty = libcmis_object_getProperty( tested, "Property1" );
    libcmis_vector_string_Ptr newValues = libcmis_property_getStrings( checkedProperty );
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), libcmis_vector_string_size( newValues ) );

    // Free it all
    libcmis_vector_string_free( newValues );
    libcmis_property_free( checkedProperty );
    libcmis_document_free( newVersion );
    libcmis_property_free( property );
    libcmis_property_type_free( propertyType );
    libcmis_object_type_free( objectType );
    libcmis_vector_property_free( properties );
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::getAllVersionsTest( )
{
    libcmis_DocumentPtr tested = getTested( true, false );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // get all versions (tested method)
    libcmis_vector_document_Ptr versions = libcmis_document_getAllVersions( tested, error );

    // Check
    CPPUNIT_ASSERT_EQUAL( size_t( 2 ), libcmis_vector_document_size( versions ) );
    libcmis_DocumentPtr actualVersion = libcmis_vector_document_get( versions, 0 );
    char* actualId = libcmis_object_getId( actualVersion );
    CPPUNIT_ASSERT( actualId != NULL );
    free( actualId );

    // Free it all
    libcmis_document_free( actualVersion );
    libcmis_vector_document_free( versions );
    libcmis_error_free( error );
    libcmis_document_free( tested );
}

void DocumentTest::getAllVersionsErrorTest( )
{
    libcmis_DocumentPtr tested = getTested( true, true );
    libcmis_ErrorPtr error = libcmis_error_create( );

    // get all versions (tested method)
    libcmis_vector_document_Ptr versions = libcmis_document_getAllVersions( tested, error );

    // Check
    CPPUNIT_ASSERT( NULL == versions );
    CPPUNIT_ASSERT( !string( libcmis_error_getMessage( error ) ).empty( ) );

    // Free it all
    libcmis_vector_document_free( versions );
    libcmis_error_free( error );
    libcmis_document_free( tested );
}
