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
class GDriveTest : public CppUnit::TestFixture
{
    public:
        void sessionAuthenticationTest( );
        void getRepositoriesTest( );
        void getTypeTest( );
        void getObjectTest( );
        void getDocumentTest( );
        void getFolderTest( );
        void getDocumentParentsTest( );
        void getContentStreamTest( );
        void setContentStreamTest( );
        void setContentStreamGdocTest( );
        void getChildrenTest( );
        void getDocumentAllowableActionsTest( );
        void getFolderAllowableActionsTest( );
        void checkOutTest( );
        void checkInTest( );
        void deleteTest( );
        void moveTest( );
        void createDocumentTest( );
        void createFolderTest( );
        void updatePropertiesTest( );
        void removeTreeTest( );
        void getContentStreamWithRenditionsTest( );

        CPPUNIT_TEST_SUITE( GDriveTest );
        CPPUNIT_TEST( sessionAuthenticationTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( getTypeTest );
        CPPUNIT_TEST( getObjectTest );
        CPPUNIT_TEST( getDocumentTest );
        CPPUNIT_TEST( getFolderTest );
        CPPUNIT_TEST( getDocumentParentsTest );
        CPPUNIT_TEST( getContentStreamTest ); 
        CPPUNIT_TEST( setContentStreamTest );  
        CPPUNIT_TEST( setContentStreamGdocTest );
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST( getDocumentAllowableActionsTest );
        CPPUNIT_TEST( getFolderAllowableActionsTest );
        CPPUNIT_TEST( checkOutTest );
        CPPUNIT_TEST( checkInTest );
        CPPUNIT_TEST( deleteTest );
        CPPUNIT_TEST( moveTest );
        CPPUNIT_TEST( createDocumentTest );
        CPPUNIT_TEST( createFolderTest );
        CPPUNIT_TEST( updatePropertiesTest );
        CPPUNIT_TEST( removeTreeTest );
        CPPUNIT_TEST( getContentStreamWithRenditionsTest );
        CPPUNIT_TEST_SUITE_END( );

    private:
        GDriveSession getTestSession( string username, string password );
};

void GDriveTest::getDocumentTest( )
{
    curl_mockup_reset( );
    static const string objectId ("aFileId");
 
    GDriveSession session = getTestSession( USERNAME, PASSWORD );
    string url = BASE_URL + "/files/" + objectId;
    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", "data/gdrive/document.json", 200, true);

    libcmis::ObjectPtr obj = session.getObject( objectId );
 
    // Check if we got the document object.
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( obj );
    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of libcmis::DocumentPtr",
            NULL != document );
 
    // Test the document properties
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document ID", objectId, document->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document name", 
                                  string( "GDrive File" ), 
                                  document->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document type", 
                                  string( "application/vnd.google-apps.form" ), 
                                  document->getContentType( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:document" ), document->getBaseType( ) );

    CPPUNIT_ASSERT_MESSAGE( "CreatedBy is missing", !document->getCreatedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "CreationDate is missing", !document->getCreationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "LastModifiedBy is missing", !document->getLastModifiedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "LastModificationDate is missing", !document->getLastModificationDate( ).is_not_a_date_time() );
 
    CPPUNIT_ASSERT_MESSAGE( "Content length is incorrect", 123 == document->getContentLength( ) );

}

void GDriveTest::getFolderTest( )
{
    curl_mockup_reset( );

    GDriveSession session = getTestSession( USERNAME, PASSWORD );    
    static const string folderId( "aFolderId" );
    static const string parentId( "parentID" );
    string url = BASE_URL + "/files/" + folderId;
    string parentUrl = BASE_URL + "/files/" + parentId;

    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", "data/gdrive/folder.json", 200, true);
    curl_mockup_addResponse( parentUrl.c_str( ), "",
                             "GET", "data/gdrive/folder.json", 200, true);
    // Check if we got the Folder object.
    libcmis::FolderPtr folder = session.getFolder( folderId );
    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of libcmis::FolderPtr",
            NULL != folder );


    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder ID", folderId, folder->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder name", string( "testFolder" ), folder->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:folder" ), folder->getBaseType( ) );
    CPPUNIT_ASSERT_MESSAGE( "Missing folder parent", folder->getFolderParent( ).get( ) );
    CPPUNIT_ASSERT_MESSAGE( "Not a root folder", !folder->isRootFolder() );
 
