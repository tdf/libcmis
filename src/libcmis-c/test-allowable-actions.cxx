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

#include "allowable-actions.h"
#include "internals.hxx"
#include "test-dummies.hxx"

using namespace std;

class AllowableActionsTest : public CppUnit::TestFixture
{
    private:
        libcmis_AllowableActionsPtr getTested( );

    public:
        void isDefinedTest( );
        void isAllowedTest( );

        CPPUNIT_TEST_SUITE( AllowableActionsTest );
        CPPUNIT_TEST( isDefinedTest );
        CPPUNIT_TEST( isAllowedTest );
        CPPUNIT_TEST_SUITE_END( );
};

CPPUNIT_TEST_SUITE_REGISTRATION( AllowableActionsTest );

libcmis_AllowableActionsPtr AllowableActionsTest::getTested( )
{
    libcmis_AllowableActionsPtr result = new libcmis_allowable_actions( );
    libcmis::AllowableActionsPtr handle( new dummies::AllowableActions( ) );
    result->handle = handle;

    return result;
}

void AllowableActionsTest::isDefinedTest( )
{
    libcmis_AllowableActionsPtr allowableActions = getTested( );
    CPPUNIT_ASSERT( !libcmis_allowable_actions_isDefined( allowableActions, libcmis_DeleteObject ) );
    CPPUNIT_ASSERT( libcmis_allowable_actions_isDefined( allowableActions, libcmis_GetFolderParent ) );

    libcmis_allowable_actions_free( allowableActions );
}

void AllowableActionsTest::isAllowedTest( )
{
    libcmis_AllowableActionsPtr allowableActions = getTested( );
    CPPUNIT_ASSERT( libcmis_allowable_actions_isAllowed( allowableActions, libcmis_GetProperties ) );
    CPPUNIT_ASSERT( !libcmis_allowable_actions_isAllowed( allowableActions, libcmis_GetFolderParent ) );

    libcmis_allowable_actions_free( allowableActions );
}
