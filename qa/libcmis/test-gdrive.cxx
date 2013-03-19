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
 
    string authRequest( curl_mockup_getRequest( 
                        "https://accounts.google.com/ServiceLoginAuth", 
                        "", "POST" ) );
           
    string codeRequest( curl_mockup_getRequest( 
                        "https://accounts.google.com/o/oauth2/approval", 
                        "as=-7b1ae72178f10481&hl=en_US&xsrfsign=APsBz4gAAAAAUUI"
                        "KghwgPtjfu5KA_cPF2ich0o-kYdp3", 
                        "POST" ) );
    
    string tokenRequest( curl_mockup_getRequest( "https://accounts.google.com/o"
                                                 "/oauth2/token", "", "POST" ));
    
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong authentication request", 
        string ( "continue=https://accounts.google.com/o/oauth2/auth?response_"
                 "type=code&scope=https://www.googleapis.com/auth/drive&redirec"
                 "t_uri=urn:ietf:wg:oauth:2.0:oob&client_id=mock-id&hl=en-US&fr"
                 "om_login=1&as=-7b1ae72178f10481&service=lso&dsh=3400322089026"
                 "361568&ltmpl=embedded&shdf=CoEBCxIRdGhpcmRQYXJ0eUxvZ29VcmwaAA"
                 "wLEhV0aGlyZFBhcnR5RGlzcGxheU5hbWUaC0dkcml2ZSBDTUlTDAsSBmRvbWF"
                 "pbhoLR2RyaXZlIENNSVMMCxIVdGhpcmRQYXJ0eURpc3BsYXlUeXBlGhJOQVRJ"
                 "VkVfQVBQTElDQVRJT04MEgNsc28iFG67YENXm6ncjnKqhNGAhZwxkiE8KAEyF"
                 "Op0y7E6l-eGKSqZSDe0c2j8_Pkf&scc=1&GALX=tDzdDJzzQwM&pstMsg=1&c"
                 "heckConnection=youtube:234:1&checkedDomains=youtube&_utf8=☃&b"
                 "gresponse=js_disabled&signIn=Sign in&Email=mock-user&Passwd=m"
                 "ock-password" ), 
        authRequest );
 
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong approval request", 
        string ( "_utf8=☃&state_wrapper=CoQCZnJvbV9sb2dpbj0xJnJlc3BvbnNlX3R5cGU"
                 "9Y29kZSZyZWRpcmVjdF91cmk9dXJuOmlldGY6d2c6b2F1dGg6Mi4wOm9vYiZh"
                 "cz0tN2IxYWU3MjE3OGYxMDQ4MSZobD1lbi1VUyZjbGllbnRfaWQ9MTIxMTkwN"
                 "DgzNTY2LTdtZWFwMmtkM3RiMjE3YmwzYjZnZnA3NWcwdm8wYm1qLmFwcHMuZ2"
                 "9vZ2xldXNlcmNvbnRlbnQuY29tJmFjY2Vzc190eXBlPW9mZmxpbmUmc2NvcGU"
                 "9aHR0cHM6Ly93d3cuZ29vZ2xlYXBpcy5jb20vYXV0aC9kcml2ZSZkaXNwbGF5"
                 "PXBhZ2USFTEwNTYyODU0Njg5MzM0Njk2NzEyNw&submit_access=true"), 
        codeRequest);

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong token request", 
        string ( "code=4/0Bst4GNF1fZ54QufJglu0IeiJ9DD.8lT5e9eELwYeOl05ti8ZT3b5D"
                 "PzLegI&client_id=mock-id&client_secret=mock-secret&redirect_u"
                 "ri=urn:ietf:wg:oauth:2.0:oob&grant_type=authorization_code"), 
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
    static const string objectId ("/files/mock-file-id");
    GDriveSession session = getTestSession( );

    string url = baseUrl + objectId;
    curl_mockup_addResponse ( url.c_str( ), "",
                                  "GET", "data/gdrive/mock-file.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( objectId );

    boost::shared_ptr<GDriveObject> obj = boost::dynamic_pointer_cast
                                            <GDriveObject>(object);

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Object Id", string ( "mock-file-id"), 
                                                     obj->getId( ) );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Object name", string ( "Mock title"), 
                                                       obj->getName( ) );

    string exportLinks("{ \"application\\/pdf\": \"https:\\/\\/docs.google.com"
            "\\/feeds\\/download\\/spreadsheets\\/Export?key=0Ar9E3szV96I6dFFpO"
            "UVBd010dXlPdVE&exportFormat=pdf\", \"application\\/x-vnd.oasis.ope"
            "ndocument.spreadsheet\": \"https:\\/\\/docs.google.com\\/feeds\\/d"
            "ownload\\/spreadsheets\\/Export?key=0Ar9E3sFFpOdXEwMGxaVUNFdklPdVE"
            "&exportFormat=ods\", \"application\\/vnd.openxmlformats-officedocu"
            "ment.spreadsheetml.sheet\": \"https:\\/\\/docs.google.com\\/feeds"
            "\\/download\\/spreadsheets\\/Export?key=0Ar996I6dFFpOUVBd010dXEwMG"
            "xaVUNFdklPdVE&exportFormat=xlsx\" }"); 
    
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong export links", 
                                  exportLinks,
                                  obj->getExportLinks( ).toString( ) );
                
}

GDriveSession GDriveMockTest::createSession( string username, 
                                             string password )
{
    libcmis::OAuth2DataPtr oauth2( 
        new libcmis::OAuth2Data( authUrl, tokenUrl, scopeFull, 
                                 redirectUri, clientId, clientSecret ));

    return GDriveSession( baseUrl, username, password, oauth2, false );
}

GDriveSession GDriveMockTest::getTestSession( )
{
    static const string username( "mock-user" );
    static const string password( "mock-password" );
    curl_mockup_reset( );
    //login response
    curl_mockup_addResponse ( "https://accounts.google.com/o/oauth2/auth",
                            "scope=https://www.googleapis.com/auth/drive&"
                            "redirect_uri=urn:ietf:wg:oauth:2.0:oob&response"
                            "_type=code&client_id=mock-id",
                            "GET", "data/gdrive/login.html", 200, true);

    //authentication response
    curl_mockup_addResponse ( "https://accounts.google.com/ServiceLoginAuth",
                             "", "POST", "data/gdrive/approve.html",
                             200, true);

    //approval response
    curl_mockup_addResponse ( "https://accounts.google.com/o/oauth2/approval",
                            "as=-7b1ae72178f10481&hl=en_US&xsrfsign=APsBz4gAA"
                            "AAAUUIKghwgPtjfu5KA_cPF2ich0o-kYdp3",
                             "POST", "data/gdrive/authcode.html", 200, true);

    curl_mockup_addResponse ( "https://accounts.google.com/o/oauth2/token",
                            "", "POST", "data/gdrive/token-response.json",
                            200, true );

    // The authentication should happen automatically when creating
    // the session
    return createSession( username, password );
}


CPPUNIT_TEST_SUITE_REGISTRATION( GDriveMockTest );
