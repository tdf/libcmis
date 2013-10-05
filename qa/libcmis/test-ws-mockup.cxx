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
        string outBuf;
        do
        {
            read = fread( buf, 1, bufSize, fd );
            outBuf += string( buf, read );
        } while ( read == bufSize );

        fclose( fd );
        delete[] buf;

        string emptyLine = ("\n\n" );
        size_t pos = outBuf.find( emptyLine );
        string headers = outBuf.substr( 0, pos );
        string body = outBuf.substr( pos + emptyLine.size() );

        curl_mockup_addResponse( url, "", "POST", body.c_str(), status, false, headers.c_str() );
    }

    string lcl_getCmisRequestXml( string url )
    {
        const struct HttpRequest* request = curl_mockup_getRequest( url.c_str(), "", "POST" );
        char* contentType = curl_mockup_HttpRequest_getHeader( request, "Content-Type" );
        RelatedMultipart multipart( request->body, string( contentType ) );
        RelatedPartPtr part = multipart.getPart( multipart.getStartId() );
        string xml = part->getContent( );
        curl_mockup_HttpRequest_free( request );
        free( contentType );

        return test::getXmlNodeAsString( xml, "/soap-env:Envelope/soap-env:Body/child::*" );
    }

    string lcl_getExpectedNs( )
    {
        string ns = " xmlns=\"http://docs.oasis-open.org/ns/cmis/core/200908/\""
                    " xmlns:cmism=\"http://docs.oasis-open.org/ns/cmis/messaging/200908/\"";
        return ns;
    }
}

class WSTest : public CppUnit::TestFixture
{
    public:

        void getRepositoriesTest( );
        void getRepositoryInfosTest( );

        CPPUNIT_TEST_SUITE( WSTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( getRepositoryInfosTest );
        CPPUNIT_TEST_SUITE_END( );

        libcmis::RepositoryPtr getTestRepository( );
        WSSession getTestSession( string username, string password, bool noRepos = false );
};

CPPUNIT_TEST_SUITE_REGISTRATION( WSTest );

void WSTest::getRepositoriesTest()
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/repositories.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    map< string, string > actual = session.getRepositoryService().getRepositories( );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of repositories", size_t( 1 ), actual.size( ) );

    // Test the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/RepositoryService" );
    string expectedRequest = "<cmism:getRepositories" + lcl_getExpectedNs() + "/>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

void WSTest::getRepositoryInfosTest()
{
    curl_mockup_reset( );
    curl_mockup_setCredentials( SERVER_USERNAME, SERVER_PASSWORD );
    lcl_addWsResponse( "http://mockup/ws/services/RepositoryService", DATA_DIR "/ws/repository-infos.http" );

    WSSession session  = getTestSession( SERVER_USERNAME, SERVER_PASSWORD, true );
    string validId = "mock";
    libcmis::RepositoryPtr actual = session.getRepositoryService().getRepositoryInfo( validId );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Root folder is wrong", string( "root-folder" ), actual->getRootId( ) );

    // Test the sent request
    string xmlRequest = lcl_getCmisRequestXml( "http://mockup/ws/services/RepositoryService" );
    string expectedRequest = "<cmism:getRepositoryInfo" + lcl_getExpectedNs() + ">"
                                 "<cmism:repositoryId>" + validId + "</cmism:repositoryId>"
                             "</cmism:getRepositoryInfo>";
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong request sent", expectedRequest, xmlRequest );
}

WSSession WSTest::getTestSession( string username, string password, bool noRepos )
{
    WSSession session;
    session.m_username = username;
    session.m_password = password;

    string buf;
    test::loadFromFile( DATA_DIR "/ws/CMISWS-Service.wsdl", buf );
    session.parseWsdl( buf );
    session.initializeResponseFactory( );

    // Manually define the repositories to avoid the HTTP query
    if ( !noRepos )
    {
        libcmis::RepositoryPtr repo = getTestRepository( );
        session.m_repositories.push_back( repo );
        session.m_repositoryId = repo->getId( );
    }

    return session;
}

libcmis::RepositoryPtr WSTest::getTestRepository()
{
    libcmis::RepositoryPtr repo( new libcmis::Repository( ) );
    repo->m_id = "mock";
    repo->m_name = "Mockup";
    repo->m_description = "Repository sent by mockup server";
    repo->m_vendorName = "libcmis";
    repo->m_productName = "Libcmis mockup";
    repo->m_productVersion = "some-version";
    repo->m_cmisVersionSupported = "1.1";

    map< libcmis::Repository::Capability, string > capabilities;
    capabilities[libcmis::Repository::ACL] = "manage";
    capabilities[libcmis::Repository::AllVersionsSearchable] = "false";
    capabilities[libcmis::Repository::Changes] = "none";
    capabilities[libcmis::Repository::ContentStreamUpdatability] = "anytime";
    capabilities[libcmis::Repository::GetDescendants] = "true";
    capabilities[libcmis::Repository::GetFolderTree] = "true";
    capabilities[libcmis::Repository::Multifiling] = "true";
    capabilities[libcmis::Repository::PWCSearchable] = "false";
    capabilities[libcmis::Repository::PWCUpdatable] = "true";
    capabilities[libcmis::Repository::Query] = "bothcombined";
    capabilities[libcmis::Repository::Renditions] = "none";
    capabilities[libcmis::Repository::Unfiling] = "true";
    capabilities[libcmis::Repository::VersionSpecificFiling] = "false";
    capabilities[libcmis::Repository::Join] = "none";
    repo->m_capabilities = capabilities;

    return repo;
}