    CPPUNIT_ASSERT_MESSAGE( "CreatedBy is missing", !folder->getCreatedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "CreationDate is missing", !folder->getCreationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "LastModifiedBy is missing", !folder->getLastModifiedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "LastModificationDate is missing", !folder->getLastModificationDate( ).is_not_a_date_time() );
}

void GDriveTest::getDocumentParentsTest( )
{
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );

    static const string documentId( "aFileId" );
    static const string parentId( "aFolderId" );
    static const string parentId2( "aNewFolderId" );
    string url = BASE_URL + "/files/" + documentId;
    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", "data/gdrive/document.json", 200, true);

    string parent1Url = BASE_URL + "/files/" + parentId;
    string parent2Url = BASE_URL + "/files/" + parentId2;
    curl_mockup_addResponse( parent1Url.c_str( ), "",
                             "GET", "data/gdrive/folder.json", 200, true);
    curl_mockup_addResponse( parent2Url.c_str( ), "",
                             "GET", "data/gdrive/folder2.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( "aFileId" );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );
 
    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );
    
    vector< libcmis::FolderPtr > parents= document->getParents( );
 
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad number of parents", size_t( 2 ), parents.size() );
    
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parent Id", parentId, parents[0]->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parent Id", parentId2, parents[1]->getId( ) );
}

void GDriveTest::getContentStreamTest( )
{
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );

    static const string documentId( "aFileId" );
    string url = BASE_URL + "/files/" + documentId;
    curl_mockup_addResponse( url.c_str( ), "",
                               "GET", "data/gdrive/document.json", 200, true);
    string expectedContent( "Test content stream" );
    string downloadUrl = "https://downloadLink";
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

