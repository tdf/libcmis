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
#include "atom-session.hxx"
#include "session-factory.hxx"
#include "document.hxx"

using namespace std;

class AtomTest : public CppUnit::TestFixture
{
    public:
        void getRepositoriesTest( );
        void getRepositoriesBadAuthTest( );
        void sessionCreationTest( );
        void sessionCreationBadAuthTest( );
        void sessionCreationProxyTest( );
        void authCallbackTest( );
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
        void getChildrenTest( );
        void getDocumentParentsTest( );
        void getContentStreamTest( );
        void setContentStreamTest( );

        CPPUNIT_TEST_SUITE( AtomTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( getRepositoriesBadAuthTest );
        CPPUNIT_TEST( sessionCreationTest );
        CPPUNIT_TEST( sessionCreationBadAuthTest );
        CPPUNIT_TEST( sessionCreationProxyTest );
        CPPUNIT_TEST( authCallbackTest );
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
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST( getDocumentParentsTest );
        CPPUNIT_TEST( getContentStreamTest );
        CPPUNIT_TEST( setContentStreamTest );
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

void AtomTest::getRepositoriesTest( )
{
    // Response showing one mock repository
    curl_mockup_reset( );
    curl_mockup_setResponse( "data/atom-workspaces.xml" );

    list< libcmis::RepositoryPtr > actual = AtomPubSession::getRepositories( SERVER_URL, SERVER_USERNAME, SERVER_PASSWORD );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of repositories", size_t( 1 ), actual.size( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong repository found", SERVER_REPOSITORY, actual.front()->getId( ) );
}

void AtomTest::getRepositoriesBadAuthTest( )
{
    // Response showing one mock repository
    curl_mockup_reset( );
    curl_mockup_setResponse( "data/atom-workspaces.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    try
    {
        AtomPubSession::getRepositories( SERVER_URL, "baduser", "badpass" );
        CPPUNIT_FAIL( "Exception should have been thrown" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong error type", string( "permissionDenied" ), e.getType( ) );
    }
}

void AtomTest::sessionCreationTest( )
{
    // Response showing one mock repository
    curl_mockup_reset( );
    curl_mockup_setResponse( "data/atom-workspaces.xml" );
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
    curl_mockup_setResponse( "data/atom-workspaces.xml" );
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
    curl_mockup_setResponse( "data/atom-workspaces.xml" );
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
    curl_mockup_setResponse( "data/atom-workspaces.xml" );
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

void AtomTest::getTypeTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", "data/atom-type-folder.xml" );
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
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", string( "No such type: bad_type" ), string( e.what() ) );
    }
}

void AtomTest::getTypeParentsTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", "data/atom-type-docLevel2.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel1", "GET", "data/atom-type-docLevel1.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:document", "GET", "data/atom-type-document.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectTypePtr actual = session.getType( "DocumentLevel2" );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Parent type", string( "DocumentLevel1" ), actual->getParentType( )->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Base type", string( "cmis:document" ), actual->getBaseType( )->getId( ) );
}

void AtomTest::getTypeChildrenTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel1", "GET", "data/atom-type-docLevel1.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:document", "GET", "data/atom-type-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/types", "typeId=cmis:document", "GET", "data/atom-typechildren-document.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectTypePtr actual = session.getType( "cmis:document" );
    vector< libcmis::ObjectTypePtr > children = actual->getChildren( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children", size_t( 1 ), children.size( ) );
}

void AtomTest::getObjectTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=valid-object", "GET", "data/atom-valid-object.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", "data/atom-type-folder.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    string expectedId( "valid-object" );
    libcmis::ObjectPtr actual = session.getObject( expectedId );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Id for fetched object", expectedId, actual->getId( ) );
}

void AtomTest::getDocumentTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", "data/atom-test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", "data/atom-type-docLevel2.xml" );
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
    curl_mockup_addResponse( "http://mockup/mock/id", "id=valid-object", "GET", "data/atom-valid-object.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=root-folder", "GET", "data/atom-root-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", "data/atom-type-folder.xml" );
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
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", "data/atom-test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", "data/atom-type-docLevel2.xml" );
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
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", string( "No such node: " ) + id, string( e.what() ) );
    }
}

void AtomTest::getByPathValidTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/path", "path=/Valid Object", "GET", "data/atom-valid-object.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", "data/atom-type-folder.xml" );
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
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message",
                string( "No node corresponding to path: /some/invalid/path" ), string( e.what() ) );
    }

}

void AtomTest::getAllowableActionsTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=valid-object", "GET", "data/atom-valid-object.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", "data/atom-type-folder.xml" );
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

void AtomTest::getChildrenTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/children", "id=root-folder", "GET", "data/atom-root-children.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=root-folder", "GET", "data/atom-root-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", "data/atom-type-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", "data/atom-type-docLevel2.xml" );
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
    curl_mockup_addResponse( "http://mockup/mock/parents", "id=test-document", "GET", "data/atom-test-document-parents.xml" );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", "data/atom-test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "GET", "data/atom-type-folder.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", "data/atom-type-docLevel2.xml" );
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
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", "data/atom-test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", "data/atom-type-docLevel2.xml" );

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
    curl_mockup_addResponse( "http://mockup/mock/id", "id=test-document", "GET", "data/atom-test-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "GET", "data/atom-type-docLevel2.xml" );
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
        const char* content = curl_mockup_getRequest( "http://mockup/mock/content/", "id=test-document", "PUT" );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad content uploaded", expectedContent, string( content ) );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

AtomPubSession AtomTest::getTestSession( string username, string password )
{
    AtomPubSession session;
    string buf;
    loadFromFile( "data/atom-workspaces.xml", buf );
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
