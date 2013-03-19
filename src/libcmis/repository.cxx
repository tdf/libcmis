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

#include "repository.hxx"

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
        m_principalAnyone( )
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
        m_principalAnyone( )
    {
        initializeFromNode( node );
    }
    
    Repository::Repository( const string& dummy ) :
        m_id( dummy),
        m_name( dummy ),
        m_description(dummy ),
        m_vendorName( dummy ),
        m_productName( dummy ),
        m_productVersion( dummy  ),
        m_rootId( dummy ),
        m_cmisVersionSupported( dummy ),
        m_thinClientUri( ),
        m_principalAnonymous( ),
        m_principalAnyone( )
    {
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
        }
    }
}
