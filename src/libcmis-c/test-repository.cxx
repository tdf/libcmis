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
#include "repository.h"
#include "test-dummies.hxx"

using namespace std;

class RepositoryTest : public CppUnit::TestFixture
{
    private:
        libcmis_RepositoryPtr getTested( );

    public:
        void getIdTest( );
        void getNameTest( );
        void getDescriptionTest( );
        void getVendorNameTest( );
        void getProductNameTest( );
        void getProductVersionTest( );
        void getRootIdTest( );
        void getCmisVersionSupportedTest( );
        void getThinClientUriTest( );
        void getPrincipalAnonymousTest( );
        void getPrincipalAnyoneTest( );

        CPPUNIT_TEST_SUITE( RepositoryTest );
        CPPUNIT_TEST( getIdTest );
        CPPUNIT_TEST( getNameTest );
        CPPUNIT_TEST( getDescriptionTest );
        CPPUNIT_TEST( getVendorNameTest );
        CPPUNIT_TEST( getProductNameTest );
        CPPUNIT_TEST( getProductVersionTest );
        CPPUNIT_TEST( getRootIdTest );
        CPPUNIT_TEST( getCmisVersionSupportedTest );
        CPPUNIT_TEST( getThinClientUriTest );
        CPPUNIT_TEST( getPrincipalAnonymousTest );
        CPPUNIT_TEST( getPrincipalAnyoneTest );
        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( RepositoryTest );

libcmis_RepositoryPtr RepositoryTest::getTested( )
{
    libcmis_RepositoryPtr result = new libcmis_repository( );

    libcmis::RepositoryPtr handle( new dummies::Repository( ) );
    result->handle = handle;

    return result;
}

void RepositoryTest::getIdTest( )
{
    libcmis_RepositoryPtr tested = getTested( );
    const char* actual = libcmis_repository_getId( tested );
    string expected( "Repository::Id" );
    CPPUNIT_ASSERT_EQUAL( expected, string( actual ) );

    libcmis_repository_free( tested );
}

void RepositoryTest::getNameTest( )
{
    libcmis_RepositoryPtr tested = getTested( );
    const char* actual = libcmis_repository_getName( tested );
    string expected( "Repository::Name" );
    CPPUNIT_ASSERT_EQUAL( expected, string( actual ) );

    libcmis_repository_free( tested );
}

void RepositoryTest::getDescriptionTest( )
{
    libcmis_RepositoryPtr tested = getTested( );
    const char* actual = libcmis_repository_getDescription( tested );
    string expected( "Repository::Description" );
    CPPUNIT_ASSERT_EQUAL( expected, string( actual ) );

    libcmis_repository_free( tested );
}

void RepositoryTest::getVendorNameTest( )
{
    libcmis_RepositoryPtr tested = getTested( );
    const char* actual = libcmis_repository_getVendorName( tested );
    string expected( "Repository::VendorName" );
    CPPUNIT_ASSERT_EQUAL( expected, string( actual ) );

    libcmis_repository_free( tested );
}

void RepositoryTest::getProductNameTest( )
{
    libcmis_RepositoryPtr tested = getTested( );
    const char* actual = libcmis_repository_getProductName( tested );
    string expected( "Repository::ProductName" );
    CPPUNIT_ASSERT_EQUAL( expected, string( actual ) );

    libcmis_repository_free( tested );
}

void RepositoryTest::getProductVersionTest( )
{
    libcmis_RepositoryPtr tested = getTested( );
    const char* actual = libcmis_repository_getProductVersion( tested );
    string expected( "Repository::ProductVersion" );
    CPPUNIT_ASSERT_EQUAL( expected, string( actual ) );

    libcmis_repository_free( tested );
}

void RepositoryTest::getRootIdTest( )
{
    libcmis_RepositoryPtr tested = getTested( );
    const char* actual = libcmis_repository_getRootId( tested );
    string expected( "Repository::RootId" );
    CPPUNIT_ASSERT_EQUAL( expected, string( actual ) );

    libcmis_repository_free( tested );
}

void RepositoryTest::getCmisVersionSupportedTest( )
{
    libcmis_RepositoryPtr tested = getTested( );
    const char* actual = libcmis_repository_getCmisVersionSupported( tested );
    string expected( "Repository::CmisVersionSupported" );
    CPPUNIT_ASSERT_EQUAL( expected, string( actual ) );

    libcmis_repository_free( tested );
}

void RepositoryTest::getThinClientUriTest( )
{
    libcmis_RepositoryPtr tested = getTested( );
    const char* actual = libcmis_repository_getThinClientUri( tested );
    string expected( "Repository::ThinClientUri" );
    CPPUNIT_ASSERT_EQUAL( expected, string( actual ) );

    libcmis_repository_free( tested );
}

void RepositoryTest::getPrincipalAnonymousTest( )
{
    libcmis_RepositoryPtr tested = getTested( );
    const char* actual = libcmis_repository_getPrincipalAnonymous( tested );
    string expected( "Repository::PrincipalAnonymous" );
    CPPUNIT_ASSERT_EQUAL( expected, string( actual ) );

    libcmis_repository_free( tested );
}

void RepositoryTest::getPrincipalAnyoneTest( )
{
    libcmis_RepositoryPtr tested = getTested( );
    const char* actual = libcmis_repository_getPrincipalAnyone( tested );
    string expected( "Repository::PrincipalAnyone" );
    CPPUNIT_ASSERT_EQUAL( expected, string( actual ) );

    libcmis_repository_free( tested );
}

