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

#include <boost/scoped_ptr.hpp>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

#define private public
#define protected public

#include <session-factory.hxx>
#include <atom-session.hxx>
#include <ws-session.hxx>
#include <gdrive-session.hxx>
#include <onedrive-session.hxx>
#include <sharepoint-session.hxx>

#include <mockup-config.h>
#include <test-helpers.hxx>
#include <test-mockup-helpers.hxx>

#define BINDING_ATOM string( "http://mockup/atom" )
#define BINDING_WS string( "http://mockup/ws" )
#define BINDING_SHAREPOINT string ( "http://mockup/sharepoint/_api/web" )
#define CONTEXTINFO_URL string ( "http://mockup/sharepoint/_api/contextinfo" )
#define BINDING_BAD "http://mockup/bad"
#define BINDING_GDRIVE  string ( "https://www.googleapis.com/drive/v2" )
#define BINDING_ONEDRIVE  string ( "https://apis.live.net/v5.0" )
#define SERVER_REPOSITORY string( "mock" )
#define SERVER_USERNAME "tester"
#define SERVER_PASSWORD "somepass"

#define OAUTH_CLIENT_ID  string ( "mock-id" )
#define OAUTH_CLIENT_SECRET  string ( "mock-secret" )
#define OAUTH_SCOPE  string ( "https://scope/url" )
#define OAUTH_REDIRECT_URI  string ("redirect:uri" )

#define GDRIVE_AUTH_URL string ( "https://auth/url" )
#define GDRIVE_LOGIN_URL  string ("https://login/url" )
#define GDRIVE_LOGIN_URL2  string ("https://login2/url" )
#define GDRIVE_APPROVAL_URL  string ("https://approval/url" )
#define GDRIVE_TOKEN_URL  string ( "https://token/url" )

#define ONEDRIVE_AUTH_URL string ( "https://auth/url" )
#define ONEDRIVE_TOKEN_URL  string ( "https://token/url" )

using namespace std;

namespace
{
    void lcl_init_mockup_ws( )
    {
        curl_mockup_reset( );
        curl_mockup_addResponse( BINDING_WS.c_str( ), "", "GET",
                                 DATA_DIR "/ws/CMISWS-Service.wsdl" );
        test::addWsResponse( string( BINDING_WS + "/services/RepositoryService" ).c_str(),
                                 DATA_DIR "/ws/repositories.http" );
        curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    }

    void lcl_init_mockup_atom( )
    {
        curl_mockup_reset( );
        curl_mockup_addResponse( BINDING_ATOM.c_str( ), "", "GET",
                                 DATA_DIR "/atom/workspaces.xml" );
        curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    }

    void lcl_init_mockup_gdrive( )
    {
        curl_mockup_reset( );

        //login response
        string loginIdentifier = string("scope=") + OAUTH_SCOPE +
                                 string("&redirect_uri=") + OAUTH_REDIRECT_URI +
                                 string("&response_type=code") +
                                 string("&client_id=") + OAUTH_CLIENT_ID;

        curl_mockup_addResponse ( GDRIVE_AUTH_URL.c_str(), loginIdentifier.c_str( ),
                                "GET", DATA_DIR "/gdrive/login1.html", 200, true);

        //authentication email
        curl_mockup_addResponse( GDRIVE_LOGIN_URL2.c_str( ), "", "POST",
                                 DATA_DIR "/gdrive/login2.html", 200, true);

        //authentication password,
        curl_mockup_addResponse( GDRIVE_LOGIN_URL.c_str( ), "", "POST",
                                 DATA_DIR "/gdrive/approve.html", 200, true);

        //approval response
        curl_mockup_addResponse( GDRIVE_APPROVAL_URL.c_str( ), "",
                                 "POST", DATA_DIR "/gdrive/authcode.html", 200, true);

        curl_mockup_addResponse ( GDRIVE_TOKEN_URL.c_str( ), "", "POST",
                                  DATA_DIR "/gdrive/token-response.json", 200, true );
    }

    char* authCodeFallback( const char* /*url*/, const char* /*username*/, const char* /*password*/ )
    {
        char *authCode = strdup( "authCode" );
        return authCode;
    }

