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

#include <sstream>

#define SERVER_URL string( "http://mockup/binding" )
#define SERVER_REPOSITORY string( "mock" )
#define SERVER_USERNAME "tester"
#define SERVER_PASSWORD "somepass"

#define private public
#define protected public

#include <mockup-config.h>
#include "test-helpers.hxx"
#include "atom-session.hxx"
#include "session-factory.hxx"
#include "document.hxx"

using namespace std;
using libcmis::PropertyPtrMap;

class AtomTest : public CppUnit::TestFixture
{
    public:
        void sessionCreationTest( );
        void sessionCreationBadAuthTest( );
        void sessionCreationProxyTest( );
        void authCallbackTest( );
        void getRepositoriesTest( );
        void getTypeTest( );
        void getUnexistantTypeTest( );
        void getTypeParentsTest( );
        void getTypeChildrenTest( );
        void getObjectTest( );
        void getDocumentTest( );
        void getUnexistantObjectTest( );
        void getFolderTest( );
        void getFolderBadTypeTest( );
        void getByPathValidTest( );
        void getByPathInvalidTest( );
        void getAllowableActionsTest( );
        void getAllowableActionsNotIncludedTest( );
        void getChildrenTest( );
        void getDocumentParentsTest( );
        void getContentStreamTest( );
        void setContentStreamTest( );
        void updatePropertiesTest( );
        void createFolderTest( );
        void createFolderBadTypeTest( );
        void createDocumentTest( );
        void deleteDocumentTest( );
        void deleteFolderTreeTest( );
        void checkOutTest( );
        void cancelCheckOutTest( );
        void checkInTest( );

        CPPUNIT_TEST_SUITE( AtomTest );
        CPPUNIT_TEST( sessionCreationTest );
        CPPUNIT_TEST( sessionCreationBadAuthTest );
        CPPUNIT_TEST( sessionCreationProxyTest );
        CPPUNIT_TEST( authCallbackTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( getTypeTest );
        CPPUNIT_TEST( getUnexistantTypeTest );
        CPPUNIT_TEST( getTypeParentsTest );
        CPPUNIT_TEST( getTypeChildrenTest );
        CPPUNIT_TEST( getObjectTest );
        CPPUNIT_TEST( getDocumentTest );
        CPPUNIT_TEST( getUnexistantObjectTest );
        CPPUNIT_TEST( getFolderTest );
        CPPUNIT_TEST( getFolderBadTypeTest );
        CPPUNIT_TEST( getByPathValidTest );
        CPPUNIT_TEST( getByPathInvalidTest );
        CPPUNIT_TEST( getAllowableActionsTest );
        CPPUNIT_TEST( getAllowableActionsNotIncludedTest );
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST( getDocumentParentsTest );
        CPPUNIT_TEST( getContentStreamTest );
        CPPUNIT_TEST( setContentStreamTest );
        CPPUNIT_TEST( updatePropertiesTest );
        CPPUNIT_TEST( createFolderTest );
        CPPUNIT_TEST( createFolderBadTypeTest );
        CPPUNIT_TEST( createDocumentTest );
        CPPUNIT_TEST( deleteDocumentTest );
        CPPUNIT_TEST( deleteFolderTreeTest );
        CPPUNIT_TEST( checkOutTest );
        CPPUNIT_TEST( cancelCheckOutTest );
        CPPUNIT_TEST( checkInTest );
        CPPUNIT_TEST_SUITE_END( );

        AtomPubSession getTestSession( string username = string( ), string password = string( ) );
        void loadFromFile( const char* path, string& buf );
};

class TestAuthProvider : public libcmis::AuthProvider
{
    bool m_fail;

    public:
        TestAuthProvider( bool fail ) : m_fail( fail ) { }

