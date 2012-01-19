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

#include <ctime>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>

#include "atom-document.hxx"
#include "atom-folder.hxx"
#include "atom-session.hxx"
#include "atom-utils.hxx"

// InMemory local test server data
#define SERVER_ATOM_URL string( "http://localhost:8080/inmemory/atom" )
#define SERVER_REPOSITORIES_COUNT list< string >::size_type( 1 )
#define SERVER_REPOSITORY string( "A1" )
#define SERVER_USERNAME string( "tester" )
#define SERVER_PASSWORD string( "somepass" )

#define TEST_UNEXISTANT_NODE_ID string( "99" )
#define INVALID_ID_EXCEPTION_MSG string( "No such node: 99" )

#define TEST_FOLDER_ID string( "101" )
#define TEST_FOLDER_NAME string( "My_Folder-0-0" )
#define TEST_FOLDER_PATH string( "/My_Folder-0-0" )

#define TEST_DOCUMENT_ID string( "116" )
#define TEST_DOCUMENT_NAME string( "My_Document-1-2" )
#define TEST_DOCUMENT_TYPE string( "text/plain" )
#define TEST_SAMPLE_CONTENT string( "Some sample text to upload" )
#define TEST_SAMPLE_MIME_TYPE string( "plain/text" )

#define TEST_CHILDREN_FOLDER_COUNT 2
#define TEST_CHILDREN_DOCUMENT_COUNT 3
#define TEST_CHILDREN_COUNT vector<libcmis::ObjectPtr>::size_type( TEST_CHILDREN_FOLDER_COUNT + TEST_CHILDREN_DOCUMENT_COUNT )

#define TEST_PATH_VALID string( "/My_Folder-0-0/My_Document-1-2" )
#define TEST_PATH_INVALID string( "/some/dummy/path" )
#define INVALID_PATH_EXCEPTION_MSG string( "No node corresponding to path: /some/dummy/path" )

using namespace boost;
using namespace std;

class AtomTest : public CppUnit::TestFixture
{
    public:

        // Generic sesssion factory tests

        void getRepositoriesTest( );
        void sessionCreationTest( );

        // Node fetching tests

        void getFolderCreationFromUrlTest( );
        void getUnexistantFolderTest( );
        void getUnexistantObjectTest( );
        void getFolderFromOtherNodeTest( );
        void getDocumentCreationFromUrlTest( );
        void getByPathValidTest( );
        void getByPathInvalidTest( );

        // Node operations tests

        void getChildrenTest( );
        void getContentTest( );
        void getContentStreamTest( );
        void setContentStreamTest( );

        // Other useful tests.

        void parseDateTimeTest( );

