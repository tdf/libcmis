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

#include "ws-soap.hxx"
#include "test-helpers.hxx"

using namespace std;

class SoapTest : public CppUnit::TestFixture
{
    private:
        map< string, SoapResponseCreator > getTestMapping( );
        map< string, string > getTestNamespaces( );

    public:

        void createResponseTest( );
        void parseResponseTest( );
        void parseResponseFaultTest( );

        CPPUNIT_TEST_SUITE( SoapTest );
        CPPUNIT_TEST( createResponseTest );
        CPPUNIT_TEST( parseResponseTest );
        CPPUNIT_TEST( parseResponseFaultTest );
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
    mapping[ "{test-ns-url}testResponse" ] = &TestResponse::create;
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
