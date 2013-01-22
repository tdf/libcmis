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
#include "atom-session.hxx"
#include "session-factory.hxx"
#include "ws-session.hxx"

using namespace std;

namespace libcmis
{
    string SessionFactory::s_proxy;
    string SessionFactory::s_noProxy;
    string SessionFactory::s_proxyUser;
    string SessionFactory::s_proxyPass;

    void SessionFactory::setProxySettings( string proxy, string noProxy,
            string proxyUser, string proxyPass )
    {
        SessionFactory::s_proxy = proxy;
        SessionFactory::s_noProxy = noProxy;
        SessionFactory::s_proxyUser = proxyUser;
        SessionFactory::s_proxyPass = proxyPass;
    }

    Session* SessionFactory::createSession( string bindingUrl, string username,
            string password, string repository,
            bool verbose ) throw ( Exception )
    {
        Session* session = NULL;
        
        if ( !bindingUrl.empty( ) )
        {
            try
            {
                session = new AtomPubSession( bindingUrl, repository, username, password, verbose );
            }
            catch ( const Exception& e )
            {
                if ( e.getType( ) == "permissionDenied" )
                    throw;
            }
            
            if ( session == NULL )
            {
                // We couldn't get an AtomSession, we may have an URL for the WebService binding
                try
                {
                    session = new WSSession( bindingUrl, repository, username, password, verbose );
                }
                catch ( const Exception& e )
                {
                    if ( e.getType( ) == "permissionDenied" )
                        throw;
                }
            }
        }

        return session;
    }

    list< RepositoryPtr > SessionFactory::getRepositories( string bindingUrl,
            string username, string password, bool verbose ) throw ( Exception )
    {
        list< RepositoryPtr > repos;

        if ( !bindingUrl.empty( ) )
        {
            bool tryNext = true;
            try
            {
                repos = AtomPubSession::getRepositories( bindingUrl, username, password, verbose );
                tryNext = false;
            }
            catch ( const Exception& e )
            {
                if ( e.getType( ) == "permissionDenied" )
                    throw;
            }
            
            if ( tryNext )
            {
                // It didn't work with AtomSession, we may have an URL for the WebService binding
                try
                {
                    repos = WSSession::getRepositories( bindingUrl, username, password, verbose );
                }
                catch ( const Exception& e )
                {
                    if ( e.getType( ) == "permissionDenied" )
                        throw;
                }
            }
        }

        return repos;
    }
}
