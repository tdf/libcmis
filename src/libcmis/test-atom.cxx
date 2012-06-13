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

#define CANCELLED_EXCEPTION_MSG string( "User cancelled authentication request" )

#define TEST_UNEXISTANT_TYPE_ID string( "bad_type" )
#define INVALID_TYPE_ID_EXCEPTION_MSG string( "No such type: bad_type" )

#define TEST_TYPE_ID string( "ComplexType" )
#define PARENT_TYPE_ID string( "cmis:document" )
#define BASE_TYPE_ID string( "cmis:document" )

#define CHILDREN_TEST_TYPE_ID string( "cmis:document" )
#define CHILDREN_TEST_COUNT 10

#define TEST_UNEXISTANT_NODE_ID string( "99" )
#define INVALID_ID_EXCEPTION_MSG string( "No such node: 99" )

#define TEST_FOLDER_ID string( "101" )
#define TEST_FOLDER_NAME string( "My_Folder-0-0" )
#define TEST_FOLDER_PATH string( "/My_Folder-0-0" )

#define TEST_DOCUMENT_ID string( "116" )
#define TEST_DOCUMENT_NAME string( "My_Document-1-2" )
#define TEST_DOCUMENT_TYPE string( "text/plain" )
#define TEST_SAMPLE_CONTENT string( "Some sample text to upload" )
#define TEST_DOCUMENT_PARENTS_COUNT vector< libcmis::FolderPtr >::size_type( 1 )
#define TEST_DOCUMENT_PARENT string( "101" )

#define TEST_CHILDREN_FOLDER_COUNT 2
#define TEST_CHILDREN_DOCUMENT_COUNT 3
#define TEST_CHILDREN_COUNT vector<libcmis::ObjectPtr>::size_type( TEST_CHILDREN_FOLDER_COUNT + TEST_CHILDREN_DOCUMENT_COUNT )

#define TEST_PATH_VALID string( "/My_Folder-0-0/My_Document-1-2" )
#define TEST_PATH_INVALID string( "/some/dummy/path" )
#define INVALID_PATH_EXCEPTION_MSG string( "No node corresponding to path: /some/dummy/path" )

#define TEST_UPDATE_DOCUMENT_ID string( "114" )
#define TEST_UPDATED_PROPERTY_NAME string( "cmis:name" )
#define TEST_UPDATED_PROPERTY_VALUE string( "New name" )

using namespace boost;
using namespace std;

class AtomTest : public CppUnit::TestFixture
{
    public:

        // Generic sesssion factory tests

        void getRepositoriesTest( );
        void sessionCreationTest( );
        void authCallbackTest( );

        // Types fetching tests
        void getUnexistantTypeTest( );
        void getNormalTypeTest( );
        void getTypeChildrenTest( );

        // Node fetching tests
        void getFolderCreationFromUrlTest( );
        void getUnexistantFolderTest( );
        void getUnexistantObjectTest( );
        void getFolderFromOtherNodeTest( );
        void getDocumentCreationFromUrlTest( );
        void getByPathValidTest( );
        void getByPathInvalidTest( );

        // Node operations tests

        void getAllowableActionsTest( );
        void getChildrenTest( );
        void getObjectParentsTest( );
        void getContentStreamTest( );
        void setContentStreamTest( );
        void updatePropertiesTest( );
        void createFolderTest( );
        void createFolderBadTypeTest( );
        void dumpDocumentToXmlTest( );
        void createDocumentTest( );
        void deleteDocumentTest( );
        void deleteTreeTest( );
        void checkOutTest( );
        void cancelCheckOutTest( );
        void checkInTest( );

