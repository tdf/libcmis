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

#include "document.hxx"
#include "ws-requests.hxx"
#include "ws-session.hxx"
#include "test-helpers.hxx"

#define SERVER_WSDL_URL string( "http://localhost:8080/inmemory/services/RepositoryService" )
#define SERVER_REPOSITORIES_COUNT list< string >::size_type( 1 )
#define SERVER_REPOSITORY string( "A1" )
#define SERVER_USERNAME string( "tester" )
#define SERVER_PASSWORD string( "somepass" )

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
}

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
        void getObjectDocumentTest( );
        void getObjectParentsTest( );
        void getChildrenTest( );
        void getByPathValidTest( );
        void getByPathInvalidTest( );
        void updatePropertiesTest( );
        void createFolderTest( );
        void createFolderBadTypeTest( );
        void createDocumentTest( );
        void deleteObjectTest( );
        void deleteTreeTest( );
        void moveTest( );
        void getContentStreamTest( );
        void setContentStreamTest( );
        void checkOutTest( );
        void cancelCheckOutTest( );
        void checkInTest( );
        void getAllVersionsTest( );


        CPPUNIT_TEST_SUITE( WSTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( sessionCreationTest );
        CPPUNIT_TEST( getRepositoryTest );
        CPPUNIT_TEST( getRepositoryBadTest );
        CPPUNIT_TEST( getTypeDefinitionTest );
        CPPUNIT_TEST( getTypeDefinitionErrorTest );
        CPPUNIT_TEST( getTypeChildrenTest );
        CPPUNIT_TEST( getObjectTest );
        CPPUNIT_TEST( getObjectDocumentTest );
        CPPUNIT_TEST( getObjectParentsTest );
        CPPUNIT_TEST( getChildrenTest );
        CPPUNIT_TEST( getByPathValidTest );
        CPPUNIT_TEST( getByPathInvalidTest );
        CPPUNIT_TEST( updatePropertiesTest );
        CPPUNIT_TEST( createFolderTest );
        CPPUNIT_TEST( createFolderBadTypeTest );
        CPPUNIT_TEST( createDocumentTest );
        CPPUNIT_TEST( deleteObjectTest );
        CPPUNIT_TEST( moveTest );
        CPPUNIT_TEST( getContentStreamTest );
        CPPUNIT_TEST( setContentStreamTest );
        CPPUNIT_TEST( checkOutTest );
        CPPUNIT_TEST( cancelCheckOutTest );
        CPPUNIT_TEST( checkInTest );
        CPPUNIT_TEST( getAllVersionsTest );
        CPPUNIT_TEST_SUITE_END( );
};

void WSTest::getRepositoriesTest()
{
    WSSession session( SERVER_WSDL_URL, "", SERVER_USERNAME, SERVER_PASSWORD );
    vector< libcmis::RepositoryPtr > actual = session.getRepositories( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of repositories", size_t( 1 ), actual.size( ) );
}

void WSTest::sessionCreationTest( )
{
    WSSession session( SERVER_WSDL_URL, "", SERVER_USERNAME, SERVER_PASSWORD );
    CPPUNIT_ASSERT_MESSAGE( "No RepositoryService URL", !session.getServiceUrl( "RepositoryService" ).empty( ) );
}

void WSTest::getRepositoryTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Repository info badly retrieved", string( "100" ), session.getRepository()->getRootId( ) );
}

void WSTest::getRepositoryBadTest( )
{
    WSSession session( SERVER_WSDL_URL, "", SERVER_USERNAME, SERVER_PASSWORD );
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
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    string id( "ComplexType" ); 
    libcmis::ObjectTypePtr actual = session.getType( id );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id", id, actual->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parent type", string( "cmis:document" ), actual->getParentType( )->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:document" ), actual->getBaseType( )->getId( ) );
}

void WSTest::getTypeDefinitionErrorTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    
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
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::ObjectTypePtr actual = session.getType( "cmis:document" );

    vector< libcmis::ObjectTypePtr > children = actual->getChildren( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children imported", size_t( 10 ), children.size() );
}

void WSTest::getObjectTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    string id( "101" ); 
    libcmis::ObjectPtr actual = session.getObject( id );

    CPPUNIT_ASSERT_MESSAGE( "getTypeDescription doesn't work: properties are likely to be empty",
            NULL != actual->getTypeDescription( ).get( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id", id, actual->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:folder" ), actual->getBaseType( ) );
    CPPUNIT_ASSERT_MESSAGE( "Not a libcmis::Folder instance",
            boost::dynamic_pointer_cast< libcmis::Folder >( actual ).get( ) != NULL );
    CPPUNIT_ASSERT( 0 != actual->getRefreshTimestamp( ) );
}

void WSTest::getObjectDocumentTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    string id( "114" ); 
    libcmis::ObjectPtr actual = session.getObject( id );

    CPPUNIT_ASSERT_MESSAGE( "getTypeDescription doesn't work: properties are likely to be empty",
            NULL != actual->getTypeDescription( ).get( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong id", id, actual->getId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong base type", string( "cmis:document" ), actual->getBaseType( ) );
    CPPUNIT_ASSERT_MESSAGE( "Not a libcmis::Document instance",
            boost::dynamic_pointer_cast< libcmis::Document >( actual ).get( ) != NULL );
    CPPUNIT_ASSERT( 0 != actual->getRefreshTimestamp( ) );
}

void WSTest::getObjectParentsTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::ObjectPtr object = session.getObject( "116" );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    
    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );
    vector< libcmis::FolderPtr > actual = document->getParents( );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad number of parents",
           size_t( 1 ), actual.size() );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parent ID",
            string( "101" ), actual.front( )->getId( ) );
}

void WSTest::getChildrenTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::FolderPtr folder = session.getRootFolder( );

    vector< libcmis::ObjectPtr > children = folder->getChildren( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of children", size_t( 5 ), children.size() );

    int folderCount = 0;
    int documentCount = 0;
    for ( vector< libcmis::ObjectPtr >::iterator it = children.begin( );
          it != children.end( ); ++it )
    {
        if ( boost::dynamic_pointer_cast< libcmis::Folder >( *it ).get( ) != NULL )
            ++folderCount;
        else if ( boost::dynamic_pointer_cast< libcmis::Document >( *it ).get( ) != NULL )
            ++documentCount;
    }
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of folder children", 2, folderCount );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of document children", 3, documentCount );
}

void WSTest::getByPathValidTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    try
    {
        libcmis::ObjectPtr object = session.getObjectByPath( "/My_Folder-0-0/My_Document-1-2" );

        CPPUNIT_ASSERT_MESSAGE( "Missing return object", object.get() );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void WSTest::getByPathInvalidTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    try
    {
        libcmis::ObjectPtr object = session.getObjectByPath( "/some/dummy/path" );
        CPPUNIT_FAIL( "Exception should be thrown: invalid Path" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong error type", string( "objectNotFound" ), e.getType() );
        CPPUNIT_ASSERT_MESSAGE( "Empty error message", !string( e.what() ).empty( ) );
    }
}

void WSTest::updatePropertiesTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    
    // Values for the test
    libcmis::ObjectPtr object = session.getObject( "114" );
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

    // Checks
    PropertyPtrMap::iterator propIt = updated->getProperties( ).find( propertyName );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong value after refresh", expectedValue, propIt->second->getStrings().front( ) );
}

void WSTest::createFolderTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::FolderPtr parent = session.getFolder( session.getRootId( ) );

    // Prepare the properties for the new object, object type is cmis:folder
    PropertyPtrMap props;
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

