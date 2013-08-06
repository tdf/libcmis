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

#include <map>
#include <string>
#include <vector>

typedef size_t ( *write_callback )( char *ptr, size_t size, size_t nmemb, void *userdata );
typedef size_t ( *read_callback )( char *ptr, size_t size, size_t nmemb, void *userdata );
typedef size_t ( *headers_callback )( char *ptr, size_t size, size_t nmemb, void *userdata );

class CurlHandle
{
    public:
        CurlHandle( );
        CurlHandle( const CurlHandle& copy );
        CurlHandle& operator=( const CurlHandle& copy );
        
        std::string m_url;

        write_callback m_writeFn;
        void* m_writeData;
        read_callback m_readFn;
        void* m_readData;
        long m_readSize;
        headers_callback m_headersFn;
        void* m_headersData;

        std::string m_username;
        std::string m_password;
        std::string m_proxy;
        std::string m_noProxy;
        std::string m_proxyUser;
        std::string m_proxyPass;

        bool m_verifyHost;
        bool m_verifyPeer;
        bool m_certInfo;

        struct curl_certinfo m_certs;

        long m_httpError;
        std::string m_method;
       
        void reset( ); 
};

namespace mockup
{
    class Response
    {
        public:
            Response( std::string response, unsigned int status, bool isFilePath,
                      std::string headers );

            std::string m_response;
            unsigned int m_status;
            bool m_isFilePath;
            std::string m_headers;
    };

    class Request
    {
        public:
            Request( std::string url, std::string method, std::string body );

            std::string m_url;
            std::string m_method;
            std::string m_body;
    };

    class RequestMatcher
    {
        public:
            RequestMatcher( std::string baseUrl, std::string matchParam, std::string method );
            bool operator< ( const RequestMatcher& compare ) const;

            std::string m_baseUrl;
            std::string m_matchParam;
            std::string m_method;
    };

    class Configuration
    {
        public:
            Configuration( );

            bool hasCredentials( );
            CURLcode writeResponse( CurlHandle* handle );

            std::map< RequestMatcher, Response > m_responses;
            std::vector< Request > m_requests;
            std::string m_username;
            std::string m_password;
            std::string m_badSSLCertificate;
    };
}
