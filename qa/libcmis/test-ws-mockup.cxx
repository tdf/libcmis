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

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

#define private public
#define protected public

#include <ws-session.hxx>

#include <mockup-config.h>
#include <test-helpers.hxx>

#define SERVER_URL string( "http://mockup/ws" )
#define SERVER_REPOSITORY string( "mock" )
#define SERVER_USERNAME "tester"
#define SERVER_PASSWORD "somepass"

using namespace std;
using libcmis::PropertyPtrMap;

namespace
{
    string lcl_getStreamAsString( boost::shared_ptr< istream > is )
    {
        is->seekg( 0, ios::end );
        long size = is->tellg( );
        is->seekg( 0, ios::beg );

        char* buf = new char[ size ];
        is->read( buf, size );
        string content( buf, size );
        delete[ ] buf;

        return content;
    }

    void lcl_addWsResponse( const char* url, const char* filename,
                            const char* bodyMatch = 0 )
    {
        FILE* fd = fopen( filename, "r" );

        size_t bufSize = 2048;
        char* buf = new char[bufSize];

        size_t read = 0;
        string outBuf;
        do
        {
            read = fread( buf, 1, bufSize, fd );
            outBuf += string( buf, read );
        } while ( read == bufSize );

        fclose( fd );
        delete[] buf;

        string emptyLine = ( "\n\n" );
        size_t pos = outBuf.find( emptyLine );
        string headers = outBuf.substr( 0, pos );
        string body = outBuf.substr( pos + emptyLine.size() );

        curl_mockup_addResponse( url, "", "POST", body.c_str(), 0, false,
                                 headers.c_str(), bodyMatch );
    }

    string lcl_getCmisRequestXml( string url )
    {
        const struct HttpRequest* request = curl_mockup_getRequest( url.c_str(), "", "POST" );
        char* contentType = curl_mockup_HttpRequest_getHeader( request, "Content-Type" );
        RelatedMultipart multipart( request->body, string( contentType ) );
        RelatedPartPtr part = multipart.getPart( multipart.getStartId() );
        string xml = part->getContent( );
        curl_mockup_HttpRequest_free( request );
        free( contentType );

        return test::getXmlNodeAsString( xml, "/soap-env:Envelope/soap-env:Body/child::*" );
    }

    string lcl_getExpectedNs( )
    {
        string ns = " xmlns:cmis=\"http://docs.oasis-open.org/ns/cmis/core/200908/\""
                    " xmlns:cmism=\"http://docs.oasis-open.org/ns/cmis/messaging/200908/\"";
        return ns;
    }
}

class WSTest : public CppUnit::TestFixture
{
    public:

        void getRepositoriesTest( );
        void getRepositoryInfosTest( );
        void getRepositoryInfosBadTest( );

        void getTypeTest( );
        void getUnexistantTypeTest( );
        void getTypeParentsTest( );
        void getTypeChildrenTest( );

        void getObjectTest( );
        void getDocumentTest( );
        void getFolderTest( );
        void getByPathValidTest( );
        void getByPathInvalidTest( );
        void getDocumentParentsTest( );
        void getChildrenTest( );