        CPPUNIT_TEST_SUITE( AtomTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( sessionCreationTest );
        CPPUNIT_TEST( authCallbackTest );
        CPPUNIT_TEST( getUnexistantTypeTest );
        CPPUNIT_TEST( getNormalTypeTest );
        CPPUNIT_TEST( getTypeChildrenTest );
        CPPUNIT_TEST( getFolderCreationFromUrlTest );
        CPPUNIT_TEST( getUnexistantFolderTest );
        CPPUNIT_TEST( getUnexistantObjectTest );
        CPPUNIT_TEST( getFolderFromOtherNodeTest );
        CPPUNIT_TEST( getDocumentCreationFromUrlTest );
        CPPUNIT_TEST( getByPathValidTest );
        CPPUNIT_TEST( getByPathInvalidTest );
        CPPUNIT_TEST( getAllowableActionsTest );
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST( getObjectParentsTest );
        CPPUNIT_TEST( getContentStreamTest );
        CPPUNIT_TEST( setContentStreamTest );
        CPPUNIT_TEST( updatePropertiesTest );
        CPPUNIT_TEST( createFolderTest );
        CPPUNIT_TEST( createFolderBadTypeTest );
        CPPUNIT_TEST( createDocumentTest );
        CPPUNIT_TEST( deleteDocumentTest );
        CPPUNIT_TEST( deleteTreeTest );
        CPPUNIT_TEST( checkOutTest );
        CPPUNIT_TEST( cancelCheckOutTest );
        CPPUNIT_TEST( checkInTest );
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
    CPPUNIT_ASSERT_MESSAGE( "Root folder isn't considering itself a root",
            session.getRootFolder( )->isRootFolder() );
}

class TestAuthProvider : public libcmis::AuthProvider
{
    bool m_fail;

    public:
        TestAuthProvider( bool fail ) : m_fail( fail ) { }

        bool authenticationQuery( std::string& username, std::string& password )
        {
            password = SERVER_PASSWORD;
            return !m_fail;
        }
};

void AtomTest::authCallbackTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, string( ), false);

    // Test cancelled authentication
    {
        libcmis::AuthProviderPtr authProvider( new TestAuthProvider( true ) );
        session.setAuthenticationProvider( authProvider );
        try
        {
            session.getRootFolder( );
            CPPUNIT_FAIL( "Should raise an exception saying the user cancelled the authentication" );
        }
        catch ( const libcmis::Exception& e )
        {
            CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", CANCELLED_EXCEPTION_MSG , string( e.what() ) );
        }
    }
    
    // Test provided authentication
    {
        libcmis::AuthProviderPtr authProvider( new TestAuthProvider( false ) );
        session.setAuthenticationProvider( authProvider );
        session.getRootFolder( );
    }
}

void AtomTest::getUnexistantTypeTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    
    try
    {
        session.getType( TEST_UNEXISTANT_TYPE_ID );
        CPPUNIT_FAIL( "Exception should be raised: invalid ID" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", INVALID_TYPE_ID_EXCEPTION_MSG , string( e.what() ) );
    }
}

void AtomTest::getNormalTypeTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    
    libcmis::ObjectTypePtr type = session.getType( TEST_TYPE_ID );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parent type", PARENT_TYPE_ID, type->getParentType( )->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", BASE_TYPE_ID, type->getParentType( )->getId( ) );

}

void AtomTest::getTypeChildrenTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    
    libcmis::ObjectTypePtr type = session.getType( CHILDREN_TEST_TYPE_ID );
    vector< libcmis::ObjectTypePtr > children = type->getChildren( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children imported", CHILDREN_TEST_COUNT, int( children.size() ) );
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
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:folder" ), atomFolder->getBaseType( ) );
    CPPUNIT_ASSERT_MESSAGE( "Missing folder parent", atomFolder->getFolderParent( ).get( ) );
    CPPUNIT_ASSERT_MESSAGE( "Not a root folder", !atomFolder->isRootFolder() );

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
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:document" ), atomDocument->getBaseType( ) );

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
        
void AtomTest::getAllowableActionsTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::FolderPtr folder = session.getRootFolder( );

    boost::shared_ptr< libcmis::AllowableActions > toCheck = folder->getAllowableActions( );
    CPPUNIT_ASSERT_MESSAGE( "ApplyACL allowable action not defined... are all the actions read?",
            toCheck->isDefined( libcmis::ObjectAction::ApplyACL ) );

    CPPUNIT_ASSERT_MESSAGE( "GetChildren allowable action should be true",
            toCheck->isDefined( libcmis::ObjectAction::GetChildren ) &&
            toCheck->isAllowed( libcmis::ObjectAction::GetChildren ) );
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

void AtomTest::getObjectParentsTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::ObjectPtr object = session.getObject( TEST_DOCUMENT_ID );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    
    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );
    vector< libcmis::FolderPtr > actual = document->getParents( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad number of parents",
           TEST_DOCUMENT_PARENTS_COUNT, actual.size() );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parent ID",
            TEST_DOCUMENT_PARENT, actual.front( )->getId( ) );
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
        boost::shared_ptr< ostream > os ( new stringstream ( TEST_SAMPLE_CONTENT ) );
        document->setContentStream( os, TEST_DOCUMENT_TYPE );
        
        CPPUNIT_ASSERT_MESSAGE( "Object not refreshed during setContentStream", object->getRefreshTimestamp( ) > 0 );

        // Get the new content to check is has been properly uploaded
        shared_ptr< istream > newIs = document->getContentStream( );
        stringstream is;
        is << newIs->rdbuf();
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad content uploaded",
                TEST_SAMPLE_CONTENT, is.str() );
    
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

