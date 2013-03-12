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
#ifndef _SESSION_FACTORY_HXX_
#define _SESSION_FACTORY_HXX_

#include <list>
#include <map>
#include <string>

#include "exception.hxx"
#include "oauth2-data.hxx"
#include "repository.hxx"
#include "session.hxx"

namespace libcmis
{
    typedef char* ( *OAuth2AuthCodeProvider )( const char* authUrl,
        const char* username, const char* password );

    class AuthProvider 
    {
        public:
            virtual ~AuthProvider() { };

            /** The function implementing it needs to fill the username and password parameters
                and return true. Returning false means that the user cancelled the authentication
                and will fail the query.
              */
            virtual bool authenticationQuery( std::string& username, std::string& password ) = 0;
    };
    typedef ::boost::shared_ptr< AuthProvider > AuthProviderPtr;
    
    class SessionFactory
    {
        private:

            static AuthProviderPtr s_authProvider;

            static std::string s_proxy;
            static std::string s_noProxy;
            static std::string s_proxyUser;
            static std::string s_proxyPass;

            static OAuth2AuthCodeProvider s_oauth2AuthCodeProvider;

        public:

            static void setAuthenticationProvider( AuthProviderPtr provider ) { s_authProvider = provider; }
            static AuthProviderPtr getAuthenticationProvider( ) { return s_authProvider; }
            
            static void setOAuth2AuthCodeProvider( OAuth2AuthCodeProvider provider ) { s_oauth2AuthCodeProvider = provider; }
            static OAuth2AuthCodeProvider getOAuth2AuthCodeProvider( ) { return s_oauth2AuthCodeProvider; }

            static void setProxySettings( std::string proxy,
                    std::string noProxy,
                    std::string proxyUser,
                    std::string proxyPass );

            static const std::string& getProxy() { return s_proxy; }
            static const std::string& getNoProxy() { return s_noProxy; }
            static const std::string& getProxyUser() { return s_proxyUser; }
            static const std::string& getProxyPass() { return s_proxyPass; }

            /** Create a session from the given parameters. The binding type is automatically
                detected based on the provided URL.

                The resulting pointer should be deleted by the caller.
              */
            static Session* createSession( std::string bindingUrl,
                    std::string username = std::string( ),
                    std::string password = std::string( ),
                    std::string repositoryId = std::string( ),
                    OAuth2DataPtr oauth2 = OAuth2DataPtr(), bool verbose = false ) throw ( Exception );

            /**
                Gets the informations of the repositories on the server.

                \deprecated
                    Since libcmis 0.4.0, this helper function simply creates a session
                    using the createSession function with no repository and then calls
                    getRepositories on the resulting session.
                    Kept only for backward API compatibility.
              */
            static std::list< RepositoryPtr > getRepositories( std::string bindingUrl,
                    std::string username = std::string( ),
                    std::string password = std::string( ),
                    bool verbose = false ) throw ( Exception );
    };
}

#endif