        bool authenticationQuery( std::string&, std::string& password )
        {
            password = SERVER_PASSWORD;
            return !m_fail;
        }
};

CPPUNIT_TEST_SUITE_REGISTRATION( AtomTest );

void AtomTest::sessionCreationTest( )
{
    // Response showing one mock repository
    curl_mockup_reset( );
    curl_mockup_setResponse( DATA_DIR "/atom/workspaces.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session( SERVER_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );

    // Check for the mandatory collection URLs
    CPPUNIT_ASSERT_MESSAGE( "root collection URL missing",
            !session.getAtomRepository()->getCollectionUrl( Collection::Root ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "types collection URL missing",
            !session.getAtomRepository()->getCollectionUrl( Collection::Types ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "query collection URL missing",
            !session.getAtomRepository()->getCollectionUrl( Collection::Query ).empty() );

    // The optional collection URLs are present on InMemory, so check them
    CPPUNIT_ASSERT_MESSAGE( "checkedout collection URL missing",
            !session.getAtomRepository()->getCollectionUrl( Collection::CheckedOut ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "unfiled collection URL missing",
            !session.getAtomRepository()->getCollectionUrl( Collection::Unfiled ).empty() );

    // Check for the mandatory URI template URLs
    CPPUNIT_ASSERT_MESSAGE( "objectbyid URI template URL missing",
            !session.getAtomRepository()->getUriTemplate( UriTemplate::ObjectById ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "objectbypath URI template URL missing",
            !session.getAtomRepository()->getUriTemplate( UriTemplate::ObjectByPath ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "typebyid URI template URL missing",
            !session.getAtomRepository()->getUriTemplate( UriTemplate::TypeById ).empty() );
    
    // The optional URI template URL is present on InMemory, so check it
    CPPUNIT_ASSERT_MESSAGE( "query URI template URL missing",
            !session.getAtomRepository()->getUriTemplate( UriTemplate::Query ).empty() );

    // Check that the root id is defined
    CPPUNIT_ASSERT_MESSAGE( "Root node ID is missing",
            !session.getRootId().empty() );
}

void AtomTest::sessionCreationBadAuthTest( )
{
    // Response showing one mock repository
    curl_mockup_reset( );
    curl_mockup_setResponse( DATA_DIR "/atom/workspaces.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    try
    {
        AtomPubSession session( SERVER_URL, SERVER_REPOSITORY, "bad", "bad" );
        CPPUNIT_FAIL( "Exception should have been thrown" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong error type", string( "permissionDenied" ), e.getType( ) );
    }
}

void AtomTest::sessionCreationProxyTest( )
{
    // Response showing one mock repository
    curl_mockup_reset( );
    curl_mockup_setResponse( DATA_DIR "/atom/workspaces.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    string proxy( "proxy" );
    string noProxy( "noProxy" );
    string proxyUser( "proxyUser" );
    string proxyPass( "proxyPass" );

    libcmis::SessionFactory::setProxySettings( proxy, noProxy, proxyUser, proxyPass );

    AtomPubSession session( SERVER_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Proxy not set", proxy, string( curl_mockup_getProxy( session.m_curlHandle ) ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "NoProxy not set", noProxy, string( curl_mockup_getNoProxy( session.m_curlHandle ) ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Proxy User not set", proxyUser, string( curl_mockup_getProxyUser( session.m_curlHandle ) ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Proxy Pass not set", proxyPass, string( curl_mockup_getProxyPass( session.m_curlHandle ) ) );
    
    // Reset proxy settings to default for next tests
    libcmis::SessionFactory::setProxySettings( string(), string(), string(), string() );
}

void AtomTest::authCallbackTest( )
{
    // Response showing one mock repository
    curl_mockup_reset( );
    curl_mockup_setResponse( DATA_DIR "/atom/workspaces.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );


    // Test cancelled authentication
    {
        libcmis::AuthProviderPtr authProvider( new TestAuthProvider( true ) );
        libcmis::SessionFactory::setAuthenticationProvider( authProvider );
        try
        {
            AtomPubSession session( SERVER_URL, SERVER_REPOSITORY, SERVER_USERNAME, string( ) );
            CPPUNIT_FAIL( "Should raise an exception saying the user cancelled the authentication" );
        }
        catch ( const libcmis::Exception& e )
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message",
                    string( "User cancelled authentication request" ), string( e.what() ) );
        }
    }
    
    // Test provided authentication
    {
        libcmis::AuthProviderPtr authProvider( new TestAuthProvider( false ) );
        libcmis::SessionFactory::setAuthenticationProvider( authProvider );
        AtomPubSession session( SERVER_URL, SERVER_REPOSITORY, SERVER_USERNAME, string( ) );
    }
}

void AtomTest::getRepositoriesTest( )
{
    // Response showing one mock repository
    curl_mockup_reset( );
    curl_mockup_setResponse( DATA_DIR "/atom/workspaces.xml" );

    AtomPubSession session( SERVER_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD );
    vector< libcmis::RepositoryPtr > actual = session.getRepositories( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of repositories", size_t( 1 ), actual.size( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong repository found", SERVER_REPOSITORY, actual.front()->getId( ) );
}

void AtomTest::getTypeTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", DATA_DIR "/atom/type-folder.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    string expectedId( "cmis:folder" );
    libcmis::ObjectTypePtr actual = session.getType( expectedId );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Id for fetched type", expectedId, actual->getId( ) );
}

void AtomTest::getUnexistantTypeTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    string expectedId( "bad_type" );
    try
    {
        session.getType( expectedId );
        CPPUNIT_FAIL( "Exception should be raised: invalid ID" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong error type", string( "objectNotFound" ), e.getType() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", string( "No such type: bad_type" ), string( e.what() ) );
    }
}

void AtomTest::getTypeParentsTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", DATA_DIR "/atom/type-docLevel2.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel1", "GET", DATA_DIR "/atom/type-docLevel1.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:document", "GET", DATA_DIR "/atom/type-document.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectTypePtr actual = session.getType( "DocumentLevel2" );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Parent type", string( "DocumentLevel1" ), actual->getParentType( )->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Base type", string( "cmis:document" ), actual->getBaseType( )->getId( ) );
}

void AtomTest::getTypeChildrenTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel1", "GET", DATA_DIR "/atom/type-docLevel1.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:document", "GET", DATA_DIR "/atom/type-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/types", "typeId=cmis:document", "GET", DATA_DIR "/atom/typechildren-document.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectTypePtr actual = session.getType( "cmis:document" );
    vector< libcmis::ObjectTypePtr > children = actual->getChildren( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children", size_t( 1 ), children.size( ) );
}

void AtomTest::getObjectTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=valid-object", "GET", DATA_DIR "/atom/valid-object.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", DATA_DIR "/atom/type-folder.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    string expectedId( "valid-object" );
    libcmis::ObjectPtr actual = session.getObject( expectedId );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Id for fetched object", expectedId, actual->getId( ) );
}

void AtomTest::getDocumentTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", DATA_DIR "/atom/test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", DATA_DIR "/atom/type-docLevel2.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    string expectedId( "test-document" );
    libcmis::ObjectPtr actual = session.getObject( expectedId );

    // Do we have a document?
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( actual );
    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of libcmis::DocumentPtr",
            NULL != document );

    // Test the document properties
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
}

void AtomTest::getFolderTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=valid-object", "GET", DATA_DIR "/atom/valid-object.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=root-folder", "GET", DATA_DIR "/atom/root-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", DATA_DIR "/atom/type-folder.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    string expectedId( "valid-object" );
    libcmis::FolderPtr actual = session.getFolder( expectedId );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder ID", expectedId, actual->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder name", string( "Valid Object" ), actual->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder path", string( "/Valid Object" ), actual->getPath( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:folder" ), actual->getBaseType( ) );
    CPPUNIT_ASSERT_MESSAGE( "Missing folder parent", actual->getFolderParent( ).get( ) );
    CPPUNIT_ASSERT_MESSAGE( "Not a root folder", !actual->isRootFolder() );

    CPPUNIT_ASSERT_MESSAGE( "CreatedBy is missing", !actual->getCreatedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "CreationDate is missing", !actual->getCreationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "LastModifiedBy is missing", !actual->getLastModifiedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "LastModificationDate is missing", !actual->getLastModificationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "ChangeToken is missing", !actual->getChangeToken( ).empty( ) );
}

void AtomTest::getFolderBadTypeTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", DATA_DIR "/atom/test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", DATA_DIR "/atom/type-docLevel2.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::FolderPtr actual = session.getFolder( "test-document" );

