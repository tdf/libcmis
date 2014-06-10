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
 * Copyright (C) 2014 Mihai Varga <mihai.mv13@gmail.com>
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

#include <fstream>
#include "onedrive-object.hxx"
#include "onedrive-property.hxx"
#include "onedrive-session.hxx"
#include "oauth2-handler.hxx"
#include "document.hxx"

using namespace std;
using namespace libcmis;

static const string CLIENT_ID ( "mock-id" );
static const string CLIENT_SECRET ( "mock-secret" );
static const string USERNAME( "mock-user" );
static const string PASSWORD( "mock-password" );
static const string LOGIN_URL ("https://login/url" );
static const string APPROVAL_URL ("https://approval/url" );
static const string AUTH_URL ( "https://auth/url" );
static const string TOKEN_URL ( "https://token/url" );
static const string SCOPE ( "https://scope/url" );
static const string REDIRECT_URI ("redirect:uri" );
static const string BASE_URL ( "https://base/url" );

class OneDriveTest : public CppUnit::TestFixture
{
    public:
        void sessionAuthenticationTest( );
        void sessionExpiryTokenGetTest( );
        void getRepositoriesTest( );
        void getObjectTest( );
        void filePropertyTest( );
        void deleteTest( );
        void updatePropertiesTest( );
        void getFileAllowableActionsTest( );
        void getFolderAllowableActionsTest( );

        CPPUNIT_TEST_SUITE( OneDriveTest );
        CPPUNIT_TEST( sessionAuthenticationTest );
        CPPUNIT_TEST( sessionExpiryTokenGetTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( getObjectTest );
        CPPUNIT_TEST( filePropertyTest );
        CPPUNIT_TEST( deleteTest );
        CPPUNIT_TEST( updatePropertiesTest );
        CPPUNIT_TEST( getFileAllowableActionsTest );
        CPPUNIT_TEST( getFolderAllowableActionsTest );
        CPPUNIT_TEST_SUITE_END( );

    private:
        OneDriveSession getTestSession( string username, string password );
};

OneDriveSession OneDriveTest::getTestSession( string username, string password )
{
    libcmis::OAuth2DataPtr oauth2(
        new libcmis::OAuth2Data( AUTH_URL, TOKEN_URL, SCOPE,
                                 REDIRECT_URI, CLIENT_ID, CLIENT_SECRET ));
    curl_mockup_reset( );
    string empty;
    // login, authentication & approval are done manually at the moment, so I'll
    // temporarily borrow them from gdrive
    //login response
    string loginIdentifier = string("scope=") + SCOPE +
                             string("&redirect_uri=") + REDIRECT_URI +
                             string("&response_type=code") +
                             string("&client_id=") + CLIENT_ID;
    curl_mockup_addResponse ( AUTH_URL.c_str(), loginIdentifier.c_str( ),
                            "GET", DATA_DIR "/gdrive/login.html", 200, true);

    //authentication response
    curl_mockup_addResponse( LOGIN_URL.c_str( ), empty.c_str( ), "POST",
                             DATA_DIR "/gdrive/approve.html", 200, true);

    //approval response
    curl_mockup_addResponse( APPROVAL_URL.c_str( ), empty.c_str( ),
                             "POST", DATA_DIR "/gdrive/authcode.html", 200, true);


    // token response
    curl_mockup_addResponse ( TOKEN_URL.c_str( ), empty.c_str( ), "POST",
                              DATA_DIR "/onedrive/token-response.json", 200, true );

    return OneDriveSession( BASE_URL, username, password, oauth2, false );
}

void OneDriveTest::sessionAuthenticationTest( )
{
    OneDriveSession session = getTestSession( USERNAME, PASSWORD );
    string empty;

    // Check token request
    string expectedTokenRequest =
        string( "code=AuthCode") +
        string( "&client_id=") + CLIENT_ID +
        string( "&client_secret=") + CLIENT_SECRET +
        string( "&redirect_uri=") + REDIRECT_URI +
        string( "&grant_type=authorization_code" );

    string tokenRequest( curl_mockup_getRequestBody( TOKEN_URL.c_str(), empty.c_str( ),
                                                 "POST" ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong token request",
                                  expectedTokenRequest, tokenRequest );

    // Check token
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Wrong access token",
         string ( "mock-access-token" ),
         session.m_oauth2Handler->getAccessToken( ));
    CPPUNIT_ASSERT_EQUAL_MESSAGE(
        "Wrong refresh token",
        string ("mock-refresh-token"),
        session.m_oauth2Handler->getRefreshToken( ));
}

void OneDriveTest::sessionExpiryTokenGetTest( )
{
    // Access_token will expire after expires_in seconds,
    // We need to use the refresh key to get a new one.

    curl_mockup_reset( );
    OneDriveSession session = getTestSession( USERNAME, PASSWORD );

    curl_mockup_reset( );
    static const string objectId("aFileId");
    string url = BASE_URL + "/me/skydrive/files/" + objectId;

    // 401 response, token is expired
    curl_mockup_addResponse( url.c_str( ),"", "GET", "", 401, false );

    curl_mockup_addResponse( TOKEN_URL.c_str(), "",
                             "POST", DATA_DIR "/onedrive/refresh-response.json", 200, true);
    try
    {
        // GET expires, need to refresh then GET again
        libcmis::ObjectPtr obj = session.getObject( objectId );
    }
    catch ( ... )
    {
        if ( session.getHttpStatus( ) == 401 )
        {
            // Check if access token is refreshed
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                   "wrong access token",
                   string ( "new-access-token" ),
                   session.m_oauth2Handler->getAccessToken( ) );
        }
    }
}

