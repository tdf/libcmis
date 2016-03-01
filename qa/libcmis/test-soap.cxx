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

#define private public

#include "ws-relatedmultipart.hxx"
#include "ws-requests.hxx"
#include "ws-soap.hxx"
#include "test-helpers.hxx"

using namespace std;

class SoapTest : public CppUnit::TestFixture
{
    private:
        map< string, SoapResponseCreator > getTestMapping( );
        map< string, string > getTestNamespaces( );
        map< string, SoapFaultDetailCreator > getTestDetailMapping( );

    public:

        // Copy tests
        void soapResponseFactoryCopyTest();

        // Soap Responses tests

        void createResponseTest( );
        void parseFaultDetailEmptyTest( );
        void parseFaultDetailUnknownTest( );
        void parseFaultDetailValidTest( );
        void createFaultDefaultTest( );
        void parseResponseTest( );
        void parseResponseXmlTest( );
        void parseResponseFaultTest( );

        // RelatedMultipart tests

        void serializeMultipartSimpleTest( );
        void serializeMultipartComplexTest( );
        void parseMultipartTest( );
        void getStreamFromNodeXopTest( );
        void getStreamFromNodeBase64Test( );

        // CMISM utilities tests
        void writeCmismStreamTest( );

        CPPUNIT_TEST_SUITE( SoapTest );
        CPPUNIT_TEST( soapResponseFactoryCopyTest );

        CPPUNIT_TEST( createResponseTest );
        CPPUNIT_TEST( parseFaultDetailEmptyTest );
        CPPUNIT_TEST( parseFaultDetailUnknownTest );
        CPPUNIT_TEST( parseFaultDetailValidTest );
        CPPUNIT_TEST( parseResponseTest );
        CPPUNIT_TEST( parseResponseXmlTest );
        CPPUNIT_TEST( parseResponseFaultTest );

        CPPUNIT_TEST( serializeMultipartSimpleTest );
        CPPUNIT_TEST( serializeMultipartComplexTest );
        CPPUNIT_TEST( parseMultipartTest );
        CPPUNIT_TEST( getStreamFromNodeXopTest );
        CPPUNIT_TEST( getStreamFromNodeBase64Test );

        CPPUNIT_TEST( writeCmismStreamTest );

        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( SoapTest );

/** Dummy response class to use for testing
  */
class TestResponse : public SoapResponse
{
    private:
        TestResponse( ) { };

    public:

        static SoapResponsePtr create( xmlNodePtr, RelatedMultipart&, SoapSession* )
        {
            SoapResponsePtr resp ( new TestResponse( ) );
            return resp;
        }
};

class TestFaultDetail : public SoapFaultDetail
{
    private:
        TestFaultDetail( ) : SoapFaultDetail( ) { };

    public:
        ~TestFaultDetail( ) throw ( ) { };

