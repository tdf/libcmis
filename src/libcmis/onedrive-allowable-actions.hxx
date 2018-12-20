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
 * Copyright (C) 2014 Varga Mihai <mihai.mv13@gmail.com> 
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

#ifndef _ONEDRIVE_ALLOWABLE_ACTIONS_HXX_
#define _ONEDRIVE_ALLOWABLE_ACTIONS_HXX_

#include <libcmis/allowable-actions.hxx>

class OneDriveAllowableActions: public libcmis::AllowableActions
{
    public:
        OneDriveAllowableActions( bool isFolder ) : AllowableActions( )
        {
            m_states.clear( );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::DeleteObject, true ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::UpdateProperties, true ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetProperties, true ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetObjectRelationships, false ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetObjectParents, true ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::MoveObject, true ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CreateRelationship, false ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::ApplyPolicy, false ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetAppliedPolicies, false ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::RemovePolicy, false ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetACL, false ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::ApplyACL, false ) );

            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetFolderTree, isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetFolderParent, isFolder) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetDescendants, isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::DeleteContentStream, !isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CheckOut, !isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CancelCheckOut, !isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CheckIn, !isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetContentStream, !isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::SetContentStream, !isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetAllVersions, false ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::AddObjectToFolder, !isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::RemoveObjectFromFolder, !isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetRenditions, !isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::GetChildren, isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CreateDocument, isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::CreateFolder, isFolder ) );
            m_states.insert( std::pair< libcmis::ObjectAction::Type, bool> (
                             libcmis::ObjectAction::DeleteTree, isFolder ) );
        }
};

#endif