        CPPUNIT_TEST_SUITE( WSTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( getRepositoryInfosTest );
        CPPUNIT_TEST( getRepositoryInfosBadTest );
        CPPUNIT_TEST( getTypeTest );
        CPPUNIT_TEST( getUnexistantTypeTest );
        CPPUNIT_TEST( getTypeParentsTest );
        CPPUNIT_TEST( getTypeChildrenTest );
        CPPUNIT_TEST( getObjectTest );
        CPPUNIT_TEST( getDocumentTest );
        CPPUNIT_TEST( getFolderTest );
        CPPUNIT_TEST( getByPathValidTest );
        CPPUNIT_TEST( getByPathInvalidTest );
        CPPUNIT_TEST( getDocumentParentsTest );
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST_SUITE_END( );

        libcmis::RepositoryPtr getTestRepository( );
        WSSession getTestSession( string username, string password, bool noRepos = false );
};

CPPUNIT_TEST_SUITE_REGISTRATION( WSTest );

void WSTest::getRepositoriesTest()
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/repositories.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    map< string, string > actual = session.getRepositoryService().getRepositories( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of repositories", size_t( 1 ), actual.size( ) );

    // Test the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/RepositoryService" );
    string expectedRequest = "<cmism:getRepositories" + lcl_getExpectedNs() + "/>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::getRepositoryInfosTest()
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/repository-infos.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    string validId = "mock";
    libcmis::RepositoryPtr actual = session.getRepositoryService().getRepositoryInfo( validId );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Root folder is wrong", string( "root-folder" ), actual->getRootId( ) );

    // Test the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/RepositoryService" );
    string expectedRequest = "<cmism:getRepositoryInfo" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>" + validId + "</cmism:repositoryId>"
                             "</cmism:getRepositoryInfo>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::getRepositoryInfosBadTest()
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/repository-infos-bad.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    string badId = "bad";
    try
    {
        session.getRepositoryService().getRepositoryInfo( badId );
    }
    catch( const libcmis::Exception& e )
    {
        // Test the caught exception
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception type", string( "invalidArgument" ), e.getType( ) );

        // Test the sent request
        string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/RepositoryService" );
        string expectedRequest = "<cmism:getRepositoryInfo" + lcl_getExpectedNs() + ">"
                                     "<cmism:repositoryId>" + badId + "</cmism:repositoryId>"
                                 "</cmism:getRepositoryInfo>";
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
    }

}

void WSTest::getTypeTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    string id( "cmis:folder" );
    libcmis::ObjectTypePtr actual = session.getType( id );

    // Check the returned type
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id", id, actual->getId( ) );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/RepositoryService" );
    string expectedRequest = "<cmism:getTypeDefinition" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:typeId>" + id + "</cmism:typeId>"
                             "</cmism:getTypeDefinition>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::getUnexistantTypeTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-bad.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    string id( "bad_type" );
    try
    {
        session.getType( id );
    }
    catch ( const libcmis::Exception& e )
    {
        // Check the caught exception
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong error type", string( "objectNotFound" ), e.getType() );

        // Check the sent request
        string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/RepositoryService" );
        string expectedRequest = "<cmism:getTypeDefinition" + lcl_getExpectedNs() + ">"
                                     "<cmism:repositoryId>mock</cmism:repositoryId>"
                                     "<cmism:typeId>" + id + "</cmism:typeId>"
                                 "</cmism:getTypeDefinition>";
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
    }
}

