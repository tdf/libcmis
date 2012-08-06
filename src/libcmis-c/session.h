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
#ifndef _LIBCMIS_SESSION_H_
#define _LIBCMIS_SESSION_H_

#include "error.h"
#include "object.h"
#include "repository.h"

typedef struct libcmis_session* libcmis_SessionPtr;

typedef bool ( *libcmis_authenticationCallback )( char* username, char* password );


void libcmis_session_free( libcmis_SessionPtr session );

libcmis_RepositoryPtr libcmis_session_getRepository(
        libcmis_SessionPtr session,
        libcmis_ErrorPtr error );

/* TODO libcmis_FolderPtr libcmis_session_getRootFolder( libcmis_SessionPtr session, libcmis_ErrorPtr error );*/

libcmis_ObjectPtr libcmis_session_getObject(
        libcmis_SessionPtr session,
        char* id,
        libcmis_ErrorPtr error );

libcmis_ObjectPtr libcmis_session_getObjectByPath(
        libcmis_SessionPtr session,
        char* path,
        libcmis_ErrorPtr error );

/* TODO libcmis_FolderPtr libcmis_session_getFolder( libcmis_SessionPtr session, char* id, libcmis_ErrorPtr error );*/

libcmis_ObjectTypePtr libcmis_session_getType(
        libcmis_SessionPtr session,
        char* id,
        libcmis_ErrorPtr error );

void libcmis_session_setAuthenticationCallback(
        libcmis_SessionPtr session,
        libcmis_authenticationCallback callback );

#endif
