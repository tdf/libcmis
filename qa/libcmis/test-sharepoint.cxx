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
static const string BASE_URL ( "http://base/_api/Web" );

class SharePointTest : public CppUnit::TestFixture
{
    public:
        void getRepositoriesTest( );
        
        CPPUNIT_TEST_SUITE( SharePointTest );
        CPPUNIT_TEST( getRepositoriesTest );
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

CPPUNIT_TEST_SUITE_REGISTRATION( SharePointTest );