void AtomTest::updatePropertiesTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::ObjectPtr object = session.getObject( TEST_UPDATE_DOCUMENT_ID );

    map< string, libcmis::PropertyPtr >::iterator it = object->getProperties( ).find( TEST_UPDATED_PROPERTY_NAME );
    CPPUNIT_ASSERT_MESSAGE( "Property to change not found", it != object->getProperties( ).end( ) );

    vector< string > values;
    values.push_back( TEST_UPDATED_PROPERTY_VALUE );
    it->second->setValues( values );

    object->updateProperties( );

    it = object->getProperties( ).find( TEST_UPDATED_PROPERTY_NAME );
    CPPUNIT_ASSERT_MESSAGE( "Property to check not found", it != object->getProperties( ).end( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong value after refresh", TEST_UPDATED_PROPERTY_VALUE, it->second->getStrings().front( ) );
}

void AtomTest::createFolderTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::FolderPtr parent = session.getFolder( session.getRootId( ) );

    // Prepare the properties for the new object, object type is cmis:folder
    map< string, libcmis::PropertyPtr > props;
    libcmis::ObjectTypePtr type = session.getType( "cmis:folder" );
    map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

    // Set the object name
    map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:name property type not found on parent type", it != propTypes.end( ) );
    vector< string > nameValues;
    nameValues.push_back( "createFolderTest" );
    libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );
   
    // set the object type 
    it = propTypes.find( string( "cmis:objectTypeId" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:objectTypeId property type not found on parent type", it != propTypes.end( ) );
    vector< string > typeValues;
    typeValues.push_back( "cmis:folder" );
    libcmis::PropertyPtr typeProperty( new libcmis::Property( it->second, typeValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeProperty ) );

    // Actually send the folder creation request
    libcmis::FolderPtr created = parent->createFolder( props );

    // Check that something came back
    CPPUNIT_ASSERT_MESSAGE( "Change token shouldn't be empty: object should have been refreshed",
            !created->getChangeToken( ).empty() );
}

void AtomTest::createFolderBadTypeTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::FolderPtr parent = session.getFolder( session.getRootId( ) );

    // Prepare the properties for the new object, object type is cmis:document to trigger the exception
    map< string, libcmis::PropertyPtr > props;
    libcmis::ObjectTypePtr type = session.getType( "cmis:document" );
    map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

    // Set the object name
    map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:name property type not found on parent type", it != propTypes.end( ) );
    vector< string > nameValues;
    nameValues.push_back( "createFolderBadTypeTest" );
    libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );

    // Set the object type 
    it = propTypes.find( string( "cmis:objectTypeId" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:objectTypeId property type not found on parent type", it != propTypes.end( ) );
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
        CPPUNIT_ASSERT_MESSAGE( "Bad exception message",
                string( e.what( ) ).find( "Created object is not a folder: " ) != string::npos );
    }
}

