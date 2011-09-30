#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>

#include "atom-document.hxx"
#include "atom-folder.hxx"
#include "atom-session.hxx"

// InMemory local test server data
#define SERVER_ATOM_URL string( "http://localhost:8080/inmemory/atom" )
#define SERVER_REPOSITORIES_COUNT list< string >::size_type( 1 )
#define SERVER_REPOSITORY string( "A1" )

#define TEST_FOLDER_ID string( "101" )
#define TEST_FOLDER_NAME string( "My_Folder-0-0" )
#define TEST_FOLDER_PATH string( "/My_Folder-0-0" )

#define TEST_DOCUMENT_ID string( "116" )
#define TEST_DOCUMENT_NAME string( "My_Document-1-2" )
#define TEST_DOCUMENT_TYPE string( "text/plain" )

#define TEST_CHILDREN_FOLDER_COUNT 2
#define TEST_CHILDREN_DOCUMENT_COUNT 3
#define TEST_CHILDREN_COUNT vector<libcmis::CmisObjectPtr>::size_type( TEST_CHILDREN_FOLDER_COUNT + TEST_CHILDREN_DOCUMENT_COUNT )

using namespace std;

class AtomTest : public CppUnit::TestFixture
{
    public:

        void getRepositoriesTest( );
        void sessionCreationTest( );
        void getFolderCreationFromUrlTest( );
        void getDocumentCreationFromUrlTest( );
        void getChildrenTest( );

        CPPUNIT_TEST_SUITE( AtomTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( sessionCreationTest );
        CPPUNIT_TEST( getFolderCreationFromUrlTest );
        CPPUNIT_TEST( getDocumentCreationFromUrlTest );
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST_SUITE_END( );
};

void AtomTest::getRepositoriesTest()
{
    list< string > ids = AtomPubSession::getRepositories( SERVER_ATOM_URL );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "One repository should be found", SERVER_REPOSITORIES_COUNT, ids.size() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong repository found", SERVER_REPOSITORY, ids.front() );
}

void AtomTest::sessionCreationTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY );

    // Check for the mandatory collection URLs
    CPPUNIT_ASSERT_MESSAGE( "root collection URL missing",
            !session.getCollectionUrl( Collection::Root ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "types collection URL missing",
            !session.getCollectionUrl( Collection::Types ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "query collection URL missing",
            !session.getCollectionUrl( Collection::Query ).empty() );

    // The optional collection URLs are present on InMemory, so check them
    CPPUNIT_ASSERT_MESSAGE( "checkedout collection URL missing",
            !session.getCollectionUrl( Collection::CheckedOut ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "unfiled collection URL missing",
            !session.getCollectionUrl( Collection::Unfiled ).empty() );

    // Check for the mandatory URI template URLs
    CPPUNIT_ASSERT_MESSAGE( "objectbyid URI template URL missing",
            !session.getUriTemplate( UriTemplate::ObjectById ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "objectbypath URI template URL missing",
            !session.getUriTemplate( UriTemplate::ObjectByPath ).empty() );
    CPPUNIT_ASSERT_MESSAGE( "typebyid URI template URL missing",
            !session.getUriTemplate( UriTemplate::TypeById ).empty() );
    
    // The optional URI template URL is present on InMemory, so check it
    CPPUNIT_ASSERT_MESSAGE( "query URI template URL missing",
            !session.getUriTemplate( UriTemplate::Query ).empty() );

    // Check that the root id is defined
    CPPUNIT_ASSERT_MESSAGE( "Root node ID is missing",
            !session.getRootId().empty() );
}

void AtomTest::getFolderCreationFromUrlTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY );
    libcmis::FolderPtr folder = session.getFolder( TEST_FOLDER_ID );

    AtomFolder* atomFolder = dynamic_cast< AtomFolder* >( folder.get( ) );
    CPPUNIT_ASSERT_MESSAGE( "Created folder should be an instance of AtomFolder",
            NULL != atomFolder );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder ID", TEST_FOLDER_ID, folder->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder name", TEST_FOLDER_NAME, folder->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong folder path", TEST_FOLDER_PATH, folder->getPath( ) );
    CPPUNIT_ASSERT_MESSAGE( "Children URL is missing", !atomFolder->getChildrenUrl( ).empty( ) );
}

void AtomTest::getDocumentCreationFromUrlTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY );
    libcmis::CmisObjectPtr object = session.getObject( TEST_DOCUMENT_ID );

    AtomDocument* atomDocument = dynamic_cast< AtomDocument* >( object.get( ) );
    CPPUNIT_ASSERT_MESSAGE( "Fetched object should be an instance of AtomDocument",
            NULL != atomDocument );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document ID", TEST_DOCUMENT_ID, atomDocument->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document name", TEST_DOCUMENT_NAME, atomDocument->getName( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong document type", TEST_DOCUMENT_TYPE, atomDocument->getContentType( ) );

    // Don't test the exact value... the content is changing at each restart of the InMemory server
    CPPUNIT_ASSERT_MESSAGE( "Content length is missing", 0 < atomDocument->getContentLength( ) );
}

void AtomTest::getChildrenTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY );
    libcmis::FolderPtr folder = session.getRootFolder( );

    vector< libcmis::CmisObjectPtr > children = folder->getChildren( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children", TEST_CHILDREN_COUNT, children.size() );

    int folderCount = 0;
    int documentCount = 0;
    for ( vector< libcmis::CmisObjectPtr >::iterator it = children.begin( );
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

CPPUNIT_TEST_SUITE_REGISTRATION( AtomTest );

int main( int argc, char* argv[] )
{
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest( registry.makeTest() );
    bool wasSuccess = runner.run( "", false );
    return !wasSuccess;
}
