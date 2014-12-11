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
#include "test-helpers.hxx"
#include "sharepoint-document.hxx"
#include "sharepoint-object.hxx"
#include "sharepoint-property.hxx"
#include "sharepoint-session.hxx"

using namespace std;
using namespace libcmis;

static const string USERNAME( "mock-user" );
static const string PASSWORD( "mock-password" );
static const string BASE_URL( "http://base/_api/Web" );
static const string CONTEXTINFO_URL( "http://base/_api/contextinfo" );

class SharePointTest : public CppUnit::TestFixture
{
    public:
        void setRepositoryTest( );
        void getRepositoriesTest( );
        void getObjectTest( );
        void propertiesTest( );
        void deleteTest( );
        void xdigestExpiredTest( );
        void getFileAllowableActionsTest( );
        void getFolderAllowableActionsTest( );
        void getDocumentTest( );
        void getContentStreamTest( );
        void setContentStreamTest( );
        void checkOutTest( );
        void checkInTest( );
        void getAllVersionsTest( );
        void getFolderTest( );
        void getChildrenTest( );
        void createFolderTest( );
        void createDocumentTest( );
        void moveTest( );
        void getObjectByPathTest( );

        void sessionCopyTest( );
        void propertyCopyTest( );
        void objectCopyTest( );
       
        CPPUNIT_TEST_SUITE( SharePointTest );
        CPPUNIT_TEST( setRepositoryTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( getObjectTest );
        CPPUNIT_TEST( propertiesTest );
        CPPUNIT_TEST( deleteTest );
        CPPUNIT_TEST( xdigestExpiredTest );
        CPPUNIT_TEST( getFileAllowableActionsTest );
        CPPUNIT_TEST( getFolderAllowableActionsTest );
        CPPUNIT_TEST( getDocumentTest );
        CPPUNIT_TEST( getContentStreamTest );
        CPPUNIT_TEST( setContentStreamTest );
        CPPUNIT_TEST( checkOutTest );
        CPPUNIT_TEST( checkInTest );
        CPPUNIT_TEST( getAllVersionsTest );
        CPPUNIT_TEST( getFolderTest );
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST( createFolderTest );
        CPPUNIT_TEST( createDocumentTest );
        CPPUNIT_TEST( moveTest );
        CPPUNIT_TEST( getObjectByPathTest );
        CPPUNIT_TEST( sessionCopyTest );
        CPPUNIT_TEST( propertyCopyTest );
        CPPUNIT_TEST( objectCopyTest );
        CPPUNIT_TEST_SUITE_END( );

    private:
        SharePointSession getTestSession( string username, string password );
};

SharePointSession SharePointTest::getTestSession( string username, string password )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( BASE_URL.c_str( ), "", "GET", "", 401, false );
    curl_mockup_addResponse( ( BASE_URL + "/currentuser" ).c_str( ), "", "GET",
                             DATA_DIR "/sharepoint/auth-resp.json", 200, true );
    curl_mockup_addResponse( CONTEXTINFO_URL.c_str( ), "", "POST",
                             DATA_DIR "/sharepoint/xdigest.json", 200, true );

    return SharePointSession( BASE_URL, username, password, false );
}

void SharePointTest::setRepositoryTest( )
{
     curl_mockup_reset( );

     SharePointSession session = getTestSession( USERNAME, PASSWORD );
     CPPUNIT_ASSERT_MESSAGE( "setRepository should never fail", session.setRepository( "Anything" ));
}

void SharePointTest::getRepositoriesTest( )
{
     curl_mockup_reset( );

     SharePointSession session = getTestSession( USERNAME, PASSWORD );
     vector< libcmis::RepositoryPtr > actual = session.getRepositories( );

     CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of repositories", size_t( 1 ),
                                   actual.size( ) );
     CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong repository found",
                                   string ( "SharePoint" ),
                                   actual.front()->getId( ) );
}

void SharePointTest::getObjectTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string authorUrl = objectId + "/Author";
    curl_mockup_addResponse ( objectId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( objectId );
    boost::shared_ptr<SharePointObject> obj = boost::dynamic_pointer_cast
                                            <SharePointObject>( object );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Object Id", objectId,
                                                     obj->getId( ) );
}

