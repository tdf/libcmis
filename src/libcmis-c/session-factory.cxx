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

#include <libcmis-c/session.h>
#include <libcmis-c/session-factory.h>
#include <libcmis-c/vectors.h>

#include "internals.hxx"

using namespace std;

namespace
{
    size_t const CRED_MAX_LEN = 1024;

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
        /* NOTE: As I understand this, the callback is responsible for
         * filling the correct username and password (possibly using
         * the passed values as defaults in some dialog or so). But then
         * there is no guarantee that the new username/password will
         * not be longer than the present one, in which case it will
         * not fit into the available space! For now, use a buffer size
         * big enough for practical purposes.
         *
         * It might be a better idea to change the callback's signature
         * to bool ( * )( char** username, char** password )
         * and make it the callee's responsibility to reallocate the
         * strings if it needs to.
         */
        char user[CRED_MAX_LEN];
        strncpy(user, username.c_str( ), sizeof( user ) );
        user[CRED_MAX_LEN - 1] = '\0';
        char pass[CRED_MAX_LEN];
        strncpy(pass, password.c_str( ), sizeof( pass ) );
        pass[CRED_MAX_LEN - 1] = '\0';

        bool result = m_callback( user, pass );

        // Update the username and password with the input
        username = user;
        password = pass;

        return result;
    }


    class WrapperCertHandler : public libcmis::CertValidationHandler
    {
        private:
            libcmis_certValidationCallback m_callback;
        public:
            WrapperCertHandler( libcmis_certValidationCallback callback ) :
                m_callback( callback )
            {
            }
            virtual ~WrapperCertHandler( ) { };

            virtual bool validateCertificate( vector< string > certificatesChain );
    };

    bool WrapperCertHandler::validateCertificate( vector< string > certificatesChain )
    {
        libcmis_vector_string_Ptr chain = new ( nothrow ) libcmis_vector_string( );
        if ( chain )
            chain->handle = certificatesChain;

        bool result = m_callback( chain );

        libcmis_vector_string_free( chain );
        return result;
    }
}

std::string createString( char* str )
{
    if ( str )
        return string( str );
    else
        return string( );
}

void libcmis_setAuthenticationCallback( libcmis_authenticationCallback callback )
{
    libcmis::AuthProviderPtr provider( new ( nothrow ) WrapperAuthProvider( callback ) );
    if ( provider )
        libcmis::SessionFactory::setAuthenticationProvider( provider );
}

void libcmis_setCertValidationCallback( libcmis_certValidationCallback callback )
{
    libcmis::CertValidationHandlerPtr handler( new ( nothrow )WrapperCertHandler( callback ) );
    if ( handler )
        libcmis::SessionFactory::setCertificateValidationHandler( handler );
}

void libcmis_setOAuth2AuthCodeProvider( libcmis_oauth2AuthCodeProvider callback )
{
    libcmis::SessionFactory::setOAuth2AuthCodeProvider( callback );
}

libcmis_oauth2AuthCodeProvider libcmis_getOAuth2AuthCodeProvider( )
{
    return libcmis::SessionFactory::getOAuth2AuthCodeProvider( );
}

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
        bool noSslCheck,
        libcmis_OAuth2DataPtr oauth2,
        bool  verbose,
        libcmis_ErrorPtr error )
{
    libcmis_SessionPtr session = NULL;

    try
    {
        libcmis::OAuth2DataPtr oauth2Handle;
        if ( oauth2 != NULL )
            oauth2Handle = oauth2->handle;

        libcmis::Session* handle = libcmis::SessionFactory::createSession(
                createString( bindingUrl ),
                createString( username ),
                createString( password ),
                createString( repositoryId ), noSslCheck, oauth2Handle, verbose );
        session = new libcmis_session( );
        session->handle = handle;
    }
    catch ( const libcmis::Exception& e )
    {
        if ( error != NULL )
        {
            error->message = strdup( e.what() );
            error->type = strdup( e.getType().c_str() );
        }
    }
    catch ( const bad_alloc& e )
    {
        if ( error != NULL )
        {
            error->message = strdup( e.what() );
            error->badAlloc = true;
        }
    }

    return session;
}

libcmis_vector_Repository_Ptr libcmis_getRepositories(
        char* bindingUrl,
        char* username,
        char* password,
        bool  verbose,
        libcmis_ErrorPtr error )
{
    libcmis_SessionPtr session = libcmis_createSession(
            bindingUrl, NULL, username, password, false, NULL, verbose, error );
    libcmis_vector_Repository_Ptr repositories = libcmis_session_getRepositories( session );
    libcmis_session_free( session );
    return repositories;
}
