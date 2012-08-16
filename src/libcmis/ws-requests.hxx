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
#ifndef _WS_REQUESTS_HXX_
#define _WS_REQUESTS_HXX_

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>
#include <libxml/tree.h>

#include "exception.hxx"
#include "repository.hxx"
#include "ws-soap.hxx"

class CmisSoapFaultDetail : public SoapFaultDetail
{
    private:
        std::string m_type;
        long m_code;
        std::string m_message;

        CmisSoapFaultDetail( xmlNodePtr node );

    public:
        ~CmisSoapFaultDetail( ) throw ( ) { };

        std::string getType( ) { return m_type; }
        int getCode( ) { return m_code; }
        std::string getMessage( ) { return m_message; }

        libcmis::Exception toException( );

        static SoapFaultDetailPtr create( xmlNodePtr node );
};

boost::shared_ptr< libcmis::Exception > getCmisException( const SoapFault& fault );

/** getRepositories request.
  */
class GetRepositories : public SoapRequest
{
    public:
        GetRepositories( ) { };
        ~GetRepositories( ) { };

        void toXml( xmlTextWriterPtr writer );
};

class GetRepositoriesResponse : public SoapResponse
{
    private:
        std::map< std::string, std::string > m_repositories;

        GetRepositoriesResponse( ) : SoapResponse( ), m_repositories( ) { };

    public:

        /** Parse cmism:getRepositoriesResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart );

        std::map< std::string, std::string > getRepositories( ) { return m_repositories; }
};

class GetRepositoryInfo : public SoapRequest
{
    private:
        std::string m_id;

    public:
        GetRepositoryInfo( std::string id ) : m_id( id ) { };
        ~GetRepositoryInfo( ) { };

        void toXml( xmlTextWriterPtr writer );
};

class GetRepositoryInfoResponse : public SoapResponse
{
    private:
        libcmis::RepositoryPtr m_repository;

        GetRepositoryInfoResponse( ) : SoapResponse( ), m_repository( ) { };

    public:

        /** Parse cmism:getRepositoriesResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart );

        libcmis::RepositoryPtr getRepository( ) { return m_repository; }
};

#endif