    void lcl_init_mockup_sharepoint( )
    {
        curl_mockup_reset( );

        curl_mockup_addResponse( BINDING_SHAREPOINT.c_str( ), "", "GET", "", 401, false );
        curl_mockup_addResponse( ( BINDING_SHAREPOINT + "/currentuser" ).c_str( ), "", "GET",
                                 DATA_DIR "/sharepoint/auth-resp.json", 200, true );
        curl_mockup_addResponse( CONTEXTINFO_URL.c_str( ), "", "POST",
                                 DATA_DIR "/sharepoint/xdigest.json", 200, true );
        curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    }
}

class FactoryTest : public CppUnit::TestFixture
{
    public:

        void createSessionAtomTest( );
        void createSessionAtomBadAuthTest( );
        void createSessionWSTest( );
        void createSessionWSBadAuthTest( );
        void createSessionNoCmisTest( );
        void createSessionGDriveTest( );
        void createSessionOneDriveTest( );
        void createSessionSharePointTest( );
        void createSessionSharePointDefaultAuthTest( );
        void createSessionSharePointBadAuthTest( );

        CPPUNIT_TEST_SUITE( FactoryTest );
        CPPUNIT_TEST( createSessionAtomTest );
        CPPUNIT_TEST( createSessionAtomBadAuthTest );
        CPPUNIT_TEST( createSessionWSTest );
        CPPUNIT_TEST( createSessionWSBadAuthTest );
        CPPUNIT_TEST( createSessionNoCmisTest );
        CPPUNIT_TEST( createSessionGDriveTest );
        CPPUNIT_TEST( createSessionOneDriveTest );
        CPPUNIT_TEST( createSessionSharePointTest );
        CPPUNIT_TEST( createSessionSharePointDefaultAuthTest );
        CPPUNIT_TEST( createSessionSharePointBadAuthTest );
        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( FactoryTest );

void FactoryTest::createSessionAtomTest( )
{
    lcl_init_mockup_atom( );

    boost::scoped_ptr< libcmis::Session > session( libcmis::SessionFactory::createSession(
            BINDING_ATOM, SERVER_USERNAME, SERVER_PASSWORD,
            SERVER_REPOSITORY ) );
    CPPUNIT_ASSERT_MESSAGE( "Not an AtomPubSession",
            dynamic_cast< AtomPubSession* >( session.get() ) != NULL );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "More than one request for the binding",
            int( 1 ), curl_mockup_getRequestsCount( BINDING_ATOM.c_str( ), "", "GET" ) );
}

void FactoryTest::createSessionAtomBadAuthTest( )
{
    lcl_init_mockup_atom( );

    try
    {
        libcmis::SessionFactory::createSession(
                BINDING_ATOM, "Bad user", "Bad Password",
                SERVER_REPOSITORY );
        CPPUNIT_FAIL( "Should throw exception" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception type",
                string( "permissionDenied" ), e.getType( ) );
    }
}

void FactoryTest::createSessionWSTest( )
{
    lcl_init_mockup_ws( );

    boost::scoped_ptr< libcmis::Session > session( libcmis::SessionFactory::createSession(
            BINDING_WS, SERVER_USERNAME, SERVER_PASSWORD,
            SERVER_REPOSITORY ) );
    CPPUNIT_ASSERT_MESSAGE( "Not a WSSession",
            dynamic_cast< WSSession* >( session.get() ) != NULL );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "More than one request for the binding",
            int( 1 ), curl_mockup_getRequestsCount( BINDING_WS.c_str( ), "", "GET" ) );
}

void FactoryTest::createSessionWSBadAuthTest( )
{
    lcl_init_mockup_ws( );

    try
    {
        libcmis::SessionFactory::createSession(
                BINDING_WS, "Bad User", "Bad Pass",
                SERVER_REPOSITORY );
        CPPUNIT_FAIL( "Should throw exception" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception type",
                string( "permissionDenied" ), e.getType( ) );
    }
}

