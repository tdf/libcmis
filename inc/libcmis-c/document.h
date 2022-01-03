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
#ifndef _LIBCMIS_DOCUMENT_H_
#define _LIBCMIS_DOCUMENT_H_

#include "libcmis-c/libcmis-c-api.h"
#include "libcmis-c/types.h"

#ifdef __cplusplus
extern "C" {
#endif

LIBCMIS_C_API void libcmis_vector_document_free( libcmis_vector_document_Ptr vector );
LIBCMIS_C_API size_t libcmis_vector_document_size( libcmis_vector_document_Ptr vector );
LIBCMIS_C_API libcmis_DocumentPtr libcmis_vector_document_get( libcmis_vector_document_Ptr vector, size_t i );

LIBCMIS_C_API bool libcmis_is_document( libcmis_ObjectPtr object );
LIBCMIS_C_API libcmis_DocumentPtr libcmis_document_cast( libcmis_ObjectPtr object );

LIBCMIS_C_API void libcmis_document_free( libcmis_DocumentPtr document );

LIBCMIS_C_API libcmis_vector_folder_Ptr libcmis_document_getParents( libcmis_DocumentPtr document, libcmis_ErrorPtr error );

LIBCMIS_C_API void libcmis_document_getContentStream(
        libcmis_DocumentPtr document,
        libcmis_writeFn writeFn,
        void* userData,
        libcmis_ErrorPtr error );

LIBCMIS_C_API void libcmis_document_setContentStream(
        libcmis_DocumentPtr document,
        libcmis_readFn readFn,
        void* userData,
        const char* contentType,
        const char* filename,
        bool overwrite,
        libcmis_ErrorPtr );

/** The resulting value needs to be free'd
  */
LIBCMIS_C_API char* libcmis_document_getContentType( libcmis_DocumentPtr document );

/** The resulting value needs to be free'd
  */
LIBCMIS_C_API char* libcmis_document_getContentFilename( libcmis_DocumentPtr document );

LIBCMIS_C_API long libcmis_document_getContentLength( libcmis_DocumentPtr document );

LIBCMIS_C_API libcmis_DocumentPtr libcmis_document_checkOut( libcmis_DocumentPtr document, libcmis_ErrorPtr error );
LIBCMIS_C_API void libcmis_document_cancelCheckout( libcmis_DocumentPtr document, libcmis_ErrorPtr error );

LIBCMIS_C_API libcmis_DocumentPtr libcmis_document_checkIn(
        libcmis_DocumentPtr document,
        bool isMajor,
        const char* comment,
        libcmis_vector_property_Ptr properties,
        libcmis_readFn readFn,
        void* userData,
        const char* contentType,
        const char* filename,
        libcmis_ErrorPtr error );

LIBCMIS_C_API libcmis_vector_document_Ptr libcmis_document_getAllVersions(
        libcmis_DocumentPtr document,
        libcmis_ErrorPtr error );

#ifdef __cplusplus
}
#endif

#endif
