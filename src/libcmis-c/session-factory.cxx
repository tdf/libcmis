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
#include <stdlib.h> 

#include <libcmis/session-factory.hxx>

#include "internals.hxx"
#include "session-factory.h"

using namespace std;

void libcmis_setProxySettings( char* proxy, char* noProxy,
        char* proxyUser, char* proxyPass )
{
    libcmis::SessionFactory::setProxySettings( string( proxy ), string( noProxy ),
            string( proxyUser ), string( proxyPass ) );
}

const char* libcmis_getProxy( )
{
    return libcmis::SessionFactory::getProxy( ).c_str();
}

const char* libcmis_getNoProxy( )
{
    return libcmis::SessionFactory::getNoProxy( ).c_str();
}

const char* libcmis_getProxyUser( )
{
    return libcmis::SessionFactory::getProxyUser( ).c_str();
}

const char* libcmis_getProxyPass( )
{
    return libcmis::SessionFactory::getProxyPass( ).c_str();
}

libcmis_SessionPtr libcmis_createSession(
        char* bindingUrl,
        char* repositoryId,
        char* username,
        char* password,
        bool  verbose,
        libcmis_ErrorPtr error )
{
    libcmis_SessionPtr session = NULL;

    try
    {
        libcmis::Session* handle = libcmis::SessionFactory::createSession( bindingUrl, username,
                password, repositoryId, verbose );
        session = new libcmis_session( );
        session->handle = handle;
    }
    catch ( const libcmis::Exception& e )
    {
        // Set the error handle
        if ( error != NULL )
            error->handle = new libcmis::Exception( e );
    }

    return session;
}

libcmis_RepositoryPtr* libcmis_getRepositories(
        char* bindingUrl,
        char* username,
        char* password,
        bool  verbose,
        libcmis_ErrorPtr error )
{
    libcmis_RepositoryPtr* repositories = NULL;
    try
    {
        list< libcmis::RepositoryPtr > repos = libcmis::SessionFactory::getRepositories(
               bindingUrl, username, password, verbose );

        repositories = new libcmis_RepositoryPtr[ repos.size() ];
        list< libcmis::RepositoryPtr >::iterator it = repos.begin( );
        for ( int i = 0; it != repos.end( ); ++it, ++i )
        {
            libcmis_RepositoryPtr repository = new libcmis_repository( );
            repository->handle = *it;
            repositories[i] = repository;
        }
    }
    catch ( const libcmis::Exception& e )
    {
        // Set the error handle
        if ( error != NULL )
            error->handle = new libcmis::Exception( e );
    }
    return repositories;
}
