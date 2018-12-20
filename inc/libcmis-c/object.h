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
#ifndef _LIBCMIS_OBJECT_H_
#define _LIBCMIS_OBJECT_H_

#include <time.h>

#include "types.h"

#ifdef __cplusplus
extern "C" {
#endif

void libcmis_vector_object_free( libcmis_vector_object_Ptr vector );
size_t libcmis_vector_object_size( libcmis_vector_object_Ptr vector );
libcmis_ObjectPtr libcmis_vector_object_get( libcmis_vector_object_Ptr vector, size_t i );


void libcmis_object_free( libcmis_ObjectPtr object );

/** The resulting value needs to be free'd.
  */
char* libcmis_object_getId( libcmis_ObjectPtr object );

/** The resulting value needs to be free'd.
  */
char* libcmis_object_getName( libcmis_ObjectPtr object );

libcmis_vector_string_Ptr libcmis_object_getPaths( libcmis_ObjectPtr object );


/** The resulting value needs to be free'd.
  */
char* libcmis_object_getBaseType( libcmis_ObjectPtr object );

/** The resulting value needs to be free'd.
  */
char* libcmis_object_getType( libcmis_ObjectPtr object );


/** The resulting value needs to be free'd.
  */
char* libcmis_object_getCreatedBy( libcmis_ObjectPtr object );
time_t libcmis_object_getCreationDate( libcmis_ObjectPtr object );

/** The resulting value needs to be free'd.
  */
char* libcmis_object_getLastModifiedBy( libcmis_ObjectPtr object );
time_t libcmis_object_getLastModificationDate( libcmis_ObjectPtr object );


/** The resulting value needs to be free'd.
  */
char* libcmis_object_getChangeToken( libcmis_ObjectPtr object );
bool libcmis_object_isImmutable( libcmis_ObjectPtr object );

/** The resulting value needs to be free'd.
  */
libcmis_vector_string_Ptr libcmis_object_getSecondaryTypes( libcmis_ObjectPtr object );

/** The resulting value needs to be free'd.
  */
char* libcmis_object_getThumbnailUrl( libcmis_ObjectPtr object );

libcmis_vector_rendition_Ptr libcmis_object_getRenditions( libcmis_ObjectPtr object, libcmis_ErrorPtr error );

libcmis_ObjectPtr
libcmis_object_addSecondaryType( libcmis_ObjectPtr object,
                                 const char* id,
                                 libcmis_vector_property_Ptr properties,
                                 libcmis_ErrorPtr error );

libcmis_ObjectPtr
libcmis_object_removeSecondaryType( libcmis_ObjectPtr object,
                                    const char* id,
                                    libcmis_ErrorPtr error );

libcmis_vector_property_Ptr libcmis_object_getProperties( libcmis_ObjectPtr object );
libcmis_PropertyPtr libcmis_object_getProperty( libcmis_ObjectPtr object, const char* name );
void libcmis_object_setProperty( libcmis_ObjectPtr object, libcmis_PropertyPtr property );
libcmis_ObjectPtr libcmis_object_updateProperties(
        libcmis_ObjectPtr object,
        libcmis_vector_property_Ptr properties,
        libcmis_ErrorPtr error );

libcmis_ObjectTypePtr libcmis_object_getTypeDescription( libcmis_ObjectPtr object );
libcmis_AllowableActionsPtr libcmis_object_getAllowableActions( libcmis_ObjectPtr object );

void libcmis_object_refresh( libcmis_ObjectPtr object, libcmis_ErrorPtr error );
time_t libcmis_object_getRefreshTimestamp( libcmis_ObjectPtr object );

void libcmis_object_remove( libcmis_ObjectPtr object, bool allVersions, libcmis_ErrorPtr error );

void libcmis_object_move( libcmis_ObjectPtr object,
        libcmis_FolderPtr source,
        libcmis_FolderPtr dest,
        libcmis_ErrorPtr error );


/** The resulting value needs to be free'd.
  */
char* libcmis_object_toString( libcmis_ObjectPtr object );

#ifdef __cplusplus
}
#endif

#endif
