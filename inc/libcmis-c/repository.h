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
#ifndef _REPOSITORY_H_
#define _REPOSITORY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <libxml/tree.h>

#include "types.h"

void libcmis_vector_repository_free( libcmis_vector_Repository_Ptr vector );
size_t libcmis_vector_repository_size( libcmis_vector_Repository_Ptr vector );
libcmis_RepositoryPtr libcmis_vector_repository_get( libcmis_vector_Repository_Ptr vector, size_t i );


libcmis_RepositoryPtr libcmis_repository_create( xmlNodePtr node );

void libcmis_repository_free( libcmis_RepositoryPtr repository );

/** The resulting value needs to be freed.
  */
char* libcmis_repository_getId( libcmis_RepositoryPtr repository );

/** The resulting value needs to be freed.
  */
char* libcmis_repository_getName( libcmis_RepositoryPtr repository );

/** The resulting value needs to be freed.
  */
char* libcmis_repository_getDescription( libcmis_RepositoryPtr repository );

/** The resulting value needs to be freed.
  */
char* libcmis_repository_getVendorName( libcmis_RepositoryPtr repository );

/** The resulting value needs to be freed.
  */
char* libcmis_repository_getProductName( libcmis_RepositoryPtr repository );

/** The resulting value needs to be freed.
  */
char* libcmis_repository_getProductVersion( libcmis_RepositoryPtr repository );

/** The resulting value needs to be freed.
  */
char* libcmis_repository_getRootId( libcmis_RepositoryPtr repository );

/** The resulting value needs to be freed.
  */
char* libcmis_repository_getCmisVersionSupported( libcmis_RepositoryPtr repository );

/** The resulting value needs to be freed.
  */
char* libcmis_repository_getThinClientUri( libcmis_RepositoryPtr repository );

/** The resulting value needs to be freed.
  */
char* libcmis_repository_getPrincipalAnonymous( libcmis_RepositoryPtr repository );

/** The resulting value needs to be freed.
  */
char* libcmis_repository_getPrincipalAnyone( libcmis_RepositoryPtr repository );

/** The resulting value needs to be freed.
  */
char* libcmis_repository_getCapability(
        libcmis_RepositoryPtr repository,
        libcmis_repository_capability_Type capability );

bool libcmis_repository_getCapabilityAsBool(
        libcmis_RepositoryPtr repository,
        libcmis_repository_capability_Type capability );

#ifdef __cplusplus
}
#endif

#endif