    CPPUNIT_ASSERT_MESSAGE( "returned folder should be an empty pointer", NULL == actual );
}

void AtomTest::getUnexistantObjectTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    string id( "bad_object" );
    try
    {
        session.getObject( id );
        CPPUNIT_FAIL( "Exception should be raised: invalid ID" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong CMIS exception type", string( "objectNotFound" ), e.getType() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", string( "No such node: " ) + id, string( e.what() ) );
    }
}

void AtomTest::getByPathValidTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/path", "path=/Valid Object", "GET", DATA_DIR "/atom/valid-object.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", DATA_DIR "/atom/type-folder.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectPtr actual = session.getObjectByPath( string( "/Valid Object" ) );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Id for fetched object", string( "valid-object" ), actual->getId( ) );
}

void AtomTest::getByPathInvalidTest( )
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    try
    {
        session.getObjectByPath( string( "/some/invalid/path" ) );
        CPPUNIT_FAIL( "Exception should be thrown: invalid Path" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong error type", string( "objectNotFound" ), e.getType() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message",
                string( "No node corresponding to path: /some/invalid/path" ), string( e.what() ) );
    }

}

void AtomTest::getAllowableActionsTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=valid-object", "GET", DATA_DIR "/atom/valid-object.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", DATA_DIR "/atom/type-folder.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    string expectedId( "valid-object" );
    libcmis::FolderPtr actual = session.getFolder( expectedId );

    boost::shared_ptr< libcmis::AllowableActions > toCheck = actual->getAllowableActions( );
    CPPUNIT_ASSERT_MESSAGE( "ApplyACL allowable action not defined... are all the actions read?",
            toCheck->isDefined( libcmis::ObjectAction::ApplyACL ) );

    CPPUNIT_ASSERT_MESSAGE( "GetChildren allowable action should be true",
            toCheck->isDefined( libcmis::ObjectAction::GetChildren ) &&
            toCheck->isAllowed( libcmis::ObjectAction::GetChildren ) );
}

