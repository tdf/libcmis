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
#ifndef _OAUTH2_HANDLER_HXX_
#define _OAUTH2_HANDLER_HXX_

#include <string>
#include "http-session.hxx"
#include "oauth2-providers.hxx"

namespace libcmis
{
    class OAuth2Data;
}

class OAuth2Handler
{
    private:
        HttpSession* m_session;
        libcmis::OAuth2DataPtr m_data;

        std::string m_access;
        std::string m_refresh;

        OAuth2Parser m_oauth2Parser;

    public:

        OAuth2Handler( HttpSession* session, libcmis::OAuth2DataPtr data );

        OAuth2Handler( const OAuth2Handler& copy );
        ~OAuth2Handler( );

        OAuth2Handler& operator=( const OAuth2Handler& copy );

        std::string getAuthURL();

        std::string getAccessToken( ) ;
        std::string getRefreshToken( ) ;
        void setRefreshToken( std::string refreshToken ) ;

        // adding HTTP auth header
        std::string getHttpHeader( ) ;

        /** Exchange the previously obtained authentication code with the
            access/refresh tokens.

            \param authCode
               the authentication code normally obtained from authenticate
               method.
          */
        void fetchTokens( std::string authCode );
        void refresh( );

        /** Get the authentication code given credentials.

            This method should be overridden to parse the authentication URL response,
            authenticate using the form and get the token to avoid asking the user
            to launch a web browser and do it himself.

            \return
                the authentication code to transform into access/refresh tokens.
                If no code is found, an empty string is returned.
          */
        std::string oauth2Authenticate( );

        void setOAuth2Parser( OAuth2Parser parser );

    protected:
        OAuth2Handler( );
};

#endif /* _OAUTH2_HANDLER_HXX_ */
