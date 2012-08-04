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
#ifndef _LIBCMIS_ALLOWABLE_ACTIONS_H_
#define _LIBCMIS_ALLOWABLE_ACTIONS_H_

typedef struct libcmis_allowable_actions* libcmis_AllowableActionsPtr;

enum libcmis_allowable_actions_Type
{
    libcmis_DeleteObject,
    libcmis_UpdateProperties,
    libcmis_GetFolderTree,
    libcmis_GetProperties,
    libcmis_GetObjectRelationships,
    libcmis_GetObjectParents,
    libcmis_GetFolderParent,
    libcmis_GetDescendants,
    libcmis_MoveObject,
    libcmis_DeleteContentStream,
    libcmis_CheckOut,
    libcmis_CancelCheckOut,
    libcmis_CheckIn,
    libcmis_SetContentStream,
    libcmis_GetAllVersions,
    libcmis_AddObjectToFolder,
    libcmis_RemoveObjectFromFolder,
    libcmis_GetContentStream,
    libcmis_ApplyPolicy,
    libcmis_GetAppliedPolicies,
    libcmis_RemovePolicy,
    libcmis_GetChildren,
    libcmis_CreateDocument,
    libcmis_CreateFolder,
    libcmis_CreateRelationship,
    libcmis_DeleteTree,
    libcmis_GetRenditions,
    libcmis_GetACL,
    libcmis_ApplyACL
};

void libcmis_allowable_actions_free( libcmis_AllowableActionsPtr allowable );

bool libcmis_allowable_actions_isAllowed( libcmis_AllowableActionsPtr allowable,
        libcmis_allowable_actions_Type action );

bool libcmis_allowable_actions_isDefined( libcmis_AllowableActionsPtr allowable,
        libcmis_allowable_actions_Type action );

#endif