void AtomTest::getAllowableActionsNotIncludedTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=valid-object", "GET", DATA_DIR "/atom/valid-object-noactions.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", DATA_DIR "/atom/type-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/allowableactions", "id=valid-object", "GET", DATA_DIR "/atom/allowable-actions.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    string expectedId( "valid-object" );
    libcmis::FolderPtr actual = session.getFolder( expectedId );

    // In some cases (mostly when getting folder children), we may not have the allowable actions
    // included in the object answer. Test that we are querying them when needed in those cases.
    boost::shared_ptr< libcmis::AllowableActions > toCheck = actual->getAllowableActions( );
    CPPUNIT_ASSERT_MESSAGE( "ApplyACL allowable action not defined... are all the actions read?",
            toCheck->isDefined( libcmis::ObjectAction::ApplyACL ) );

    CPPUNIT_ASSERT_MESSAGE( "GetChildren allowable action should be true",
            toCheck->isDefined( libcmis::ObjectAction::GetChildren ) &&
            toCheck->isAllowed( libcmis::ObjectAction::GetChildren ) );
}

void AtomTest::getChildrenTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/children", "id=root-folder", "GET", DATA_DIR "/atom/root-children.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=root-folder", "GET", DATA_DIR "/atom/root-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", DATA_DIR "/atom/type-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", DATA_DIR "/atom/type-docLevel2.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    vector< libcmis::ObjectPtr > children = session.getRootFolder()->getChildren( );
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
}

void AtomTest::getDocumentParentsTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/parents", "id=test-document", "GET", DATA_DIR "/atom/test-document-parents.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", DATA_DIR "/atom/test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", DATA_DIR "/atom/type-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", DATA_DIR "/atom/type-docLevel2.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectPtr object = session.getObject( "test-document" );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );
    
    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );
    vector< libcmis::FolderPtr > actual = document->getParents( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad number of parents", size_t( 2 ), actual.size() );
}

void AtomTest::getContentStreamTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", DATA_DIR "/atom/test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", DATA_DIR "/atom/type-docLevel2.xml" );

    string expectedContent( "Some content stream" );
    curl_mockup_addResponse( "http://mockup/mock/content/data.txt", "id=test-document", "GET", expectedContent.c_str( ), 0, false );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectPtr object = session.getObject( "test-document" );
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