void GDriveTest::setContentStreamTest( )
{   
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );

    const string documentId( "aFileId" );
    const string uploadBaseUrl = "https://www.googleapis.com/upload/drive/v2/files/";

    string url = BASE_URL + "/files/" + documentId;
    string putUrl = uploadBaseUrl + documentId;
    curl_mockup_addResponse( url.c_str( ), "",
                               "GET", "data/gdrive/document2.json", 200, true);
       
    libcmis::ObjectPtr object = session.getObject( documentId );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );

    curl_mockup_addResponse( url.c_str( ), "",
                               "PUT", "data/gdrive/document2.json", 200, true);
    curl_mockup_addResponse( putUrl.c_str( ), "", "PUT", "Updated", 0, false );
    try
    {
        string expectedContent( "Test set content stream" );
        boost::shared_ptr< ostream > os ( new stringstream ( expectedContent ) );
        string filename( "aFileName" );
        document->setContentStream( os, "text/plain", filename );
        
        CPPUNIT_ASSERT_MESSAGE( "Object not refreshed during setContentStream", object->getRefreshTimestamp( ) > 0 );

        // Check if metadata has been properly uploaded
        const char* meta = curl_mockup_getRequest( url.c_str( ), "", "PUT" );
        string expectedMeta = "{ \"title\": \"" + filename + "\"" + " }";
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad meta uploaded", expectedMeta, string( meta ) );
        // Check the content has been properly uploaded
        const char* content = curl_mockup_getRequest( putUrl.c_str( ), "", "PUT" );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad content uploaded", expectedContent, string( content ) );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void GDriveTest::setContentStreamGdocTest( )
{
    
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );

    const string documentId( "aFileId" );
    const string uploadBaseUrl = "https://www.googleapis.com/upload/drive/v2/files/";

    string url = BASE_URL + "/files/" + documentId;
    string putUrl = uploadBaseUrl + documentId;
    curl_mockup_addResponse( url.c_str( ), "",
                               "GET", "data/gdrive/document.json", 200, true);
   
    libcmis::ObjectPtr object = session.getObject( documentId );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );

    curl_mockup_addResponse( url.c_str( ), "convert=true",
                               "PUT", "data/gdrive/document.json", 200, true);
    curl_mockup_addResponse( putUrl.c_str( ), "convert=true", "PUT", "Updated", 0, false );
    try
    {
        string expectedContent( "Test set content stream" );
        boost::shared_ptr< ostream > os ( new stringstream ( expectedContent ) );
        string filename( "aFileName" );
        document->setContentStream( os, "text/plain", filename );
        
        CPPUNIT_ASSERT_MESSAGE( "Object not refreshed during setContentStream", object->getRefreshTimestamp( ) > 0 );

        // Check the content has been properly uploaded
        const char* content = curl_mockup_getRequest( putUrl.c_str( ), "", "PUT" );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad content uploaded", expectedContent, string( content ) );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void GDriveTest::getChildrenTest( )
{
    curl_mockup_reset( );

    GDriveSession session = getTestSession( USERNAME, PASSWORD );    
    static const string folderId ("aFolderId");
    string url = BASE_URL + "/files/" + folderId;
    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", "data/gdrive/folder.json", 200, true);
    
    string urlChildFolder = BASE_URL + "/files/" + string ("aChildFolder");
    curl_mockup_addResponse( urlChildFolder.c_str( ), "",
                             "GET", "data/gdrive/folder.json", 200, true);

    string urlChildDocument = BASE_URL + "/files/" + string ("aChildDocument");
    curl_mockup_addResponse( urlChildDocument.c_str( ), "",
                             "GET", "data/gdrive/document.json", 200, true);
    
    libcmis::ObjectPtr obj = session.getObject( folderId );
 
    // Check if we got the Folder object.
    libcmis::FolderPtr folder = boost::dynamic_pointer_cast< libcmis::Folder >( obj );

    CPPUNIT_ASSERT_MESSAGE( "Folder expected", folder != NULL );
    
    
    string query = "q=\"" + folderId + "\"+in+parents+and+trashed+=+false";
    string childrenUrl = BASE_URL + "/files";
    curl_mockup_addResponse( childrenUrl.c_str( ), query.c_str( ),
                             "GET", "data/gdrive/folder_children.json", 200, true);

    vector< libcmis::ObjectPtr > children= folder->getChildren( );
 
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad number of children", size_t( 2 ), children.size() );
    
    int folderCount = 0;
    int documentCount = 0;
    for ( vector< libcmis::ObjectPtr >::iterator it = children.begin( );
          it != children.end( ); ++it )
    {
        if ( NULL != boost::dynamic_pointer_cast< libcmis::Folder >( *it ) )
            ++folderCount;
        else if ( NULL != boost::dynamic_pointer_cast< libcmis::Document >( *it ) )
            ++documentCount;
    }
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of folder children", 1, folderCount );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of document children", 1, documentCount );
}

void GDriveTest::getTypeTest( )
{
    curl_mockup_reset( );
 
    GDriveSession session = getTestSession( USERNAME, PASSWORD );

    string expectedId( "cmis:document" );
    libcmis::ObjectTypePtr actual = session.getType( expectedId );
 
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Id for fetched type", expectedId, actual->getId( ) );
}

void GDriveTest::getRepositoriesTest( )
{
     curl_mockup_reset( );
 
     GDriveSession session = getTestSession( USERNAME, PASSWORD );
     vector< libcmis::RepositoryPtr > actual = session.getRepositories( );
 
     CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of repositories", size_t( 1 ), 
                                   actual.size( ) );
     CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong repository found",
                                   string ( "GoogleDrive" ),
                                   actual.front()->getId( ) );
}