void WSTest::createFolderBadTypeTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::FolderPtr parent = session.getFolder( session.getRootId( ) );

    // Prepare the properties for the new object, object type is cmis:document to trigger the exception
    PropertyPtrMap props;
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
        CPPUNIT_ASSERT_MESSAGE( "Empty exception message", !string( e.what( ) ).empty( ) );
    }
}

void WSTest::createDocumentTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::FolderPtr parent = session.getFolder( session.getRootId( ) );

    // Prepare the properties for the new object, object type is cmis:folder
    PropertyPtrMap props;
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
    string filename( "name.txt" );
    libcmis::DocumentPtr created = parent->createDocument( props, os, contentType, filename );

    // Check that something came back
    CPPUNIT_ASSERT_MESSAGE( "Change token shouldn't be empty: object should have been refreshed",
            !created->getChangeToken( ).empty() );

    // Check that the content is properly set
    boost::shared_ptr< istream >  is = created->getContentStream( );
    string content = lcl_getStreamAsString( is );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong content set", contentStr, content );
}

void WSTest::deleteObjectTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );

    // Get the object to remove
    string id( "130" );
    libcmis::ObjectPtr object = session.getObject( id );

    // Remove the object (method to test)
    object->remove( false );

    // Check that the node doesn't exist anymore
    try
    {
        libcmis::ObjectPtr newObject = session.getObject( id );
        CPPUNIT_FAIL( "Should be removed, exception should have been thrown" );
    }
    catch ( const libcmis::Exception& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception type", string( "objectNotFound" ), e.getType( ) );
    }
}

void WSTest::deleteTreeTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );

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

void WSTest::moveTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    
    string id( "135" );
    libcmis::ObjectPtr object = session.getObject( id );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    CPPUNIT_ASSERT_MESSAGE( "Document to move is missing", document != NULL );

    libcmis::FolderPtr src = document->getParents( ).front( );
    libcmis::FolderPtr dest = session.getFolder( "101" );

    document->move( src, dest );

    vector< libcmis::FolderPtr > parents = document->getParents( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parents size", size_t( 1 ), parents.size( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong parent", string( "101" ), parents.front( )->getId( ) );
}

void WSTest::getContentStreamTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::ObjectPtr object = session.getObject( "116" );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    
    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );

    try
    {
        boost::shared_ptr< istream >  is = document->getContentStream( );
        string content = lcl_getStreamAsString( is );

        CPPUNIT_ASSERT_MESSAGE( "Content stream should be returned", NULL != is.get() );
        CPPUNIT_ASSERT_MESSAGE( "Non-empty content stream should be returned", !content.empty( ) );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void WSTest::setContentStreamTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );
    libcmis::ObjectPtr object = session.getObject( "116" );
    libcmis::Document* document = dynamic_cast< libcmis::Document* >( object.get() );
    
    CPPUNIT_ASSERT_MESSAGE( "Document expected", document != NULL );

    string expectedContent( "Some content to upload" );
    string expectedType( "text/plain" );
    try
    {
        boost::shared_ptr< ostream > os ( new stringstream ( expectedContent ) );
        string filename( "name.txt" );
        document->setContentStream( os, expectedType, filename );
        
        CPPUNIT_ASSERT_MESSAGE( "Object not refreshed during setContentStream", object->getRefreshTimestamp( ) > 0 );

        // Get the new content to check is has been properly uploaded
        boost::shared_ptr< istream > newIs = document->getContentStream( );
        string content = lcl_getStreamAsString( newIs );

        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Bad content uploaded",
                expectedContent, content );
    
        // Testing other values like LastModifiedBy or LastModificationTime
        // is server dependent... don't do it. 
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Node not properly refreshed",
                ( long )expectedContent.size(), document->getContentLength() );
    }
    catch ( const libcmis::Exception& e )
    {
        string msg = "Unexpected exception: ";
        msg += e.what();
        CPPUNIT_FAIL( msg.c_str() );
    }
}