void AtomTest::setContentStreamTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", DATA_DIR "/atom/test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", DATA_DIR "/atom/type-docLevel2.xml" );
    curl_mockup_addResponse( "http://mockup/mock/content/data.txt", "id=test-document", "PUT", "Updated", 0, false );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectPtr object = session.getObject( "test-document" );
    libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object );

    try
    {
        string expectedContent( "Some content stream to set" );
        boost::shared_ptr< ostream > os ( new stringstream ( expectedContent ) );
        string filename( "name.txt" );
        document->setContentStream( os, "text/plain", filename );
        
        CPPUNIT_ASSERT_MESSAGE( "Object not refreshed during setContentStream", object->getRefreshTimestamp( ) > 0 );

        // Check the content has been properly uploaded
        const char* content = curl_mockup_getRequestBody( "http://mockup/mock/content/", "id=test-document", "PUT" );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad content uploaded", expectedContent, string( content ) );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void AtomTest::updatePropertiesTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", DATA_DIR "/atom/test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", DATA_DIR "/atom/type-docLevel2.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "PUT", DATA_DIR "/atom/test-document-updated.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    // Values for the test
    libcmis::ObjectPtr object = session.getObject( "test-document" );
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

    // Update the properties (method to test)
    libcmis::ObjectPtr updated = object->updateProperties( newProperties );

    // Check that the proper request has been send
    // In order to avoid to check changing strings (containing a timestamp),
    // get the cmisra:object tree and compare it.
    string request( curl_mockup_getRequestBody( "http://mockup/mock/id", "id=test-document", "PUT" ) );
    string actualObject = test::getXmlNodeAsString( request, "/atom:entry/cmisra:object" );

    string expectedObject = "<cmisra:object>"
                                "<cmis:properties>"
                                    "<cmis:propertyString propertyDefinitionId=\"cmis:name\" localName=\"cmis:name\" "
                                                          "displayName=\"Name\" queryName=\"cmis:name\">"
                                        "<cmis:value>New name</cmis:value>"
                                    "</cmis:propertyString>"
                                "</cmis:properties>"
                            "</cmisra:object>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request content sent", expectedObject, actualObject );

    // Check that the properties are updated after the call
    PropertyPtrMap::iterator propIt = updated->getProperties( ).find( propertyName );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong value after refresh", expectedValue, propIt->second->getStrings().front( ) );
}

void AtomTest::createFolderTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/children", "id=root-folder", "POST", DATA_DIR "/atom/create-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=root-folder", "GET", DATA_DIR "/atom/root-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", DATA_DIR "/atom/type-folder.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::FolderPtr parent = session.getRootFolder( );

    // Prepare the properties for the new object, object type is cmis:folder
    PropertyPtrMap props;
    libcmis::ObjectTypePtr type = session.getType( "cmis:folder" );
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

    // Actually send the folder creation request
    libcmis::FolderPtr created = parent->createFolder( props );

    // Check that something came back
    CPPUNIT_ASSERT_MESSAGE( "Change token shouldn't be empty: object should have been refreshed",
            !created->getChangeToken( ).empty() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong name", expectedName, created->getName( ) );

    // Check that the proper request has been sent
    string request( curl_mockup_getRequestBody( "http://mockup/mock/children", "id=root-folder", "POST" ) );
    string actualObject = test::getXmlNodeAsString( request, "/atom:entry/cmisra:object" );

    string expectedObject = "<cmisra:object>"
                                "<cmis:properties>"
                                    "<cmis:propertyString propertyDefinitionId=\"cmis:name\" localName=\"cmis:name\" "
                                                          "displayName=\"Name\" queryName=\"cmis:name\">"
                                        "<cmis:value>create folder</cmis:value>"
                                    "</cmis:propertyString>"
                                    "<cmis:propertyId propertyDefinitionId=\"cmis:objectTypeId\" localName=\"cmis:objectTypeId\""
                                                          " displayName=\"Type-Id\" queryName=\"cmis:objectTypeId\">"
                                        "<cmis:value>cmis:folder</cmis:value>"
                                    "</cmis:propertyId>"
                                "</cmis:properties>"
                            "</cmisra:object>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request content sent", expectedObject, actualObject );
}