void FactoryTest::createSessionGDriveTest( )
{
    lcl_init_mockup_gdrive( );

    libcmis::OAuth2DataPtr oauth2Data(
        new libcmis::OAuth2Data( GDRIVE_AUTH_URL, GDRIVE_TOKEN_URL,
                                 OAUTH_SCOPE, OAUTH_REDIRECT_URI,
                                 OAUTH_CLIENT_ID, OAUTH_CLIENT_SECRET ));

    boost::scoped_ptr< libcmis::Session > session( libcmis::SessionFactory::createSession(
            BINDING_GDRIVE, SERVER_USERNAME, SERVER_PASSWORD,
            SERVER_REPOSITORY, false,
            oauth2Data ) );
    CPPUNIT_ASSERT_MESSAGE( "Not a GDriveSession",
            dynamic_cast< GDriveSession* >( session.get() ) != NULL );
}

void FactoryTest::createSessionOneDriveTest( )
{
    lcl_init_mockup_gdrive( );

    libcmis::OAuth2DataPtr oauth2Data(
        new libcmis::OAuth2Data( ONEDRIVE_AUTH_URL, ONEDRIVE_TOKEN_URL,
                                 OAUTH_SCOPE, OAUTH_REDIRECT_URI,
                                 OAUTH_CLIENT_ID, OAUTH_CLIENT_SECRET ));

    libcmis::SessionFactory::setOAuth2AuthCodeProvider( authCodeFallback );
    boost::scoped_ptr< libcmis::Session > session( libcmis::SessionFactory::createSession(
            BINDING_ONEDRIVE, SERVER_USERNAME, SERVER_PASSWORD,
            SERVER_REPOSITORY, false,
            oauth2Data ) );
    CPPUNIT_ASSERT_MESSAGE( "Not a OneDriveSession",
            dynamic_cast< OneDriveSession* >( session.get() ) != NULL );
}

void FactoryTest::createSessionSharePointTest( )
{
    lcl_init_mockup_sharepoint( );

    boost::scoped_ptr< libcmis::Session > session( libcmis::SessionFactory::createSession(
            BINDING_SHAREPOINT, SERVER_USERNAME, SERVER_PASSWORD,
            SERVER_REPOSITORY ) );
    CPPUNIT_ASSERT_MESSAGE( "Not a SharePoint Session",
            dynamic_cast< SharePointSession* >( session.get() ) != NULL );
}

void FactoryTest::createSessionSharePointDefaultAuthTest( )
{
    curl_mockup_addResponse( BINDING_SHAREPOINT.c_str( ), "", "GET",
                             DATA_DIR "/sharepoint/auth-xml-resp.xml", 200, true );
    curl_mockup_addResponse( CONTEXTINFO_URL.c_str( ), "", "POST",
                             DATA_DIR "/sharepoint/xdigest.json", 200, true );

    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    boost::scoped_ptr< libcmis::Session > session( libcmis::SessionFactory::createSession(
            BINDING_SHAREPOINT, SERVER_USERNAME, SERVER_PASSWORD,
            SERVER_REPOSITORY ) );
    CPPUNIT_ASSERT_MESSAGE( "Not a SharePoint Session",
            dynamic_cast< SharePointSession* >( session.get() ) != NULL );
}

void FactoryTest::createSessionSharePointBadAuthTest( )
{
    lcl_init_mockup_sharepoint( );

    try
    {
        libcmis::SessionFactory::createSession(
                BINDING_ATOM, "Bad user", "Bad Password",
                SERVER_REPOSITORY );
        CPPUNIT_FAIL( "Should throw exception" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception type",
                string( "permissionDenied" ), e.getType( ) );
    }
}

void FactoryTest::createSessionNoCmisTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( BINDING_BAD, "", "GET",
                             "<p>Some non CMIS content</p>", 200, false );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    boost::scoped_ptr< libcmis::Session > session( libcmis::SessionFactory::createSession(
            BINDING_BAD, SERVER_USERNAME, SERVER_PASSWORD,
            SERVER_REPOSITORY ) );
    CPPUNIT_ASSERT_MESSAGE( "Session should be NULL", !session );
}