void WSTest::getTypeParentsTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string id = "DocumentLevel2";
    libcmis::ObjectTypePtr actual = session.getType( id );

    // Check the resulting type
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Parent type", string( "DocumentLevel1" ), actual->getParentTypeId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Base type", string( "cmis:document" ), actual->getBaseTypeId( ) );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/RepositoryService" );
    string expectedRequest = "<cmism:getTypeDefinition" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:typeId>" + id + "</cmism:typeId>"
                             "</cmism:getTypeDefinition>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::getTypeChildrenTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/typechildren-document.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string id = "cmis:document";
    vector< libcmis::ObjectTypePtr > children = session.getRepositoryService().
                                                    getTypeChildren(
                                                            session.m_repositoryId,
                                                            id );

    // Check the actual children returned
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children", size_t( 1 ), children.size( ) );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/RepositoryService" );
    string expectedRequest = "<cmism:getTypeChildren" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:typeId>" + id + "</cmism:typeId>"
                                 "<cmism:includePropertyDefinitions>true</cmism:includePropertyDefinitions>"
                             "</cmism:getTypeChildren>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::getObjectTest( )
{
    // Setup the mockup
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );
    lcl_addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/valid-object.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    // Run the tested method
    string expectedId( "valid-object" );
    libcmis::ObjectPtr actual = session.getObject( expectedId );

    // Check the returned object
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Id for fetched object",
            expectedId, actual->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong type for fetched object",
            string( "cmis:folder" ), actual->getType() );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService" );
    string expectedRequest = "<cmism:getObject" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:objectId>" + expectedId + "</cmism:objectId>"
                                 "<cmism:includeAllowableActions>true</cmism:includeAllowableActions>"
                                 "<cmism:renditionFilter>*</cmism:renditionFilter>"
                             "</cmism:getObject>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::getDocumentTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );
    lcl_addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string expectedId( "test-document" );
    libcmis::ObjectPtr actual = session.getObject( expectedId );

    // Do we have a document?
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( actual );
    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of libcmis::DocumentPtr",
            NULL != document );

    // Check the document properties
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document ID", expectedId, document->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document name", string( "Test Document" ), document->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document type", string( "text/plain" ), document->getContentType( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:document" ), document->getBaseType( ) );

    CPPUNIT_ASSERT_MESSAGE( "CreatedBy is missing", !document->getCreatedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "CreationDate is missing", !document->getCreationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "LastModifiedBy is missing", !document->getLastModifiedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "LastModificationDate is missing", !document->getLastModificationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "ChangeToken is missing", !document->getChangeToken( ).empty( ) );

    CPPUNIT_ASSERT_MESSAGE( "Content length is missing", 12345 == document->getContentLength( ) );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService" );
    string expectedRequest = "<cmism:getObject" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:objectId>" + expectedId + "</cmism:objectId>"
                                 "<cmism:includeAllowableActions>true</cmism:includeAllowableActions>"
                                 "<cmism:renditionFilter>*</cmism:renditionFilter>"
                             "</cmism:getObject>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::getFolderTest( )
{
    // Setup the mockup
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );
    lcl_addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/valid-object.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    // Run the method under test
    string expectedId( "valid-object" );
    libcmis::FolderPtr actual = session.getFolder( expectedId );

    // Check the returned folder
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder ID", expectedId, actual->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder name", string( "Valid Object" ), actual->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder path", string( "/Valid Object" ), actual->getPath( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:folder" ), actual->getBaseType( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Missing folder parent ID",
            string( "root-folder" ), actual->getParentId() );
    CPPUNIT_ASSERT_MESSAGE( "Not a root folder", !actual->isRootFolder() );

    CPPUNIT_ASSERT_MESSAGE( "CreatedBy is missing", !actual->getCreatedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "CreationDate is missing", !actual->getCreationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "LastModifiedBy is missing", !actual->getLastModifiedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "LastModificationDate is missing", !actual->getLastModificationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "ChangeToken is missing", !actual->getChangeToken( ).empty( ) );

    // No need to check the request: we do the same one in another test
}

void WSTest::getByPathValidTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );
    lcl_addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/valid-object.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string path = "/Valid Object";
    libcmis::ObjectPtr actual = session.getObjectByPath( path );

    // Check the returned object
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Id for fetched object", string( "valid-object" ), actual->getId( ) );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService" );
    string expectedRequest = "<cmism:getObjectByPath" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:path>" + path + "</cmism:path>"
                                 "<cmism:includeAllowableActions>true</cmism:includeAllowableActions>"
                                 "<cmism:renditionFilter>*</cmism:renditionFilter>"
                             "</cmism:getObjectByPath>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::getByPathInvalidTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/getbypath-bad.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string path = "/some/invalid/path";
    try
    {
        session.getObjectByPath( path );
        CPPUNIT_FAIL( "Exception should be thrown: invalid Path" );
    }
    catch ( const libcmis::Exception& e )
    {
        // Check the caught exception
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong error type", string( "objectNotFound" ), e.getType() );

        // Check the sent request
        string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService" );
        string expectedRequest = "<cmism:getObjectByPath" + lcl_getExpectedNs() + ">"
                                     "<cmism:repositoryId>mock</cmism:repositoryId>"
                                     "<cmism:path>" + path + "</cmism:path>"
                                     "<cmism:includeAllowableActions>true</cmism:includeAllowableActions>"
                                     "<cmism:renditionFilter>*</cmism:renditionFilter>"
                                 "</cmism:getObjectByPath>";
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
    }

}