void AtomTest::createFolderBadTypeTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/children", "id=root-folder", "POST", DATA_DIR "/atom/create-folder-bad-type.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=root-folder", "GET", DATA_DIR "/atom/root-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", DATA_DIR "/atom/type-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:document", "GET", DATA_DIR "/atom/type-document.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::FolderPtr parent = session.getRootFolder( );

    // Prepare the properties for the new object, object type is cmis:folder
    PropertyPtrMap props;
    libcmis::ObjectTypePtr type = session.getType( "cmis:folder" );
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
        parent->createFolder( props );
        CPPUNIT_FAIL( "Should not succeed to return a folder" );
    }
    catch ( libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong error type", string( "constraint" ), e.getType() );
        CPPUNIT_ASSERT_MESSAGE( "Bad exception message",
                string( e.what( ) ).find( "Created object is not a folder: " ) != string::npos );
    }

    // Check that the proper request has been sent
    string request( curl_mockup_getRequestBody( "http://mockup/mock/children", "id=root-folder", "POST" ) );
    string actualObject = test::getXmlNodeAsString( request, "/atom:entry/cmisra:object" );

    string expectedObject = "<cmisra:object>"
                                "<cmis:properties>"
                                    "<cmis:propertyString propertyDefinitionId=\"cmis:name\" localName=\"cmis:name\" "
                                                          "displayName=\"Name\" queryName=\"cmis:name\">"
                                        "<cmis:value>create folder</cmis:value>"
                                    "</cmis:propertyString>"
                                    "<cmis:propertyId propertyDefinitionId=\"cmis:objectTypeId\" localName=\"cmis:objectTypeId\""
                                                          " displayName=\"Type-Id\" queryName=\"cmis:objectTypeId\">"
                                        "<cmis:value>cmis:document</cmis:value>"
                                    "</cmis:propertyId>"
                                "</cmis:properties>"
                            "</cmisra:object>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request content sent", expectedObject, actualObject );
}

void AtomTest::createDocumentTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/children", "id=root-folder", "POST", "Response body up to server", 201, false,
           "Location: http://mockup/mock/id?id=create-document\r\n" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=create-document", "GET", DATA_DIR "/atom/create-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=root-folder", "GET", DATA_DIR "/atom/root-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", DATA_DIR "/atom/type-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:document", "GET", DATA_DIR "/atom/type-document.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::FolderPtr parent = session.getRootFolder( );

    // Prepare the properties for the new object, object type is cmis:folder
    PropertyPtrMap props;
    libcmis::ObjectTypePtr type = session.getType( "cmis:document" );
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

    // Check that the request is the expected one
    string request( curl_mockup_getRequestBody( "http://mockup/mock/children", "id=root-folder", "POST" ) );
    string actualContent = test::getXmlNodeAsString( request, "/atom:entry/cmisra:content" );
    string expectedContent = "<cmisra:content>"
                                "<cmisra:mediatype>text/plain</cmisra:mediatype>"
                                "<cmisra:base64>U29tZSBjb250ZW50</cmisra:base64>"
                             "</cmisra:content>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request content sent", expectedContent, actualContent );

    string actualObject = test::getXmlNodeAsString( request, "/atom:entry/cmisra:object" );
    string expectedObject = "<cmisra:object>"
                                "<cmis:properties>"
                                    "<cmis:propertyString propertyDefinitionId=\"cmis:name\" localName=\"cmis:name\" "
                                                          "displayName=\"Name\" queryName=\"cmis:name\">"
                                        "<cmis:value>create document</cmis:value>"
                                    "</cmis:propertyString>"
                                    "<cmis:propertyId propertyDefinitionId=\"cmis:objectTypeId\" localName=\"cmis:objectTypeId\""
                                                          " displayName=\"Type-Id\" queryName=\"cmis:objectTypeId\">"
                                        "<cmis:value>cmis:document</cmis:value>"
                                    "</cmis:propertyId>"
                                "</cmis:properties>"
                            "</cmisra:object>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request object sent", expectedObject, actualObject );
}

void AtomTest::deleteDocumentTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", DATA_DIR "/atom/test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", DATA_DIR "/atom/type-docLevel2.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "DELETE", "", 204, false );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectPtr object = session.getObject( "test-document" );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );

    document->remove( );

    // Test the sent request
    const char* request = curl_mockup_getRequestBody( "http://mockup/mock/id", "id=test-document", "DELETE" );
    CPPUNIT_ASSERT_MESSAGE( "DELETE request not sent", request );
}

void AtomTest::deleteFolderTreeTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=valid-object", "GET", DATA_DIR "/atom/valid-object.xml" );
    curl_mockup_addResponse( "http://mockup/mock/descendants", "id=valid-object", "DELETE", "", 204, false );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", DATA_DIR "/atom/type-folder.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectPtr object = session.getObject( "valid-object" );
    libcmis::Folder* folder = dynamic_cast< libcmis::Folder* >( object.get() );

    folder->removeTree( );

    // Test the sent request
    const struct HttpRequest* request = curl_mockup_getRequest( "http://mockup/mock/descendants", "id=valid-object", "DELETE" );
    CPPUNIT_ASSERT_MESSAGE( "DELETE request not sent", request );
    delete request;
}

