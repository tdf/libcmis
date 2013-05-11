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
#include "gdrive-object.hxx"

using std::string;
using namespace libcmis;

static const string clientId ( "mock-id" );
static const string clientSecret ( "mock-secret" );
static const string loginUrl ("https://accounts.google.com/ServiceLoginAuth" );
static const string approvalUrl ("https://accounts.google.com/o/oauth2/approval" );
static const string authUrl ( "https://accounts.google.com/o/oauth2/auth" );
static const string tokenUrl ( "https://accounts.google.com/o/oauth2/token" );
static const string scopeFull ( "https://www.googleapis.com/auth/drive" );
static const string redirectUri ("urn:ietf:wg:oauth:2.0:oob" );
static const string baseUrl ( "https://www.googleapis.com/drive/v2" );
class GDriveMockTest : public CppUnit::TestFixture
{
    public:
        void sessionAuthenticationTest( );
        void getObjectTest( );

        CPPUNIT_TEST_SUITE( GDriveMockTest );
        CPPUNIT_TEST( sessionAuthenticationTest );
        CPPUNIT_TEST( getObjectTest );
        CPPUNIT_TEST_SUITE_END( );

    private:
        GDriveSession createSession( string username, string password );
        GDriveSession getTestSession ( );
};

void GDriveMockTest::sessionAuthenticationTest( )
{
    GDriveSession session = getTestSession( );
    
    string empty;

    string authRequest( curl_mockup_getRequest( loginUrl.c_str(), empty.c_str( ),
                                                "POST" ) );
    string codeRequest( curl_mockup_getRequest( approvalUrl.c_str(),
                        "ref", "POST" ) );
    string tokenRequest( curl_mockup_getRequest( tokenUrl.c_str(), empty.c_str( ),
                                                 "POST" ));
            
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong authentication request",
        string ( "continue=redirectLink&scope=Scope&service=lso&GALX=cookie"
                 "&Email=mock-user&Passwd=mock-password" ),
        authRequest );
 
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong approval request",
        string( "state_wrapper=stateWrapper&submit_access=true" ), codeRequest);

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong token request",
        string ( "code=AuthCode&client_id=mock-id&client_secret=mock-secret&"
                 "redirect_uri=urn:ietf:wg:oauth:2.0:oob&"
                  "grant_type=authorization_code"),
        tokenRequest );

    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Wrong access token", 
         string ( "mock-access-token" ), 
         session.m_oauth2Handler->getAccessToken( ));

    CPPUNIT_ASSERT_EQUAL_MESSAGE( 
        "Wrong refresh token", 
        string ("mock-refresh-token"), 
        session.m_oauth2Handler->getRefreshToken( ));
}

void GDriveMockTest::getObjectTest()
{
    static const string objectId ("aFileId");
    GDriveSession session = getTestSession( );

    string url = baseUrl + "/files/" + objectId;
    curl_mockup_addResponse ( url.c_str( ), "",
                              "GET", "data/gdrive/gdoc-file.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( objectId );

    boost::shared_ptr<GDriveObject> obj = boost::dynamic_pointer_cast
                                            <GDriveObject>(object);

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Object Id", objectId,
                                                     obj->getId( ) );
}

GDriveSession GDriveMockTest::createSession( string username, 
                                             string password )
{
    libcmis::OAuth2DataPtr oauth2(
        new libcmis::OAuth2Data( authUrl, tokenUrl, scopeFull,
                                 redirectUri, clientId, clientSecret ));
    curl_mockup_reset( );
    string empty;
    //login response
    curl_mockup_addResponse ( authUrl.c_str(),
                            "scope=https://www.googleapis.com/auth/drive&"
                            "redirect_uri=urn:ietf:wg:oauth:2.0:oob&response"
                            "_type=code&client_id=mock-id",
                            "GET", "data/gdrive/login.html", 200, true);

    //authentication response
    curl_mockup_addResponse( loginUrl.c_str( ), empty.c_str( ), "POST",
                             "data/gdrive/approve.html", 200, true);

    //approval response
    curl_mockup_addResponse( approvalUrl.c_str( ), "ref",
                             "POST", "data/gdrive/authcode.html", 200, true);

    curl_mockup_addResponse ( tokenUrl.c_str( ), empty.c_str( ), "POST",
                              "data/gdrive/token-response.json", 200, true );

    return GDriveSession( baseUrl, username, password, oauth2, false );
}

GDriveSession GDriveMockTest::getTestSession( )
{
    static const string username( "mock-user" );
    static const string password( "mock-password" );

    // The authentication should happen automatically when creating
    // the session
    return createSession( username, password );
}

CPPUNIT_TEST_SUITE_REGISTRATION( GDriveMockTest );
