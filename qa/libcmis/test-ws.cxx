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

#include <memory>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

#if defined __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#define protected public
#if defined __clang__
#pragma clang diagnostic pop
#endif

#include <ws-session.hxx>
#include <ws-object-type.hxx>

#include <mockup-config.h>
#include <test-helpers.hxx>
#include <test-mockup-helpers.hxx>

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

    string lcl_getCmisRequestXml( string url, const char* bodyMatch = NULL )
    {
        const struct HttpRequest* request = curl_mockup_getRequest( url.c_str(), "", "POST", bodyMatch );
        char* contentType = curl_mockup_HttpRequest_getHeader( request, "Content-Type" );
        RelatedMultipart multipart( request->body, string( contentType ) );
        RelatedPartPtr part = multipart.getPart( multipart.getStartId() );
        string xml = part->getContent( );
        curl_mockup_HttpRequest_free( request );
        free( contentType );

        string requestStr = test::getXmlNodeAsString( xml, "/soap-env:Envelope/soap-env:Body/child::*" );

        // Obfuscate the xop:Include ids
        string xopSearch = "<xop:Include xmlns:xop=\"http://www.w3.org/2004/08/xop/include\" href=\"cid:";
        size_t pos = requestStr.find( xopSearch );
        if ( pos != string::npos )
        {
            pos = pos + xopSearch.size();
            size_t endPos = requestStr.find( "\"", pos );
            requestStr = requestStr.replace( pos,
                                             endPos - pos,
                                             "obfuscated" );
        }
        return requestStr;
    }

    string lcl_getExpectedNs( )
    {
        string ns = " xmlns:cmis=\"http://docs.oasis-open.org/ns/cmis/core/200908/\""
                    " xmlns:cmism=\"http://docs.oasis-open.org/ns/cmis/messaging/200908/\"";
        return ns;
    }
}

typedef std::unique_ptr<WSSession> WSSessionPtr;

class WSTest : public CppUnit::TestFixture
{
    public:

        void getRepositoriesTest( );
        void getRepositoryInfosTest( );
        void getRepositoryInfosBadTest( );

        void getTypeTest( );
        void getTypeRefreshTest( );
        void objectTypeCopyTest( );
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
        void getContentStreamTest( );
        void setContentStreamTest( );
        void getRenditionsTest( );
        void updatePropertiesTest( );
        void updatePropertiesEmptyTest( );
        void createFolderTest( );
        void createFolderBadTypeTest( );
        void createDocumentTest( );
        void deleteDocumentTest( );
        void deleteFolderTreeTest( );
        void moveTest( );
        void addSecondaryTypeTest( );

        void checkOutTest( );
        void cancelCheckOutTest( );
        void checkInTest( );
        void getAllVersionsTest( );

        void navigationServiceCopyTest();
        void repositoryServiceCopyTest();
        void objectServiceCopyTest();
        void versioningServiceCopyTest();

        CPPUNIT_TEST_SUITE( WSTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( getRepositoryInfosTest );
        CPPUNIT_TEST( getRepositoryInfosBadTest );
        CPPUNIT_TEST( getTypeTest );
        CPPUNIT_TEST( getTypeRefreshTest );
        CPPUNIT_TEST( objectTypeCopyTest );
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
        CPPUNIT_TEST( getContentStreamTest );
        CPPUNIT_TEST( setContentStreamTest );
        CPPUNIT_TEST( getRenditionsTest );
        CPPUNIT_TEST( updatePropertiesTest );
        CPPUNIT_TEST( updatePropertiesEmptyTest );
        CPPUNIT_TEST( createFolderTest );
        CPPUNIT_TEST( createFolderBadTypeTest );
        CPPUNIT_TEST( createDocumentTest );
        CPPUNIT_TEST( deleteDocumentTest );
        CPPUNIT_TEST( deleteFolderTreeTest );
        CPPUNIT_TEST( moveTest );
        CPPUNIT_TEST( addSecondaryTypeTest );
        CPPUNIT_TEST( checkOutTest );
        CPPUNIT_TEST( cancelCheckOutTest );
        CPPUNIT_TEST( checkInTest );
        CPPUNIT_TEST( getAllVersionsTest );
        CPPUNIT_TEST( navigationServiceCopyTest );
        CPPUNIT_TEST( repositoryServiceCopyTest );
        CPPUNIT_TEST( objectServiceCopyTest );
        CPPUNIT_TEST( versioningServiceCopyTest );
        CPPUNIT_TEST_SUITE_END( );

        libcmis::RepositoryPtr getTestRepository( );
        WSSessionPtr getTestSession( string username, string password, bool noRepos = false );
};

CPPUNIT_TEST_SUITE_REGISTRATION( WSTest );

void WSTest::getRepositoriesTest()
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/repositories.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    map< string, string > actual = session->getRepositoryService().getRepositories( );
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
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/repository-infos.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    string validId = "mock";
    libcmis::RepositoryPtr actual = session->getRepositoryService().getRepositoryInfo( validId );
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
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/repository-infos-bad.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    string badId = "bad";
    try
    {
        session->getRepositoryService().getRepositoryInfo( badId );
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
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    string id( "cmis:folder" );
    libcmis::ObjectTypePtr actual = session->getType( id );

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

void WSTest::getTypeRefreshTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService",
                         DATA_DIR "/ws/type-docLevel2.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    string id( "DocumentLevel2" );
    libcmis::ObjectTypePtr actual = session->getType( id );

    // Check the returned type
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id", id, actual->getId( ) );

    test::addWsResponse( "http://mockup/ws/services/RepositoryService",
                         DATA_DIR "/ws/type-docLevel1.http" );


    // Do the refresh
    actual->refresh();

    // Check the refreshed object
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id", string( "DocumentLevel1" ),
                                  actual->getId( ) );
}