void GDriveTest::sessionAuthenticationTest( )
{
    GDriveSession session = getTestSession( USERNAME, PASSWORD );
    string empty;
    
    // Check authentication request
    string authRequest( curl_mockup_getRequest( LOGIN_URL.c_str(), empty.c_str( ),
                                                "POST" ) );
    string expectedAuthRequest =
        string ( "continue=redirectLink&scope=Scope&service=lso&GALX=cookie"
                 "&Email=") + USERNAME + string("&Passwd=") + PASSWORD;

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong authentication request",
                                  expectedAuthRequest, authRequest );
    
    // Check code request
    string codeRequest( curl_mockup_getRequest( APPROVAL_URL.c_str(),
                        empty.c_str( ), "POST" ) );
    string expectedCodeRequest = string( "state_wrapper=stateWrapper&submit_access=true" );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong approval request", 
                                  expectedCodeRequest, codeRequest);
    
    // Check token request
    string expectedTokenRequest = 
        string( "code=AuthCode") + 
        string( "&client_id=") + CLIENT_ID +
        string( "&client_secret=") + CLIENT_SECRET + 
        string( "&redirect_uri=") + REDIRECT_URI + 
        string( "&grant_type=authorization_code" );

    string tokenRequest( curl_mockup_getRequest( TOKEN_URL.c_str(), empty.c_str( ),
                                                 "POST" ));
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

void GDriveTest::getObjectTest()
{
    static const string objectId ("aFileId");

    GDriveSession session = getTestSession( USERNAME, PASSWORD );
    string url = BASE_URL + "/files/" + objectId;
    curl_mockup_addResponse ( url.c_str( ), "",
                              "GET", "data/gdrive/gdoc-file.json", 200, true);
    libcmis::ObjectPtr object = session.getObject( objectId );
    boost::shared_ptr<GDriveObject> obj = boost::dynamic_pointer_cast
                                            <GDriveObject>(object);
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Object Id", objectId,
                                                     obj->getId( ) );
}

void GDriveTest::getDocumentAllowableActionsTest( )
{
    curl_mockup_reset( );
    static const string objectId ("aFileId");
 
    GDriveSession session = getTestSession( USERNAME, PASSWORD );
    string url = BASE_URL + "/files/" + objectId;
    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", "data/gdrive/document.json", 200, true);

    libcmis::ObjectPtr obj = session.getObject( objectId );
 
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( obj );

    boost::shared_ptr< libcmis::AllowableActions > actions = document->getAllowableActions( );

    CPPUNIT_ASSERT_MESSAGE( "GetContentStream allowable action should be true",
            actions->isDefined( libcmis::ObjectAction::GetContentStream ) &&
            actions->isAllowed( libcmis::ObjectAction::GetContentStream ) );
    CPPUNIT_ASSERT_MESSAGE( "CreateDocument allowable action should be false",
            actions->isDefined( libcmis::ObjectAction::CreateDocument ) &&
            !actions->isAllowed( libcmis::ObjectAction::CreateDocument ) );
}

void GDriveTest::getFolderAllowableActionsTest( )
{
    curl_mockup_reset( );
    static const string folderId ("aFolderId");
 
    GDriveSession session = getTestSession( USERNAME, PASSWORD );
    string url = BASE_URL + "/files/" + folderId;
    curl_mockup_addResponse( url.c_str( ), "",
                             "GET", "data/gdrive/folder.json", 200, true);

    libcmis::FolderPtr folder = session.getFolder( folderId );

    boost::shared_ptr< libcmis::AllowableActions > actions = folder->getAllowableActions( );

    CPPUNIT_ASSERT_MESSAGE( "CreateDocument allowable action should be true",
            actions->isDefined( libcmis::ObjectAction::CreateDocument ) &&
            actions->isAllowed( libcmis::ObjectAction::CreateDocument ) );

    CPPUNIT_ASSERT_MESSAGE( "GetContentStream allowable action should be false",
            actions->isDefined( libcmis::ObjectAction::GetContentStream ) &&
            !actions->isAllowed( libcmis::ObjectAction::GetContentStream ) );
}

