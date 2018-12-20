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
#ifndef _BASE_SESSION_HXX_
#define _BASE_SESSION_HXX_

#include <istream>
#include <sstream>
#include <vector>
#include <map>
#include <string>

#include <curl/curl.h>
#include <libxml/xmlstring.h>
#include <libxml/xpath.h>

#include <libcmis/exception.hxx>
#include <libcmis/oauth2-data.hxx>
#include <libcmis/session.hxx>
#include <libcmis/xml-utils.hxx>

#include "http-session.hxx"

class OAuth2Handler;

class BaseSession : public libcmis::Session,
                    public HttpSession
{
    protected:
        std::string m_bindingUrl;
        std::string m_repositoryId;

        std::vector< libcmis::RepositoryPtr > m_repositories;
    public:
        BaseSession( std::string sBindingUrl, std::string repository,
                     std::string username, std::string password,
                     bool noSslCheck = false,
                     libcmis::OAuth2DataPtr oauth2 = libcmis::OAuth2DataPtr(), bool verbose = false );

        /** This constructor copies an existing http session.
            This has been mostly designed for SessionFactory to save
            a few HTTP requests when guessing the binding to use.
          */
        BaseSession( std::string sBindingUrl, std::string repository,
                     const HttpSession& httpSession );

        BaseSession( const BaseSession& copy );
        ~BaseSession( );

        BaseSession& operator=( const BaseSession& copy );

        std::string& getRepositoryId( ) { return m_repositoryId; }

        // Utility methods

        std::string getRootId( ) { return getRepository()->getRootId( ); }

        std::string createUrl( const std::string& pattern, std::map< std::string, std::string > variables );

        std::string getBindingUrl( ) { return m_bindingUrl; }

        // HttpSession overridden methods

        virtual void setOAuth2Data( libcmis::OAuth2DataPtr oauth2 );

        // Session methods

        virtual void setNoSSLCertificateCheck( bool noCheck );

        virtual std::vector< libcmis::RepositoryPtr > getRepositories( );

        virtual libcmis::FolderPtr getRootFolder();

        virtual libcmis::FolderPtr getFolder( std::string id );

    protected:
        BaseSession( );
};

#endif