        CPPUNIT_TEST_SUITE( AtomTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( sessionCreationTest );
        CPPUNIT_TEST( getFolderCreationFromUrlTest );
        CPPUNIT_TEST( getUnexistantFolderTest );
        CPPUNIT_TEST( getUnexistantObjectTest );
        CPPUNIT_TEST( getFolderFromOtherNodeTest );
        CPPUNIT_TEST( getDocumentCreationFromUrlTest );
        CPPUNIT_TEST( getByPathValidTest );
        CPPUNIT_TEST( getByPathInvalidTest );
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST( getContentTest );
        CPPUNIT_TEST( getContentStreamTest );
        CPPUNIT_TEST( setContentStreamTest );
        CPPUNIT_TEST( parseDateTimeTest );
        CPPUNIT_TEST_SUITE_END( );
};

void AtomTest::getRepositoriesTest()
{
    list< string > ids = AtomPubSession::getRepositories( SERVER_ATOM_URL, SERVER_USERNAME, SERVER_PASSWORD );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "One repository should be found", SERVER_REPOSITORIES_COUNT, ids.size() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong repository found", SERVER_REPOSITORY, ids.front() );
}

void AtomTest::sessionCreationTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );

    // Check for the mandatory collection URLs
    CPPUNIT_ASSERT_MESSAGE( "root collection URL missing",
            !session.getWorkspace().getCollectionUrl( atom::Collection::Root ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "types collection URL missing",
            !session.getWorkspace().getCollectionUrl( atom::Collection::Types ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "query collection URL missing",
            !session.getWorkspace().getCollectionUrl( atom::Collection::Query ).empty() );

    // The optional collection URLs are present on InMemory, so check them
    CPPUNIT_ASSERT_MESSAGE( "checkedout collection URL missing",
            !session.getWorkspace().getCollectionUrl( atom::Collection::CheckedOut ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "unfiled collection URL missing",
            !session.getWorkspace().getCollectionUrl( atom::Collection::Unfiled ).empty() );

    // Check for the mandatory URI template URLs
    CPPUNIT_ASSERT_MESSAGE( "objectbyid URI template URL missing",
            !session.getWorkspace().getUriTemplate( atom::UriTemplate::ObjectById ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "objectbypath URI template URL missing",
            !session.getWorkspace().getUriTemplate( atom::UriTemplate::ObjectByPath ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "typebyid URI template URL missing",
            !session.getWorkspace().getUriTemplate( atom::UriTemplate::TypeById ).empty() );
    
    // The optional URI template URL is present on InMemory, so check it
    CPPUNIT_ASSERT_MESSAGE( "query URI template URL missing",
            !session.getWorkspace().getUriTemplate( atom::UriTemplate::Query ).empty() );

    // Check that the root id is defined
    CPPUNIT_ASSERT_MESSAGE( "Root node ID is missing",
            !session.getRootId().empty() );
}

void AtomTest::getUnexistantFolderTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );

    try
    {
        session.getFolder( TEST_UNEXISTANT_NODE_ID );
        CPPUNIT_FAIL( "Exception should be raised: invalid ID" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", INVALID_ID_EXCEPTION_MSG , string( e.what() ) );
    }
}

void AtomTest::getUnexistantObjectTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    
    try
    {
        session.getObject( TEST_UNEXISTANT_NODE_ID );
        CPPUNIT_FAIL( "Exception should be raised: invalid ID" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", INVALID_ID_EXCEPTION_MSG , string( e.what() ) );
    }
}

void AtomTest::getFolderFromOtherNodeTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::FolderPtr folder = session.getFolder( TEST_DOCUMENT_ID );

    CPPUNIT_ASSERT_MESSAGE( "Nothing should be returned: not a folder",
            NULL == folder.get( ) );
}

void AtomTest::getFolderCreationFromUrlTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::FolderPtr folder = session.getFolder( TEST_FOLDER_ID );

    AtomFolder* atomFolder = dynamic_cast< AtomFolder* >( folder.get( ) );
    CPPUNIT_ASSERT_MESSAGE( "Created folder should be an instance of AtomFolder",
            NULL != atomFolder );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder ID", TEST_FOLDER_ID, folder->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder name", TEST_FOLDER_NAME, folder->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder path", TEST_FOLDER_PATH, folder->getPath( ) );
    CPPUNIT_ASSERT_MESSAGE( "Children URL is missing", !atomFolder->getChildrenUrl( ).empty( ) );

    CPPUNIT_ASSERT_MESSAGE( "CreatedBy is missing", !atomFolder->getCreatedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "CreationDate is missing", !atomFolder->getCreationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "LastModifiedBy is missing", !atomFolder->getLastModifiedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "LastModificationDate is missing", !atomFolder->getLastModificationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "ChangeToken is missing", !atomFolder->getChangeToken( ).empty( ) );
}

void AtomTest::getDocumentCreationFromUrlTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::ObjectPtr object = session.getObject( TEST_DOCUMENT_ID );

    AtomDocument* atomDocument = dynamic_cast< AtomDocument* >( object.get( ) );
    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of AtomDocument",
            NULL != atomDocument );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document ID", TEST_DOCUMENT_ID, atomDocument->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document name", TEST_DOCUMENT_NAME, atomDocument->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document type", TEST_DOCUMENT_TYPE, atomDocument->getContentType( ) );

    CPPUNIT_ASSERT_MESSAGE( "CreatedBy is missing", !atomDocument->getCreatedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "CreationDate is missing", !atomDocument->getCreationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "LastModifiedBy is missing", !atomDocument->getLastModifiedBy( ).empty( ) );
    CPPUNIT_ASSERT_MESSAGE( "LastModificationDate is missing", !atomDocument->getLastModificationDate( ).is_not_a_date_time() );
    CPPUNIT_ASSERT_MESSAGE( "ChangeToken is missing", !atomDocument->getChangeToken( ).empty( ) );

    // Don't test the exact value... the content is changing at each restart of the InMemory server
    CPPUNIT_ASSERT_MESSAGE( "Content length is missing", 0 < atomDocument->getContentLength( ) );
}

void AtomTest::getByPathValidTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    try
    {
        libcmis::ObjectPtr object = session.getObjectByPath( TEST_PATH_VALID );

        CPPUNIT_ASSERT_MESSAGE( "Missing return object", object.get() );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void AtomTest::getByPathInvalidTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    try
    {
        libcmis::ObjectPtr object = session.getObjectByPath( TEST_PATH_INVALID );
        CPPUNIT_FAIL( "Exception should be thrown: invalid Path" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", INVALID_PATH_EXCEPTION_MSG , string( e.what() ) );
    }
}

void AtomTest::getChildrenTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::FolderPtr folder = session.getRootFolder( );

    vector< libcmis::ObjectPtr > children = folder->getChildren( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children", TEST_CHILDREN_COUNT, children.size() );

    int folderCount = 0;
    int documentCount = 0;
    for ( vector< libcmis::ObjectPtr >::iterator it = children.begin( );
          it != children.end( ); it++ )
    {
        if ( NULL != dynamic_cast< AtomFolder* >( it->get() ) )
            ++folderCount;
        else if ( NULL != dynamic_cast< AtomDocument* >( it->get() ) )
            ++documentCount;
    }
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of folder children",
            TEST_CHILDREN_FOLDER_COUNT, folderCount );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of document children",
            TEST_CHILDREN_DOCUMENT_COUNT, documentCount );
}