void AtomTest::createDocumentTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );
    libcmis::FolderPtr parent = session.getFolder( session.getRootId( ) );

    // Prepare the properties for the new object, object type is cmis:folder
    map< string, libcmis::PropertyPtr > props;
    libcmis::ObjectTypePtr type = session.getType( "cmis:document" );
    map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

    // Set the object name
    map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:name property type not found on parent type", it != propTypes.end( ) );
    vector< string > nameValues;
    nameValues.push_back( "createDocumentTest" );
    libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );
   
    // set the object type 
    it = propTypes.find( string( "cmis:objectTypeId" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:objectTypeId property type not found on parent type", it != propTypes.end( ) );
    vector< string > typeValues;
    typeValues.push_back( "cmis:document" );
    libcmis::PropertyPtr typeProperty( new libcmis::Property( it->second, typeValues ) );
    props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeProperty ) );

    // Actually send the document creation request
    string contentStr = "Some content";
    boost::shared_ptr< ostream > os ( new stringstream( contentStr ) );
    string contentType = "text/plain";
    libcmis::DocumentPtr created = parent->createDocument( props, os, contentType );

    // Check that something came back
    CPPUNIT_ASSERT_MESSAGE( "Change token shouldn't be empty: object should have been refreshed",
            !created->getChangeToken( ).empty() );

    // Check that the content is properly set
    shared_ptr< istream >  is = created->getContentStream( );
    stringstream buf;
    buf << is->rdbuf();
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong content set", contentStr, buf.str( ) );
}

void AtomTest::deleteDocumentTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );

    string id( "130" );
    libcmis::ObjectPtr object = session.getObject( id );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    CPPUNIT_ASSERT_MESSAGE( "Document to remove is missing", document != NULL );

    document->remove( );

    try
    {
        libcmis::ObjectPtr newObject = session.getObject( id );
        CPPUNIT_FAIL( "Should be removed, exception should have been thrown" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", string( "No such node: " + id ) , string( e.what() ) );
    }
}

void AtomTest::deleteTreeTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );

    string id( "117" );
    libcmis::ObjectPtr object = session.getObject( id );
    libcmis::Folder* folder = dynamic_cast< libcmis::Folder* >( object.get() );
    CPPUNIT_ASSERT_MESSAGE( "Document to remove is missing", folder != NULL );

    folder->removeTree( );

    try
    {
        libcmis::ObjectPtr newObject = session.getObject( id );
        CPPUNIT_FAIL( "Should be removed, exception should have been thrown" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", string( "No such node: " + id ) , string( e.what() ) );
    }
}

void AtomTest::checkOutTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );

    // First create a document of type VersionableType
    libcmis::DocumentPtr doc;
    {
        libcmis::FolderPtr parent = session.getRootFolder( );

        // Prepare the properties for the new object, object type is cmis:folder
        map< string, libcmis::PropertyPtr > props;
        libcmis::ObjectTypePtr type = session.getType( "VersionableType" );
        map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

        // Set the object name
        map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
        vector< string > nameValues;
        nameValues.push_back( "checkOutTest" );
        libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
        props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );
       
        // set the object type 
        it = propTypes.find( string( "cmis:objectTypeId" ) );
        vector< string > typeValues;
        typeValues.push_back( "VersionableType" );
        libcmis::PropertyPtr typeProperty( new libcmis::Property( it->second, typeValues ) );
        props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeProperty ) );

        // Actually send the document creation request
        string contentStr = "Some content";
        boost::shared_ptr< ostream > os ( new stringstream( contentStr ) );
        string contentType = "text/plain";
        doc = parent->createDocument( props, os, contentType );
    }

    CPPUNIT_ASSERT_MESSAGE( "Failed to create versionable document", doc.get() != NULL );

    libcmis::DocumentPtr pwc = doc->checkOut( );
    
    CPPUNIT_ASSERT_MESSAGE( "Missing returned Private Working Copy", pwc.get( ) != NULL );

    map< string, libcmis::PropertyPtr >::iterator it = pwc->getProperties( ).find( string( "cmis:isVersionSeriesCheckedOut" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:isVersionSeriesCheckedOut property is missing", it != pwc->getProperties( ).end( ) );
    vector< bool > values = it->second->getBools( );
    CPPUNIT_ASSERT_MESSAGE( "cmis:isVersionSeriesCheckedOut isn't true", values.front( ) );
}

void AtomTest::cancelCheckOutTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );

    // First create a versionable document and check it out
    libcmis::DocumentPtr pwc;
    {
        libcmis::FolderPtr parent = session.getRootFolder( );

        // Prepare the properties for the new object, object type is cmis:folder
        map< string, libcmis::PropertyPtr > props;
        libcmis::ObjectTypePtr type = session.getType( "VersionableType" );
        map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

        // Set the object name
        map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
        vector< string > nameValues;
        nameValues.push_back( "cancelCheckOutTest" );
        libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
        props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );
       
        // set the object type 
        it = propTypes.find( string( "cmis:objectTypeId" ) );
        vector< string > typeValues;
        typeValues.push_back( "VersionableType" );
        libcmis::PropertyPtr typeProperty( new libcmis::Property( it->second, typeValues ) );
        props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeProperty ) );

        // Actually send the document creation request
        string contentStr = "Some content";
        boost::shared_ptr< ostream > os ( new stringstream( contentStr ) );
        string contentType = "text/plain";
        libcmis::DocumentPtr doc = parent->createDocument( props, os, contentType );
    
        pwc = doc->checkOut( );
    }

    CPPUNIT_ASSERT_MESSAGE( "Failed to create Private Working Copy document", pwc.get() != NULL );

    string id = pwc->getId( );
    pwc->cancelCheckout( );

    // Check that the PWC doesn't exist any more
    try
    {
        session.getObject( id );
        CPPUNIT_FAIL( "Private Working Copy object should no longer exist" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception message", string( "No such node: " ) + id, string( e.what() ) );
    }
}