void GDriveTest::checkOutTest( )
{
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );

    static const string documentId( "aFileId" );
    string url = BASE_URL + "/files/" + documentId;
    curl_mockup_addResponse( url.c_str( ), "",
                               "GET", "data/gdrive/document.json", 200, true);
    
    libcmis::ObjectPtr object = session.getObject( documentId );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );
    libcmis::DocumentPtr checkout = document->checkOut( );
    CPPUNIT_ASSERT_MESSAGE( "CheckOut failed", NULL != checkout );
}

void GDriveTest::checkInTest( )
{
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );

    const string documentId( "aFileId" );
    const string uploadBaseUrl = "https://www.googleapis.com/upload/drive/v2/files/";

    string url = BASE_URL + "/files/" + documentId;
    string putUrl = uploadBaseUrl + documentId;
    curl_mockup_addResponse( url.c_str( ), "",
                               "GET", "data/gdrive/document2.json", 200, true);
   
    libcmis::ObjectPtr object = session.getObject( documentId );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );

    curl_mockup_addResponse( url.c_str( ), "",
                               "PUT", "data/gdrive/document2.json", 200, true);
    curl_mockup_addResponse( putUrl.c_str( ), "", "PUT", "Updated", 0, false );
   
    string expectedContent( "content stream" );
    boost::shared_ptr< ostream > os ( new stringstream ( expectedContent ) );
    string filename( "aFileName" );

    const PropertyPtrMap& properties = document->getProperties( );
    libcmis::DocumentPtr checkIn = document->checkIn( true, "", properties, os, "text/plain", filename);
    CPPUNIT_ASSERT_MESSAGE( "CheckIn failed", NULL != checkIn );
}

void GDriveTest::deleteTest( )
{
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );

    const string objectId( "aFileId" );

    string url = BASE_URL + "/files/" + objectId;
    curl_mockup_addResponse( url.c_str( ), "",
                               "GET", "data/gdrive/document2.json", 200, true);
    curl_mockup_addResponse( url.c_str( ),"", "DELETE", "", 204, false);

    libcmis::ObjectPtr object = session.getObject( objectId );
    
    object->remove( );
    const char* deleteRequest = curl_mockup_getRequest( url.c_str( ), "", "DELETE" );
    CPPUNIT_ASSERT_MESSAGE( "Delete request not sent", deleteRequest );
}

void GDriveTest::moveTest( )
{
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );
    const string objectId( "aFileId" );
    const string sourceId( "aFolderId" );
    const string desId( "aNewFolderId" );

    string url = BASE_URL + "/files/" + objectId;
    string sourceUrl = BASE_URL + "/files/" + sourceId;
    string desUrl = BASE_URL + "/files/" + desId;
    curl_mockup_addResponse( url.c_str( ), "",
                               "GET", "data/gdrive/document2.json", 200, true );
    curl_mockup_addResponse( url.c_str( ), "", 
                               "PUT", "data/gdrive/document2.json", 200, true );
    curl_mockup_addResponse( sourceUrl.c_str( ), "",
                               "GET", "data/gdrive/folder.json", 200, true );
    curl_mockup_addResponse( desUrl.c_str( ), "",
                               "GET", "data/gdrive/folder2.json", 200, true );

    libcmis::ObjectPtr object = session.getObject( objectId );
    
    libcmis::FolderPtr source = session.getFolder( sourceId );
    libcmis::FolderPtr destination = session.getFolder( desId );    

    object->move( source, destination );
    const char* moveRequest = curl_mockup_getRequest( url.c_str( ), "", "PUT" );
    Json parentJson = Json::parse( string( moveRequest ) );
    string newParentId = parentJson["parents"][0]["id"].toString( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad new parent folder", 
                                  desId, newParentId);
}

