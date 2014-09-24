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
 * Copyright (C) 2011 Cédric Bosdonnat <cbosdo@users.sourceforge.net>
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

#include <sstream>

#include "repository.hxx"
#include "xml-utils.hxx"

using namespace std;

namespace libcmis
{
    Repository::Repository( ) :
        m_id( ),
        m_name( ),
        m_description( ),
        m_vendorName( ),
        m_productName( ),
        m_productVersion( ),
        m_rootId( ),
        m_cmisVersionSupported( ),
        m_thinClientUri( ),
        m_principalAnonymous( ),
        m_principalAnyone( ),
        m_capabilities( )
    {
    }

    Repository::Repository( xmlNodePtr node ) :
        m_id( ),
        m_name( ),
        m_description( ),
        m_vendorName( ),
        m_productName( ),
        m_productVersion( ),
        m_rootId( ),
        m_cmisVersionSupported( ),
        m_thinClientUri( ),
        m_principalAnonymous( ),
        m_principalAnyone( ),
        m_capabilities( )
    {
        initializeFromNode( node );
    }

    string Repository::getId( ) const
    {
        return m_id;
    }

    string Repository::getName( ) const
    {
        return m_name;
    }

    string Repository::getDescription( ) const
    {
        return m_description;
    }

    string Repository::getVendorName( ) const
    {
        return m_vendorName;
    }

    string Repository::getProductName( ) const
    {
        return m_productName;
    }

    string Repository::getProductVersion( ) const
    {
        return m_productVersion;
    }

    string Repository::getRootId( ) const
    {
        return m_rootId;
    }

    string Repository::getCmisVersionSupported( ) const
    {
        return m_cmisVersionSupported;
    }

    boost::shared_ptr< string > Repository::getThinClientUri( ) const
    {
        return m_thinClientUri;
    }

    boost::shared_ptr< string > Repository::getPrincipalAnonymous( ) const
    {
        return m_principalAnonymous;
    }

    boost::shared_ptr< string > Repository::getPrincipalAnyone( ) const
    {
        return m_principalAnyone;
    }


    void Repository::initializeFromNode( xmlNodePtr node )
    {
        for ( xmlNodePtr child = node->children; child; child = child->next )
        {
            string localName( ( char* ) child->name );

            xmlChar* content = xmlNodeGetContent( child );
            string value( ( char* )content );
            xmlFree( content );

            if ( localName == "repositoryId" )
                m_id = value;
            else if ( localName == "repositoryName" )
                m_name = value;
            else if ( localName == "repositoryDescription" )
                m_description = value;
            else if ( localName == "vendorName" )
                m_vendorName = value;
            else if ( localName == "productName" )
                m_productName = value;
            else if ( localName == "productVersion" )
                m_productVersion = value;
            else if ( localName == "rootFolderId" )
                m_rootId = value;
            else if ( localName == "cmisVersionSupported" )
                m_cmisVersionSupported = value;
            else if ( localName == "thinClientURI" )
                m_thinClientUri.reset( new string( value ) );
            else if ( localName == "principalAnonymous" )
                m_principalAnonymous.reset( new string( value ) );
            else if ( localName == "principalAnyone" )
                m_principalAnyone.reset( new string( value ) );
            else if ( localName == "capabilities" )
            {
                m_capabilities = parseCapabilities( child );
            }
        }
    }

    string Repository::getCapability( Capability capability ) const
    {
        string result;

        map< Capability, string >::const_iterator it = m_capabilities.find( capability );
        if ( it != m_capabilities.end() )
            result = it->second;

        return result;
    }

    bool Repository::getCapabilityAsBool( Capability capability ) const
   {
       string value = getCapability( capability );
       bool result = false;
       try
       {
           result = libcmis::parseBool( value );
       }
       catch ( Exception )
       {
       }
       return result;
   }

    // LCOV_EXCL_START
    string Repository::toString( ) const
    {
        stringstream buf;

        buf << "Id:          " << getId( ) << endl;
        buf << "Name:        " << getName( ) << endl;
        buf << "Description: " << getDescription( ) << endl;
        buf << "Vendor:      " << getVendorName( ) << endl;
        buf << "Product:     " << getProductName( ) << " - version " << getProductVersion( )  << endl;
        buf << "Root Id:     " << getRootId( ) << endl;
        buf << "Supported CMIS Version: " << getCmisVersionSupported( ) << endl;
        if ( getThinClientUri( ) )
            buf << "Thin Client URI:        " << *getThinClientUri( ) << endl;
        if ( getPrincipalAnonymous( ) )
            buf << "Anonymous user:         " << *getPrincipalAnonymous( ) << endl;
        if ( getPrincipalAnyone( ) )
            buf << "Anyone user:            " << *getPrincipalAnyone( ) << endl;
        buf << endl;
        buf << "Capabilities:" << endl;

        static string capabilitiesNames[] =
        {
            "ACL",
            "AllVersionsSearchable",
            "Changes",
            "ContentStreamUpdatability",
            "GetDescendants",
            "GetFolderTree",
            "OrderBy",
            "Multifiling",
            "PWCSearchable",
            "PWCUpdatable",
            "Query",
            "Renditions",
            "Unfiling",
            "VersionSpecificFiling",
            "Join"
        };

        for ( int i = ACL; i < Join; ++i )
        {
            buf << "\t" << capabilitiesNames[i] << ": " << getCapability( ( Capability )i ) << endl;
        }

        return buf.str();
    }
    // LCOV_EXCL_STOP

    map< Repository::Capability, string > Repository::parseCapabilities( xmlNodePtr node )
    {
        map< Capability, string > capabilities;

        for ( xmlNodePtr child = node->children; child; child = child->next )
        {
            string localName( ( char* ) child->name );

            xmlChar* content = xmlNodeGetContent( child );
            string value( ( char* )content );
            xmlFree( content );

            Capability capability = ACL;
            bool ignore = false;
            if ( localName == "capabilityACL" )
                capability = ACL;
            else if ( localName == "capabilityAllVersionsSearchable" )
                capability = AllVersionsSearchable;
            else if ( localName == "capabilityChanges" )
                capability = Changes;
            else if ( localName == "capabilityContentStreamUpdatability" )
                capability = ContentStreamUpdatability;
            else if ( localName == "capabilityGetDescendants" )
                capability = GetDescendants;
            else if ( localName == "capabilityGetFolderTree" )
                capability = GetFolderTree;
            else if ( localName == "capabilityOrderBy" )
                capability = OrderBy;
            else if ( localName == "capabilityMultifiling" )
                capability = Multifiling;
            else if ( localName == "capabilityPWCSearchable" )
                capability = PWCSearchable;
            else if ( localName == "capabilityPWCUpdatable" )
                capability = PWCUpdatable;
            else if ( localName == "capabilityQuery" )
                capability = Query;
            else if ( localName == "capabilityRenditions" )
                capability = Renditions;
            else if ( localName == "capabilityUnfiling" )
                capability = Unfiling;
            else if ( localName == "capabilityVersionSpecificFiling" )
                capability = VersionSpecificFiling;
            else if ( localName == "capabilityJoin" )
                capability = Join;
            else
                ignore = true;

            if ( !ignore )
                capabilities[capability] = value;
        }

        return capabilities;
    }
}
