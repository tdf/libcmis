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

#define private public
#define protected public

#include <ws-session.hxx>

#include <mockup-config.h>
#include <test-helpers.hxx>

#define SERVER_URL string( "http://mockup/ws" )
#define SERVER_REPOSITORY string( "mock" )
#define SERVER_USERNAME "tester"
#define SERVER_PASSWORD "somepass"

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

    void lcl_addWsResponse( const char* url, const char* filename,
                            unsigned int status = 0 )
    {
        FILE* fd = fopen( filename, "r" );

        size_t bufSize = 2048;
        char* buf = new char[bufSize];

        size_t read = 0;
        size_t written = 0;
        string outBuf;
        do
        {
            read = fread( buf, 1, bufSize, fd );
            outBuf += buf;
        } while ( read == bufSize && written == read );

        fclose( fd );
        delete[] buf;

        string emptyLine = ("\n\n" );
        size_t pos = outBuf.find( emptyLine );
        string headers = outBuf.substr( 0, pos );
        string body = outBuf.substr( pos + emptyLine.size() );

        curl_mockup_addResponse( url, "", "POST", body.c_str(), status, false, headers.c_str() );
    }
}

class WSTest : public CppUnit::TestFixture
{
    public:

        void getRepositoriesTest( );

        CPPUNIT_TEST_SUITE( WSTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST_SUITE_END( );

        WSSession getTestSession( string username, string password );
};

CPPUNIT_TEST_SUITE_REGISTRATION( WSTest );

void WSTest::getRepositoriesTest()
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD );
    vector< libcmis::RepositoryPtr > actual = session.getRepositories( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of repositories", size_t( 1 ), actual.size( ) );
}

WSSession WSTest::getTestSession( string username, string password )
{
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/repositories.http" );

    WSSession session;
    session.m_username = username;
    session.m_password = password;

    string buf;
    test::loadFromFile( DATA_DIR "/ws/CMISWS-Service.wsdl", buf );
    session.parseWsdl( buf );
    session.initializeResponseFactory( );
    session.initializeRepositories( );

    return session;
}
