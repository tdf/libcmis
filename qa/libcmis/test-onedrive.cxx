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
        void getFolderTest( );
        void getChildrenTest( );
        void moveTest( );
        void getDocumentTest( );
        void getDocumentParentTest( );
        void getContentStreamTest( );
        void setContentStreamTest( );
        void createDocumentTest( );
        void getObjectByPathTest( );
        
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
        CPPUNIT_TEST( getFolderTest );
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST( moveTest );
        CPPUNIT_TEST( getDocumentTest );
        CPPUNIT_TEST( getDocumentParentTest );
        CPPUNIT_TEST( getContentStreamTest );
        CPPUNIT_TEST( setContentStreamTest );
        CPPUNIT_TEST( createDocumentTest );
        CPPUNIT_TEST( getObjectByPathTest );
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
                                   string ( "2014-06-09T08:24:04+0000" ),
                                   obj->getStringProperty( "cmis:creationDate" ) );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong object id",
                                   string ( "aFileId" ),
                                   obj->getStringProperty( "cmis:objectId" ) );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong author",
                                   string ( "onedriveUser" ),
                                   obj->getStringProperty( "cmis:createdBy" ) );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong file name",
                                   string ( "OneDriveFile" ),
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

void OneDriveTest::getFolderTest( )
{
    curl_mockup_reset( );

    OneDriveSession session = getTestSession( USERNAME, PASSWORD );
    static const string folderId( "aFolderId" );
    static const string parentId( "aParentId" );
    string url = BASE_URL + "/" + folderId;
    string parentUrl = BASE_URL + "/" + parentId;

    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", DATA_DIR "/onedrive/folder.json", 200, true);

    curl_mockup_addResponse( parentUrl.c_str( ), "",
                             "GET", DATA_DIR "/onedrive/parent-folder.json", 200, true);

    libcmis::FolderPtr folder = session.getFolder( folderId );
    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of libcmis::FolderPtr",
            NULL != folder );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder ID", folderId, folder->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder name", string( "OneDrive Folder" ), folder->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:folder" ), folder->getBaseType( ) );

    CPPUNIT_ASSERT_MESSAGE( "Missing folder parent", folder->getFolderParent( ).get( ) );
    CPPUNIT_ASSERT_MESSAGE( "Not a root folder", !folder->isRootFolder() );
    CPPUNIT_ASSERT_MESSAGE( "CreatedBy is missing", !folder->getCreatedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "CreationDate is missing", !folder->getCreationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "LastModificationDate is missing", !folder->getLastModificationDate( ).is_not_a_date_time() );
}

void OneDriveTest::getChildrenTest( )
{
    curl_mockup_reset( );

    OneDriveSession session = getTestSession( USERNAME, PASSWORD );
    static const string folderId ("aFolderId");
    string url = BASE_URL + "/" + folderId;
    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", DATA_DIR "/onedrive/folder.json", 200, true);

    libcmis::ObjectPtr obj = session.getObject( folderId );

    libcmis::FolderPtr folder = session.getFolder( folderId );
    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of libcmis::FolderPtr",
            NULL != folder );

    string childrenUrl = BASE_URL + "/" + folderId + "/files";
    curl_mockup_addResponse( childrenUrl.c_str( ), "",
                             "GET", DATA_DIR "/onedrive/folder-listed.json", 200, true);

    vector< libcmis::ObjectPtr > children= folder->getChildren( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad number of children", size_t( 2 ), children.size() );

    int folderCount = 0;
    int fileCount = 0;
    for ( vector< libcmis::ObjectPtr >::iterator it = children.begin( );
          it != children.end( ); ++it )
    {
        if ( NULL != boost::dynamic_pointer_cast< libcmis::Folder >( *it ) )
            ++folderCount;
        else
            ++fileCount;
    }
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of folder children", 1, folderCount );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of file children", 1, fileCount );
}

