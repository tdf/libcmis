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
#include "libcmis-c.h"
#include "test-dummies.hxx"

using namespace std;

class SessionTest : public CppUnit::TestFixture
{
    private:
        libcmis_SessionPtr getTested( );

    public:
        void getRepositoriesTest( );
        void getBaseTypesTest( );

        CPPUNIT_TEST_SUITE( SessionTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( getBaseTypesTest );
        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( SessionTest );

libcmis_SessionPtr SessionTest::getTested( )
{
    libcmis_SessionPtr result = new libcmis_session();
    libcmis::Session* handle = new dummies::Session( );
    result->handle = handle;
    return result;
}

void SessionTest::getRepositoriesTest( )
{
    libcmis_SessionPtr session = getTested( );
    libcmis_vector_Repository_Ptr repos = libcmis_session_getRepositories( session );
    size_t actualSize = libcmis_vector_repository_size( repos );
    CPPUNIT_ASSERT_EQUAL( size_t( 2 ), actualSize );
    libcmis_vector_repository_free( repos );
    libcmis_session_free( session );
}

void SessionTest::getBaseTypesTest( )
{
    libcmis_SessionPtr session = getTested( );
    libcmis_ErrorPtr error = libcmis_error_create( );

    libcmis_vector_object_type_Ptr types = libcmis_session_getBaseTypes( session, error );
    
    size_t size = libcmis_vector_object_type_size( types );
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), size );

    libcmis_error_free( error );
    libcmis_vector_object_type_free( types );
    libcmis_session_free( session );
}
