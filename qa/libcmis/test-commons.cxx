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

#if defined __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wkeyword-macro"
#endif
#define private public
#define protected public
#if defined __clang__
#pragma clang diagnostic pop
#endif

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

        // Methods that should never be called
        void objectTypeNocallTest();

        CPPUNIT_TEST_SUITE( CommonsTest );
        CPPUNIT_TEST( oauth2DataCopyTest );
        CPPUNIT_TEST( oauth2HandlerCopyTest );
        CPPUNIT_TEST( objectTypeCopyTest );
        CPPUNIT_TEST( objectTypeNocallTest );
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
    CPPUNIT_ASSERT_EQUAL( expected.getRefreshTimestamp(), actual.getRefreshTimestamp() );
    CPPUNIT_ASSERT_EQUAL( expected.getId(), actual.getId() );
    CPPUNIT_ASSERT_EQUAL( expected.getLocalName(), actual.getLocalName() );
    CPPUNIT_ASSERT_EQUAL( expected.getLocalNamespace(), actual.getLocalNamespace() );
    CPPUNIT_ASSERT_EQUAL( expected.getDisplayName(), actual.getDisplayName() );
    CPPUNIT_ASSERT_EQUAL( expected.getQueryName(), actual.getQueryName() );
    CPPUNIT_ASSERT_EQUAL( expected.getDescription(), actual.getDescription() );
    CPPUNIT_ASSERT_EQUAL( expected.getParentTypeId(), actual.getParentTypeId() );
    CPPUNIT_ASSERT_EQUAL( expected.getBaseTypeId(), actual.getBaseTypeId() );
    CPPUNIT_ASSERT_EQUAL( expected.isCreatable(), actual.isCreatable() );
    CPPUNIT_ASSERT_EQUAL( expected.isFileable(), actual.isFileable() );
    CPPUNIT_ASSERT_EQUAL( expected.isQueryable(), actual.isQueryable() );
    CPPUNIT_ASSERT_EQUAL( expected.isFulltextIndexed(), actual.isFulltextIndexed() );
    CPPUNIT_ASSERT_EQUAL( expected.isIncludedInSupertypeQuery(), actual.isIncludedInSupertypeQuery() );
    CPPUNIT_ASSERT_EQUAL( expected.isControllablePolicy(), actual.isControllablePolicy() );
    CPPUNIT_ASSERT_EQUAL( expected.isControllableACL(), actual.isControllableACL() );
    CPPUNIT_ASSERT_EQUAL( expected.isVersionable(), actual.isVersionable() );
    CPPUNIT_ASSERT_EQUAL( expected.getContentStreamAllowed(), actual.getContentStreamAllowed() );
    CPPUNIT_ASSERT_EQUAL( const_cast< ObjectType& >( expected ).getPropertiesTypes().size(),
                          const_cast< ObjectType& >( actual ).getPropertiesTypes().size() );
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

void CommonsTest::objectTypeNocallTest( )
{
    ObjectType type;

    try
    {
        type.refresh();
        CPPUNIT_FAIL( "refresh() shouldn't succeed" );
    }
    catch ( const Exception& e )
    {
    }

    try
    {
        type.getParentType();
        CPPUNIT_FAIL( "getParentType() shouldn't succeed" );
    }
    catch ( const Exception& e )
    {
    }

    try
    {
        type.getBaseType();
        CPPUNIT_FAIL( "getBaseType() shouldn't succeed" );
    }
    catch ( const Exception& e )
    {
    }

    try
    {
        type.getChildren();
        CPPUNIT_FAIL( "getChildren() shouldn't succeed" );
    }
    catch ( const Exception& e )
    {
    }
}

CPPUNIT_TEST_SUITE_REGISTRATION( CommonsTest );