void WSTest::objectTypeCopyTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    string id( "cmis:folder" );
    libcmis::ObjectTypePtr expected = session->getType( id );
    WSObjectType* type = dynamic_cast< WSObjectType* >( expected.get( ) );

    {
        WSObjectType copy( *type );

        CPPUNIT_ASSERT_EQUAL( type->getId( ), copy.getId( ) );
        CPPUNIT_ASSERT_EQUAL( type->m_session, copy.m_session );
    }

    {
        WSObjectType copy;
        copy = *type;

        CPPUNIT_ASSERT_EQUAL( type->getId( ), copy.getId( ) );
        CPPUNIT_ASSERT_EQUAL( type->m_session, copy.m_session );
    }
}

void WSTest::getUnexistantTypeTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-bad.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    string id( "bad_type" );
    try
    {
        session->getType( id );
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
    test::addWsResponse( "http://mockup/ws/services/RepositoryService",
                         DATA_DIR "/ws/type-docLevel2.http",
                         "<cmism:typeId>DocumentLevel2</cmism:typeId>" );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService",
                         DATA_DIR "/ws/type-docLevel1.http",
                         "<cmism:typeId>DocumentLevel1</cmism:typeId>" );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService",
                         DATA_DIR "/ws/type-document.http",
                         "<cmism:typeId>cmis:document</cmism:typeId>" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string id = "DocumentLevel2";
    libcmis::ObjectTypePtr actual = session->getType( id );

    // Check the resulting type
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Parent type Id", string( "DocumentLevel1" ),
                                  actual->getParentTypeId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Base type Id", string( "cmis:document" ),
                                  actual->getBaseTypeId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Parent type", string( "DocumentLevel1" ),
                                  actual->getParentType()->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Base type", string( "cmis:document" ),
                                  actual->getBaseType()->getId( ) );

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
    test::addWsResponse( "http://mockup/ws/services/RepositoryService",
                         DATA_DIR "/ws/typechildren-document.http",
                         "<cmism:getTypeChildren ");
    test::addWsResponse( "http://mockup/ws/services/RepositoryService",
                         DATA_DIR "/ws/type-document.http",
                         "<cmism:typeId>cmis:document</cmism:typeId>" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string id = "cmis:document";
    libcmis::ObjectTypePtr actual = session->getType( id );
    vector< libcmis::ObjectTypePtr > children = actual->getChildren();

    // Check the actual children returned
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children", size_t( 1 ), children.size( ) );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/RepositoryService",
                                               "<cmism:getTypeChildren " );
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
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/valid-object.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    // Run the tested method
    string expectedId( "valid-object" );
    libcmis::ObjectPtr actual = session->getObject( expectedId );

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
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string expectedId( "test-document" );
    libcmis::ObjectPtr actual = session->getObject( expectedId );

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
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/valid-object.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    // Run the method under test
    string expectedId( "valid-object" );
    libcmis::FolderPtr actual = session->getFolder( expectedId );

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
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/valid-object.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string path = "/Valid Object";
    libcmis::ObjectPtr actual = session->getObjectByPath( path );

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
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/getbypath-bad.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string path = "/some/invalid/path";
    try
    {
        session->getObjectByPath( path );
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
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );
    test::addWsResponse( "http://mockup/ws/services/NavigationService", DATA_DIR "/ws/test-document-parents.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string id = "test-document";
    vector< libcmis::FolderPtr > actual = session->getNavigationService().
                                            getObjectParents( session->m_repositoryId,
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
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http", "<cmism:typeId>cmis:folder</cmism:typeId>" );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http", "<cmism:typeId>DocumentLevel2</cmism:typeId>" );
    test::addWsResponse( "http://mockup/ws/services/NavigationService", DATA_DIR "/ws/root-children.http" );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );


    string id = "root-folder";
    vector< libcmis::ObjectPtr > children = session->getNavigationService().
                                                getChildren( session->m_repositoryId,
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

void WSTest::getContentStreamTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document.http", "<cmism:getObject " );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/get-content-stream.http", "<cmism:getContentStream " );


    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string id = "test-document";
    libcmis::ObjectPtr object = session->getObject( id );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );

    boost::shared_ptr< istream >  is = document->getContentStream( );

    // Check the fetched content
    string actualContent = lcl_getStreamAsString( is );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Content stream doesn't match",
                                  string( "Some content stream" ), actualContent );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService", "<cmism:getContentStream " );
    string expectedRequest = "<cmism:getContentStream" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:objectId>" + id + "</cmism:objectId>"
                             "</cmism:getContentStream>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::setContentStreamTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document.http", "<cmism:getObject " );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/set-content-stream.http", "<cmism:setContentStream " );
    curl_mockup_addResponse( "http://mockup/mock/content/data.txt", "id=test-document", "PUT", "Updated", 0, false );

    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string id = "test-document";
    libcmis::ObjectPtr object = session->getObject( id );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );

    try
    {
        string oldChangeToken = object->getChangeToken( );
        string expectedContent( "Some content stream to set" );
        boost::shared_ptr< ostream > os ( new stringstream ( expectedContent ) );
        string filename( "name.txt" );
        string contentType( "text/plain" );
        document->setContentStream( os, contentType, filename, true );

        CPPUNIT_ASSERT_MESSAGE( "Object not refreshed during setContentStream", object->getRefreshTimestamp( ) > 0 );
        // We do not check the change token as we are lazy
        // That would require to write another answer file for the refresh

        // Check the sent request
        ostringstream converter;
        converter << expectedContent.size( );
        string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService", "<cmism:setContentStream " );
        string expectedRequest = "<cmism:setContentStream" + lcl_getExpectedNs() + ">"
                                     "<cmism:repositoryId>mock</cmism:repositoryId>"
                                     "<cmism:objectId>" + id + "</cmism:objectId>"
                                     "<cmism:overwriteFlag>true</cmism:overwriteFlag>"
                                     "<cmism:changeToken>" + oldChangeToken + "</cmism:changeToken>"
                                     "<cmism:contentStream>"
                                         "<cmism:length>" + converter.str() + "</cmism:length>"
                                         "<cmism:mimeType>" + contentType + "</cmism:mimeType>"
                                         "<cmism:filename>" + filename + "</cmism:filename>"
                                         "<cmism:stream>"
                                             "<xop:Include xmlns:xop=\"http://www.w3.org/2004/08/xop/include\" "
                                                           "href=\"cid:obfuscated\"/>"
                                         "</cmism:stream>"
                                     "</cmism:contentStream>"
                                 "</cmism:setContentStream>";
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void WSTest::getRenditionsTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document.http", "<cmism:getObject " );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/get-renditions.http", "<cmism:getRenditions " );

    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string expectedId( "test-document" );
    libcmis::ObjectPtr actual = session->getObject( expectedId );

    std::vector< libcmis::RenditionPtr > renditions = actual->getRenditions( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad renditions count", size_t( 2 ), renditions.size( ) );

    libcmis::RenditionPtr rendition = renditions[1];
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad rendition mime type", string( "application/pdf" ), rendition->getMimeType( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad rendition stream id", string( "test-document-rendition2" ), rendition->getStreamId() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad rendition length - default case", long( -1 ), rendition->getLength( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad rendition Title", string( "Doc as PDF" ), rendition->getTitle( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad rendition kind", string( "pdf" ), rendition->getKind( ) );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad rendition length - filled case", long( 40385 ), renditions[0]->getLength( ) );
}

