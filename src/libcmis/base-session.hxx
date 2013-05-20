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

#include "exception.hxx"
#include "oauth2-data.hxx"
#include "session.hxx"
#include "xml-utils.hxx"

class OAuth2Handler;

class CurlException : public std::exception
{
    private:
        std::string m_message;
        CURLcode    m_code;
        std::string m_url;
        long        m_httpStatus;

        bool        m_cancelled;

        mutable std::string m_errorMessage;

    public:
        CurlException( std::string message, CURLcode code, std::string url, long httpStatus ) :
            exception( ),
            m_message( message ),
            m_code( code ),
            m_url( url ),
            m_httpStatus( httpStatus ),
            m_cancelled( false ),
            m_errorMessage( )
        {
        }
        
        CurlException( std::string message ) :
            exception( ),
            m_message( message ),
            m_code( CURLE_OK ),
            m_url( ),
            m_httpStatus( 0 ),
            m_cancelled( true ),
            m_errorMessage( )
        {
        }

        ~CurlException( ) throw () { }
        virtual const char* what( ) const throw ();

        CURLcode getErrorCode( ) const { return m_code; }
        std::string getErrorMessage( ) const { return m_message; }
        bool isCancelled( ) const { return m_cancelled; }
        long getHttpStatus( ) const { return m_httpStatus; }

        libcmis::Exception getCmisException ( ) const;
};

class BaseSession : public libcmis::Session
{
    private:
        CURL* m_curlHandle;
        bool  m_no100Continue;
    protected: 
        OAuth2Handler* m_oauth2Handler;
        std::string m_bindingUrl;
        std::string m_repositoryId;
        std::string m_username;
        std::string m_password;
        bool m_authProvided;

        std::vector< libcmis::RepositoryPtr > m_repositories;

        bool m_verbose;
        bool m_noHttpErrors;
        bool m_refreshedToken;
    public:
        BaseSession( std::string sBindingUrl, std::string repository,
                     std::string username, std::string password,
                     libcmis::OAuth2DataPtr oauth2 = libcmis::OAuth2DataPtr(), bool verbose = false )
            throw ( libcmis::Exception );

        BaseSession( const BaseSession& copy );
        ~BaseSession( );

        BaseSession& operator=( const BaseSession& copy );

        std::string& getUsername( ) throw ( CurlException );

        std::string& getPassword( ) throw ( CurlException );

        std::string& getRepositoryId( ) { return m_repositoryId; }

        /** Don't throw the HTTP errors as CurlExceptions. 
          */
        void setNoHttpErrors( bool noHttpErrors ) { m_noHttpErrors = noHttpErrors; }
        
        /** Set the OAuth2 data and get the access / refresh tokens.
          */
        void setOAuth2Data( libcmis::OAuth2DataPtr oauth2 ) throw ( libcmis::Exception );

        // Utility methods
       
        std::string getRootId( ) throw ( libcmis::Exception ) { return getRepository()->getRootId( ); }

        std::string createUrl( const std::string& pattern, std::map< std::string, std::string > variables );

        libcmis::HttpResponsePtr httpGetRequest( std::string url ) throw ( CurlException );
        libcmis::HttpResponsePtr httpPutRequest( std::string url, std::istream& is, std::vector< std::string > headers ) throw ( CurlException );
        libcmis::HttpResponsePtr httpPostRequest( const std::string& url, std::istream& is, const std::string& contentType ) throw ( CurlException );
        void httpDeleteRequest( std::string url ) throw ( CurlException );
        
        long getHttpStatus( );

        // Session methods

        virtual std::vector< libcmis::RepositoryPtr > getRepositories( );

        virtual libcmis::FolderPtr getRootFolder() throw ( libcmis::Exception );
        
        virtual libcmis::FolderPtr getFolder( std::string id ) throw ( libcmis::Exception );

        /** Get the authentication code given credentials.

            This method should be overridden to parse the authentication URL response,
            authenticate using the form and get the token to avoid asking the user
            to launch a web browser and do it himself.

            \return
                the authentication code to transform into access/refresh tokens or NULL.
                The resulting authentication code will need to be deleted by the client
                code.
          */
        virtual char* oauth2Authenticate( ) throw ( CurlException );

    protected:
        BaseSession( );

    private:
        void checkCredentials( ) throw ( CurlException );
        void httpRunRequest( std::string url, std::vector< std::string > headers = std::vector< std::string > ( ) ) throw ( CurlException );
        void initProtocols( );
};

#endif
