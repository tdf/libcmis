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
 * Copyright (C) 2013 Cao Cuong Ngo <cao.cuong.ngo@gmail.com>
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

#include <string>

#define private public
#define protected public

#include <mockup-config.h>

#include "gdrive-session.hxx"
#include "oauth2-handler.hxx"

using namespace std;
using namespace libcmis;

#define CLIENT_ID string ("mock-id")
#define CLIENT_SECRET string ("mock-secret")

class GDriveMockTest : public CppUnit::TestFixture
{
    public:
        void sessionAuthenticationTest( );

        CPPUNIT_TEST_SUITE( GDriveMockTest );
        CPPUNIT_TEST( sessionAuthenticationTest );
        CPPUNIT_TEST_SUITE_END( );

    private:
        GDriveSession createSession( const string& username, const string& password );
};

void GDriveMockTest::sessionAuthenticationTest( )
{
    curl_mockup_reset( );
    curl_mockup_setResponse( "data/json-response.json" );

    const string username( "fake-google-username" );
    const string password( "fake-password" );

    // The authentication should happen automatically when creating the session
    GDriveSession session = createSession( username, password );

    CPPUNIT_ASSERT_EQUAL_MESSAGE("wrong access token", string("mock-access-token"), session.m_oauth2Handler->getAccessToken( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE("wrong refresh token", string("mock-refresh-token"), session.m_oauth2Handler->getRefreshToken( ) );
}

GDriveSession GDriveMockTest::createSession( const string& username, const string& password )
{
    return GDriveSession( CLIENT_ID, CLIENT_SECRET, username, password );
}

CPPUNIT_TEST_SUITE_REGISTRATION( GDriveMockTest );
