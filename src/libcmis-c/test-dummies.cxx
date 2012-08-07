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

#include "test-dummies.hxx"

using namespace std;

namespace dummies
{
    Repository::Repository( ) :
        libcmis::Repository( )
    {
        m_id = string( "Repository::Id" );
        m_name = string( "Repository::Name" );
        m_description = string( "Repository::Description" );
        m_vendorName = string( "Repository::VendorName" );
        m_productName = string( "Repository::ProductName" );
        m_productVersion = string( "Repository::ProductVersion" );
        m_rootId = string( "Repository::RootId" );
        m_cmisVersionSupported = string( "Repository::CmisVersionSupported" );
        m_thinClientUri.reset( new string( "Repository::ThinClientUri" ) );
        m_principalAnonymous.reset( new string( "Repository::PrincipalAnonymous" ) );
        m_principalAnyone.reset( new string( "Repository::PrincipalAnyone" ) );
    }

    Repository::~Repository( )
    {
    }

    PropertyType::PropertyType( string id, string xmlType ) :
        libcmis::PropertyType( )
    {
        setId( id );
        setLocalName( string( "PropertyType::LocalName" ) );
        setLocalNamespace( string( "PropertyType::LocalNamespace" ) );
        setDisplayName( string( "PropertyType::DisplayName" ) );
        setQueryName( string( "PropertyType::QueryName" ) );
        setTypeFromXml( xmlType );

        // Setting true for the tests to see a difference with 
        // the default false result of the tested functions
        setMultiValued( true );
        setUpdatable( true );
        setInherited( true );
        setRequired( true );
        setQueryable( true );
        setOrderable( true );
        setOpenChoice( true );
    }

    PropertyType::~PropertyType( )
    {
    }
}
