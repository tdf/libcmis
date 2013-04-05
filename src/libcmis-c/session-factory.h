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
#ifndef _SESSION_FACTORY_H_
#define _SESSION_FACTORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

void libcmis_setAuthenticationCallback( libcmis_authenticationCallback callback );

void libcmis_setProxySettings(
        char* proxy,
        char* noProxy,
        char* proxyUser,
        char* proxyPass );

const char* libcmis_getProxy( );
const char* libcmis_getNoProxy( );
const char* libcmis_getProxyUser( );
const char* libcmis_getProxyPass( );

libcmis_SessionPtr libcmis_createSession(
        char* bindingUrl,
        char* repositoryId,
        char* username,
        char* password,
        libcmis_OAuth2DataPtr oauth2,
        bool  verbose,
        libcmis_ErrorPtr error );

/**
    \deprecated
        use libcmis_createSession and libcmis_session_getRepositories instead
  */
libcmis_vector_Repository_Ptr libcmis_getRepositories(
        char* bindingUrl,
        char* username,
        char* password,
        bool  verbose,
        libcmis_ErrorPtr error );

#ifdef __cplusplus
}
#endif

#endif
