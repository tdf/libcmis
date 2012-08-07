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

#include "allowable-actions.h"
#include "error.h"
#include "object-type.h"
#include "property.h"

typedef struct libcmis_object* libcmis_ObjectPtr;

void libcmis_object_free( libcmis_ObjectPtr object );
void libcmis_object_list_free( libcmis_ObjectPtr* list );

const char* libcmis_object_getId( libcmis_ObjectPtr object );
const char* libcmis_object_getName( libcmis_ObjectPtr object );

const char** libcmis_object_getPaths( libcmis_ObjectPtr object );

const char* libcmis_object_getBaseType( libcmis_ObjectPtr object );
const char* libcmis_object_getType( libcmis_ObjectPtr object );

const char* libcmis_object_getCreatedBy( libcmis_ObjectPtr object );
time_t libcmis_object_getCreationDate( libcmis_ObjectPtr object );
const char* libcmis_object_getLastModifiedBy( libcmis_ObjectPtr object );
time_t libcmis_object_getLastModificationDate( libcmis_ObjectPtr object );

const char* libcmis_object_getChangeToken( libcmis_ObjectPtr object );
bool libcmis_object_isImmutable( libcmis_ObjectPtr object );

libcmis_PropertyPtr* libcmis_object_getProperties( libcmis_ObjectPtr object );
libcmis_PropertyPtr libcmis_object_getProperty( libcmis_ObjectPtr object, char* name );
void libcmis_object_setProperty( libcmis_ObjectPtr object, libcmis_PropertyPtr property );
void libcmis_object_removeProperty( libcmis_ObjectPtr object, char* name );
void libcmis_object_clearProperties( libcmis_ObjectPtr object );
void libcmis_object_updateProperties( libcmis_ObjectPtr object, libcmis_ErrorPtr error );

libcmis_ObjectTypePtr libcmis_object_getTypeDescription( libcmis_ObjectPtr object );
libcmis_AllowableActionsPtr libcmis_object_getAllowableActions( libcmis_ObjectPtr object );

void libcmis_object_refresh( libcmis_ObjectPtr object, libcmis_ErrorPtr error );
time_t libcmis_object_getRefreshTimestamp( libcmis_ObjectPtr object );

void libcmis_object_remove( libcmis_ObjectPtr object, bool allVersions, libcmis_ErrorPtr error );

const char* libcmis_object_toString( libcmis_ObjectPtr object );

#endif
