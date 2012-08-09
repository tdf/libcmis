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

#include "internals.hxx"
#include "vectors.h"


void libcmis_vector_bool_free( libcmis_vector_bool_Ptr vector )
{
    delete vector;
}

size_t libcmis_vector_bool_size( libcmis_vector_bool_Ptr vector )
{
    size_t size = 0;
    if ( vector != NULL )
        size = vector->handle.size( );
    return size;
}

bool libcmis_vector_bool_get( libcmis_vector_bool_Ptr vector, size_t i )
{
    bool item = false;
    if ( vector != NULL && i < vector->handle.size( ) )
        item = vector->handle[i];
    return item;
}

void libcmis_vector_string_free( libcmis_vector_string_Ptr vector )
{
    delete vector;
}

size_t libcmis_vector_string_size( libcmis_vector_string_Ptr vector )
{
    size_t size = 0;
    if ( vector != NULL )
        size = vector->handle.size( );
    return size;
}

const char* libcmis_vector_string_get( libcmis_vector_string_Ptr vector, size_t i )
{
    const char* item = NULL;
    if ( vector != NULL && i < vector->handle.size( ) )
        item = vector->handle[i].c_str( );
    return item;
}

void libcmis_vector_long_free( libcmis_vector_long_Ptr vector )
{
    delete vector;
}

size_t libcmis_vector_long_size( libcmis_vector_long_Ptr vector )
{
    size_t size = 0;
    if ( vector != NULL )
        size = vector->handle.size( );
    return size;
}

long libcmis_vector_long_get( libcmis_vector_long_Ptr vector, size_t i )
{
    long item = 0;
    if ( vector != NULL && i < vector->handle.size( ) )
        item = vector->handle[i];
    return item;
}

void libcmis_vector_double_free( libcmis_vector_double_Ptr vector )
{
    delete vector;
}

size_t libcmis_vector_double_size( libcmis_vector_double_Ptr vector )
{
    size_t size = 0;
    if ( vector != NULL )
        size = vector->handle.size( );
    return size;
}

double libcmis_vector_double_get( libcmis_vector_double_Ptr vector, size_t i )
{
    double item = 0.0;
    if ( vector != NULL && i < vector->handle.size( ) )
        item = vector->handle[i];
    return item;
}

void libcmis_vector_time_free( libcmis_vector_time_Ptr vector )
{
    delete vector;
}

size_t libcmis_vector_time_size( libcmis_vector_time_Ptr vector )
{
    size_t size = 0;
    if ( vector != NULL )
        size = vector->handle.size( );
    return size;
}

time_t libcmis_vector_time_get( libcmis_vector_time_Ptr vector, size_t i )
{
    time_t item = 0;
    if ( vector != NULL && i < vector->handle.size( ) )
    {
        tm time = boost::posix_time::to_tm( vector->handle[i] );
        item = mktime( &time );
    }
    return item;
}
