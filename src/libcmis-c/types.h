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
#ifndef _LIBCMIS_TYPES_H_
#define _LIBCMIS_TYPES_H_

#ifndef __cplusplus
#include <stdbool.h>
#else
extern "C" {
#endif

#include <stddef.h>


/* Vectors of simple types */


typedef struct libcmis_vector_bool* libcmis_vector_bool_Ptr;

typedef struct libcmis_vector_string* libcmis_vector_string_Ptr;

typedef struct libcmis_vector_long* libcmis_vector_long_Ptr;

typedef struct libcmis_vector_double* libcmis_vector_double_Ptr;

typedef struct libcmis_vector_time* libcmis_vector_time_Ptr;

typedef struct libcmis_vector_repository* libcmis_vector_Repository_Ptr;


/* AllowableActions */

    
typedef struct libcmis_allowable_actions* libcmis_AllowableActionsPtr;

typedef enum 
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
} libcmis_allowable_actions_Type;


/* Document */


typedef struct libcmis_document* libcmis_DocumentPtr;
typedef size_t ( *libcmis_writeFn )( const void*, size_t, size_t, void* );
typedef size_t ( *libcmis_readFn )( void*, size_t, size_t, void* );

typedef struct libcmis_vector_document* libcmis_vector_document_Ptr;

/* Error */

    
typedef struct libcmis_error* libcmis_ErrorPtr;


/* Folder */


typedef struct libcmis_folder* libcmis_FolderPtr;


typedef struct libcmis_vector_folder* libcmis_vector_folder_Ptr;
    
typedef enum
{
    libcmis_Unfile,
    libcmis_DeleteSingleFiled,
    libcmis_Delete
} libcmis_folder_UnfileObjects;


/* ObjectType */


typedef struct libcmis_object_type* libcmis_ObjectTypePtr;

typedef struct libcmis_vector_object_type* libcmis_vector_object_type_Ptr;

typedef enum
{
    libcmis_NotAllowed,
    libcmis_Allowed,
    libcmis_Required
} libcmis_object_type_ContentStreamAllowed;


/* Object */


typedef struct libcmis_object* libcmis_ObjectPtr;

typedef struct libcmis_vector_object* libcmis_vector_object_Ptr;


/* Property */


typedef struct libcmis_property* libcmis_PropertyPtr;

typedef struct libcmis_vector_property* libcmis_vector_property_Ptr;


/* PropertyType */


typedef struct libcmis_property_type* libcmis_PropertyTypePtr;

typedef struct libcmis_vector_property_type* libcmis_vector_property_type_Ptr;

typedef enum
{
    libcmis_String,
    libcmis_Integer,
    libcmis_Decimal,
    libcmis_Bool,
    libcmis_DateTime
} libcmis_property_type_Type;


/* Repository */


typedef struct libcmis_repository* libcmis_RepositoryPtr;


/* Session */


typedef struct libcmis_session* libcmis_SessionPtr;

typedef bool ( *libcmis_authenticationCallback )( char* username, char* password );
typedef bool ( *libcmis_certValidationCallback )( libcmis_vector_string* certificatesChain );
typedef char * ( *libcmis_oauth2AuthCodeProvider ) ( const char* authUrl, const char* username, const char* password );


/* OAuth2Data */


typedef struct libcmis_oauth2data* libcmis_OAuth2DataPtr;

typedef char* ( *libcmis_OAuth2AuthCodeProvider )( const char* authUrl,
        const char* username, const char* password );


/* Rendition */


typedef struct libcmis_rendition* libcmis_RenditionPtr;
typedef struct libcmis_vector_rendition* libcmis_vector_rendition_Ptr;

#ifdef __cplusplus
}
#endif

#endif
