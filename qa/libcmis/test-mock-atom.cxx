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

#define SERVER_URL string( "http://mockup/binding" )
#define SERVER_REPOSITORY string( "mock" )
#define SERVER_USERNAME "tester"
#define SERVER_PASSWORD "somepass"

#define private public
#define protected public

#include <mockup-config.h>
#include "atom-session.hxx"
#include "session-factory.hxx"

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
        void getUnexistantObjectTest( );

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
        CPPUNIT_TEST( getUnexistantObjectTest );
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
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "data/atom-type-folder.xml" );
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
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel2", "data/atom-type-docLevel2.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel1", "data/atom-type-docLevel1.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:document", "data/atom-type-document.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectTypePtr actual = session.getType( "DocumentLevel2" );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Parent type", string( "DocumentLevel1" ), actual->getParentType( )->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Base type", string( "cmis:document" ), actual->getBaseType( )->getId( ) );
}

void AtomTest::getTypeChildrenTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=DocumentLevel1", "data/atom-type-docLevel1.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:document", "data/atom-type-document.xml" );
    curl_mockup_addResponse( "http://mockup/mock/types", "typeId=cmis:document", "data/atom-typechildren-document.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    libcmis::ObjectTypePtr actual = session.getType( "cmis:document" );
    vector< libcmis::ObjectTypePtr > children = actual->getChildren( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children", size_t( 1 ), children.size( ) );
}

void AtomTest::getObjectTest( )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( "http://mockup/mock/id", "id=valid-object", "data/atom-valid-object.xml" );
    curl_mockup_addResponse( "http://mockup/mock/type", "id=cmis:folder", "data/atom-type-folder.xml" );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    AtomPubSession session = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );

    string expectedId( "valid-object" );
    libcmis::ObjectPtr actual = session.getObject( expectedId );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Id for fetched object", expectedId, actual->getId( ) );
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
