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
 * Copyright (C) 2014 SUSE <cedric@bosdonnat.fr>
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

#include <time.h>

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/ui/text/TestRunner.h>
#include <libxml/tree.h>

#define private public
#define protected public

#include "oauth2-data.hxx"
#include "oauth2-handler.hxx"
#include "object-type.hxx"

using namespace libcmis;
using namespace std;

class CommonsTest : public CppUnit::TestFixture
{
    public:

        // constructors tests
        void oauth2DataCopyTest();
        void oauth2HandlerCopyTest();
        void objectTypeCopyTest();

        CPPUNIT_TEST_SUITE( CommonsTest );
        CPPUNIT_TEST( oauth2DataCopyTest );
        CPPUNIT_TEST( oauth2HandlerCopyTest );
        CPPUNIT_TEST( objectTypeCopyTest );
        CPPUNIT_TEST_SUITE_END( );
};

static void assertOAuth2DataEquals( const OAuth2Data& expected, const OAuth2Data& actual )
{
    CPPUNIT_ASSERT_EQUAL( expected.m_authUrl, actual.m_authUrl );
    CPPUNIT_ASSERT_EQUAL( expected.m_tokenUrl, actual.m_tokenUrl );
    CPPUNIT_ASSERT_EQUAL( expected.m_scope, actual.m_scope );
    CPPUNIT_ASSERT_EQUAL( expected.m_redirectUri, actual.m_redirectUri );
    CPPUNIT_ASSERT_EQUAL( expected.m_clientId, actual.m_clientId );
    CPPUNIT_ASSERT_EQUAL( expected.m_clientSecret, actual.m_clientSecret );
}

void CommonsTest::oauth2DataCopyTest( )
{
    OAuth2Data data( "url", "token", "scope", "redirect",
                     "clientid", "clientsecret" );
    {
        OAuth2Data copy;
        copy = data;
        assertOAuth2DataEquals( data, copy );
    }
    
    {
        OAuth2Data copy( data );
        assertOAuth2DataEquals( data, copy );
    }
}

string DummyOAuth2Parser( HttpSession*, const string&, const string&, const string& )
{
    return "Fake";
}

void CommonsTest::oauth2HandlerCopyTest( )
{
    OAuth2DataPtr data( new OAuth2Data ( "url", "token", "scope", "redirect",
                                         "clientid", "clientsecret" ) );
    HttpSession session( "user", "pass" );
    OAuth2Handler handler( &session, data );
    handler.m_access = "access";
    handler.m_refresh = "refresh";
    handler.m_oauth2Parser = &DummyOAuth2Parser;

    {
        OAuth2Handler copy;
        copy = handler;

        CPPUNIT_ASSERT_EQUAL( &session, copy.m_session );
        CPPUNIT_ASSERT_EQUAL( data, copy.m_data );
        CPPUNIT_ASSERT_EQUAL( handler.m_access, copy.m_access );
        CPPUNIT_ASSERT_EQUAL( handler.m_refresh, copy.m_refresh );
        CPPUNIT_ASSERT_EQUAL( &DummyOAuth2Parser, copy.m_oauth2Parser );
    }

    {
        OAuth2Handler copy( handler );

        CPPUNIT_ASSERT_EQUAL( &session, copy.m_session );
        CPPUNIT_ASSERT_EQUAL( data, copy.m_data );
        CPPUNIT_ASSERT_EQUAL( handler.m_access, copy.m_access );
        CPPUNIT_ASSERT_EQUAL( handler.m_refresh, copy.m_refresh );
        CPPUNIT_ASSERT_EQUAL( &DummyOAuth2Parser, copy.m_oauth2Parser );
    }
}

static void assertObjectTypeEquals( const ObjectType& expected, const ObjectType& actual )
{
    CPPUNIT_ASSERT_EQUAL( expected.m_refreshTimestamp, actual.m_refreshTimestamp );
    CPPUNIT_ASSERT_EQUAL( expected.m_id, actual.m_id );
    CPPUNIT_ASSERT_EQUAL( expected.m_baseTypeId, actual.m_baseTypeId );
    CPPUNIT_ASSERT_EQUAL( expected.m_refreshTimestamp, actual.m_refreshTimestamp );
    CPPUNIT_ASSERT_EQUAL( expected.m_id, actual.m_id );
    CPPUNIT_ASSERT_EQUAL( expected.m_localName, actual.m_localName );
    CPPUNIT_ASSERT_EQUAL( expected.m_localNamespace, actual.m_localNamespace );
    CPPUNIT_ASSERT_EQUAL( expected.m_displayName, actual.m_displayName );
    CPPUNIT_ASSERT_EQUAL( expected.m_queryName, actual.m_queryName );
    CPPUNIT_ASSERT_EQUAL( expected.m_description, actual.m_description );
    CPPUNIT_ASSERT_EQUAL( expected.m_parentTypeId, actual.m_parentTypeId );
    CPPUNIT_ASSERT_EQUAL( expected.m_baseTypeId, actual.m_baseTypeId );
    CPPUNIT_ASSERT_EQUAL( expected.m_creatable, actual.m_creatable );
    CPPUNIT_ASSERT_EQUAL( expected.m_fileable, actual.m_fileable );
    CPPUNIT_ASSERT_EQUAL( expected.m_queryable, actual.m_queryable );
    CPPUNIT_ASSERT_EQUAL( expected.m_fulltextIndexed, actual.m_fulltextIndexed );
    CPPUNIT_ASSERT_EQUAL( expected.m_includedInSupertypeQuery, actual.m_includedInSupertypeQuery );
    CPPUNIT_ASSERT_EQUAL( expected.m_controllablePolicy, actual.m_controllablePolicy );
    CPPUNIT_ASSERT_EQUAL( expected.m_controllableAcl, actual.m_controllableAcl );
    CPPUNIT_ASSERT_EQUAL( expected.m_versionable, actual.m_versionable );
    CPPUNIT_ASSERT_EQUAL( expected.m_contentStreamAllowed, actual.m_contentStreamAllowed );
    CPPUNIT_ASSERT_EQUAL( expected.m_propertiesTypes.size(), actual.m_propertiesTypes.size() );
}

void CommonsTest::objectTypeCopyTest( )
{
    ObjectType type;
    time_t refresh = time( NULL );
    type.m_refreshTimestamp = refresh;
    type.m_id = "id";
    type.m_baseTypeId = "base";

    {
        ObjectType copy;
        copy = type;
        assertObjectTypeEquals( type, copy );
    }

    {
        ObjectType copy ( type );
        assertObjectTypeEquals( type, copy );
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION( CommonsTest );
