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
 * Copyright (C) 2013 Cao Cuong Ngo <cao.cuong.ngo@gmail.com>
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

#ifndef _OAUTH2_PROVIDERS_HXX_
#define _OAUTH2_PROVIDERS_HXX_

#include <string>

class HttpSession;

typedef std::string ( *OAuth2Parser ) ( HttpSession* session, const std::string& authUrl, 
                                       const std::string& username, const std::string& password );

class OAuth2Providers
{
    public :
        static std::string OAuth2Dummy( HttpSession* session, const std::string& authUrl,
                                       const std::string& username, const std::string& password );
        static std::string OAuth2Alfresco( HttpSession* session, const std::string& authUrl, 
                                       const std::string& username, const std::string& password );

        static OAuth2Parser getOAuth2Parser( const std::string& baseUrl );

        /*
        * Parse the authorization code from the response page
        * in the input tag, with id = code
        */
        static std::string parseCode ( const char* response );

        /*
         * Parse input values and redirect link from the response page
         */
        static int parseResponse ( const char* response, 
                                   std::string& post, 
                                   std::string& link );
};

#endif /* _OAUTH2_PROVIDERS_HXX_ */
