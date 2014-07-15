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
 * Copyright (C) 2014 Mihai Varga <mihai.mv13@gmail.com>
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

#include <string>

#define private public
#define protected public

#include <mockup-config.h>

#include <fstream>
#include "sharepoint-object.hxx"
#include "sharepoint-property.hxx"
#include "sharepoint-session.hxx"

using namespace std;
using namespace libcmis;

static const string USERNAME( "mock-user" );
static const string PASSWORD( "mock-password" );
static const string BASE_URL( "http://base/_api/Web" );
static const string CONTEXTINFO_URL( "http://base/_api/contextinfo" );

class SharePointTest : public CppUnit::TestFixture
{
    public:
        void getRepositoriesTest( );
        void getObjectTest( );
        void propertiesTest( );
        void deleteTest( );
        
        CPPUNIT_TEST_SUITE( SharePointTest );
        CPPUNIT_TEST( getRepositoriesTest );
        CPPUNIT_TEST( getObjectTest );
        CPPUNIT_TEST( propertiesTest );
        CPPUNIT_TEST( deleteTest );
        CPPUNIT_TEST_SUITE_END( );

    private:
        SharePointSession getTestSession( string username, string password );
};

SharePointSession SharePointTest::getTestSession( string username, string password )
{
    curl_mockup_reset( );
    curl_mockup_addResponse( BASE_URL.c_str( ), "", "GET", "", 401, false );
    curl_mockup_addResponse( ( BASE_URL + "/currentuser" ).c_str( ), "", "GET",
                             DATA_DIR "/sharepoint/auth-resp.json", 200, true );
    curl_mockup_addResponse( CONTEXTINFO_URL.c_str( ), "", "POST",
                             DATA_DIR "/sharepoint/xdigest.json", 200, true );

    return SharePointSession( BASE_URL, username, password, false );
}

void SharePointTest::getRepositoriesTest( )
{
     curl_mockup_reset( );

     SharePointSession session = getTestSession( USERNAME, PASSWORD );
     vector< libcmis::RepositoryPtr > actual = session.getRepositories( );

     CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong number of repositories", size_t( 1 ),
                                   actual.size( ) );
     CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong repository found",
                                   string ( "SharePoint" ),
                                   actual.front()->getId( ) );
}

void SharePointTest::getObjectTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string url = objectId;
    string authorUrl = url + "/Author";
    curl_mockup_addResponse ( url.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( objectId );
    boost::shared_ptr<SharePointObject> obj = boost::dynamic_pointer_cast
                                            <SharePointObject>( object );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong Object Id", objectId,
                                                     obj->getId( ) );
}

void SharePointTest::propertiesTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string url = objectId;
    string authorUrl = url + "/Author";
    curl_mockup_addResponse ( url.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);

    libcmis::ObjectPtr object = session.getObject( objectId );

    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong creation date",
                                   string ( "2014-07-08T09:29:29Z" ),
                                   object->getStringProperty( "cmis:creationDate" ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong object id",
                                   string ( "http://base/_api/Web/aFileId" ),
                                   object->getStringProperty( "cmis:objectId" ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong author",
                                   string ( "aUserId" ),
                                   object->getStringProperty( "cmis:createdBy" ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong file name",
                                   string ( "SharePoint File" ),
                                   object->getStringProperty( "cmis:contentStreamFileName" ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong checkin comment",
                                   string ( "aCheckinComment" ),
                                   object->getStringProperty( "cmis:checkinComment" ) );
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Wrong version",
                                   string ( "2" ),
                                   object->getStringProperty( "cmis:versionLabel" ) );
}

void SharePointTest::deleteTest( )
{
    static const string objectId ( "http://base/_api/Web/aFileId" );

    SharePointSession session = getTestSession( USERNAME, PASSWORD );
    string url = objectId;
    string authorUrl = url + "/Author";
    curl_mockup_addResponse ( url.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/file.json", 200, true);
    curl_mockup_addResponse ( authorUrl.c_str( ), "",
                              "GET", DATA_DIR "/sharepoint/author.json", 200, true);
    curl_mockup_addResponse( url.c_str( ),"", "DELETE", "", 204, false);

    libcmis::ObjectPtr object = session.getObject( objectId );

    object->remove( );
    const struct HttpRequest* deleteRequest = curl_mockup_getRequest( url.c_str( ), "", "DELETE" );
    CPPUNIT_ASSERT_MESSAGE( "Delete request not sent", deleteRequest );
}

CPPUNIT_TEST_SUITE_REGISTRATION( SharePointTest );