void WSTest::updatePropertiesTest( )
{
    curl_mockup_reset( );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document.http", "<cmism:getObject " );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/update-properties.http", "<cmism:updateProperties " );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    // Values for the test
    string id = "test-document";
    libcmis::ObjectPtr object = session->getObject( id );
    string propertyName( "cmis:name" );
    string expectedValue( "New name" );

    // Fill the map of properties to change
    PropertyPtrMap newProperties;

    libcmis::ObjectTypePtr objectType = object->getTypeDescription( );
    map< string, libcmis::PropertyTypePtr >::iterator it = objectType->getPropertiesTypes( ).find( propertyName );
    vector< string > values;
    values.push_back( expectedValue );
    libcmis::PropertyPtr property( new libcmis::Property( it->second, values ) );
    newProperties[ propertyName ] = property;

    // Change the object response to provide the updated values
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document-updated.http", "<cmism:getObject " );

    // Update the properties (method to test)
    libcmis::ObjectPtr updated = object->updateProperties( newProperties );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService", "<cmism:updateProperties " );
    string expectedRequest = "<cmism:updateProperties" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:objectId>" + id + "</cmism:objectId>"
                                 "<cmism:changeToken>some-change-token</cmism:changeToken>"
                                 "<cmism:properties>"
                                    "<cmis:propertyString propertyDefinitionId=\"cmis:name\" localName=\"cmis:name\" "
                                                          "displayName=\"Name\" queryName=\"cmis:name\">"
                                        "<cmis:value>New name</cmis:value>"
                                    "</cmis:propertyString>"
                                 "</cmism:properties>"
                             "</cmism:updateProperties>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );

    // Check that the properties are updated after the call
    PropertyPtrMap::iterator propIt = updated->getProperties( ).find( propertyName );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong value after refresh", expectedValue, propIt->second->getStrings().front( ) );
}

