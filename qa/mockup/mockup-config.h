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

#include <curl/curl.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* Mockup behavior configuration functions */
void curl_mockup_reset( );

/** Add a new HTTP response the server should send.

    \param baseURL
        the base URL of the request without parameters
    \param matchParam
        a string to find in the parameters part of the URL to match
    \param response
        a string corresponding either to the file path of the request
        body to send or directly the content to send. This value has
        a different meaning depending on isFilePath parameter.
    \param method
        HTTP method to match like PUT, GET, POST or DELETE. An empty
        string matches any method.
    \param status
        the HTTP status to return. 0 means HTTP OK (200).
    \param isFilePath
        if this value is true the response value is used as a file path,
        otherwise, the response value is used as the body of the HTTP
        response to send.
    \param headers
        the HTTP headers block to send with the response. By default
        no header is sent.
    \param matchBody
        a string to find in the request body to match
  */
void curl_mockup_addResponse( const char* baseUrl, const char* matchParam, const char* method,
                              const char* response, unsigned int status = 0, bool isFilePath = true,
                              const char* headers = 0, const char* matchBody = 0 );

/** Set the HTTP response the server is supposed to send.
    This will reset all already defined responses.
 */
void curl_mockup_setResponse( const char* filepath );
void curl_mockup_setCredentials( const char* username, const char* password );

struct HttpRequest
{
    const char* url;
    const char* body;
    ///< NULL terminated array of headers.
    const char** headers;
};

const struct HttpRequest* curl_mockup_getRequest( const char* baseUrl,
                                                  const char* matchParam,
                                                  const char* method,
                                                  const char* matchBody = 0 );
const char* curl_mockup_getRequestBody( const char* baseUrl,
                                        const char* matchParam,
                                        const char* method,
                                        const char* matchBody = 0 );
int curl_mockup_getRequestsCount( const char* urlBase,
                                  const char* matchParam,
                                  const char* method,
                                  const char* matchBody = "" );

void curl_mockup_HttpRequest_free( const struct HttpRequest* request );

/** The resulting value is either NULL (no such header found) or the value
    of the header. In such a case, the result needs to be freed by the caller.
  */
char* curl_mockup_HttpRequest_getHeader( const struct HttpRequest* request, const char* name );

const char* curl_mockup_getProxy( CURL* handle );
const char* curl_mockup_getNoProxy( CURL* handle );
const char* curl_mockup_getProxyUser( CURL* handle );
const char* curl_mockup_getProxyPass( CURL* handle );

/** Set a fake invalid certificate to raise CURLE_SSL_CACERT. Setting it
    to an empty string will reset to no certificate.
  */
void curl_mockup_setSSLBadCertificate( const char* certificate );

#ifdef __cplusplus
}
#endif
