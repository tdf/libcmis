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

    curl_mockup_addResponse ( "https://accounts.google.com/o/oauth2/auth", "scope=https://www.googleapis.com/auth/drive+&redirect_uri=urn:ietf:wg:oauth:2.0:oob&response_type=code&client_id=mock-id", "GET", "data/gdrive/login.html", 200, true);

    //authentication responses
    
    curl_mockup_addResponse ( "https://accounts.google.com/ServiceLoginAuth", "", "POST", "data/gdrive/approve.html", 200, true);

    //approval responses

    curl_mockup_addResponse ( "https://accounts.google.com/o/oauth2/approval", "as=-7b1ae72178f10481&hl=en_US&xsrfsign=APsBz4gAAAAAUUIKghwgPtjfu5KA_cPF2ich0o-kYdp3", "POST", "data/gdrive/authcode.html", 200, true,"");

    curl_mockup_addResponse ( "https://accounts.google.com/o/oauth2/token", "", "POST", "data/gdrive/token-response.json", 200, true,"");

    // The authentication should happen automatically when creating the session
    GDriveSession session = createSession( username, password );
 
    string authRequest( curl_mockup_getRequest( "https://accounts.google.com/ServiceLoginAuth", "", "POST" ) );
    
       
    string codeRequest( curl_mockup_getRequest( "https://accounts.google.com/o/oauth2/approval", "as=-7b1ae72178f10481&hl=en_US&xsrfsign=APsBz4gAAAAAUUIKghwgPtjfu5KA_cPF2ich0o-kYdp3", "POST" ) );
    
    string tokenRequest( curl_mockup_getRequest( "https://accounts.google.com/o/oauth2/token", "", "POST" ) );
    
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong authentication request", string ( "continue=https://accounts.google.com/o/oauth2/auth?response_type=code&scope=https://www.googleapis.com/auth/drive&redirect_uri=urn:ietf:wg:oauth:2.0:oob&client_id=mock-id&hl=en-US&from_login=1&as=-7b1ae72178f10481&service=lso&dsh=3400322089026361568&ltmpl=embedded&shdf=CoEBCxIRdGhpcmRQYXJ0eUxvZ29VcmwaAAwLEhV0aGlyZFBhcnR5RGlzcGxheU5hbWUaC0dkcml2ZSBDTUlTDAsSBmRvbWFpbhoLR2RyaXZlIENNSVMMCxIVdGhpcmRQYXJ0eURpc3BsYXlUeXBlGhJOQVRJVkVfQVBQTElDQVRJT04MEgNsc28iFG67YENXm6ncjnKqhNGAhZwxkiE8KAEyFOp0y7E6l-eGKSqZSDe0c2j8_Pkf&scc=1&GALX=tDzdDJzzQwM&pstMsg=1&checkConnection=youtube:234:1&checkedDomains=youtube&_utf8=☃&bgresponse=js_disabled&signIn=Sign in&Email=mock-user&Passwd=mock-password" ), authRequest );
 
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong approval request", string ( "_utf8=☃&state_wrapper=CoQCZnJvbV9sb2dpbj0xJnJlc3BvbnNlX3R5cGU9Y29kZSZyZWRpcmVjdF91cmk9dXJuOmlldGY6d2c6b2F1dGg6Mi4wOm9vYiZhcz0tN2IxYWU3MjE3OGYxMDQ4MSZobD1lbi1VUyZjbGllbnRfaWQ9MTIxMTkwNDgzNTY2LTdtZWFwMmtkM3RiMjE3YmwzYjZnZnA3NWcwdm8wYm1qLmFwcHMuZ29vZ2xldXNlcmNvbnRlbnQuY29tJmFjY2Vzc190eXBlPW9mZmxpbmUmc2NvcGU9aHR0cHM6Ly93d3cuZ29vZ2xlYXBpcy5jb20vYXV0aC9kcml2ZSZkaXNwbGF5PXBhZ2USFTEwNTYyODU0Njg5MzM0Njk2NzEyNw&submit_access=true"), codeRequest);

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong token request", string ( "code=4/0Bst4GNF1fZ54QufJglu0IeiJ9DD.8lT5e9eELwYeOl05ti8ZT3b5DPzLegI&client_id=mock-id&client_secret=mock-secret&redirect_uri=urn:ietf:wg:oauth:2.0:oob&grant_type=authorization_code" ), tokenRequest );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong access token", string ("mock-access-token"), session.m_oauth2Handler->getAccessToken( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong refresh token", string ("mock-refresh-token"), session.m_oauth2Handler->getRefreshToken( ) );

}

GDriveSession GDriveMockTest::createSession( const string& username, const string& password )
{
    return GDriveSession( CLIENT_ID, CLIENT_SECRET, username, password );
}

CPPUNIT_TEST_SUITE_REGISTRATION( GDriveMockTest );