void WSTest::updatePropertiesEmptyTest( )
{
    curl_mockup_reset( );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document.http", "<cmism:getObject " );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    // Values for the test
    string id = "test-document";
    libcmis::ObjectPtr object = session->getObject( id );

    // Just leave the map empty and update
    PropertyPtrMap emptyProperties;
    libcmis::ObjectPtr updated = object->updateProperties( emptyProperties );

    // Check that no HTTP request was sent
    int count = curl_mockup_getRequestsCount( "http://mockup/ws/services/ObjectService",
                                              "", "POST", "<cmism:updateProperties" );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "No HTTP request should have been sent", 0, count );

    // Check that the object we got is the same than previous one
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong change token", object->getChangeToken(), updated->getChangeToken() );
}

void WSTest::createFolderTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/root-folder.http", "<cmism:getObject " );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/create-folder.http", "<cmism:createFolder " );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    libcmis::FolderPtr parent = session->getRootFolder( );

    // Prepare the properties for the new object, object type is cmis:folder
    PropertyPtrMap props;
    libcmis::ObjectTypePtr type = session->getType( "cmis:folder" );
    map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

    // Set the object name
    string expectedName( "create folder" );
    map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
    vector< string > nameValues;
    nameValues.push_back( expectedName );
    libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );

    // set the object type
    it = propTypes.find( string( "cmis:objectTypeId" ) );
    vector< string > typeValues;
    typeValues.push_back( "cmis:folder" );
    libcmis::PropertyPtr typeProperty( new libcmis::Property( it->second, typeValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeProperty ) );

    // Set the mockup to send the updated folder now that we had the parent
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/created-folder.http", "<cmism:getObject " );

    // Actually send the folder creation request
    libcmis::FolderPtr created = parent->createFolder( props );

    // Check that something came back
    CPPUNIT_ASSERT_MESSAGE( "Change token shouldn't be empty: object should have been refreshed",
            !created->getChangeToken( ).empty() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong name", expectedName, created->getName( ) );

    // Check that the proper request has been sent
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService", "<cmism:createFolder " );
    string expectedRequest = "<cmism:createFolder" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:properties>"
                                    "<cmis:propertyString propertyDefinitionId=\"cmis:name\" localName=\"cmis:name\" "
                                                          "displayName=\"Name\" queryName=\"cmis:name\">"
                                        "<cmis:value>create folder</cmis:value>"
                                    "</cmis:propertyString>"
                                    "<cmis:propertyId propertyDefinitionId=\"cmis:objectTypeId\" localName=\"cmis:objectTypeId\""
                                                          " displayName=\"Type-Id\" queryName=\"cmis:objectTypeId\">"
                                        "<cmis:value>cmis:folder</cmis:value>"
                                    "</cmis:propertyId>"
                                 "</cmism:properties>"
                                 "<cmism:folderId>root-folder</cmism:folderId>"
                             "</cmism:createFolder>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::createFolderBadTypeTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/root-folder.http", "<cmism:getObject " );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/create-folder-bad-type.http", "<cmism:createFolder " );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    libcmis::FolderPtr parent = session->getRootFolder( );

    // Prepare the properties for the new object, object type is cmis:folder
    PropertyPtrMap props;
    libcmis::ObjectTypePtr type = session->getType( "cmis:folder" );
    map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

    // Set the object name
    string expectedName( "create folder" );
    map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
    vector< string > nameValues;
    nameValues.push_back( expectedName );
    libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );

    // set the object type
    it = propTypes.find( string( "cmis:objectTypeId" ) );
    vector< string > typeValues;
    typeValues.push_back( "cmis:document" );
    libcmis::PropertyPtr typeProperty( new libcmis::Property( it->second, typeValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeProperty ) );

    // Actually send the folder creation request
    try
    {
        libcmis::FolderPtr created = parent->createFolder( props );
        CPPUNIT_FAIL( "Should not succeed to return a folder" );
    }
    catch ( libcmis::Exception& e )
    {
        // Check the caught exception
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong error type", string( "constraint" ), e.getType() );

        // Check that the proper request has been sent
        string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService", "<cmism:createFolder " );
        string expectedRequest = "<cmism:createFolder" + lcl_getExpectedNs() + ">"
                                     "<cmism:repositoryId>mock</cmism:repositoryId>"
                                     "<cmism:properties>"
                                        "<cmis:propertyString propertyDefinitionId=\"cmis:name\" localName=\"cmis:name\" "
                                                              "displayName=\"Name\" queryName=\"cmis:name\">"
                                            "<cmis:value>create folder</cmis:value>"
                                        "</cmis:propertyString>"
                                        "<cmis:propertyId propertyDefinitionId=\"cmis:objectTypeId\" localName=\"cmis:objectTypeId\""
                                                              " displayName=\"Type-Id\" queryName=\"cmis:objectTypeId\">"
                                            "<cmis:value>cmis:document</cmis:value>"
                                        "</cmis:propertyId>"
                                     "</cmism:properties>"
                                     "<cmism:folderId>root-folder</cmism:folderId>"
                                 "</cmism:createFolder>";
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
    }
}

