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

#include "allowable-actions.hxx"
#include "object.hxx"
#include "xml-utils.hxx"

using namespace std;

namespace libcmis
{
    ObjectAction::ObjectAction( xmlNodePtr node ) :
        m_type( ObjectAction::DeleteObject ),
        m_enabled( false ),
        m_valid( false )
    {
        try
        {
            m_type = parseType( string( ( char* ) node->name ) );
            m_valid = true;
        }
        catch ( const Exception& e )
        {
            m_valid = false;
        }

        // Invalid xsd:bool will be mean false... not sure what the spec says
        try
        {
            xmlChar* content = xmlNodeGetContent( node );
            m_enabled = parseBool( string( ( char* )content ) );
            xmlFree( content );
        }
        catch ( const Exception& e )
        {
            m_enabled = false;
        }
    }

    ObjectAction::Type ObjectAction::parseType( string type ) throw ( Exception )
    {
        Type value = DeleteObject;
        if ( type == "canDeleteObject" )
            value = DeleteObject;
        else if ( type == "canUpdateProperties" )
            value = UpdateProperties;
        else if ( type == "canGetFolderTree" )
            value = GetFolderTree;
        else if ( type == "canGetProperties" )
            value = GetProperties;
        else if ( type == "canGetObjectRelationships" )
            value = GetObjectRelationships;
        else if ( type == "canGetObjectParents" )
            value = GetObjectParents;
        else if ( type == "canGetFolderParent" )
            value = GetFolderParent;
        else if ( type == "canGetDescendants" )
            value = GetDescendants;
        else if ( type == "canMoveObject" )
            value = MoveObject;
        else if ( type == "canDeleteContentStream" )
            value = DeleteContentStream;
        else if ( type == "canCheckOut" )
            value = CheckOut;
        else if ( type == "canCancelCheckOut" )
            value = CancelCheckOut;
        else if ( type == "canCheckIn" )
            value = CheckIn;
        else if ( type == "canSetContentStream" )
            value = SetContentStream;
        else if ( type == "canGetAllVersions" )
            value = GetAllVersions;
        else if ( type == "canAddObjectToFolder" )
            value = AddObjectToFolder;
        else if ( type == "canRemoveObjectFromFolder" )
            value = RemoveObjectFromFolder;
        else if ( type == "canGetContentStream" )
            value = GetContentStream;
        else if ( type == "canApplyPolicy" )
            value = ApplyPolicy;
        else if ( type == "canGetAppliedPolicies" )
            value = GetAppliedPolicies;
        else if ( type == "canRemovePolicy" )
            value = RemovePolicy;
        else if ( type == "canGetChildren" )
            value = GetChildren;
        else if ( type == "canCreateDocument" )
            value = CreateDocument;
        else if ( type == "canCreateFolder" )
            value = CreateFolder;
        else if ( type == "canCreateRelationship" )
            value = CreateRelationship;
        else if ( type == "canDeleteTree" )
            value = DeleteTree;
        else if ( type == "canGetRenditions" )
            value = GetRenditions;
        else if ( type == "canGetACL" )
            value = GetACL;
        else if ( type == "canApplyACL" )
            value = ApplyACL;
        else
            throw Exception( "Invalid AllowableAction type: " + type );
        
        return value;
    }

    AllowableActions::AllowableActions( xmlNodePtr node ) :
        m_states( )
    {
        for ( xmlNodePtr child = node->children; child; child = child->next )
        {
            // Check for non text children... "\n" is also a node ;)
            if ( !xmlNodeIsText( child ) )
            {
                ObjectAction action( child );
                if ( action.isValid( ) )
                    m_states.insert( pair< libcmis::ObjectAction::Type, bool >(
                                action.getType( ),
                                action.isEnabled() ) );
            }
        }
    }

    AllowableActions::AllowableActions( const AllowableActions& copy ) :
        m_states( copy.m_states )
    {
    }

    AllowableActions::~AllowableActions( )
    {
        m_states.clear();
    }

    AllowableActions& AllowableActions::operator=( const AllowableActions& copy )
    {
        if ( this != &copy )
            m_states = copy.m_states;

        return *this;
    }

    bool AllowableActions::isAllowed( ObjectAction::Type action )
    {
        bool allowed = false;

        map< ObjectAction::Type, bool>::iterator it = m_states.find( action );
        if ( it != m_states.end() )
            allowed = it->second;

        return allowed;
    }
    
    bool AllowableActions::isDefined( ObjectAction::Type action )
    {
        map< ObjectAction::Type, bool>::iterator it = m_states.find( action );
        return it != m_states.end();
    }
}
