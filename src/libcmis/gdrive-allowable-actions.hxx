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
 * Copyright (C) 2013 Cao Cuong Ngo <cao.cuong.ngo@gmail.com>
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

#ifndef _GDRIVE_ALLOWABLE_ACTIONS_HXX_
#define _GDRIVE_ALLOWABLE_ACTIONS_HXX_

#include <allowable-actions.hxx>

class GdriveAllowableActions: public libcmis::AllowableActions
{
    public:
        GdriveAllowableActions( bool isFolder ) : AllowableActions( )
        {
            m_states.clear( );
            m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::DeleteObject, true ) );
            m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::UpdateProperties, true ) );
            m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetProperties, true ) );
            m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetObjectRelationships, false ) );
            m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetObjectParents, true ) );
            m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::MoveObject, true ) );


            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetFolderTree, true ) );
            else
                 m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetFolderTree, false ) );  
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetFolderParent, true ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetFolderParent, false ) );
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetDescendants, true ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetDescendants, false ) );
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::DeleteContentStream, false ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::DeleteContentStream, true ) ); 
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CheckOut, false ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CheckOut, true ) );
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CancelCheckOut, false ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CancelCheckOut, true ) );            
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CheckIn, false ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CheckIn, true ) ); 
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetContentStream, false ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetContentStream, true ) );

            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::SetContentStream, false ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::SetContentStream, true ) );
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetAllVersions, false ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetAllVersions, true ) ); 
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::AddObjectToFolder, false ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::AddObjectToFolder, true ) ); 
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::RemoveObjectFromFolder, false ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::RemoveObjectFromFolder, true ) );
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetRenditions, false ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetRenditions, true ) );
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetChildren, true ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetChildren, false ) );
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CreateDocument, true ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CreateDocument, false ) );
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CreateFolder, true ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CreateFolder, false ) );
            if ( isFolder )
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::DeleteTree, true ) );
            else
                m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::DeleteTree, false ) );

            m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CreateRelationship, false ) );
            m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::ApplyPolicy, false ) );
            m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetAppliedPolicies, false ) );
            m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::RemovePolicy, false ) );
            m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetACL, true ) );
            m_states.insert( pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::ApplyACL, true ) );
        }
};

#endif