void WSTest::createDocumentTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/create-document.http", "<cmism:createDocument " );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/root-folder.http", "<cmism:getObject " );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );

    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    libcmis::FolderPtr parent = session->getRootFolder( );

    // Make the mockup know about cmis:document now
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-document.http" );

    // Prepare the properties for the new object, object type is cmis:folder
    PropertyPtrMap props;
    libcmis::ObjectTypePtr type = session->getType( "cmis:document" );
    map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

    // Set the object name
    string expectedName( "create document" );

    map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
    vector< string > nameValues;
    nameValues.push_back( expectedName );
    libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );

    // set the object type
    it = propTypes.find( string( "cmis:objectTypeId" ) );
    vector< string > typeValues;
    typeValues.push_back( "cmis:document" );
    libcmis::PropertyPtr typeProperty( new libcmis::Property( it->second, typeValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeProperty ) );

    // Make the mockup able to send the response to update the object
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/created-document.http", "<cmism:getObject " );

    // Actually send the document creation request
    string content = "Some content";
    boost::shared_ptr< ostream > os ( new stringstream( content ) );
    string contentType = "text/plain";
    string filename( "name.txt" );
    libcmis::DocumentPtr created = parent->createDocument( props, os, contentType, filename );

    // Check that something came back
    CPPUNIT_ASSERT_MESSAGE( "Change token shouldn't be empty: object should have been refreshed",
            !created->getChangeToken( ).empty() );

    // Check that the name is ok
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong name set", expectedName, created->getName( ) );

    // Check that the sent request is the expected one
    ostringstream converter;
    converter << content.size( );
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService", "<cmism:createDocument " );
    string expectedRequest = "<cmism:createDocument" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:properties>"
                                    "<cmis:propertyString propertyDefinitionId=\"cmis:name\" localName=\"cmis:name\" "
                                                          "displayName=\"Name\" queryName=\"cmis:name\">"
                                        "<cmis:value>create document</cmis:value>"
                                    "</cmis:propertyString>"
                                    "<cmis:propertyId propertyDefinitionId=\"cmis:objectTypeId\" localName=\"cmis:objectTypeId\""
                                                          " displayName=\"Type-Id\" queryName=\"cmis:objectTypeId\">"
                                        "<cmis:value>cmis:document</cmis:value>"
                                    "</cmis:propertyId>"
                                 "</cmism:properties>"
                                 "<cmism:folderId>root-folder</cmism:folderId>"
                                 "<cmism:contentStream>"
                                    "<cmism:length>" + converter.str( ) + "</cmism:length>"
                                    "<cmism:mimeType>" + contentType + "</cmism:mimeType>"
                                    "<cmism:filename>" + filename + "</cmism:filename>"
                                    "<cmism:stream>"
                                        "<xop:Include xmlns:xop=\"http://www.w3.org/2004/08/xop/include\" "
                                                      "href=\"cid:obfuscated\"/>"
                                    "</cmism:stream>"
                                 "</cmism:contentStream>"
                             "</cmism:createDocument>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::deleteDocumentTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document.http", "<cmism:getObject " );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/delete-object.http", "<cmism:deleteObject " );

    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string id = "test-document";
    libcmis::ObjectPtr object = session->getObject( id );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );

    // Run the tested method. Here we delete the object with all its versions
    document->remove( true );

    // Check that the proper request has been sent
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService", "<cmism:deleteObject " );
    string expectedRequest = "<cmism:deleteObject" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:objectId>" + id + "</cmism:objectId>"
                                 "<cmism:allVersions>true</cmism:allVersions>"
                             "</cmism:deleteObject>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::deleteFolderTreeTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/valid-object.http", "<cmism:getObject " );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/delete-tree.http", "<cmism:deleteTree " );

    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string id = "valid-object";
    libcmis::ObjectPtr object = session->getObject( id );
    libcmis::Folder* folder = dynamic_cast< libcmis::Folder* >( object.get() );

    vector<string> failed = folder->removeTree( true, libcmis::UnfileObjects::Delete, false );

    // Check that we had the failed ids
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong ids for non-deleted objects",
            string( "bad-delete" ), failed[0] );

    // Test the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService", "<cmism:deleteTree " );
    string expectedRequest = "<cmism:deleteTree" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:folderId>valid-object</cmism:folderId>"
                                 "<cmism:allVersions>true</cmism:allVersions>"
                                 "<cmism:unfileObjects>delete</cmism:unfileObjects>"
                                 "<cmism:continueOnFailure>false</cmism:continueOnFailure>"
                             "</cmism:deleteTree>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::moveTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-folder.http", "<cmism:typeId>cmis:folder</cmism:typeId>" );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http", "<cmism:typeId>DocumentLevel2</cmism:typeId>" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document.http", "<cmism:getObject " );
    test::addWsResponse( "http://mockup/ws/services/NavigationService", DATA_DIR "/ws/test-document-parents.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/move-object.http", "<cmism:moveObject " );

    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string id = "test-document";
    libcmis::ObjectPtr object = session->getObject( id );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );

    string destFolderId = "valid-object";
    libcmis::FolderPtr src = document->getParents( ).front( );

    // Tell the mockup about the destination folder
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/valid-object.http", "<cmism:getObject " );
    libcmis::FolderPtr dest = session->getFolder( destFolderId );

    document->move( src, dest );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService", "<cmism:moveObject " );
    string expectedRequest = "<cmism:moveObject" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:objectId>" + id + "</cmism:objectId>"
                                 "<cmism:targetFolderId>" + destFolderId + "</cmism:targetFolderId>"
                                 "<cmism:sourceFolderId>" + src->getId( ) + "</cmism:sourceFolderId>"
                             "</cmism:moveObject>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::addSecondaryTypeTest( )
{
    curl_mockup_reset( );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/secondary-type.http",
                         "<cmism:typeId>secondary-type</cmism:typeId>" );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2-secondary.http",
                         "<cmism:typeId>DocumentLevel2</cmism:typeId>" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document.http", "<cmism:getObject " );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/update-properties.http", "<cmism:updateProperties " );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    WSSessionPtr session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    // Values for the test
    string id = "test-document";
    libcmis::ObjectPtr object = session->getObject( id );
    string secondaryType = "secondary-type";
    string propertyName = "secondary-prop";
    string expectedValue = "some-value";

    // Fill the map of properties to change
    PropertyPtrMap newProperties;

    libcmis::ObjectTypePtr objectType = session->getType( secondaryType );
    map< string, libcmis::PropertyTypePtr >::iterator it = objectType->getPropertiesTypes( ).find( propertyName );
    vector< string > values;
    values.push_back( expectedValue );
    libcmis::PropertyPtr property( new libcmis::Property( it->second, values ) );
    newProperties[ propertyName ] = property;

    // Change the object response to provide the updated values
    test::addWsResponse( "http://mockup/ws/services/ObjectService",
                         DATA_DIR "/ws/test-document-add-secondary.http",
                         "<cmism:getObject " );

    // add the secondary type (method to test)
    libcmis::ObjectPtr updated = object->addSecondaryType( secondaryType, newProperties );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/ObjectService", "<cmism:updateProperties " );
    string expectedRequest = "<cmism:updateProperties" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:objectId>" + id + "</cmism:objectId>"
                                 "<cmism:changeToken>some-change-token</cmism:changeToken>"
                                 "<cmism:properties>"
                                    "<cmis:propertyString propertyDefinitionId=\"cmis:secondaryObjectTypeIds\""
                                                        " localName=\"cmis:secondaryObjectTypeIds\""
                                                        " displayName=\"cmis:secondaryObjectTypeIds\""
                                                        " queryName=\"cmis:secondaryObjectTypeIds\">"
                                        "<cmis:value>" + secondaryType + "</cmis:value>"
                                    "</cmis:propertyString>"
                                    "<cmis:propertyString propertyDefinitionId=\"" + propertyName +  "\""
                                                        " localName=\"" + propertyName + "\""
                                                        " displayName=\"" + propertyName + "\""
                                                        " queryName=\"" + propertyName + "\">"
                                        "<cmis:value>" + expectedValue + "</cmis:value>"
                                    "</cmis:propertyString>"
                                 "</cmism:properties>"
                             "</cmism:updateProperties>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );

    // Check that the properties are updated after the call
    PropertyPtrMap::iterator propIt = updated->getProperties( ).find( propertyName );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong value after refresh", expectedValue, propIt->second->getStrings().front( ) );
}