void WSTest::getDocumentParentsTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );
    lcl_addWsResponse( "http://mockup/ws/services/NavigationService", DATA_DIR "/ws/test-document-parents.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string id = "test-document";
    vector< libcmis::FolderPtr > actual = session.getNavigationService().
                                            getObjectParents( session.m_repositoryId,
                                                              id );

    // Check the actual parents
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad number of parents", size_t( 2 ), actual.size() );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/NavigationService" );
    string expectedRequest = "<cmism:getObjectParents" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:objectId>" + id + "</cmism:objectId>"
                                 "<cmism:includeAllowableActions>true</cmism:includeAllowableActions>"
                                 "<cmism:renditionFilter>*</cmism:renditionFilter>"
                             "</cmism:getObjectParents>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::getChildrenTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http", "<cmism:typeId>cmis:folder</cmism:typeId>" );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http", "<cmism:typeId>DocumentLevel2</cmism:typeId>" );
    lcl_addWsResponse( "http://mockup/ws/services/NavigationService", DATA_DIR "/ws/root-children.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );


    string id = "root-folder";
    vector< libcmis::ObjectPtr > children = session.getNavigationService().
                                                getChildren( session.m_repositoryId,
                                                             id );

    // Check the returned children
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children", size_t( 5 ), children.size() );

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
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of folder children", 2, folderCount );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of document children", 3, documentCount );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/NavigationService" );
    string expectedRequest = "<cmism:getChildren" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:folderId>" + id + "</cmism:folderId>"
                                 "<cmism:includeAllowableActions>true</cmism:includeAllowableActions>"
                                 "<cmism:renditionFilter>*</cmism:renditionFilter>"
                             "</cmism:getChildren>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

WSSession WSTest::getTestSession( string username, string password, bool noRepos )
{
    WSSession session;
    session.m_username = username;
    session.m_password = password;

    string buf;
    test::loadFromFile( DATA_DIR "/ws/CMISWS-Service.wsdl", buf );
    session.parseWsdl( buf );
    session.initializeResponseFactory( );

    // Manually define the repositories to avoid the HTTP query
    if ( noRepos )
    {
        libcmis::RepositoryPtr repo = getTestRepository( );
        session.m_repositories.push_back( repo );
        session.m_repositoryId = repo->getId( );
    }

    return session;
}

libcmis::RepositoryPtr WSTest::getTestRepository()
{
    libcmis::RepositoryPtr repo( new libcmis::Repository( ) );
    repo->m_id = "mock";
    repo->m_name = "Mockup";
    repo->m_description = "Repository sent by mockup server";
    repo->m_vendorName = "libcmis";
    repo->m_productName = "Libcmis mockup";
    repo->m_productVersion = "some-version";
    repo->m_cmisVersionSupported = "1.1";

    map< libcmis::Repository::Capability, string > capabilities;
    capabilities[libcmis::Repository::ACL] = "manage";
    capabilities[libcmis::Repository::AllVersionsSearchable] = "false";
    capabilities[libcmis::Repository::Changes] = "none";
    capabilities[libcmis::Repository::ContentStreamUpdatability] = "anytime";
    capabilities[libcmis::Repository::GetDescendants] = "true";
    capabilities[libcmis::Repository::GetFolderTree] = "true";
    capabilities[libcmis::Repository::Multifiling] = "true";
    capabilities[libcmis::Repository::PWCSearchable] = "false";
    capabilities[libcmis::Repository::PWCUpdatable] = "true";
    capabilities[libcmis::Repository::Query] = "bothcombined";
    capabilities[libcmis::Repository::Renditions] = "none";
    capabilities[libcmis::Repository::Unfiling] = "true";
    capabilities[libcmis::Repository::VersionSpecificFiling] = "false";
    capabilities[libcmis::Repository::Join] = "none";
    repo->m_capabilities = capabilities;

    return repo;
}
