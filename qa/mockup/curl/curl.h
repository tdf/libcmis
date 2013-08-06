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
#ifndef _MOCKUP_CURL_CURL_H_
#define _MOCKUP_CURL_CURL_H_

#ifdef  __cplusplus
extern "C" {
#endif

/* Curl used symbols to mockup */

typedef void CURL;

typedef enum
{
  CURLIOE_OK,            /* I/O operation successful */
  CURLIOE_UNKNOWNCMD,    /* command was unknown to callback */
  CURLIOE_FAILRESTART,   /* failed to restart the read */
  CURLIOE_LAST           /* never use */
} curlioerr;

#define CURL_GLOBAL_SSL (1<<0)
#define CURL_GLOBAL_WIN32 (1<<1)
#define CURL_GLOBAL_ALL (CURL_GLOBAL_SSL|CURL_GLOBAL_WIN32)

#define CURLOPTTYPE_LONG          0
#define CURLOPTTYPE_OBJECTPOINT   10000
#define CURLOPTTYPE_FUNCTIONPOINT 20000
#define CURLOPTTYPE_OFF_T         30000

typedef enum
{
    CURLOPT_WRITEFUNCTION = CURLOPTTYPE_FUNCTIONPOINT + 11,
    CURLOPT_READFUNCTION =  CURLOPTTYPE_FUNCTIONPOINT + 12,
    CURLOPT_WRITEDATA = CURLOPTTYPE_OBJECTPOINT + 1,
    CURLOPT_HEADERFUNCTION = CURLOPTTYPE_FUNCTIONPOINT + 79,
    CURLOPT_WRITEHEADER = CURLOPTTYPE_OBJECTPOINT + 29,
    CURLOPT_FOLLOWLOCATION = CURLOPTTYPE_LONG + 52,
    CURLOPT_MAXREDIRS = CURLOPTTYPE_LONG + 68,
    CURLOPT_INFILESIZE =  CURLOPTTYPE_LONG + 14,
    CURLOPT_READDATA = CURLOPTTYPE_OBJECTPOINT + 9,
    CURLOPT_UPLOAD = CURLOPTTYPE_LONG + 46,
    CURLOPT_IOCTLFUNCTION = CURLOPTTYPE_FUNCTIONPOINT + 130,
    CURLOPT_IOCTLDATA = CURLOPTTYPE_OBJECTPOINT + 131,
    CURLOPT_HTTPHEADER = CURLOPTTYPE_OBJECTPOINT + 23,
    CURLOPT_POSTFIELDSIZE = CURLOPTTYPE_LONG + 60,
    CURLOPT_POST = CURLOPTTYPE_LONG + 47,
    CURLOPT_CUSTOMREQUEST = CURLOPTTYPE_OBJECTPOINT + 36,
    CURLOPT_URL =     CURLOPTTYPE_OBJECTPOINT + 2,
    CURLOPT_HTTPAUTH = CURLOPTTYPE_LONG + 107,
    CURLOPT_USERNAME = CURLOPTTYPE_OBJECTPOINT + 173,
    CURLOPT_PASSWORD = CURLOPTTYPE_OBJECTPOINT + 174,
    CURLOPT_USERPWD = CURLOPTTYPE_OBJECTPOINT + 5,
    CURLOPT_ERRORBUFFER = CURLOPTTYPE_OBJECTPOINT + 10,
    CURLOPT_FAILONERROR = CURLOPTTYPE_LONG + 45,
    CURLOPT_VERBOSE = CURLOPTTYPE_LONG + 41,
    CURLOPT_PROXY = CURLOPTTYPE_OBJECTPOINT + 4,
    CURLOPT_PROXYUSERPWD = CURLOPTTYPE_OBJECTPOINT + 6,
    CURLOPT_PROXYAUTH = CURLOPTTYPE_LONG + 111,
    CURLOPT_PROXYUSERNAME = CURLOPTTYPE_OBJECTPOINT + 175,
    CURLOPT_PROXYPASSWORD = CURLOPTTYPE_OBJECTPOINT + 176,
    CURLOPT_NOPROXY = CURLOPTTYPE_OBJECTPOINT + 177,
    CURLOPT_SSL_VERIFYPEER = CURLOPTTYPE_LONG + 64,
    CURLOPT_SSL_VERIFYHOST = CURLOPTTYPE_LONG + 81,
    CURLOPT_CERTINFO = CURLOPTTYPE_LONG + 172
} CURLoption;

#define CURLAUTH_DIGEST_IE    (((unsigned long)1)<<4)
#define CURLAUTH_ANY          (~CURLAUTH_DIGEST_IE)

typedef enum
{
  CURLE_OK = 0,
  CURLE_HTTP_RETURNED_ERROR = 22,
  CURLE_SSL_CACERT = 60,
  /* TODO Add some more error codes from curl? */
  CURL_LAST
} CURLcode;

struct curl_slist
{
  char *data;
  struct curl_slist *next;
};

struct curl_slist *curl_slist_append( struct curl_slist *, const char * );
void curl_slist_free_all( struct curl_slist * );

void curl_free( void *p );
CURLcode curl_global_init( long flags );

CURL *curl_easy_init( void );
void curl_easy_cleanup( CURL *curl );
CURLcode curl_easy_setopt( CURL *curl, CURLoption option, ... );
char *curl_easy_escape( CURL *handle, const char *string, int length );
char *curl_unescape( const char *string, int length );
char *curl_easy_unescape( CURL *handle, const char *string, int length, int *outlength );
CURLcode curl_easy_perform( CURL *curl );
void curl_easy_reset( CURL *curl );

struct curl_certinfo
{
    int num_of_certs;
    struct curl_slist **certinfo;
};

#define CURLINFO_LONG     0x200000
#define CURLINFO_SLIST    0x400000

typedef enum
{
  CURLINFO_NONE,
  CURLINFO_RESPONSE_CODE    = CURLINFO_LONG   + 2,
  CURLINFO_CERTINFO         = CURLINFO_SLIST  + 34,
  CURLINFO_LASTONE          = 42
} CURLINFO;

CURLcode curl_easy_getinfo( CURL *curl, CURLINFO info, ... );

#define LIBCURL_VERSION_MAJOR 7
#define LIBCURL_VERSION_MINOR 26
#define LIBCURL_VERSION_PATCH 0

#ifdef __cplusplus
}
#endif

#endif