void AtomTest::checkOutTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", DATA_DIR "/atom/test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", DATA_DIR "/atom/type-docLevel2.xml" );
    curl_mockup_addResponse( "http://mockup/mock/checkedout", "", "POST", DATA_DIR "/atom/working-copy.xml", 201 );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectPtr object = session.getObject( "test-document" );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );

    libcmis::DocumentPtr pwc = document->checkOut( );
    
    CPPUNIT_ASSERT_MESSAGE( "Missing returned Private Working Copy", pwc.get( ) != NULL );

    PropertyPtrMap::iterator it = pwc->getProperties( ).find( string( "cmis:isVersionSeriesCheckedOut" ) );
    vector< bool > values = it->second->getBools( );
    CPPUNIT_ASSERT_MESSAGE( "cmis:isVersionSeriesCheckedOut isn't true", values.front( ) );
}

void AtomTest::cancelCheckOutTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=working-copy", "GET", DATA_DIR "/atom/working-copy.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=working-copy", "DELETE", "", 204, false );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", DATA_DIR "/atom/type-docLevel2.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    // First get a checked out document
    libcmis::ObjectPtr object = session.getObject( "working-copy" );
    libcmis::DocumentPtr pwc = boost::dynamic_pointer_cast< libcmis::Document >( object );

    pwc->cancelCheckout( );

    // Check that the DELETE request was sent out
    const struct HttpRequest* request = curl_mockup_getRequest( "http://mockup/mock/id", "id=working-copy", "DELETE" );
    CPPUNIT_ASSERT_MESSAGE( "DELETE request not sent", request );
}

void AtomTest::checkInTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", DATA_DIR "/atom/type-docLevel2.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=working-copy", "GET", DATA_DIR "/atom/working-copy.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=working-copy", "PUT", DATA_DIR "/atom/test-document.xml", 200, true,
           "Location: http://mockup/mock/id?id=valid-object" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    // First get a checked out document
    libcmis::ObjectPtr object = session.getObject( "working-copy" );
    libcmis::DocumentPtr pwc = boost::dynamic_pointer_cast< libcmis::Document >( object );

    // Do the checkin
    bool isMajor = true;
    string comment( "Some check-in comment" );
    PropertyPtrMap properties;
    string newContent = "Some New content to check in";
    boost::shared_ptr< ostream > stream ( new stringstream( newContent ) );
    pwc->checkIn( isMajor, comment, properties, stream, "text/plain", "filename.txt" );

    // Make sure that the expected request has been sent
    const struct HttpRequest* request = curl_mockup_getRequest( "http://mockup/mock/id", "id=working-copy", "PUT" );
    CPPUNIT_ASSERT_MESSAGE( "PUT request not sent", request );

    string actualContent = test::getXmlNodeAsString( request->body, "/atom:entry/cmisra:content" );
    string expectedContent = "<cmisra:content><cmisra:mediatype>text/plain</cmisra:mediatype><cmisra:base64>U29tZSBOZXcgY29udGVudCB0byBjaGVjayBpbg==</cmisra:base64></cmisra:content>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong content sent", expectedContent, actualContent );

    // Still needs to test that checkin request parameters were OK
    string url( request->url );
    CPPUNIT_ASSERT_MESSAGE( "Sent checkin request has wroong major parameter", url.find("major=true") != string::npos );
    CPPUNIT_ASSERT_MESSAGE( "Sent checkin request has wrong checkinComment parameter", url.find( "checkinComment=" + comment ) != string::npos );
    CPPUNIT_ASSERT_MESSAGE( "Sent checkin request has no checkin parameter", url.find("checkin=true") != string::npos );
}

AtomPubSession AtomTest::getTestSession( string username, string password )
{
    AtomPubSession session;
    string buf;
    loadFromFile( DATA_DIR "/atom/workspaces.xml", buf );
    session.parseServiceDocument( buf );
    
    session.m_username = username;
    session.m_password = password;

    return session;
}

void AtomTest::loadFromFile( const char* path, string& buf )
{
    ifstream in( path );

    in.seekg( 0, ios::end );
    int length = in.tellg( );
    in.seekg( 0, ios::beg );

    char* buffer = new char[length];
    in.read( buffer, length );
    in.close( );

    buf = string( buffer, length );
    delete[] buffer;
}
