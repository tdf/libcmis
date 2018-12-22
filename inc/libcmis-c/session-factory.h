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

#include "libcmis-c/libcmis-c-api.h"
#include "libcmis-c/types.h"

LIBCMIS_C_API void libcmis_setAuthenticationCallback( libcmis_authenticationCallback callback );
LIBCMIS_C_API void libcmis_setCertValidationCallback( libcmis_certValidationCallback callback );
LIBCMIS_C_API void libcmis_setOAuth2AuthCodeProvider( libcmis_oauth2AuthCodeProvider callback );
LIBCMIS_C_API libcmis_oauth2AuthCodeProvider libcmis_getOAuth2AuthCodeProvider( );

LIBCMIS_C_API void libcmis_setProxySettings(
        char* proxy,
        char* noProxy,
        char* proxyUser,
        char* proxyPass );

LIBCMIS_C_API const char* libcmis_getProxy( );
LIBCMIS_C_API const char* libcmis_getNoProxy( );
LIBCMIS_C_API const char* libcmis_getProxyUser( );
LIBCMIS_C_API const char* libcmis_getProxyPass( );

LIBCMIS_C_API libcmis_SessionPtr libcmis_createSession(
        char* bindingUrl,
        char* repositoryId,
        char* username,
        char* password,
        bool noSslCheck,
        libcmis_OAuth2DataPtr oauth2,
        bool  verbose,
        libcmis_ErrorPtr error );

/**
    \deprecated
        use libcmis_createSession and libcmis_session_getRepositories instead
  */
LIBCMIS_C_API libcmis_vector_Repository_Ptr libcmis_getRepositories(
        char* bindingUrl,
        char* username,
        char* password,
        bool  verbose,
        libcmis_ErrorPtr error );

#ifdef __cplusplus
}
#endif

#endif