void OneDriveTest::moveTest( )
{
    curl_mockup_reset( );
    OneDriveSession session = getTestSession( USERNAME, PASSWORD );
    const string objectId( "aFileId" );
    const string sourceId( "aFolderId" );
    const string desId( "aParentId" );

    string url = BASE_URL + "/" + objectId;
    string sourceUrl = BASE_URL + "/" + sourceId;
    string desUrl = BASE_URL + "/" + desId;

    curl_mockup_addResponse( url.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/file.json", 200, true );
    curl_mockup_addResponse( url.c_str( ), "method=MOVE",
                               "POST", DATA_DIR "/onedrive/file.json", 200, true );
    curl_mockup_addResponse( desUrl.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/parent-folder.json", 200, true );
    curl_mockup_addResponse( sourceUrl.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/folder.json", 200, true );

    libcmis::ObjectPtr object = session.getObject( objectId );
    libcmis::FolderPtr source = session.getFolder( sourceId );
    libcmis::FolderPtr destination = session.getFolder( desId );

    object->move( source, destination );
    const char* moveRequest = curl_mockup_getRequestBody( url.c_str( ), "method=MOVE", "POST" );
    Json parentJson = Json::parse( string( moveRequest ) );
    string newParentId = parentJson["destination"].toString( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad new parent folder", desId, newParentId);
}

void OneDriveTest::getDocumentTest( )
{
    curl_mockup_reset( );
    static const string objectId ("aFileId");

    OneDriveSession session = getTestSession( USERNAME, PASSWORD );
    string url = BASE_URL + "/" + objectId;
    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", DATA_DIR "/onedrive/file.json", 200, true);

    libcmis::ObjectPtr obj = session.getObject( objectId );

    // Check if we got the document object.
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( obj );
    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of libcmis::DocumentPtr",
            NULL != document );

    // Test the document properties
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document ID", objectId, document->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document name",
                                  string( "OneDriveFile" ),
                                  document->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:document" ), document->getBaseType( ) );

    CPPUNIT_ASSERT_MESSAGE( "CreatedBy is missing", !document->getCreatedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "CreationDate is missing", !document->getCreationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "LastModificationDate is missing", !document->getLastModificationDate( ).is_not_a_date_time() );

    CPPUNIT_ASSERT_MESSAGE( "Content length is incorrect", 42 == document->getContentLength( ) );
}

void OneDriveTest::getDocumentParentTest( )
{
    curl_mockup_reset( );
    OneDriveSession session = getTestSession( USERNAME, PASSWORD );

    static const string documentId( "aFileId" );
    static const string parentId( "aParentId" );
    string url = BASE_URL + "/" + documentId;
    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", DATA_DIR "/onedrive/file.json", 200, true);

    string parentUrl = BASE_URL + "/" + parentId;
    curl_mockup_addResponse( parentUrl.c_str( ), "",
                             "GET", DATA_DIR "/onedrive/parent-folder.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( "aFileId" );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );

    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );

    vector< libcmis::FolderPtr > parents= document->getParents( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad number of parents", size_t( 1 ), parents.size() );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parent Id", parentId, parents[0]->getId( ) );
}

void OneDriveTest::getContentStreamTest( )
{
    curl_mockup_reset( );
    OneDriveSession session = getTestSession( USERNAME, PASSWORD );

    static const string documentId( "aFileId" );
    string url = BASE_URL + "/" + documentId;
    curl_mockup_addResponse( url.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/file.json", 200, true);
    string expectedContent( "Test content stream" );
    string downloadUrl = "sourceUrl";
    curl_mockup_addResponse( downloadUrl.c_str( ), "", "GET", expectedContent.c_str( ), 0, false );

    libcmis::ObjectPtr object = session.getObject( documentId );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );

    try
    {
        boost::shared_ptr< istream >  is = document->getContentStream( );
        ostringstream out;
        out << is->rdbuf();

        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Content stream doesn't match", expectedContent, out.str( ) );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void OneDriveTest::setContentStreamTest( )
{
    curl_mockup_reset( );
    OneDriveSession session = getTestSession( USERNAME, PASSWORD );

    const string documentId( "aFileId" );

    string url = BASE_URL + "/" + documentId;
    string putUrl = BASE_URL + "/aParentId/files/OneDriveFile";
    curl_mockup_addResponse( url.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/file.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( documentId );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );

    curl_mockup_addResponse( url.c_str( ), "",
                               "PUT", DATA_DIR "/onedrive/file.json", 200, true);
    curl_mockup_addResponse( putUrl.c_str( ), "overwrite=true", "PUT", "Updated", 0, false );
    try
    {
        string expectedContent( "Test set content stream" );
        boost::shared_ptr< ostream > os ( new stringstream ( expectedContent ) );
        string filename( "aFileName" );
        document->setContentStream( os, "text/plain", filename );

        CPPUNIT_ASSERT_MESSAGE( "Object not refreshed during setContentStream", object->getRefreshTimestamp( ) > 0 );

        // Check if metadata has been properly uploaded
        const char* meta = curl_mockup_getRequestBody( url.c_str( ), "", "PUT" );
        string expectedMeta = "{\n    \"name\": \"aFileName\"\n}\n";
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad meta uploaded", expectedMeta, string( meta ) );
        // Check the content has been properly uploaded
        const char* content = curl_mockup_getRequestBody( putUrl.c_str( ), "", "PUT" );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad content uploaded", expectedContent, string( content ) );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void OneDriveTest::createDocumentTest( )
{
    curl_mockup_reset( );
    OneDriveSession session = getTestSession( USERNAME, PASSWORD );
    const string documentId( "aFileId" );
    const string folderId( "aParentId" );
    const string filename( "aFileName" );

    const string folderUrl = BASE_URL + "/" + folderId;
    const string uploadUrl = folderUrl + "/files/" + filename;
    const string documentUrl = BASE_URL + "/" + documentId;
    const string uploadLocation = BASE_URL + "/aParentId/files/OneDriveFile";

    curl_mockup_addResponse( folderUrl.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/parent-folder.json", 200, true );
    curl_mockup_addResponse( uploadUrl.c_str( ), "",
                               "PUT", DATA_DIR "/onedrive/new-file.json", 200, true );
    curl_mockup_addResponse( documentUrl.c_str( ), "",
                               "PUT", DATA_DIR "/onedrive/file.json", 200, true );
    curl_mockup_addResponse( documentUrl.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/file.json", 200, true );
    curl_mockup_addResponse( uploadLocation.c_str( ), "overwrite=true",
                               "PUT", DATA_DIR "/onedrive/file.json", 200, true );

    libcmis::FolderPtr parent = session.getFolder( folderId );
    try
    {
        string expectedContent( "Test set content stream" );
        boost::shared_ptr< ostream > os ( new stringstream ( expectedContent ) );
        PropertyPtrMap properties;

        parent->createDocument( properties, os, "text/plain", filename );

        curl_mockup_getRequestBody( documentUrl.c_str( ), "", "PUT" );
        const char* content = curl_mockup_getRequestBody( uploadLocation.c_str( ), "", "PUT" );

        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad content uploaded", expectedContent, string( content ) );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what( );
        CPPUNIT_FAIL( msg.c_str( ) );
    }
}

void OneDriveTest::getObjectByPathTest( )
{
    curl_mockup_reset( );
    OneDriveSession session = getTestSession( USERNAME, PASSWORD );
    const string documentId( "rightFile" );
    const string wrongDocumentId( "wrongFile" );
    const string folderAId( "folderA" ); // root
    const string folderBId( "folderB" );
    const string folderCId( "folderC" );
    const string path( "/Folder B/Folder C/OneDriveFile" );

    const string documentUrl = BASE_URL + "/" + documentId;
    const string wrongDocumentUrl = BASE_URL + "/" + wrongDocumentId;
    const string folderAUrl = BASE_URL + "/" + folderAId;
    const string folderBUrl = BASE_URL + "/" + folderBId;
    const string folderCUrl = BASE_URL + "/" + folderCId;
    const string searchUrl = BASE_URL + "/me/skydrive/search";

    curl_mockup_addResponse( documentUrl.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/searched-file.json", 200, true );
    curl_mockup_addResponse( wrongDocumentUrl.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/searched-wrong-file.json", 200, true );
    curl_mockup_addResponse( searchUrl.c_str( ), "q=OneDriveFile",
                                "GET", DATA_DIR "/onedrive/search-result.json", 200, true );
    curl_mockup_addResponse( folderAUrl.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/folderA.json", 200, true );
    curl_mockup_addResponse( folderBUrl.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/folderB.json", 200, true );
    curl_mockup_addResponse( folderCUrl.c_str( ), "",
                               "GET", DATA_DIR "/onedrive/folderC.json", 200, true );

    libcmis::ObjectPtr object = session.getObjectByPath( path );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong objectFetched", documentId, object->getId( ) );
}

CPPUNIT_TEST_SUITE_REGISTRATION( OneDriveTest );