void SharePointTest::propertiesTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string authorUrl = objectId + "/Author";
    curl_mockup_addResponse ( objectId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( objectId );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong creation date",
                                   string ( "2014-07-08T09:29:29Z" ),
                                   object->getStringProperty( "cmis:creationDate" ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong object id",
                                   string ( "http://base/_api/Web/aFileId" ),
                                   object->getStringProperty( "cmis:objectId" ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong author",
                                   string ( "aUserId" ),
                                   object->getStringProperty( "cmis:createdBy" ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong file name",
                                   string ( "SharePointFile" ),
                                   object->getStringProperty( "cmis:contentStreamFileName" ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong checkin comment",
                                   string ( "aCheckinComment" ),
                                   object->getStringProperty( "cmis:checkinComment" ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong version",
                                   string ( "1.0" ),
                                   object->getStringProperty( "cmis:versionLabel" ) );
}

void SharePointTest::deleteTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string authorUrl = objectId + "/Author";
    curl_mockup_addResponse ( objectId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);
    curl_mockup_addResponse( objectId.c_str( ),"", "DELETE", "", 204, false);

    libcmis::ObjectPtr object = session.getObject( objectId );

    object->remove( );
    const struct HttpRequest* deleteRequest = curl_mockup_getRequest( objectId.c_str( ), "", "DELETE" );
    CPPUNIT_ASSERT_MESSAGE( "Delete request not sent", deleteRequest );
}

void SharePointTest::xdigestExpiredTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string authorUrl = objectId + "/Author";
    curl_mockup_addResponse ( objectId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);
    curl_mockup_addResponse( objectId.c_str( ),"", "DELETE", "", 401, false);
    curl_mockup_addResponse( CONTEXTINFO_URL.c_str( ), "", "POST",
                             DATA_DIR "/sharepoint/new-xdigest.json", 200, true );
    
    libcmis::ObjectPtr object = session.getObject( objectId );
    try
    {
        object->remove( );
    }
    catch ( ... )
    {
        if ( session.getHttpStatus( ) == 401 )
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE(
                   "wrong xdigest code",
                   string ( "new-xdigest-code" ),
                   session.m_digestCode );
        }
    }
}

void SharePointTest::getFileAllowableActionsTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string authorUrl = objectId + "/Author";
    curl_mockup_addResponse ( objectId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( objectId );
    boost::shared_ptr< libcmis::AllowableActions > actions = object->getAllowableActions( );

    CPPUNIT_ASSERT_MESSAGE( "GetContentStream allowable action should be true",
            actions->isDefined( libcmis::ObjectAction::GetContentStream ) &&
            actions->isAllowed( libcmis::ObjectAction::GetContentStream ) );
    CPPUNIT_ASSERT_MESSAGE( "CreateDocument allowable action should be false",
            actions->isDefined( libcmis::ObjectAction::CreateDocument ) &&
            !actions->isAllowed( libcmis::ObjectAction::CreateDocument ) );
}

void SharePointTest::getFolderAllowableActionsTest( )
{
    static const string objectId ( "http://base/_api/Web/aFolderId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string folderPropUrl = objectId + "/Properties";
    curl_mockup_addResponse ( objectId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/folder.json", 200, true);
    curl_mockup_addResponse ( folderPropUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/folder-properties.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( objectId );
    boost::shared_ptr< libcmis::AllowableActions > actions = object->getAllowableActions( );

    CPPUNIT_ASSERT_MESSAGE( "CreateDocument allowable action should be true",
            actions->isDefined( libcmis::ObjectAction::CreateDocument ) &&
            actions->isAllowed( libcmis::ObjectAction::CreateDocument ) );

    CPPUNIT_ASSERT_MESSAGE( "GetContentStream allowable action should be false",
            actions->isDefined( libcmis::ObjectAction::GetContentStream ) &&
            !actions->isAllowed( libcmis::ObjectAction::GetContentStream ) );
}

void SharePointTest::getDocumentTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string authorUrl = objectId + "/Author";
    curl_mockup_addResponse ( objectId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( objectId );
    // Check if we got the document object.
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );
    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of libcmis::DocumentPtr",
            NULL != document );

    // Test the document properties
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document ID", objectId, document->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document name",
                                  string( "SharePointFile" ),
                                  document->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:document" ), document->getBaseType( ) );

    CPPUNIT_ASSERT_MESSAGE( "CreatedBy is missing", !document->getCreatedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "CreationDate is missing", !document->getCreationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "LastModificationDate is missing", !document->getLastModificationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "Content length is incorrect", 18045 == document->getContentLength( ) );
}

void SharePointTest::getContentStreamTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string authorUrl = objectId + "/Author";
    string expectedContent( "Test content stream" );
    string downloadUrl = objectId + "/%24value";

    curl_mockup_addResponse ( objectId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);
    curl_mockup_addResponse( downloadUrl.c_str( ), "", "GET", expectedContent.c_str( ), 0, false );

    libcmis::ObjectPtr object = session.getObject( objectId );
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

void SharePointTest::setContentStreamTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string authorUrl = objectId + "/Author";
    string expectedContent( "Test content stream" );
    string putUrl = objectId + "/%24value";

    curl_mockup_addResponse ( objectId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);
    curl_mockup_addResponse( putUrl.c_str( ), "", "PUT", "Updated", 0, false );

    libcmis::ObjectPtr object = session.getObject( objectId );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );
    try
    {
        boost::shared_ptr< ostream > os ( new stringstream ( expectedContent ) );
        string filename( "aFileName" );
        document->setContentStream( os, "text/plain", filename );

        CPPUNIT_ASSERT_MESSAGE( "Object not refreshed during setContentStream", object->getRefreshTimestamp( ) > 0 );
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

void SharePointTest::checkOutTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );
    static const string authorUrl = objectId + "/Author";
    static const string checkOutUrl = objectId + "/checkout";
    static const string cancelCheckOutUrl = objectId + "/undocheckout";

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    curl_mockup_addResponse( objectId.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/file.json", 200, true );
    curl_mockup_addResponse( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true );
    curl_mockup_addResponse( checkOutUrl.c_str( ), "",
                             "POST", DATA_DIR "/sharepoint/file.json", 200, true );
    curl_mockup_addResponse( cancelCheckOutUrl.c_str( ), "",
                             "POST", DATA_DIR "/sharepoint/file.json", 200, true );

    libcmis::ObjectPtr object = session.getObject( objectId );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );

    libcmis::DocumentPtr checkedOutDocument = document->checkOut( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong checkedOut document",
                                   objectId,
                                   checkedOutDocument->getId( ) );

    checkedOutDocument->cancelCheckout( );
}

