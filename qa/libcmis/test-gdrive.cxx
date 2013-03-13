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
    const string username( "mock-user" );
    const string password( "mock-password" );

    curl_mockup_reset( );

    //login responses
    const string loginHeader ( "Set-Cookie: GALX=jvNnpnJZV8Q;Path=/;Secure" );

    curl_mockup_addResponse ( "https://accounts.google.com/o/oauth2/auth", "scope=https://www.googleapis.com/auth/drive+&redirect_uri=urn:ietf:wg:oauth:2.0:oob&response_type=code&client_id=mock-id", "GET", "data/gdrive/login.html", 200, true, loginHeader.c_str( ) );

    //authentication responses
    
    const string authenticateHeader ( "Set-Cookie: LSOSID=DQAAALkAAACKFqTOjkTLyL;Path=/" ); 

    curl_mockup_addResponse ( "https://accounts.google.com/ServiceLoginAuth", "", "POST", "data/gdrive/approve.html", 200, true, authenticateHeader.c_str( ) );

    //approval responses

    curl_mockup_addResponse ( "https://accounts.google.com/o/oauth2/approval", "as=3051bd2d56da96f6&hl=en&xsrfsign=APsBz4gAAAAAUUEH0b136gUoiuPTOZc1HvmJDzFvM6r5", "POST", "data/gdrive/authcode.html", 200, true,"");

    curl_mockup_addResponse ( "https://accounts.google.com/o/oauth2/token", "", "POST", "data/gdrive/token-response.json", 200, true,"");

    // The authentication should happen automatically when creating the session
    GDriveSession session = createSession( username, password );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong access token", string ("mock-access-token"), session.m_oauth2Handler->getAccessToken( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong refresh token", string ("mock-refresh-token"), session.m_oauth2Handler->getRefreshToken( ) );

}

GDriveSession GDriveMockTest::createSession( const string& username, const string& password )
{
    return GDriveSession( CLIENT_ID, CLIENT_SECRET, username, password );
}

CPPUNIT_TEST_SUITE_REGISTRATION( GDriveMockTest );