        static SoapFaultDetailPtr create( xmlNodePtr )
        {
            return SoapFaultDetailPtr( new TestFaultDetail( ) );
        }
};

map< string, SoapResponseCreator > SoapTest::getTestMapping( )
{
    map< string, SoapResponseCreator > mapping;
    mapping[ "{test-ns-url}testResponse" ] = &TestResponse::create;
    return mapping;
}

map< string, SoapFaultDetailCreator > SoapTest::getTestDetailMapping( )
{
    map< string, SoapFaultDetailCreator > mapping;
    mapping[ "{test-ns-url}testFault" ] = &TestFaultDetail::create;
    return mapping;
}

map< string, string > SoapTest::getTestNamespaces( )
{
    map< string, string > namespaces;
    namespaces[ "test" ] = "test-ns-url";
    return namespaces;
}

void SoapTest::soapResponseFactoryCopyTest( )
{
    SoapResponseFactory factory;
    factory.setMapping( getTestMapping() );
    factory.setNamespaces( getTestNamespaces( ) );
    factory.setDetailMapping( getTestDetailMapping( ) );

    {
        SoapResponseFactory copy;
        copy = factory;

        CPPUNIT_ASSERT_EQUAL( factory.m_mapping.size(), copy.m_mapping.size() );
        CPPUNIT_ASSERT_EQUAL( factory.m_namespaces.size(), copy.m_namespaces.size() );
        CPPUNIT_ASSERT_EQUAL( factory.m_detailMapping.size(), copy.m_detailMapping.size() );
        CPPUNIT_ASSERT_EQUAL( factory.m_session, copy.m_session );
    }

    {
        SoapResponseFactory copy( factory );

        CPPUNIT_ASSERT_EQUAL( factory.m_mapping.size(), copy.m_mapping.size() );
        CPPUNIT_ASSERT_EQUAL( factory.m_namespaces.size(), copy.m_namespaces.size() );
        CPPUNIT_ASSERT_EQUAL( factory.m_detailMapping.size(), copy.m_detailMapping.size() );
        CPPUNIT_ASSERT_EQUAL( factory.m_session, copy.m_session );
    }
}

void SoapTest::createResponseTest( )
{
    SoapResponseFactory factory;
    factory.setMapping( getTestMapping() );
    factory.setNamespaces( getTestNamespaces( ) );
    factory.setDetailMapping( getTestDetailMapping( ) );

    string xml = "<n1:testResponse xmlns:n1=\"test-ns-url\"/>";
    RelatedMultipart multipart; // Multipart won't be used in that test

    SoapResponsePtr actual = factory.createResponse( test::getXmlNode( xml ), multipart );
    CPPUNIT_ASSERT_MESSAGE( "Wrong response created", dynamic_cast< TestResponse* >( actual.get( ) ) != NULL );
}

void SoapTest::parseFaultDetailEmptyTest( )
{
    SoapResponseFactory factory;
    factory.setMapping( getTestMapping() );
    factory.setNamespaces( getTestNamespaces( ) );
    factory.setDetailMapping( getTestDetailMapping( ) );

    string xml = "<detail/>";

    vector< SoapFaultDetailPtr > actual = factory.parseFaultDetail( test::getXmlNode( xml ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Shouldn't have any detail", size_t( 0 ), actual.size() );
}

void SoapTest::parseFaultDetailUnknownTest( )
{
    SoapResponseFactory factory;
    factory.setMapping( getTestMapping() );
    factory.setNamespaces( getTestNamespaces( ) );
    factory.setDetailMapping( getTestDetailMapping( ) );

    string xml = "<detail><unknown-detail/></detail>";

    vector< SoapFaultDetailPtr > actual = factory.parseFaultDetail( test::getXmlNode( xml ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Shouldn't have ignored unknonw details", size_t( 0 ), actual.size() );
}
void SoapTest::parseFaultDetailValidTest( )
{
    SoapResponseFactory factory;
    factory.setMapping( getTestMapping() );
    factory.setNamespaces( getTestNamespaces( ) );
    factory.setDetailMapping( getTestDetailMapping( ) );

    string xml = "<detail><n1:testFault xmlns:n1=\"test-ns-url\"/></detail>";

    vector< SoapFaultDetailPtr > actual = factory.parseFaultDetail( test::getXmlNode( xml ) );
    CPPUNIT_ASSERT_MESSAGE( "Wrong fault detail created",
            dynamic_cast< TestFaultDetail* >( actual.front( ).get( ) ) != NULL );
}

void SoapTest::parseResponseTest( )
{
    SoapResponseFactory factory;
    factory.setMapping( getTestMapping() );
    factory.setNamespaces( getTestNamespaces( ) );
    factory.setDetailMapping( getTestDetailMapping( ) );

    string xml = "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Body>"
                 "<test:testResponse xmlns:test=\"test-ns-url\"/>"
                 "<test:testResponse xmlns:test=\"test-ns-url\"/>"
                 "</S:Body></S:Envelope>";
    string name( "name" );
    string type( "application/xop+xml" );
    RelatedPartPtr requestPart( new RelatedPart( name, type, xml ) );

    RelatedMultipart multipart;
    string cid = multipart.addPart( requestPart );
    string startInfo( "text/xml" );
    multipart.setStart( cid, startInfo );

    vector< SoapResponsePtr > actual = factory.parseResponse( multipart );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of responses", size_t( 2 ), actual.size( ) );
}

void SoapTest::parseResponseXmlTest( )
{
    SoapResponseFactory factory;
    factory.setMapping( getTestMapping() );
    factory.setNamespaces( getTestNamespaces( ) );
    factory.setDetailMapping( getTestDetailMapping( ) );

    string xml = "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\"><S:Body>"
                 "<test:testResponse xmlns:test=\"test-ns-url\"/>"
                 "<test:testResponse xmlns:test=\"test-ns-url\"/>"
                 "</S:Body></S:Envelope>";

    vector< SoapResponsePtr > actual = factory.parseResponse( xml );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of responses", size_t( 2 ), actual.size( ) );
}

void SoapTest::parseResponseFaultTest( )
{
    SoapResponseFactory factory;
    factory.setMapping( getTestMapping() );
    factory.setNamespaces( getTestNamespaces( ) );
    factory.setDetailMapping( getTestDetailMapping( ) );

    string xml = "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\""
                 "            xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\""
                 "            xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">"
                 "  <S:Body><S:Fault>"
                 "      <faultcode xsi:type=\"xsd:string\">S:Client</faultcode>"
                 "      <faultstring xsi:type=\"xsd:string\">Some Error Message</faultstring>"
                 "      <detail><n1:testFault xmlns:n1=\"test-ns-url\"/></detail>"
                 "  </S:Fault></S:Body>"
                 "</S:Envelope>";

    string name( "name" );
    string type( "application/xop+xml" );
    RelatedPartPtr requestPart( new RelatedPart( name, type, xml ) );

    RelatedMultipart multipart;
    string cid = multipart.addPart( requestPart );
    string startInfo( "text/xml" );
    multipart.setStart( cid, startInfo );
   
    try
    {
        factory.parseResponse( multipart );
        CPPUNIT_FAIL( "Should have thrown the SoapFault" );
    }
    catch ( const SoapFault& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong detail string", string( "Some Error Message" ), e.getFaultstring() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong detail string", string( "Client" ), e.getFaultcode() );
        CPPUNIT_ASSERT_MESSAGE( "Wrong fault detail created",
                dynamic_cast< TestFaultDetail* >( e.getDetail( ).front( ).get( ) ) != NULL );
    }
}

void SoapTest::serializeMultipartSimpleTest( )
{
    string partName = "data";
    string partType = "text/plain";
    string partContent = "Some content";
    string startInfo = "some info";


    RelatedMultipart multipart;
    RelatedPartPtr part( new RelatedPart( partName, partType, partContent ) );
    string cid = multipart.addPart( part );
    multipart.setStart( cid, startInfo );

    boost::shared_ptr< istringstream > actual = multipart.toStream( );

    string boundary = multipart.getBoundary( );
    string expected = "\r\n--" + boundary + "\r\n" +
                      "Content-Id: <" + cid + ">\r\n" +
                      "Content-Type: " + partType + "\r\n" +
                      "Content-Transfer-Encoding: binary\r\n" +
                      "\r\n" +
                      partContent +
                      "\r\n--" + boundary + "--\r\n";

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong body", expected, actual->str() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong content type",
            "multipart/related;start=\"" + cid + "\";type=\"" + partType + "\";boundary=\"" + boundary + "\";start-info=\"" + startInfo + "\"",
            multipart.getContentType() );
}

void SoapTest::serializeMultipartComplexTest( )
{
    string rootName = "root";
    string rootType = "text/plain";
    string rootContent = "Some content";

    string part2Name = "part2";
    string part2Type = "application/octet-stream";
    string part2Content = "Some content 2";
    
    string startInfo = "some info";


    RelatedMultipart multipart;
    RelatedPartPtr rootPart( new RelatedPart( rootName, rootType, rootContent ) );
    string rootCid = multipart.addPart( rootPart );

    RelatedPartPtr part2( new RelatedPart( part2Name, part2Type, part2Content ) );
    string part2Cid = multipart.addPart( part2 );
    
    multipart.setStart( rootCid, startInfo );

    boost::shared_ptr< istringstream > actual = multipart.toStream( );

    string boundary = multipart.getBoundary( );
    string expected = "\r\n--" + boundary + "\r\n" +
                      "Content-Id: <" + rootCid + ">\r\n" +
                      "Content-Type: " + rootType + "\r\n" +
                      "Content-Transfer-Encoding: binary\r\n" +
                      "\r\n" +
                      rootContent +
                      "\r\n--" + boundary + "\r\n" +
                      "Content-Id: <" + part2Cid + ">\r\n" +
                      "Content-Type: " + part2Type + "\r\n" +
                      "Content-Transfer-Encoding: binary\r\n" +
                      "\r\n" +
                      part2Content +
                      "\r\n--" + boundary + "--\r\n";

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong body", expected, actual->str() );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong content type",
            "multipart/related;start=\"" + rootCid + "\";type=\"" + rootType + "\";boundary=\"" + boundary + "\";start-info=\"" + startInfo + "\"",
            multipart.getContentType() );
}

void SoapTest::parseMultipartTest( )
{
    string rootCid = "root-cid";
    string rootType = "text/plain";
    string rootContent = "Some content";

    string part2Cid = "part2-cid";
    string part2Type = "application/octet-stream";
    string part2Content = "Some content 2\r\nwith windows-style line endings\r\n";
    
    string startInfo = "some info";

    string boundary = "------------ABCDEF-Boundary";
    string body = "\r\n--" + boundary + "\r\n" +
                  "Content-Id: <" + rootCid + ">\r\n" +
                  "Content-Type: " + rootType + "\r\n" +
                  "Content-Transfer-Encoding: binary\r\n" +
                  "\r\n" +
                  rootContent +
                  "\r\n--" + boundary + "\r\n" +
                  // Voluntarily make a case-sensitivity error to test the SharePoint case
                  "Content-ID: <" + part2Cid + ">\r\n" +
                  "Content-Type: " + part2Type + "\r\n" +
                  "Content-Transfer-Encoding: binary\r\n" +
                  "\r\n" +
                  part2Content +
                  "\r\n--" + boundary + "--\r\n";

    // Added a space before one of the items as it may happen 
    string contentType = "multipart/related; start=\"" + rootCid + "\";type=\"" + rootType + "\";" +
                         "boundary=\"" + boundary + "\";start-info=\"" + startInfo + "\"";

    RelatedMultipart multipart( body, contentType );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong start Content id", rootCid, multipart.getStartId( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong start info", startInfo, multipart.getStartInfo( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong boundary", boundary, multipart.getBoundary( ) );

    vector< string > cids = multipart.getIds( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of parts parsed", size_t( 2 ), cids.size( ) );

    RelatedPartPtr actualRoot = multipart.getPart( rootCid );
    CPPUNIT_ASSERT_MESSAGE( "No part corresponding to root cid", actualRoot.get( ) != NULL );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong root part content type", rootType, actualRoot->getContentType( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong root part content", rootContent, actualRoot->getContent( ) );
    
    RelatedPartPtr actualPart2 = multipart.getPart( part2Cid );
    CPPUNIT_ASSERT_MESSAGE( "No part corresponding to part2 cid", actualPart2.get( ) != NULL );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong part2 part content type", part2Type, actualPart2->getContentType( ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong part2 part content", part2Content, actualPart2->getContent( ) );
}

void SoapTest::getStreamFromNodeXopTest( )
{
    // Create the test multipart
    string dataCid = "http://data-cid";
    string dataCidEncoded = "http%3A%2F%2Fdata-cid";
    string dataContent = "Some transfered content";
    
    string boundary = "------------ABCDEF-Boundary";
    string body = "\r\n--" + boundary + "\r\n" +
                  "Content-Id: <root-cid>\r\n" +
                  "Content-Type: text/plain\r\n" +
                  "Content-Transfer-Encoding: binary\r\n" +
                  "\r\n" +
                  "Who cares? we assume, this has been properly extracted in this test" +
                  "\r\n--" + boundary + "\r\n" +
                  "Content-Id: " + dataCid + "\r\n" +
                  "Content-Type: text/plain\r\n" +
                  "Content-Transfer-Encoding: binary\r\n" +
                  "\r\n" +
                  dataContent +
                  "\r\n--" + boundary + "--\r\n";
            
    string contentType = string( "multipart/related;start=\"root-cid\";type=\"text/plain\";" ) +
                         "boundary=\"" + boundary + "\";start-info=\"info\"";

    RelatedMultipart multipart( body, contentType );

    // Create test node
    stringstream buf;
    buf << "<stream>"
        << "  <xop:Include xmlns:xop=\"http://www.w3.org/2004/08/xop/include\" href=\"cid:" << dataCidEncoded << "\"/>"
        << "</stream>";
    test::XmlNodeRef node = test::getXmlNode( buf.str( ) );

    // Run the tested method
    boost::shared_ptr< istream > stream = getStreamFromNode( node, multipart );

    // Checks
    stringstream out;
    out << stream->rdbuf( );
    CPPUNIT_ASSERT_EQUAL( dataContent, out.str( ) );
}

void SoapTest::getStreamFromNodeBase64Test( )
{
    // Create the test multipart
    string boundary = "------------ABCDEF-Boundary";
    string body = "\r\n--" + boundary + "\r\n" +
                  "Content-Id: <root-cid>\r\n" +
                  "Content-Type: text/plain\r\n" +
                  "Content-Transfer-Encoding: binary\r\n" +
                  "\r\n" +
                  "Who cares? we assume, this has been properly extracted in this test" +
                  "\r\n--" + boundary + "--\r\n";
            
    string contentType = string( "multipart/related;start=\"root-cid\";type=\"text/plain\";" ) +
                         "boundary=\"" + boundary + "\";start-info=\"info\"";

    RelatedMultipart multipart( body, contentType );

    // Create test node
    string dataContent = "U29tZSB0cmFuc2ZlcmVkIGNvbnRlbnQ=";
    string expectedContent = "Some transfered content";
    
    stringstream buf;
    buf << "<stream>" << dataContent << "</stream>";
    test::XmlNodeRef node = test::getXmlNode( buf.str( ) );

    // Run the tested method
    boost::shared_ptr< istream > stream = getStreamFromNode( node, multipart );

    // Checks
    stringstream out;
    out << stream->rdbuf( );
    CPPUNIT_ASSERT_EQUAL( expectedContent, out.str( ) );
}

void SoapTest::writeCmismStreamTest( )
{
    // Initialize the writer
    xmlBufferPtr buf = xmlBufferCreate( );
    xmlTextWriterPtr writer = xmlNewTextWriterMemory( buf, 0 );
    xmlTextWriterStartDocument( writer, NULL, NULL, NULL );

    // Test writeCmismStream
    RelatedMultipart multipart;
    string contentType( "text/plain" );
    string content( "Expected content" );
    string filename( "name.txt" );
    boost::shared_ptr< ostream > os( new stringstream( content ) );
    writeCmismStream( writer, multipart, os, contentType, filename );

    // Close the writer and check the results
    xmlTextWriterEndDocument( writer );
    string str( ( const char * )xmlBufferContent( buf ) );

    vector< string > ids = multipart.getIds( );
    CPPUNIT_ASSERT_EQUAL( size_t( 1 ), ids.size( ) );
    string partId = ids.front( );

    RelatedPartPtr part = multipart.getPart( partId );
    CPPUNIT_ASSERT_MESSAGE( "Missing stream related part", part.get( ) != NULL );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Content not properly attached", content, part->getContent( ) );

    stringstream expectedXml;
    expectedXml << "<?xml version=\"1.0\"?>\n"
                << "<cmism:length>" << content.size( ) << "</cmism:length>"
                << "<cmism:mimeType>" << contentType << "</cmism:mimeType>"
                << "<cmism:filename>" << filename << "</cmism:filename>"
                << "<cmism:stream>"
                << "<xop:Include xmlns:xop=\"http://www.w3.org/2004/08/xop/include\" href=\"cid:" << partId << "\"/>"
                << "</cmism:stream>\n";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Envelope part isn't correct", expectedXml.str( ), str );

    // Free it all
    xmlFreeTextWriter( writer );
    xmlBufferFree( buf );
}