void AtomTest::checkInTest( )
{
    AtomPubSession session( SERVER_ATOM_URL, SERVER_REPOSITORY, SERVER_USERNAME, SERVER_PASSWORD, false );

    // First create a versionable document and check it out
    libcmis::DocumentPtr pwc;
    {
        libcmis::FolderPtr parent = session.getRootFolder( );

        // Prepare the properties for the new object, object type is cmis:folder
        map< string, libcmis::PropertyPtr > props;
        libcmis::ObjectTypePtr type = session.getType( "VersionableType" );
        map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

        // Set the object name
        map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
        vector< string > nameValues;
        nameValues.push_back( "checkInTest" );
        libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
        props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );
       
        // set the object type 
        it = propTypes.find( string( "cmis:objectTypeId" ) );
        vector< string > typeValues;
        typeValues.push_back( "VersionableType" );
        libcmis::PropertyPtr typeProperty( new libcmis::Property( it->second, typeValues ) );
        props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeProperty ) );

        // Actually send the document creation request
        string contentStr = "Some content";
        boost::shared_ptr< ostream > os ( new stringstream( contentStr ) );
        string contentType = "text/plain";
        libcmis::DocumentPtr doc = parent->createDocument( props, os, contentType );
    
        pwc = doc->checkOut( );
    }

    CPPUNIT_ASSERT_MESSAGE( "Failed to create Private Working Copy document", pwc.get() != NULL );

    // Do the checkin
    bool isMajor = true;
    string comment( "Some check-in comment" );
    map< string, libcmis::PropertyPtr > properties;
    string newContent = "Some New content to check in";
    boost::shared_ptr< ostream > stream ( new stringstream( newContent ) );
    pwc->checkIn( isMajor, comment, properties, stream, "text/plain" );

    map< string, libcmis::PropertyPtr > actualProperties = pwc->getProperties( );

    {
        map< string, libcmis::PropertyPtr >::iterator it = actualProperties.find( "cmis:isLatestVersion" );
        CPPUNIT_ASSERT_MESSAGE( "cmis:isLatestVersion isn't true", it->second->getBools().front( ) );
    }
    
    {
        map< string, libcmis::PropertyPtr >::iterator it = actualProperties.find( "cmis:isMajorVersion" );
        CPPUNIT_ASSERT_MESSAGE( "cmis:isMajorVersion isn't true", it->second->getBools().front( ) );
    }

    {
        map< string, libcmis::PropertyPtr >::iterator it = actualProperties.find( "cmis:checkinComment" );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "cmis:checkinComment doesn't match", comment, it->second->getStrings().front( ) );
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
