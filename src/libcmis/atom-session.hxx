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
#ifndef _ATOM_SESSION_HXX_
#define _ATOM_SESSION_HXX_

#include <istream>
#include <list>
#include <map>
#include <string>

#include <curl/curl.h>
#include <libxml/xmlstring.h>
#include <libxml/xpath.h>

#include "exception.hxx"
#include "session.hxx"
#include "atom-workspace.hxx"

namespace atom
{
    class CurlException : public std::exception
    {
        private:
            std::string m_message;
            CURLcode    m_code;
            std::string m_url;

            bool        m_cancelled;

        public:
            CurlException( std::string message, CURLcode code, std::string url ) :
                exception( ),
                m_message( message ),
                m_code( code ),
                m_url( url ),
                m_cancelled( false )
            {
            }
            
            CurlException( std::string message ) :
                exception( ),
                m_message( message ),
                m_code( CURLE_OK ),
                m_url( ),
                m_cancelled( true )
            {
            }

            ~CurlException( ) throw () { }
            virtual const char* what( ) const throw ();

            CURLcode getErrorCode( ) const { return m_code; }
            std::string getErrorMessage( ) const { return m_message; }
            bool isCancelled( ) const { return m_cancelled; }

            libcmis::Exception getCmisException ( ) const;
    };
}

class AtomPubSession : public libcmis::Session
{
    private:
        std::string m_sAtomPubUrl;
        std::string m_sRepository;
        std::string m_username;
        std::string m_password;
        bool m_authProvided;
        atom::Workspace m_workspace;

        std::list< std::string > m_repositoriesIds;

        bool m_verbose;
        libcmis::AuthProviderPtr m_authProvider;

    public:
        AtomPubSession( std::string sAtomPubUrl, std::string repository,
                        std::string username, std::string password,
                        bool verbose ) throw ( libcmis::Exception );
        ~AtomPubSession( );

        static std::list< std::string > getRepositories( std::string url,
                        std::string username, std::string password,
                        bool verbose = false ) throw ( libcmis::Exception );

        std::string getRootId( ) { return getWorkspace().getRootId( ); }

        std::string getUsername( ) { return m_username; }

        std::string getPassword( ) { return m_password; }

        atom::Workspace& getWorkspace( );

        // Utility methods

        libcmis::ObjectPtr createObjectFromEntryDoc( xmlDocPtr doc );

        std::string httpGetRequest( std::string url ) throw ( atom::CurlException );
        std::string httpPutRequest( std::string url, std::istream& is, std::string contentType ) throw ( atom::CurlException );
        std::string httpPostRequest( std::string url, std::istream& is, std::string contentType ) throw ( atom::CurlException );

        void httpRunRequest( CURL* handle, std::string url ) throw ( atom::CurlException );

        // Override session methods

        virtual libcmis::FolderPtr getRootFolder();

        virtual libcmis::ObjectPtr getObject( std::string id ) throw ( libcmis::Exception );
        
        virtual libcmis::ObjectPtr getObjectByPath( std::string path ) throw ( libcmis::Exception );
        
        virtual libcmis::FolderPtr getFolder( std::string id ) throw ( libcmis::Exception );

        virtual libcmis::ObjectTypePtr getType( std::string id ) throw ( libcmis::Exception );

        virtual void setAuthenticationProvider( libcmis::AuthProviderPtr provider ) { m_authProvider = provider; }

    private:

        void initialize( );
};

#endif
