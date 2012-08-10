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
#include "document.h"
#include "test-dummies.hxx"

using namespace std;

namespace
{
    string lcl_readFile( FILE* file )
    {
        // Get the size
        fseek( file, 0, SEEK_END );
        long size = ftell( file );
        rewind( file );

        char* buf = new char[size + 1];
        fread( buf, 1, size, file );
        buf[ size ] = '\0';

        string result( buf );
        delete[] buf;

        return result;
    }
}

class DocumentTest : public CppUnit::TestFixture
{
    private:
        libcmis_DocumentPtr getTested( bool isFiled, bool triggersFaults );

    public:
        void objectFunctionsTest( );
        void getParentsTest( );
        void getParentsErrorTest( );
        void getContentStreamTest( );
        void getContentTypeTest( );
        void getContentFilenameTest( );
        void getContentLengthTest( );
        void checkOutTest( );
        void checkOutErrorTest( );
        void cancelCheckoutTest( );
        void cancelCheckoutErrorTest( );

        CPPUNIT_TEST_SUITE( DocumentTest );
        CPPUNIT_TEST( objectFunctionsTest );
        CPPUNIT_TEST( getParentsTest );
        CPPUNIT_TEST( getParentsErrorTest );
        CPPUNIT_TEST( getContentStreamTest );
        CPPUNIT_TEST( getContentTypeTest );
        CPPUNIT_TEST( getContentFilenameTest );
        CPPUNIT_TEST( getContentLengthTest );
        CPPUNIT_TEST( checkOutTest );
        CPPUNIT_TEST( checkOutErrorTest );
        CPPUNIT_TEST( cancelCheckoutTest );
        CPPUNIT_TEST( cancelCheckoutErrorTest );
        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( DocumentTest );

libcmis_DocumentPtr DocumentTest::getTested( bool isFiled, bool triggersFaults )
{
    libcmis_DocumentPtr result = new libcmis_document( );
    libcmis::DocumentPtr handle( new dummies::Document( isFiled, triggersFaults ) );
    result->setHandle( handle );

    return result;
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
    string actual = lcl_readFile( tmp );
    fclose( tmp );
    CPPUNIT_ASSERT_EQUAL( string( ), string( libcmis_error_getMessage( error ) ) );
    CPPUNIT_ASSERT_EQUAL( string( "Document::ContentStream" ), actual );

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