void GDriveTest::createDocumentTest( )
{
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );
    const string documentId( "aFileId" );
    const string folderId( "aFolderId" );

    const string folderUrl = BASE_URL + "/files/" + folderId;
    const string metaUrl = BASE_URL + "/files/";
    const string uploadBaseUrl = "https://www.googleapis.com/upload/drive/v2/files/";
    string uploadUrl = uploadBaseUrl + documentId;
    string documentUrl = metaUrl + documentId;

    curl_mockup_addResponse( folderUrl.c_str( ), "", 
                               "GET", "data/gdrive/folder.json", 200, true );
    curl_mockup_addResponse( metaUrl.c_str( ), "",
                               "POST", "data/gdrive/document.json", 200, true );
    curl_mockup_addResponse( uploadUrl.c_str( ), "",
                               "PUT", "updated", 0, false );
    curl_mockup_addResponse( documentUrl.c_str( ), "",
                               "GET", "data/gdrive/document2.json", 200, true);
    
    libcmis::FolderPtr parent = session.getFolder( folderId );

    try
    {
        string expectedContent( "Test set content stream" );
        boost::shared_ptr< ostream > os ( new stringstream ( expectedContent ) );
        string filename( "aFileName" );
        PropertyPtrMap properties;
        
        // function to test
        parent->createDocument( properties, os, "text/plain", filename );

        const char* createRequest = curl_mockup_getRequest( metaUrl.c_str( ), "", "POST" );
        Json request = Json::parse( string( createRequest ) );
        string sentParentId = request["parents"][0]["id"].toString( );
        string sentFilename = request["title"].toString( );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad parents sent", folderId, sentParentId );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad filename sent", filename, sentFilename );

        const char* content = curl_mockup_getRequest( uploadUrl.c_str( ), "", "PUT" );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad content uploaded", expectedContent, string( content ) );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void GDriveTest::createFolderTest( )
{
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );
    const string folderId( "aFolderId" );

    const string folderUrl = BASE_URL + "/files/" + folderId;
    const string metaUrl = BASE_URL + "/files";
   
    curl_mockup_addResponse( folderUrl.c_str( ), "", 
                               "GET", "data/gdrive/folder.json", 200, true );
    curl_mockup_addResponse( metaUrl.c_str( ), "",
                               "POST", "data/gdrive/folder2.json", 200, true );    
    libcmis::FolderPtr parent = session.getFolder( folderId );
    try
    {       
        PropertyPtrMap properties;
        
        // function to test
        parent->createFolder( properties );

        const char* createRequest = curl_mockup_getRequest( metaUrl.c_str( ), "", "POST" );
        Json request = Json::parse( string( createRequest ) );

        string sentParentId = request["parents"][0]["id"].toString( );
        string sentMimeType = request["mimeType"].toString( );
        string expectedMimeType( "application/vnd.google-apps.folder" );
     
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad parents sent", folderId, sentParentId );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad mimeType", expectedMimeType, sentMimeType );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void GDriveTest::removeTreeTest( )
{
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );
    const string folderId( "aFolderId" );

    const string folderUrl = BASE_URL + "/files/" + folderId;
    const string trashUrl = folderUrl + "/trash";
   
    curl_mockup_addResponse( folderUrl.c_str( ), "", 
                               "GET", "data/gdrive/folder.json", 200, true );
    curl_mockup_addResponse( trashUrl.c_str( ), "",
                               "POST", "", 200, false );    
    libcmis::FolderPtr folder = session.getFolder( folderId );

    // just make sure it doesn't crash 
    folder->removeTree( );
}

