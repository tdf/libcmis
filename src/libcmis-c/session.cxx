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

#include "internals.hxx"
#include "session.h"

using namespace std;

namespace
{
    class WrapperAuthProvider : public libcmis::AuthProvider
    {
        private:
            libcmis_authenticationCallback m_callback;

        public:
            WrapperAuthProvider( libcmis_authenticationCallback callback ) :
                m_callback( callback )
            {
            }
            virtual ~WrapperAuthProvider( ) { };

            virtual bool authenticationQuery( string& username, string& password );
    };

    bool WrapperAuthProvider::authenticationQuery( string& username, string& password )
    {
        char* user = strdup( username.c_str( ) );
        char* pass = strdup( password.c_str( ) );

        bool result = m_callback( user, pass );

        // Update the username and password with the input
        string newUser( user );
        username.swap( newUser );

        string newPass( pass );
        password.swap( newPass );

        free( user );
        free( pass );

        return result;
    }
}

void libcmis_session_free( libcmis_SessionPtr session )
{
    if ( session != NULL )
    {
        delete session->handle;
        delete session;
    }
}

libcmis_RepositoryPtr libcmis_session_getRepository(
        libcmis_SessionPtr session,
        libcmis_ErrorPtr error )
{
    libcmis_RepositoryPtr repository = NULL;

    if ( session != NULL && session->handle != NULL )
    {
        try
        {
            libcmis::RepositoryPtr handle = session->handle->getRepository( );
            repository = new libcmis_repository( );
            repository->handle = handle;
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }

    return repository;
}

void libcmis_session_setAuthenticationCallback(
        libcmis_SessionPtr session,
        libcmis_authenticationCallback callback )
{
    if ( session != NULL && session->handle != NULL )
    {
        libcmis::AuthProviderPtr provider( new WrapperAuthProvider( callback ) );
        session->handle->setAuthenticationProvider( provider );
    }
}
