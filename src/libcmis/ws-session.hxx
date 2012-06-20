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
#ifndef _WS_SESSION_HXX_
#define _WS_SESSION_HXX_

#include <map>
#include <string>

#include "base-session.hxx"
#include "ws-repositoryservice.hxx"
#include "ws-soap.hxx"

class WSSession : public BaseSession
{
    private:
        std::map< std::string, std::string > m_servicesUrls;

        SoapResponseFactory m_responseFactory;

    public:
        WSSession( std::string bindingUrl, std::string repositoryId,
                   std::string username, std::string password,
                   bool verbose ) throw ( libcmis::Exception );
        WSSession( const WSSession& copy );
        ~WSSession( );

        WSSession& operator=( const WSSession& copy );

        static std::list< std::string > getRepositories( std::string url,
                        std::string username, std::string password,
                        bool verbose = false ) throw ( libcmis::Exception );

        // Utility methods

        /** Get an instance of the SoapResponseFactory, setup with all the
            CMIS namespaces and function pointers.
          */
        SoapResponseFactory& getResponseFactory( ) { return m_responseFactory; }

        /** Try hard to get a WSDL file at the given URL (tries to add ?wsdl if needed)
          */        
        std::string getWsdl( std::string url ) throw ( CurlException );

        std::vector< SoapResponsePtr > soapRequest( std::string& url, SoapRequest& request )
            throw ( SoapFault, CurlException );

        /** Get the service location URL given its name.
          */
        std::string getServiceUrl( std::string name );

        RepositoryService getRepositoryService( );

        // Override session methods

        virtual libcmis::RepositoryPtr getRepository( ) throw ( libcmis::Exception );

        virtual libcmis::ObjectPtr getObject( std::string id ) throw ( libcmis::Exception );
        
        virtual libcmis::ObjectPtr getObjectByPath( std::string path ) throw ( libcmis::Exception );

        virtual libcmis::ObjectTypePtr getType( std::string id ) throw ( libcmis::Exception );

    private:

        void initialize( ) throw ( libcmis::Exception );

        std::map< std::string, SoapResponseCreator > getResponseMapping( );

        std::string createEnvelope( SoapRequest& request );
};

#endif
