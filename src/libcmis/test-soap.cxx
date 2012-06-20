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

#include "ws-relatedmultipart.hxx"
#include "ws-soap.hxx"
#include "test-helpers.hxx"

using namespace std;

class SoapTest : public CppUnit::TestFixture
{
    private:
        map< string, SoapResponseCreator > getTestMapping( );
        map< string, string > getTestNamespaces( );

    public:

        // Soap Responses tests

        void createResponseTest( );
        void parseResponseTest( );
        void parseResponseFaultTest( );

        // RelatedMultipart tests

        void serializeMultipartSimpleTest( );
        void serializeMultipartComplexTest( );
        void parseMultipartTest( );

        CPPUNIT_TEST_SUITE( SoapTest );
        CPPUNIT_TEST( createResponseTest );
        CPPUNIT_TEST( parseResponseTest );
        CPPUNIT_TEST( parseResponseFaultTest );

        CPPUNIT_TEST( serializeMultipartSimpleTest );
        CPPUNIT_TEST( serializeMultipartComplexTest );
        CPPUNIT_TEST( parseMultipartTest );

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

        static SoapResponsePtr create( xmlNodePtr node )
        {
            SoapResponsePtr resp ( new TestResponse( ) );
            return resp;
        }
};

map< string, SoapResponseCreator > SoapTest::getTestMapping( )
{
    map< string, SoapResponseCreator > mapping;
    mapping[ "test:testResponse" ] = &TestResponse::create;
    return mapping;
}

map< string, string > SoapTest::getTestNamespaces( )
{
    map< string, string > namespaces;
    namespaces[ "test" ] = "test-ns-url";
    return namespaces;
}

void SoapTest::createResponseTest( )
{
    SoapResponseFactory factory;
    factory.setMapping( getTestMapping() );
    factory.setNamespaces( getTestNamespaces( ) );

    string xml = "<test:testResponse xmlns:test=\"test-ns-url\"/>";

    SoapResponsePtr actual = factory.createResponse( test::getXmlNode( xml ) );
    CPPUNIT_ASSERT_MESSAGE( "Wrong response created", dynamic_cast< TestResponse* >( actual.get( ) ) != NULL );
}

void SoapTest::parseResponseTest( )
{
    SoapResponseFactory factory;
    factory.setMapping( getTestMapping() );
    factory.setNamespaces( getTestNamespaces( ) );

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

    string xml = "<S:Envelope xmlns:S=\"http://schemas.xmlsoap.org/soap/envelope/\""
                 "            xmlns:xsi=\"http://www.w3.org/1999/XMLSchema-instance\""
                 "            xmlns:xsd=\"http://www.w3.org/1999/XMLSchema\">"
                 "  <S:Body><S:Fault>"
                 "      <faultcode xsi:type=\"xsd:string\">S:Client</faultcode>"
                 "      <faultstring xsi:type=\"xsd:string\">Some Error Message</faultstring>"
                 "  </S:Fault></S:Body>"
                 "</S:Envelope>";
   
    try
    {
        factory.parseResponse( xml );
        CPPUNIT_FAIL( "Should have thrown the SoapFault" );
    }
    catch ( const SoapFault& e )
    {
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong detail string", string( "Some Error Message" ), e.getFaultstring() );
        CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong detail string", string( "Client" ), e.getFaultcode() );
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

    boost::shared_ptr< istringstream > actual = multipart.toString( );

    string boundary = multipart.getBoundary( );
    string expected = "\n--" + boundary + "\n" +
                      "Content-Id: " + cid + "\n" +
                      "Content-Type: " + partType + "\n" +
                      "Content-Transfer-Encoding: binary\n" +
                      "\n" +
                      partContent +
                      "\n--" + boundary + "--\n";

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

    boost::shared_ptr< istringstream > actual = multipart.toString( );

    string boundary = multipart.getBoundary( );
    string expected = "\n--" + boundary + "\n" +
                      "Content-Id: " + rootCid + "\n" +
                      "Content-Type: " + rootType + "\n" +
                      "Content-Transfer-Encoding: binary\n" +
                      "\n" +
                      rootContent +
                      "\n--" + boundary + "\n" +
                      "Content-Id: " + part2Cid + "\n" +
                      "Content-Type: " + part2Type + "\n" +
                      "Content-Transfer-Encoding: binary\n" +
                      "\n" +
                      part2Content +
                      "\n--" + boundary + "--\n";

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
    string part2Content = "Some content 2";
    
    string startInfo = "some info";

    string boundary = "------------ABCDEF-Boundary";
    string body = "\n--" + boundary + "\n" +
                  "Content-Id: " + rootCid + "\n" +
                  "Content-Type: " + rootType + "\n" +
                  "Content-Transfer-Encoding: binary\n" +
                  "\n" +
                  rootContent +
                  "\n--" + boundary + "\n" +
                  "Content-Id: " + part2Cid + "\n" +
                  "Content-Type: " + part2Type + "\n" +
                  "Content-Transfer-Encoding: binary\n" +
                  "\n" +
                  part2Content +
                  "\n--" + boundary + "--\n";
            
    string contentType = "multipart/related;start=\"" + rootCid + "\";type=\"" + rootType + "\";" +
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
