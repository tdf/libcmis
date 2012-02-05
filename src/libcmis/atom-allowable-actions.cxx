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

#include <libxml/xpathInternals.h>

#include "atom-allowable-actions.hxx"
#include "atom-utils.hxx"
#include "xml-utils.hxx"

using namespace std;

namespace atom
{
    class ObjectAction : libcmis::ObjectAction
    {
        private:
            libcmis::ObjectAction::Type m_type;
            bool m_enabled;
            bool m_valid;

        public:
            ObjectAction( xmlNodePtr node );

            libcmis::ObjectAction::Type getType( ) { return m_type; }
            bool isEnabled( ) { return m_enabled; }
            bool isValid( ) { return m_valid; }

            /** Parses the atom permission name into one of the enum values or throws
                an exception for invalid input strings.
              */
            static libcmis::ObjectAction::Type parseType( string type ) throw ( libcmis::Exception );
    };

    ObjectAction::ObjectAction( xmlNodePtr node ) :
        m_type( libcmis::ObjectAction::DeleteObject ),
        m_enabled( false ),
        m_valid( false )
    {
        try
        {
            m_type = parseType( string( ( char* ) node->name ) );
            m_valid = true;
        }
        catch ( const libcmis::Exception& e )
        {
            m_valid = false;
        }

        // Invalid xsd:bool will be mean false... not sure what the spec says
        try
        {
            xmlChar* content = xmlNodeGetContent( node );
            m_enabled = libcmis::parseBool( string( ( char* )content ) );
            xmlFree( content );
        }
        catch ( const libcmis::Exception& e )
        {
            m_enabled = false;
        }
    }

    libcmis::ObjectAction::Type ObjectAction::parseType( string type ) throw ( libcmis::Exception )
    {
        libcmis::ObjectAction::Type value = libcmis::ObjectAction::DeleteObject;
        if ( type == "canDeleteObject" )
            value = libcmis::ObjectAction::DeleteObject;
        else if ( type == "canUpdateProperties" )
            value = libcmis::ObjectAction::UpdateProperties;
        else if ( type == "canGetFolderTree" )
            value = libcmis::ObjectAction::GetFolderTree;
        else if ( type == "canGetProperties" )
            value = libcmis::ObjectAction::GetProperties;
        else if ( type == "canGetObjectRelationships" )
            value = libcmis::ObjectAction::GetObjectRelationships;
        else if ( type == "canGetObjectParents" )
            value = libcmis::ObjectAction::GetObjectParents;
        else if ( type == "canGetFolderParent" )
            value = libcmis::ObjectAction::GetFolderParent;
        else if ( type == "canGetDescendants" )
            value = libcmis::ObjectAction::GetDescendants;
        else if ( type == "canMoveObject" )
            value = libcmis::ObjectAction::MoveObject;
        else if ( type == "canDeleteContentStream" )
            value = libcmis::ObjectAction::DeleteContentStream;
        else if ( type == "canCheckOut" )
            value = libcmis::ObjectAction::CheckOut;
        else if ( type == "canCancelCheckOut" )
            value = libcmis::ObjectAction::CancelCheckOut;
        else if ( type == "canCheckIn" )
            value = libcmis::ObjectAction::CheckIn;
        else if ( type == "canSetContentStream" )
            value = libcmis::ObjectAction::SetContentStream;
        else if ( type == "canGetAllVersions" )
            value = libcmis::ObjectAction::GetAllVersions;
        else if ( type == "canAddObjectToFolder" )
            value = libcmis::ObjectAction::AddObjectToFolder;
        else if ( type == "canRemoveObjectFromFolder" )
            value = libcmis::ObjectAction::RemoveObjectFromFolder;
        else if ( type == "canGetContentStream" )
            value = libcmis::ObjectAction::GetContentStream;
        else if ( type == "canApplyPolicy" )
            value = libcmis::ObjectAction::ApplyPolicy;
        else if ( type == "canGetAppliedPolicies" )
            value = libcmis::ObjectAction::GetAppliedPolicies;
        else if ( type == "canRemovePolicy" )
            value = libcmis::ObjectAction::RemovePolicy;
        else if ( type == "canGetChildren" )
            value = libcmis::ObjectAction::GetChildren;
        else if ( type == "canCreateDocument" )
            value = libcmis::ObjectAction::CreateDocument;
        else if ( type == "canCreateFolder" )
            value = libcmis::ObjectAction::CreateFolder;
        else if ( type == "canCreateRelationship" )
            value = libcmis::ObjectAction::CreateRelationship;
        else if ( type == "canDeleteTree" )
            value = libcmis::ObjectAction::DeleteTree;
        else if ( type == "canGetRenditions" )
            value = libcmis::ObjectAction::GetRenditions;
        else if ( type == "canGetACL" )
            value = libcmis::ObjectAction::GetACL;
        else if ( type == "canApplyACL" )
            value = libcmis::ObjectAction::ApplyACL;
        else
            throw libcmis::Exception( "Invalid AllowableAction type: " + type );
        
        return value;
    }
}

AtomAllowableActions::AtomAllowableActions( AtomPubSession* session ) :
    libcmis::AllowableActions( ),
    m_url( ),
    m_session( session )
{
}

AtomAllowableActions::AtomAllowableActions( AtomPubSession* session, string url ) :
    libcmis::AllowableActions( ),
    m_url( url ),
    m_session( session )
{
    refresh();
}

AtomAllowableActions::AtomAllowableActions( const AtomAllowableActions& copy ) :
    libcmis::AllowableActions( copy ),
    m_url( copy.m_url ),
    m_session( copy.m_session )
{
}

const AtomAllowableActions& AtomAllowableActions::operator=( const AtomAllowableActions& copy )
{
    AllowableActions::operator=( copy );
    m_url = copy.m_url;
    m_session = copy.m_session;

    return *this;
}

void AtomAllowableActions::refresh( ) throw ( libcmis::Exception )
{
    m_states.clear( );
    
    if ( !m_url.empty( ) )
    {
        string buf;
        try
        {
            buf  = m_session->httpGetRequest( m_url );
        }
        catch ( const atom::CurlException& e )
        {
            throw e.getCmisException( );
        }
        xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), m_url.c_str(), NULL, 0 );

        if ( NULL == doc )
            throw libcmis::Exception( "Failed to parse object infos" );

        // Populate the m_states map
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );

        atom::registerNamespaces( xpathCtx );

        if ( NULL != xpathCtx )
        {
            xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( "//cmis:allowableActions" ), xpathCtx );
            if ( xpathObj && xpathObj->nodesetval && xpathObj->nodesetval->nodeNr > 0 )
            {
                xmlNodePtr node = xpathObj->nodesetval->nodeTab[0];
                for ( xmlNodePtr child = node->children; child; child = child->next )
                {
                    atom::ObjectAction action( child );
                    if ( action.isValid( ) )
                        m_states.insert( pair< libcmis::ObjectAction::Type, bool >(
                                    action.getType( ),
                                    action.isEnabled() ) );
                }
            }
            xmlXPathFreeObject( xpathObj );
        }

        xmlXPathFreeContext( xpathCtx );

        xmlFreeDoc( doc );
    }
}
