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
#ifndef _LIBCMIS_VECTORS_HXX_
#define _LIBCMIS_VECTORS_HXX_

#ifndef __cplusplus
#include <stdbool.h>
#else
extern "C" {
#endif

typedef struct libcmis_vector_bool* libcmis_vector_bool_Ptr;

void libcmis_vector_bool_free( libcmis_vector_bool_Ptr vector );
size_t libcmis_vector_bool_size( libcmis_vector_bool_Ptr vector );
bool libcmis_vector_bool_get( libcmis_vector_bool_Ptr vector, size_t i );


typedef struct libcmis_vector_string* libcmis_vector_string_Ptr;

void libcmis_vector_string_free( libcmis_vector_string_Ptr vector );
size_t libcmis_vector_string_size( libcmis_vector_string_Ptr vector );
const char* libcmis_vector_string_get( libcmis_vector_string_Ptr vector, size_t i );


typedef struct libcmis_vector_long* libcmis_vector_long_Ptr;

void libcmis_vector_long_free( libcmis_vector_long_Ptr vector );
size_t libcmis_vector_long_size( libcmis_vector_long_Ptr vector );
long libcmis_vector_long_get( libcmis_vector_long_Ptr vector, size_t i );


typedef struct libcmis_vector_double* libcmis_vector_double_Ptr;

void libcmis_vector_double_free( libcmis_vector_double_Ptr vector );
size_t libcmis_vector_double_size( libcmis_vector_double_Ptr vector );
double libcmis_vector_double_get( libcmis_vector_double_Ptr vector, size_t i );


typedef struct libcmis_vector_time* libcmis_vector_time_Ptr;

void libcmis_vector_time_free( libcmis_vector_time_Ptr vector );
size_t libcmis_vector_time_size( libcmis_vector_time_Ptr vector );
time_t libcmis_vector_time_get( libcmis_vector_time_Ptr vector, size_t i );

#ifdef __cplusplus
}
#endif

#endif