void WSTest::checkOutTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document.http", "<cmism:objectId>test-document</cmism:objectId>" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/working-copy.http", "<cmism:objectId>working-copy</cmism:objectId>" );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );
    test::addWsResponse( "http://mockup/ws/services/VersioningService", DATA_DIR "/ws/checkout.http" );

    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    string id = "test-document";
    libcmis::ObjectPtr object = session->getObject( id );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );

    libcmis::DocumentPtr pwc = document->checkOut( );

    // Check that we have a PWC
    CPPUNIT_ASSERT_MESSAGE( "Missing returned Private Working Copy", pwc.get( ) != NULL );

    PropertyPtrMap::iterator it = pwc->getProperties( ).find( string( "cmis:isVersionSeriesCheckedOut" ) );
    vector< bool > values = it->second->getBools( );
    CPPUNIT_ASSERT_MESSAGE( "cmis:isVersionSeriesCheckedOut isn't true", values.front( ) );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/VersioningService" );
    string expectedRequest = "<cmism:checkOut" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:objectId>" + id + "</cmism:objectId>"
                             "</cmism:checkOut>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::cancelCheckOutTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/working-copy.http" );
    test::addWsResponse( "http://mockup/ws/services/VersioningService", DATA_DIR "/ws/cancel-checkout.http" );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );

    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    // First get a checked out document
    string id = "working-copy";
    libcmis::ObjectPtr object = session->getObject( id );
    libcmis::DocumentPtr pwc = boost::dynamic_pointer_cast< libcmis::Document >( object );

    pwc->cancelCheckout( );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/VersioningService" );
    string expectedRequest = "<cmism:cancelCheckOut" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:objectId>" + id + "</cmism:objectId>"
                             "</cmism:cancelCheckOut>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::checkInTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/working-copy.http", "<cmism:objectId>working-copy</cmism:objectId>" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/checked-in.http", "<cmism:objectId>test-document</cmism:objectId>" );
    test::addWsResponse( "http://mockup/ws/services/VersioningService", DATA_DIR "/ws/checkin.http" );

    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    // First get a checked out document
    string id = "working-copy";
    libcmis::ObjectPtr object = session->getObject( id );
    libcmis::DocumentPtr pwc = boost::dynamic_pointer_cast< libcmis::Document >( object );

    // Do the checkin
    bool isMajor = true;
    string comment( "Some check-in comment" );
    PropertyPtrMap properties;
    string newContent = "Some New content to check in";
    boost::shared_ptr< ostream > stream ( new stringstream( newContent ) );
    string contentType = "text/plain";
    string filename = "filename.txt";
    libcmis::DocumentPtr updated = pwc->checkIn( isMajor, comment, properties,
                                                 stream, contentType, filename );

    // Check that we had the new version
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong filename: probably not the new version",
                                  filename, updated->getContentFilename() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong commit comment: not the new version",
                                  comment, updated->getStringProperty( "cmis:checkinComment" ) );

    // Check the sent request
    ostringstream converter;
    converter << newContent.size( );
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/VersioningService" );
    string expectedRequest = "<cmism:checkIn" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:objectId>" + id + "</cmism:objectId>"
                                 "<cmism:major>true</cmism:major>"
                                 "<cmism:properties/>"
                                 "<cmism:contentStream>"
                                         "<cmism:length>" + converter.str() + "</cmism:length>"
                                         "<cmism:mimeType>" + contentType + "</cmism:mimeType>"
                                         "<cmism:filename>" + filename + "</cmism:filename>"
                                         "<cmism:stream>"
                                             "<xop:Include xmlns:xop=\"http://www.w3.org/2004/08/xop/include\" "
                                                           "href=\"cid:obfuscated\"/>"
                                         "</cmism:stream>"
                                 "</cmism:contentStream>"
                                 "<cmism:checkinComment>" + comment + "</cmism:checkinComment>"
                             "</cmism:checkIn>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::getAllVersionsTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    test::addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/type-docLevel2.http" );
    test::addWsResponse( "http://mockup/ws/services/ObjectService", DATA_DIR "/ws/test-document.http" );
    test::addWsResponse( "http://mockup/ws/services/VersioningService", DATA_DIR "/ws/get-versions.http" );

    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );

    // First get a document
    string id = "test-document";
    libcmis::ObjectPtr object = session->getObject( id );
    string seriesId = object->getStringProperty( "cmis:versionSeriesId" );
    libcmis::DocumentPtr doc = boost::dynamic_pointer_cast< libcmis::Document >( object );

    // Get all the versions (method to check)
    vector< libcmis::DocumentPtr > versions = doc->getAllVersions( );

    // Checks
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of versions", size_t( 2 ), versions.size( ) );

    // Check the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/VersioningService" );
    string expectedRequest = "<cmism:getAllVersions" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>mock</cmism:repositoryId>"
                                 "<cmism:objectId>" + seriesId + "</cmism:objectId>"
                                 "<cmism:includeAllowableActions>true</cmism:includeAllowableActions>"
                             "</cmism:getAllVersions>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::navigationServiceCopyTest()
{
    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    NavigationService service( session.get() );

    {
        NavigationService copy( service );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Session not copied", service.m_session, copy.m_session );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "URL not copied", service.m_url, copy.m_url );
    }

    {
        NavigationService copy;
        copy = service;
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Session not copied", service.m_session, copy.m_session );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "URL not copied", service.m_url, copy.m_url );
    }
}

