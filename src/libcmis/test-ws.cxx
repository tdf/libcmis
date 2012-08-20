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

#include <ws-requests.hxx>
#include <ws-session.hxx>

#define SERVER_WSDL_URL string( "http://localhost:8080/inmemory/services/RepositoryService" )
#define SERVER_REPOSITORIES_COUNT list< string >::size_type( 1 )
#define SERVER_REPOSITORY string( "A1" )
#define SERVER_USERNAME string( "tester" )
#define SERVER_PASSWORD string( "somepass" )

using namespace std;

class WSTest : public CppUnit::TestFixture
{
    public:

        // Generic session factory tests

        void getRepositoriesTest( );
        void sessionCreationTest( );
        void getRepositoryTest( );
        void getRepositoryBadTest( );

        // Types tests

        void getTypeDefinitionTest( );
        void getTypeDefinitionErrorTest( );
        void getTypeChildrenTest( );

        // Object tests
        void getObjectTest( );

        CPPUNIT_TEST_SUITE( WSTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( sessionCreationTest );
        CPPUNIT_TEST( getRepositoryTest );
        CPPUNIT_TEST( getRepositoryBadTest );
        CPPUNIT_TEST( getTypeDefinitionTest );
        CPPUNIT_TEST( getTypeDefinitionErrorTest );
        CPPUNIT_TEST( getTypeChildrenTest );
        CPPUNIT_TEST( getObjectTest );
        CPPUNIT_TEST_SUITE_END( );
};

void WSTest::getRepositoriesTest()
{
    list< libcmis::RepositoryPtr > actual = WSSession::getRepositories( SERVER_WSDL_URL, SERVER_USERNAME, SERVER_PASSWORD );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of repositories", size_t( 1 ), actual.size( ) );
}

void WSTest::sessionCreationTest( )
{
    WSSession session( SERVER_WSDL_URL, "", SERVER_USERNAME, SERVER_PASSWORD, false );
    CPPUNIT_ASSERT_MESSAGE( "No RepositoryService URL", !session.getServiceUrl( "RepositoryService" ).empty( ) );
}

void WSTest::getRepositoryTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD, false );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Repository info badly retrieved", string( "100" ), session.getRepository()->getRootId( ) );
}

void WSTest::getRepositoryBadTest( )
{
    WSSession session( SERVER_WSDL_URL, "", SERVER_USERNAME, SERVER_PASSWORD, false );
    try
    {
        session.getRepositoryService( ).getRepositoryInfo( "bad" );
        CPPUNIT_FAIL( "Should have thrown SoapFault" );
    }
    catch( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception type", string( "invalidArgument" ), e.getType( ) );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message",
                string( "Unknown repository id: bad" ), string( e.what( ) ) );
    }
}

void WSTest::getTypeDefinitionTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD, false );
    string id( "ComplexType" ); 
    libcmis::ObjectTypePtr actual = session.getType( id );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id", id, actual->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parent type", string( "cmis:document" ), actual->getParentType( )->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:document" ), actual->getBaseType( )->getId( ) );
}

void WSTest::getTypeDefinitionErrorTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD, false );
    
    string id( "bad_type" );
    try
    {
        session.getType( id );
        CPPUNIT_FAIL( "Exception should be raised: invalid ID" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception type", string( "objectNotFound" ), e.getType( ) );
        CPPUNIT_ASSERT_MESSAGE( "Empty exception message", !string( e.what() ).empty( ) );
    }
}

void WSTest::getTypeChildrenTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::ObjectTypePtr actual = session.getType( "cmis:document" );

    vector< libcmis::ObjectTypePtr > children = actual->getChildren( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children imported", size_t( 10 ), children.size() );
}

void WSTest::getObjectTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD, false );
    string id( "101" ); 
    libcmis::ObjectPtr actual = session.getObject( id );

    CPPUNIT_ASSERT_MESSAGE( "getTypeDescription doesn't work: properties are likely to be empty",
            NULL != actual->getTypeDescription( ).get( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id", id, actual->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:folder" ), actual->getBaseType( ) );
    CPPUNIT_ASSERT( 0 != actual->getRefreshTimestamp( ) );
}


CPPUNIT_TEST_SUITE_REGISTRATION( WSTest );
