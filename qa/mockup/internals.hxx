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

typedef size_t ( *write_callback )( char *ptr, size_t size, size_t nmemb, void *userdata );

class CurlHandle
{
    public:
        CurlHandle( );
        CurlHandle( const CurlHandle& copy );
        CurlHandle& operator=( const CurlHandle& copy );
        
        std::string m_url;

        write_callback m_writeFn;
        void* m_writeData;

        std::string m_username;
        std::string m_password;

        long m_httpError;
       
        void reset( ); 
};

namespace mockup
{
    class Response
    {
        public:
            Response( std::string filepath, std::string matchParam );

            std::string m_filepath;
            std::string m_matchParam;
    };

    class Configuration
    {
        public:
            Configuration( );

            bool hasCredentials( );
            std::string getResponse( CurlHandle* handle );

            std::map< std::string, Response > m_responses;
            std::string m_username;
            std::string m_password;
    };
}