void WSTest::repositoryServiceCopyTest()
{
    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    RepositoryService service( session.get() );

    {
        RepositoryService copy( service );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Session not copied", service.m_session, copy.m_session );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "URL not copied", service.m_url, copy.m_url );
    }

    {
        RepositoryService copy;
        copy = service;
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Session not copied", service.m_session, copy.m_session );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "URL not copied", service.m_url, copy.m_url );
    }
}

void WSTest::objectServiceCopyTest()
{
    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    ObjectService service( session.get() );

    {
        ObjectService copy( service );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Session not copied", service.m_session, copy.m_session );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "URL not copied", service.m_url, copy.m_url );
    }

    {
        ObjectService copy;
        copy = service;
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Session not copied", service.m_session, copy.m_session );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "URL not copied", service.m_url, copy.m_url );
    }
}

void WSTest::versioningServiceCopyTest()
{
    WSSessionPtr session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    VersioningService service( session.get() );

    {
        VersioningService copy( service );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Session not copied", service.m_session, copy.m_session );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "URL not copied", service.m_url, copy.m_url );
    }

    {
        VersioningService copy;
        copy = service;
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Session not copied", service.m_session, copy.m_session );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "URL not copied", service.m_url, copy.m_url );
    }
}

WSSessionPtr WSTest::getTestSession( string username, string password, bool noRepos )
{
    WSSessionPtr session( new WSSession( ) );
    session->m_username = username;
    session->m_password = password;

    string buf;
    test::loadFromFile( DATA_DIR "/ws/CMISWS-Service.wsdl", buf );
    session->parseWsdl( buf );
    session->initializeResponseFactory( );

    // Manually define the repositories to avoid the HTTP query
    if ( noRepos )
    {
        libcmis::RepositoryPtr repo = getTestRepository( );
        session->m_repositories.push_back( repo );
        session->m_repositoryId = repo->getId( );
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
    repo->m_rootId = "root-folder";

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
    capabilities[libcmis::Repository::Renditions] = "read";
    capabilities[libcmis::Repository::Unfiling] = "true";
    capabilities[libcmis::Repository::VersionSpecificFiling] = "false";
    capabilities[libcmis::Repository::Join] = "none";
    repo->m_capabilities = capabilities;

    return repo;
}
