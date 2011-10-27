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

using namespace std;

namespace libcmis
{
    Session* SessionFactory::createSession( map< int, string > params ) throw ( Exception )
    {
        Session* session = NULL;

        string repository;
        map< int, string >::iterator pIt = params.find( REPOSITORY_ID );
        if ( pIt != params.end( ) )
            repository = pIt->second;
        
        pIt = params.find( ATOMPUB_URL );
        if ( pIt != params.end( ) )
        {
            string repoId = pIt->second;

            // Look for the username and password
            pIt = params.find( USERNAME );
            string username;
            if ( pIt != params.end( ) )
                username = pIt->second;

            pIt = params.find( PASSWORD );
            string password;
            if ( pIt != params.end( ) )
                password = pIt->second;

            if ( !username.empty() && password.empty() )
                password = string();
            else if ( username.empty() && !password.empty() )
                username = string();

            // Do we have the verbose flag set?
            bool verbose = false;
            pIt = params.find( VERBOSE );
            if ( pIt != params.end( ) && !pIt->second.empty() )
                verbose = true;

            session = new AtomPubSession( repoId, repository, username, password, verbose );
        }

        return session;
    }

    list< string > SessionFactory::getRepositories( map< int, string > params )
    {
        list< string > repos;

        map< int, string >::iterator pIt = params.find( ATOMPUB_URL );
        if ( pIt != params.end( ) )
        {
            string repoId = pIt->second;

            // Look for the username and password
            pIt = params.find( USERNAME );
            string username;
            if ( pIt != params.end( ) )
                username = pIt->second;

            pIt = params.find( PASSWORD );
            string password;
            if ( pIt != params.end( ) )
                password = pIt->second;

            if ( !username.empty() && password.empty() )
                password = string();
            else if ( username.empty() && !password.empty() )
                username = string();
        
            // Do we have the verbose flag set?
            bool verbose = false;
            pIt = params.find( VERBOSE );
            if ( pIt != params.end( ) && !pIt->second.empty() )
                verbose = true;

            repos = AtomPubSession::getRepositories( repoId, username, password, verbose );
        }

        return repos;
    }
}