void OneDriveTest::getRepositoriesTest( )
{
     curl_mockup_reset( );

     OneDriveSession session = getTestSession( USERNAME, PASSWORD );
     vector< libcmis::RepositoryPtr > actual = session.getRepositories( );

     CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of repositories", size_t( 1 ),
                                   actual.size( ) );
     CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong repository found",
                                   string ( "OneDrive" ),
                                   actual.front()->getId( ) );
}

void OneDriveTest::getObjectTest()
{
    static const string objectId ("aFileId");

    OneDriveSession session = getTestSession( USERNAME, PASSWORD );
    string url = BASE_URL + "/" + objectId;
    curl_mockup_addResponse ( url.c_str( ), "",
                              "GET", DATA_DIR "/onedrive/file.json", 200, true);
    libcmis::ObjectPtr object = session.getObject( objectId );
    boost::shared_ptr<OneDriveObject> obj = boost::dynamic_pointer_cast
                                            <OneDriveObject>( object );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Object Id", objectId,
                                                     obj->getId( ) );
}

void OneDriveTest::filePropertyTest( )
{
    curl_mockup_reset( );
    static const string objectId ("aFileId");

    OneDriveSession session = getTestSession( USERNAME, PASSWORD );
    string url = BASE_URL + "/" + objectId;
    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", DATA_DIR "/onedrive/file.json", 200, true);

    libcmis::ObjectPtr obj = session.getObject( objectId );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong creation date",
                                   string ( "createdTime" ),
                                   obj->getStringProperty( "cmis:creationDate" ) );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong object id",
                                   string ( "aFileId" ),
                                   obj->getStringProperty( "cmis:objectId" ) );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong author",
                                   string ( "onedriveUser" ),
                                   obj->getStringProperty( "cmis:createdBy" ) );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong file name",
                                   string ( "OneDrive File" ),
                                   obj->getStringProperty( "cmis:contentStreamFileName" ) );
}

void OneDriveTest::deleteTest( )
{
    curl_mockup_reset( );
    OneDriveSession session = getTestSession( USERNAME, PASSWORD );

    const string objectId( "aFileId" );

    string url = BASE_URL + "/" + objectId;
    curl_mockup_addResponse( url.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/file.json", 200, true);
    curl_mockup_addResponse( url.c_str( ),"", "DELETE", "", 204, false);

    libcmis::ObjectPtr object = session.getObject( objectId );

    object->remove( );
    const struct HttpRequest* deleteRequest = curl_mockup_getRequest( url.c_str( ), "", "DELETE" );
    CPPUNIT_ASSERT_MESSAGE( "Delete request not sent", deleteRequest );
}

void OneDriveTest::updatePropertiesTest( )
{
    curl_mockup_reset( );
    OneDriveSession session = getTestSession( USERNAME, PASSWORD );
    const string objectId( "aFileId" );
    const string newObjectId( "aNewFileId" );

    const string objectUrl = BASE_URL + "/" + objectId;
    const string newObjectUrl = BASE_URL + "/" + newObjectId;

    curl_mockup_addResponse( objectUrl.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/file.json", 200, true );
    curl_mockup_addResponse( newObjectUrl.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/updated-file.json", 200, true );
    curl_mockup_addResponse( objectUrl.c_str( ), "",
                               "PUT", DATA_DIR "/onedrive/updated-file.json", 200, true );

    libcmis::ObjectPtr object = session.getObject( objectId );
    libcmis::ObjectPtr newObject = session.getObject( newObjectId );

    object->updateProperties( newObject->getProperties( ) );

    const char* updateRequest = curl_mockup_getRequestBody( objectUrl.c_str( ), "", "PUT" );

    Json json = Json::parse( string( updateRequest ) );
    string name = json["name"].toString( );
    string description = json["description"].toString( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Name key not converted",
                                  string( "New File Name"),
                                  name );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Description key not converted",
                                  string( "new description"),
                                  description );
}

void OneDriveTest::getFileAllowableActionsTest( )
{
    curl_mockup_reset( );
    static const string objectId ("aFileId");

    OneDriveSession session = getTestSession( USERNAME, PASSWORD );
    string url = BASE_URL + "/" + objectId;
    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", DATA_DIR "/onedrive/file.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( objectId );

    boost::shared_ptr< libcmis::AllowableActions > actions = object->getAllowableActions( );

    CPPUNIT_ASSERT_MESSAGE( "GetContentStream allowable action should be true",
            actions->isDefined( libcmis::ObjectAction::GetContentStream ) &&
            actions->isAllowed( libcmis::ObjectAction::GetContentStream ) );
    CPPUNIT_ASSERT_MESSAGE( "CreateDocument allowable action should be false",
            actions->isDefined( libcmis::ObjectAction::CreateDocument ) &&
            !actions->isAllowed( libcmis::ObjectAction::CreateDocument ) );
}

void OneDriveTest::getFolderAllowableActionsTest( )
{
    curl_mockup_reset( );
    static const string objectId ("aFolderId");

    OneDriveSession session = getTestSession( USERNAME, PASSWORD );
    string url = BASE_URL + "/" + objectId;
    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", DATA_DIR "/onedrive/folder.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( objectId );

    boost::shared_ptr< libcmis::AllowableActions > actions = object->getAllowableActions( );

    CPPUNIT_ASSERT_MESSAGE( "CreateDocument allowable action should be true",
            actions->isDefined( libcmis::ObjectAction::CreateDocument ) &&
            actions->isAllowed( libcmis::ObjectAction::CreateDocument ) );

    CPPUNIT_ASSERT_MESSAGE( "GetContentStream allowable action should be false",
            actions->isDefined( libcmis::ObjectAction::GetContentStream ) &&
            !actions->isAllowed( libcmis::ObjectAction::GetContentStream ) );
}

CPPUNIT_TEST_SUITE_REGISTRATION( OneDriveTest );
