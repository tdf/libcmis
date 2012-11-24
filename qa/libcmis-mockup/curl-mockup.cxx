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

#include <string.h>

#include "curl/curl.h"

struct curl_slist *curl_slist_append( struct curl_slist *, const char * )
{
    /* TODO Implement me */
    return NULL;
}

void curl_slist_free_all( struct curl_slist * )
{
    /* TODO Implement me */
}

void curl_free( void * /*p*/ )
{
    /* TODO Implement me */
}

CURLcode curl_global_init( long )
{
    return CURLE_OK;
}

CURL *curl_easy_init( void )
{
    return NULL;
}

void curl_easy_cleanup( CURL * )
{
}

CURLcode curl_easy_setopt( CURL *, CURLoption /*option*/, ... )
{
    /* TODO Implement me */
    return CURLE_OK;
}

char *curl_easy_escape( CURL *, const char *string, int length )
{
    return strndup( string, length );
}

char *curl_unescape( const char *string, int length )
{
    return strndup( string, length );
}

char *curl_easy_unescape( CURL *, const char *string, int length, int * )
{
    return curl_unescape( string, length );
}

CURLcode curl_easy_perform( CURL * )
{
    return CURLE_OK;
}

void curl_easy_reset( CURL * )
{
}

CURLcode curl_easy_getinfo( CURL *, CURLINFO /*info*/, ... )
{
    /* TODO Implement me */
    return CURLE_OK;
}
