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

#ifdef __cplusplus
extern "C" {
#endif

#include "error.h"
#include "folder.h"

typedef struct libcmis_document* libcmis_DocumentPtr;
typedef size_t ( *libcmis_writeFn )( const void*, size_t, size_t, void* );

void libcmis_document_free( libcmis_DocumentPtr document );

libcmis_vector_folder_Ptr libcmis_document_getParents( libcmis_DocumentPtr document, libcmis_ErrorPtr error );

void libcmis_document_getContentStream(
        libcmis_DocumentPtr document,
        libcmis_writeFn writeFn,
        void* userData,
        libcmis_ErrorPtr error );

/* TODO libcmis_document_setContentStream */

/** The resulting value needs to be free'd
  */
char* libcmis_document_getContentType( libcmis_DocumentPtr document );

/** The resulting value needs to be free'd
  */
char* libcmis_document_getContentFilename( libcmis_DocumentPtr document );

long libcmis_document_getContentLength( libcmis_DocumentPtr document );

libcmis_DocumentPtr libcmis_document_checkOut( libcmis_DocumentPtr document, libcmis_ErrorPtr error );
void libcmis_document_cancelCheckout( libcmis_DocumentPtr document, libcmis_ErrorPtr error );

/* TODO libcmis_document_checkIn */

#ifdef __cplusplus
}
#endif

#endif
