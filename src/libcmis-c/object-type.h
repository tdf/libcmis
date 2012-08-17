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
#ifndef _LIBCMIS_OBJECT_TYPE_H_
#define _LIBCMIS_OBJECT_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"


void libcmis_vector_object_type_free( libcmis_vector_object_type_Ptr vector );
size_t libcmis_vector_object_type_size( libcmis_vector_object_type_Ptr vector );
libcmis_ObjectTypePtr libcmis_vector_object_type_get( libcmis_vector_object_type_Ptr vector, size_t i );


void libcmis_object_type_free( libcmis_ObjectTypePtr type );

/** The resulting value needs to be freed
  */
char* libcmis_object_type_getId( libcmis_ObjectTypePtr type );

/** The resulting value needs to be freed
  */
char* libcmis_object_type_getLocalName( libcmis_ObjectTypePtr type );

/** The resulting value needs to be freed
  */
char* libcmis_object_type_getLocalNamespace( libcmis_ObjectTypePtr type );

/** The resulting value needs to be freed
  */
char* libcmis_object_type_getQueryName( libcmis_ObjectTypePtr type );

/** The resulting value needs to be freed
  */
char* libcmis_object_type_getDisplayName( libcmis_ObjectTypePtr type );

/** The resulting value needs to be freed
  */
char* libcmis_object_type_getDescription( libcmis_ObjectTypePtr type );

libcmis_ObjectTypePtr libcmis_object_type_getParentType(
        libcmis_ObjectTypePtr type,
        libcmis_ErrorPtr error );
libcmis_ObjectTypePtr libcmis_object_type_getBaseType(
        libcmis_ObjectTypePtr type,
        libcmis_ErrorPtr error );

libcmis_vector_object_type_Ptr libcmis_object_type_getChildren(
        libcmis_ObjectTypePtr type,
        libcmis_ErrorPtr error );

bool libcmis_object_type_isCreatable( libcmis_ObjectTypePtr type );
bool libcmis_object_type_isFileable( libcmis_ObjectTypePtr type );
bool libcmis_object_type_isQueryable( libcmis_ObjectTypePtr type );
bool libcmis_object_type_isFulltextIndexed( libcmis_ObjectTypePtr type );
bool libcmis_object_type_isIncludedInSupertypeQuery( libcmis_ObjectTypePtr type );
bool libcmis_object_type_isControllablePolicy( libcmis_ObjectTypePtr type );
bool libcmis_object_type_isControllableACL( libcmis_ObjectTypePtr type );
bool libcmis_object_type_isVersionable( libcmis_ObjectTypePtr type );

libcmis_object_type_ContentStreamAllowed libcmis_object_type_getContentStreamAllowed( libcmis_ObjectTypePtr type );

libcmis_vector_property_type_Ptr libcmis_object_type_getPropertiesTypes( libcmis_ObjectTypePtr type );
libcmis_PropertyTypePtr libcmis_object_type_getPropertyType( libcmis_ObjectTypePtr type, const char* id );


/** The resulting value needs to be freed
  */
char* libcmis_object_type_toString( libcmis_ObjectTypePtr type );

#ifdef __cplusplus
}
#endif

#endif