void SharePointTest::checkInTest( )
{
    static const string objectId( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string authorUrl = objectId + "/Author";
    string expectedContent( "Test content stream" );
    string putUrl = objectId + "/%24value";
    string checkInUrl = objectId + "/checkin(comment='checkin_comment',checkintype=1)";

    curl_mockup_addResponse ( objectId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);
    curl_mockup_addResponse( putUrl.c_str( ), "", "PUT", "Updated", 0, false );
    curl_mockup_addResponse( checkInUrl.c_str( ), "",
                             "POST", DATA_DIR "/sharepoint/file.json", 200, true );

    libcmis::ObjectPtr object = session.getObject( objectId );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );
    PropertyPtrMap properties;
    boost::shared_ptr< ostream > os ( new stringstream ( expectedContent ) );
    string fileName( "aFileName" );
    string checkInComment( "checkin_comment" );

    libcmis::DocumentPtr checkedInDocument;
    checkedInDocument = document->checkIn( true, checkInComment, properties, os, "", fileName );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong checkedIn document", objectId, checkedInDocument->getId( ) );
}

void SharePointTest::getAllVersionsTest( )
{
    static const string objectId( "http://base/_api/Web/aFileId" );
    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string authorUrl = objectId + "/Author";
    string versionsUrl = objectId + "/Versions";
    string objectV1Url = versionsUrl +"(1)";
    string objectV2Url = versionsUrl +"(2)";
    string objectV1Id = objectId + "-v1";

    curl_mockup_addResponse ( objectId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);
    curl_mockup_addResponse ( versionsUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/versions.json", 200, true);
    curl_mockup_addResponse ( objectV1Url.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( objectV2Url.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file-v1.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( objectId );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );
    vector< libcmis::DocumentPtr > allVersions = document->getAllVersions( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong version of the document - 1",
                                  objectId, allVersions[1]->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong version of the document - 2",
                                  objectV1Id, allVersions[2]->getId( ) );
}

void SharePointTest::getFolderTest( )
{
    static const string folderId( "http://base/_api/Web/aFolderId" );
    static const string parentId( "http://base/_api/Web/rootFolderId" );
    SharePointSession session = getTestSession( USERNAME, PASSWORD );

    string parentUrl = folderId + "/ParentFolder";
    string folderPropUrl = folderId + "/Properties";
    string parentFolderPropUrl = parentId + "/Properties";
    curl_mockup_addResponse( folderId.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/folder.json", 200, true );
    curl_mockup_addResponse( folderPropUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/folder-properties.json", 200, true );
    curl_mockup_addResponse( parentFolderPropUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/folder-properties.json", 200, true );
    curl_mockup_addResponse( parentUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/root-folder.json", 200, true );
    curl_mockup_addResponse( parentId.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/root-folder.json", 200, true );

    libcmis::FolderPtr folder = session.getFolder( folderId );

    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of libcmis::FolderPtr",
            NULL != folder );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder ID", folderId, folder->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder name", string( "SharePointFolder" ), folder->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:folder" ), folder->getBaseType( ) );

    CPPUNIT_ASSERT_MESSAGE( "Missing folder parent", folder->getFolderParent( ).get( ) );
    CPPUNIT_ASSERT_MESSAGE( "Not a root folder", !folder->isRootFolder() );
}

void SharePointTest::getChildrenTest( )
{
    static const string folderId( "http://base/_api/Web/aFolderId" );
    static const string authorUrl( "http://base/_api/Web/aFileId/Author" );
    SharePointSession session = getTestSession( USERNAME, PASSWORD );

    string filesUrl = folderId + "/Files";
    string foldersUrl = folderId + "/Folders";
    string folderPropUrl = folderId + "/Properties";
    curl_mockup_addResponse( folderId.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/folder.json", 200, true );
    curl_mockup_addResponse( folderPropUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/folder-properties.json", 200, true );
    curl_mockup_addResponse( filesUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/children-files.json", 200, true );
    curl_mockup_addResponse( foldersUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/children-folders.json", 200, true );
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);

    libcmis::FolderPtr folder = session.getFolder( folderId );
    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of libcmis::FolderPtr",
            NULL != folder );

    vector< libcmis::ObjectPtr > children= folder->getChildren( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad number of children", size_t( 2 ), children.size() );

    int folderCount = 0;
    int fileCount = 0;
    for ( vector< libcmis::ObjectPtr >::iterator it = children.begin( );
          it != children.end( ); ++it )
    {
        if ( NULL != boost::dynamic_pointer_cast< libcmis::Folder >( *it ) )
            ++folderCount;
        else {
            ++fileCount;
            libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( *it );
            vector< libcmis::FolderPtr > parents= document->getParents( );

            CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad number of parents", size_t( 1 ), parents.size() );
            CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parent Id", folderId, parents[0]->getId( ) );
        }

    }
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of folder children", 1, folderCount );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of file children", 1, fileCount );
}

void SharePointTest::createFolderTest( )
{
    static const string folderId( "http://base/_api/Web/aFolderId" );
    static const string parentId( "http://base/_api/Web/rootFolderId" );
    static const string newFolderUrl ( "http://base/_api/Web/folders/add('/SharePointFolder')" );
    SharePointSession session = getTestSession( USERNAME, PASSWORD );

    string folderPropUrl = folderId + "/Properties";
    string parentFolderPropUrl = parentId + "/Properties";
    curl_mockup_addResponse( folderId.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/folder.json", 200, true );
    curl_mockup_addResponse( folderPropUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/folder-properties.json", 200, true );
    curl_mockup_addResponse( parentFolderPropUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/folder-properties.json", 200, true );
    curl_mockup_addResponse( parentId.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/root-folder.json", 200, true );
    curl_mockup_addResponse( newFolderUrl.c_str( ), "",
                             "POST", DATA_DIR "/sharepoint/folder.json", 200, true );

    libcmis::FolderPtr folder = session.getFolder( parentId );
    PropertyPtrMap properties = session.getFolder( folderId )->getProperties( );

    libcmis::FolderPtr newFolder = folder->createFolder( properties );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "New folder not created", folderId, newFolder->getId( ) );
}

void SharePointTest::createDocumentTest( )
{
    static const string folderId( "http://base/_api/Web/aFolderId" );
    static const string fileId( "http://base/_api/Web/aFileId" );
    SharePointSession session = getTestSession( USERNAME, PASSWORD );

    string folderPropUrl = folderId + "/Properties";
    string newDocUrl = folderId + "/files/add(overwrite=true,url='NewDoc')";
    string authorUrl = fileId + "/Author";
    curl_mockup_addResponse( folderId.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/folder.json", 200, true );
    curl_mockup_addResponse( folderPropUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/folder-properties.json", 200, true );
    curl_mockup_addResponse( newDocUrl.c_str( ), "",
                             "POST", DATA_DIR "/sharepoint/file.json", 200, true );
    curl_mockup_addResponse( authorUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/author.json", 200, true );

    libcmis::FolderPtr folder = session.getFolder( folderId );
    try
    {
        string expectedContent( "Test set content stream" );
        boost::shared_ptr< ostream > os ( new stringstream ( expectedContent ) );
        PropertyPtrMap properties;
        string fileName = "NewDoc";

        libcmis::DocumentPtr document = folder->createDocument( properties, os, 
                                                                "text/plain", fileName );

        const char* content = curl_mockup_getRequestBody( newDocUrl.c_str( ), "", "POST" );

        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad content uploaded", expectedContent, string( content ) );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad document id", fileId, document->getId( ) );                  
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what( );
        CPPUNIT_FAIL( msg.c_str( ) );
    }
}

void SharePointTest::moveTest( )
{
    static const string fileId ( "http://base/_api/Web/aFileId" );
    static const string folderId( "http://base/_api/Web/aFolderId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string authorUrl = fileId + "/Author";
    string folderPropUrl = folderId + "/Properties";
    string moveUrl = fileId + "/moveto(newurl='/SharePointFolder/SharePointFile',flags=1)";
    curl_mockup_addResponse ( fileId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);
    curl_mockup_addResponse ( folderId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/folder.json", 200, true);
    curl_mockup_addResponse( folderPropUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/folder-properties.json", 200, true );
    curl_mockup_addResponse( moveUrl.c_str( ), "",
                             "POST", DATA_DIR "/sharepoint/file.json", 200, true );

    libcmis::ObjectPtr document = session.getObject( fileId );
    libcmis::FolderPtr folder = session.getFolder( folderId );

    document->move( folder, folder );
    // nothing to assert, making the right reqeusts should be enough
}

void SharePointTest::getObjectByPathTest( )
{
    static const string folderUrl( "http://base/_api/Web/getFolderByServerRelativeUrl('/SharePointFile')" );
    static const string fileUrl( "http://base/_api/Web/getFileByServerRelativeUrl('/SharePointFile')" );
    static string authorUrl( "http://base/_api/Web/aFileId/Author" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    curl_mockup_addResponse( fileUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse( folderUrl.c_str( ), "",
                             "GET", "", 400, true);
    curl_mockup_addResponse( authorUrl.c_str( ), "",
                             "GET", DATA_DIR "/sharepoint/author.json", 200, true);

    libcmis::ObjectPtr object = session.getObjectByPath( "/SharePointFile" );
    // Check if we got the document object.
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );
    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of libcmis::DocumentPtr",
            NULL != document );
}

void SharePointTest::sessionCopyTest( )
{
    SharePointSession session = getTestSession( USERNAME, PASSWORD );

    {
        SharePointSession copy;
        copy = session;
        CPPUNIT_ASSERT_EQUAL( session.m_bindingUrl, copy.m_bindingUrl );
        CPPUNIT_ASSERT_EQUAL( session.m_digestCode, copy.m_digestCode );
    }

    {
        SharePointSession copy( session );
        CPPUNIT_ASSERT_EQUAL( session.m_bindingUrl, copy.m_bindingUrl );
        CPPUNIT_ASSERT_EQUAL( session.m_digestCode, copy.m_digestCode );
    }
}

void SharePointTest::propertyCopyTest( )
{
    SharePointProperty property("Author", 
                  "\"__deferred\":{"
                  "     \"uri\":\"http://base/_api/Web/aFileId/Author\""
                  "}");

    {
        SharePointProperty copy;
        copy = property;

        CPPUNIT_ASSERT_EQUAL( property.m_propertyType->m_id, copy.m_propertyType->m_id );
        CPPUNIT_ASSERT_EQUAL( property.m_strValues[0], copy.m_strValues[0]);
    }

    {
        SharePointProperty copy( property );

        CPPUNIT_ASSERT_EQUAL( property.m_propertyType->m_id, copy.m_propertyType->m_id );
        CPPUNIT_ASSERT_EQUAL( property.m_strValues[0], copy.m_strValues[0]);
    }
}

void SharePointTest::objectCopyTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string authorUrl = objectId + "/Author";
    curl_mockup_addResponse ( objectId.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);

    boost::shared_ptr< SharePointObject > object = boost::dynamic_pointer_cast< SharePointObject >(session.getObject( objectId ) );

    {
        SharePointObject copy( *object );
        CPPUNIT_ASSERT_EQUAL( object->m_refreshTimestamp, copy.m_refreshTimestamp );
    }

    {
        SharePointObject copy( &session );
        copy = *object;
        CPPUNIT_ASSERT_EQUAL( object->m_refreshTimestamp, copy.m_refreshTimestamp );
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION( SharePointTest );