void WSTest::checkOutTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );

    // First create a document of type VersionableType
    libcmis::DocumentPtr doc = test::createVersionableDocument( &session, "checkOutTest" );
    CPPUNIT_ASSERT_MESSAGE( "Failed to create versionable document", doc.get() != NULL );

    libcmis::DocumentPtr pwc = doc->checkOut( );
    
    CPPUNIT_ASSERT_MESSAGE( "Missing returned Private Working Copy", pwc.get( ) != NULL );

    PropertyPtrMap::iterator it = pwc->getProperties( ).find( string( "cmis:isVersionSeriesCheckedOut" ) );
    CPPUNIT_ASSERT_MESSAGE( "cmis:isVersionSeriesCheckedOut property is missing", it != pwc->getProperties( ).end( ) );
    vector< bool > values = it->second->getBools( );
    CPPUNIT_ASSERT_MESSAGE( "cmis:isVersionSeriesCheckedOut isn't true", values.front( ) );
}

void WSTest::cancelCheckOutTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );

    // First create a versionable document and check it out
    libcmis::DocumentPtr doc = test::createVersionableDocument( &session, "cancelCheckOutTest" );
    libcmis::DocumentPtr pwc = doc->checkOut( );

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
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong exception type", string( "objectNotFound" ), e.getType()  );
    }
}

void WSTest::checkInTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );

    // First create a versionable document and check it out
    libcmis::DocumentPtr doc = test::createVersionableDocument( &session, "checkInTest" );
    libcmis::DocumentPtr pwc = doc->checkOut( );

    CPPUNIT_ASSERT_MESSAGE( "Failed to create Private Working Copy document", pwc.get() != NULL );

    // Do the checkin
    bool isMajor = true;
    string comment( "Some check-in comment" );
    PropertyPtrMap properties;
    string newContent = "Some New content to check in";
    boost::shared_ptr< ostream > stream ( new stringstream( newContent ) );
    pwc->checkIn( isMajor, comment, properties, stream, "text/plain", "filename.txt" );

    PropertyPtrMap actualProperties = pwc->getProperties( );

    {
        PropertyPtrMap::iterator it = actualProperties.find( "cmis:isLatestVersion" );
        CPPUNIT_ASSERT_MESSAGE( "cmis:isLatestVersion isn't true", it->second->getBools().front( ) );
    }
    
    {
        PropertyPtrMap::iterator it = actualProperties.find( "cmis:isMajorVersion" );
        CPPUNIT_ASSERT_MESSAGE( "cmis:isMajorVersion isn't true", it->second->getBools().front( ) );
    }

    {
        PropertyPtrMap::iterator it = actualProperties.find( "cmis:checkinComment" );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "cmis:checkinComment doesn't match", comment, it->second->getStrings().front( ) );
    }
}

void WSTest::getAllVersionsTest( )
{
    WSSession session( SERVER_WSDL_URL, "A1", SERVER_USERNAME, SERVER_PASSWORD );

    // First create a versionable document and check it out
    libcmis::DocumentPtr doc = test::createVersionableDocument( &session, "getAllVersionsTest" );
    libcmis::DocumentPtr pwc = doc->checkOut( );

    CPPUNIT_ASSERT_MESSAGE( "Failed to create Private Working Copy document", pwc.get() != NULL );

    // Create a version
    bool isMajor = true;
    string comment( "Some check-in comment" );
    PropertyPtrMap properties;
    string newContent = "Some New content to check in";
    boost::shared_ptr< ostream > stream ( new stringstream( newContent ) );
    libcmis::DocumentPtr newVersion = pwc->checkIn( isMajor, comment, properties, stream, "text/plain", "filename.txt" );

    // Get all the versions (method to check)
    vector< libcmis::DocumentPtr > versions = newVersion->getAllVersions( ); 

    // Checks
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of versions", size_t( 2 ), versions.size( ) );
}

CPPUNIT_TEST_SUITE_REGISTRATION( WSTest );
