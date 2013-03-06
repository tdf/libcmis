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
#ifndef _LIBCMIS_OAUTH2_DATA_HXX_
#define _LIBCMIS_OAUTH2_DATA_HXX_

#include <string>
#include <boost/shared_ptr.hpp>

namespace libcmis
{
    typedef char* ( *OAuth2AuthCodeProvider )( const char* authUrl,
        const char* username, const char* password );

    /** Class storing the data needed for OAuth2 authentication.
      */
    class OAuth2Data
    {
        private:

            std::string m_authUrl;
            std::string m_tokenUrl;
            std::string m_clientId;
            std::string m_clientSecret;
            std::string m_scopes;
            std::string m_redirectUri;

            OAuth2AuthCodeProvider m_authCodeProvider;

        public:

            OAuth2Data( );
            OAuth2Data( std::string authUrl, std::string tokenUrl,
                           std::string scopes, std::string redirectUri,
                           std::string clientId, std::string clientSecret,
                           OAuth2AuthCodeProvider authCodeProvider = NULL );

            OAuth2Data( const OAuth2Data& copy );
            ~OAuth2Data( );

            OAuth2Data& operator=( const OAuth2Data& copy );

            bool isComplete();

            std::string getAuthUrl() { return m_authUrl; }
            std::string getTokenUrl() { return m_tokenUrl; }
            std::string getClientId() { return m_clientId; }
            std::string getClientSecret() { return m_clientSecret; }
            std::string getScopes() { return m_scopes; }
            std::string getRedirectUri() { return m_redirectUri; }

            OAuth2AuthCodeProvider getAuthCodeProvider() { return m_authCodeProvider; }
    };
    typedef ::boost::shared_ptr< OAuth2Data > OAuth2DataPtr;
}

#endif