void AtomTest::getContentTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::ObjectPtr object = session.getObject( TEST_DOCUMENT_ID );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    
    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );

    FILE* fd = document->getContent( );
    CPPUNIT_ASSERT_MESSAGE( "Temporary file with content should be returned", NULL != fd );
}

void AtomTest::getContentStreamTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::ObjectPtr object = session.getObject( TEST_DOCUMENT_ID );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    
    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );

    try
    {
        shared_ptr< istream >  is = document->getContentStream( );
        CPPUNIT_ASSERT_MESSAGE( "Content stream should be returned", NULL != is.get() );
        CPPUNIT_ASSERT_MESSAGE( "Non-empty content stream should be returned", is->good() && !is->eof() );
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
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::ObjectPtr object = session.getObject( TEST_DOCUMENT_ID );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    
    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );

    try
    {
        stringstream is( TEST_SAMPLE_CONTENT );
        document->setContentStream( is, TEST_SAMPLE_MIME_TYPE );

        // Get the new content to check is has been properly uploaded
        shared_ptr< istream > newIs = document->getContentStream( );
        stringstream os;
        os << newIs->rdbuf();
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad content uploaded",
                TEST_SAMPLE_CONTENT, os.str() );
       
        // Testing other values like LastModifiedBy or LastModificationTime
        // is server dependent... don't do it. 
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Node not properly refreshed",
                ( long )TEST_SAMPLE_CONTENT.size(), document->getContentLength() );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void AtomTest::parseDateTimeTest( )
{
    tm basis;
    basis.tm_year = 2011 - 1900;
    basis.tm_mon = 8; // Months are in 0..11 range
    basis.tm_mday = 28;
    basis.tm_hour = 12;
    basis.tm_min = 44;
    basis.tm_sec = 28;

    // No time zone test
    {
        char toParse[50];
        strftime( toParse, sizeof( toParse ), "%FT%T", &basis );
        posix_time::ptime t = atom::parseDateTime( string( toParse ) );

        gregorian::date expDate( basis.tm_year + 1900, basis.tm_mon + 1, basis.tm_mday );
        posix_time::time_duration expTime( basis.tm_hour, basis.tm_min, basis.tm_sec );
        posix_time::ptime expected( expDate, expTime );

        CPPUNIT_ASSERT_EQUAL_MESSAGE( "No time zone case failed", expected, t );
    }
    
    // Z time zone test
    {
        char toParse[50];
        strftime( toParse, sizeof( toParse ), "%FT%TZ", &basis );
        posix_time::ptime t = atom::parseDateTime( string( toParse ) );
        
        gregorian::date expDate( basis.tm_year + 1900, basis.tm_mon + 1, basis.tm_mday );
        posix_time::time_duration expTime( basis.tm_hour, basis.tm_min, basis.tm_sec );
        posix_time::ptime expected( expDate, expTime );

        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Z time zone case failed", expected, t );
    }

    // +XX:XX time zone test
    {
        char toParse[50];
        strftime( toParse, sizeof( toParse ), "%FT%T+02:00", &basis );
        posix_time::ptime t = atom::parseDateTime( string( toParse ) );
        
        gregorian::date expDate( basis.tm_year + 1900, basis.tm_mon + 1, basis.tm_mday );
        posix_time::time_duration expTime( basis.tm_hour + 2, basis.tm_min, basis.tm_sec );
        posix_time::ptime expected( expDate, expTime );

        CPPUNIT_ASSERT_EQUAL_MESSAGE( "+XX:XX time zone case failed", expected, t );
    }

    // -XX:XX time zone test
    {
        char toParse[50];
        strftime( toParse, sizeof( toParse ), "%FT%T-02:00", &basis );
        posix_time::ptime t = atom::parseDateTime( string( toParse ) );
        
        gregorian::date expDate( basis.tm_year + 1900, basis.tm_mon + 1, basis.tm_mday );
        posix_time::time_duration expTime( basis.tm_hour - 2, basis.tm_min, basis.tm_sec );
        posix_time::ptime expected( expDate, expTime );

        CPPUNIT_ASSERT_EQUAL_MESSAGE( "+XX:XX time zone case failed", expected, t );
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION( AtomTest );

int main( int argc, char* argv[] )
{
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    bool wasSuccess = runner.run( "", false );
    return !wasSuccess;
}