void GDriveTest::getContentStreamWithRenditionsTest( )
{
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );

    static const string documentId( "aFileId" );
    string url = BASE_URL + "/files/" + documentId;
    curl_mockup_addResponse( url.c_str( ), "",
                               "GET", "data/gdrive/document.json", 200, true);
    libcmis::ObjectPtr object = session.getObject( documentId );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );

    // pdf stream
    string pdfContent( "pdf Content stream" );
    string pdfUrl = "pdflink";
    curl_mockup_addResponse( pdfUrl.c_str( ), "", "GET", pdfContent.c_str( ), 0, false );    
    
    try
    {
        boost::shared_ptr< istream >  is = document->getContentStream( "application/pdf" );
        ostringstream out;
        out << is->rdbuf();
 
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Content stream doesn't match", pdfContent, out.str( ) );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }

    // ODF stream
    string odfContent( "open document Content stream" );
    string odfUrl = "https://downloadLink";
    curl_mockup_addResponse( odfUrl.c_str( ), "", "GET", odfContent.c_str( ), 0, false );       
    try
    {
        boost::shared_ptr< istream >  is = document->getContentStream( "application/x-vnd.oasis.opendocument.spreadsheet" );
        ostringstream out;
        out << is->rdbuf();
 
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Content stream doesn't match", odfContent, out.str( ) );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }

    // MS stream
    string msContent( "office document Content stream" );
    string msUrl = "xlslink";
    curl_mockup_addResponse( msUrl.c_str( ), "", "GET", msContent.c_str( ), 0, false );       
    try
    {
        boost::shared_ptr< istream >  is = document->getContentStream( 
                    "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet" );
        ostringstream out;
        out << is->rdbuf();
 
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Content stream doesn't match", msContent, out.str( ) );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void GDriveTest::updatePropertiesTest( )
{
    curl_mockup_reset( );
    GDriveSession session = getTestSession( USERNAME, PASSWORD );
    const string documentId( "aFileId" );

    const string documentUrl = BASE_URL + "/files/" + documentId;
   
    curl_mockup_addResponse( documentUrl.c_str( ), "", 
                               "GET", "data/gdrive/document.json", 200, true );
    curl_mockup_addResponse( documentUrl.c_str( ), "",
                               "PUT", "data/gdrive/document.json", 200, true );   
 
    libcmis::ObjectPtr document = session.getObject( documentId );
 
    document->updateProperties( document->getProperties( ) );
    
    const char* updateRequest = curl_mockup_getRequest( documentUrl.c_str( ), "", "PUT" );

    // Check if properties keys are properly converted back before sending
    Json json = Json::parse( string( updateRequest ) );
    string id = json["id"].toString( );
    string createdDate = json["createdDate"].toString( );
    string title = json["title"].toString( );
    string mimeType = json["mimeType"].toString( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "id key not converted", documentId, id );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "createdDate key not converted", 
                    string( "2010-04-28T14:53:23.141Z"), createdDate );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "title key not converted", 
                                  string( "GDrive File"), title );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "mimeType key not converted", 
                    string( "application/vnd.google-apps.form"), mimeType );
}

GDriveSession GDriveTest::getTestSession( string username, string password )
{
    libcmis::OAuth2DataPtr oauth2(
        new libcmis::OAuth2Data( AUTH_URL, TOKEN_URL, SCOPE,
                                 REDIRECT_URI, CLIENT_ID, CLIENT_SECRET ));
    curl_mockup_reset( );
    string empty;
    //login response
    string loginIdentifier = string("scope=") + SCOPE +
                             string("&redirect_uri=") + REDIRECT_URI +
                             string("&response_type=code") +
                             string("&client_id=") + CLIENT_ID;
    curl_mockup_addResponse ( AUTH_URL.c_str(), loginIdentifier.c_str( ),
                            "GET", "data/gdrive/login.html", 200, true);

    //authentication response
    curl_mockup_addResponse( LOGIN_URL.c_str( ), empty.c_str( ), "POST",
                             "data/gdrive/approve.html", 200, true);

    //approval response
    curl_mockup_addResponse( APPROVAL_URL.c_str( ), empty.c_str( ),
                             "POST", "data/gdrive/authcode.html", 200, true);

    curl_mockup_addResponse ( TOKEN_URL.c_str( ), empty.c_str( ), "POST",
                              "data/gdrive/token-response.json", 200, true );

    return GDriveSession( BASE_URL, username, password, oauth2, false );
}

CPPUNIT_TEST_SUITE_REGISTRATION( GDriveTest );
