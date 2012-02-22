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
#include <cppunit/ui/text/TestRunner.h>

#include "atom-utils.hxx"

#define BASE64_ENCODING string( "base64" )

using namespace std;

class DecoderTest : public CppUnit::TestFixture
{
    private:
        atom::EncodedData* data;
        FILE* stream;

        string getActual( );

    public:

        DecoderTest( );
        DecoderTest( const DecoderTest& rCopy );

        const DecoderTest& operator=( const DecoderTest& rCopy );

        void setUp( );
        void tearDown( );

        void noEncodingTest();

        void base64DecodeSimpleBlockTest( );
        void base64DecodePaddedBlockTest( );
        void base64DecodeNoEqualsPaddedBlockTest( );
        void base64DecodeSplitRunsTest( );

        void base64EncodeSimpleBlockTest( );
        void base64EncodePaddedBlockTest( );
        void base64EncodeSplitRunsTest( );

        CPPUNIT_TEST_SUITE( DecoderTest );
        CPPUNIT_TEST( noEncodingTest );
        CPPUNIT_TEST( base64DecodeSimpleBlockTest );
        CPPUNIT_TEST( base64DecodePaddedBlockTest );
        CPPUNIT_TEST( base64DecodeNoEqualsPaddedBlockTest );
        CPPUNIT_TEST( base64DecodeSplitRunsTest );
        CPPUNIT_TEST( base64EncodeSimpleBlockTest );
        CPPUNIT_TEST( base64EncodePaddedBlockTest );
        CPPUNIT_TEST( base64EncodeSplitRunsTest );
        CPPUNIT_TEST_SUITE_END( );
};

DecoderTest::DecoderTest( ) :
    CppUnit::TestFixture( ),
    data( NULL ),
    stream( NULL )
{
}

DecoderTest::DecoderTest( const DecoderTest& rCopy ) :
    CppUnit::TestFixture( ),
    data( rCopy.data ),
    stream( rCopy.stream )
{
}

const DecoderTest& DecoderTest::operator=( const DecoderTest& rCopy )
{
    data = rCopy.data;
    stream = rCopy.stream;
    return *this;
}

void DecoderTest::setUp( )
{
    stream = tmpfile(); 
    data = new atom::EncodedData( stream );
}

void DecoderTest::tearDown( )
{
    fclose( stream );
    delete data;
}

string DecoderTest::getActual( )
{
    string actual;

    rewind( stream );
    size_t bufSize = 100;
    char buf[100];
    size_t readBytes = 0;
    do {
        readBytes = fread( buf, 1, bufSize, stream );
        actual += string( buf, readBytes );
    } while ( readBytes == bufSize );

    return actual;
}

void DecoderTest::noEncodingTest()
{
    data->decode( ( void* )"pleasure.", 1, 9 );
    data->finish( );
    CPPUNIT_ASSERT_EQUAL( string( "pleasure." ), getActual( ) );
}

/*
 *  All the test values for the Base64 have been taken from
 *  the wikipedia article: http://en.wikipedia.org/wiki/Base64
 */

void DecoderTest::base64DecodeSimpleBlockTest( )
{
    data->setEncoding( BASE64_ENCODING );
    string input( "cGxlYXN1cmUu" );
    data->decode( ( void* )input.c_str( ), 1, input.size() );
    data->finish( );
    CPPUNIT_ASSERT_EQUAL( string( "pleasure." ), getActual( ) );
}

void DecoderTest::base64DecodePaddedBlockTest( )
{
    data->setEncoding( BASE64_ENCODING );
    string input( "c3VyZS4=" );
    data->decode( ( void* )input.c_str( ), 1, input.size( ) );
    data->finish( );
    CPPUNIT_ASSERT_EQUAL( string( "sure." ), getActual( ) );
}

void DecoderTest::base64DecodeNoEqualsPaddedBlockTest( )
{
    data->setEncoding( BASE64_ENCODING );
    string input( "c3VyZS4" );
    data->decode( ( void* )input.c_str( ), 1, input.size( ) );
    data->finish( );
    CPPUNIT_ASSERT_EQUAL( string( "sure." ), getActual( ) );
}

void DecoderTest::base64DecodeSplitRunsTest( )
{
    data->setEncoding( BASE64_ENCODING );
    string input1( "cGxlYXN1c" );
    data->decode( ( void* )input1.c_str( ), 1, input1.size( ) );
    string input2( "mUu" );
    data->decode( ( void* )input2.c_str( ), 1, input2.size( ) );
    data->finish( );
    CPPUNIT_ASSERT_EQUAL( string( "pleasure." ), getActual( ) );
}

void DecoderTest::base64EncodeSimpleBlockTest( )
{
    data->setEncoding( BASE64_ENCODING );
    string input( "pleasure." );
    data->encode( ( void* )input.c_str(), 1, input.size() );
    data->finish( );
    CPPUNIT_ASSERT_EQUAL( string( "cGxlYXN1cmUu" ), getActual( ) );
}

void DecoderTest::base64EncodePaddedBlockTest( )
{
    data->setEncoding( BASE64_ENCODING );
    string input( "sure." );
    data->encode( ( void* )input.c_str(), 1, input.size() );
    data->finish( );
    CPPUNIT_ASSERT_EQUAL( string( "c3VyZS4=" ), getActual( ) );
}

void DecoderTest::base64EncodeSplitRunsTest( )
{
    data->setEncoding( BASE64_ENCODING );
    string input1( "plea" );
    data->encode( ( void* )input1.c_str(), 1, input1.size() );
    string input2( "sure." );
    data->encode( ( void* )input2.c_str(), 1, input2.size() );
    data->finish( );
    CPPUNIT_ASSERT_EQUAL( string( "cGxlYXN1cmUu" ), getActual( ) );
}

CPPUNIT_TEST_SUITE_REGISTRATION( DecoderTest );
