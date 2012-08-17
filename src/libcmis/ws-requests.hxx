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
#include <vector>

#include <boost/shared_ptr.hpp>
#include <libxml/tree.h>

#include "exception.hxx"
#include "object.hxx"
#include "object-type.hxx"
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
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

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
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        libcmis::RepositoryPtr getRepository( ) { return m_repository; }
};

class GetTypeDefinition : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_typeId;

    public:
        GetTypeDefinition( std::string repoId, std::string typeId ) :
            m_repositoryId( repoId ),
            m_typeId( typeId )
        {
        }

        ~GetTypeDefinition( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class GetTypeDefinitionResponse : public SoapResponse
{
    private:
        libcmis::ObjectTypePtr m_type;

        GetTypeDefinitionResponse( ) : SoapResponse( ), m_type( ) { }

    public:

        /** Parse cmism:getTypeDefinitionResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        libcmis::ObjectTypePtr getType( ) { return m_type; }
};

class GetTypeChildren : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_typeId;

    public:
        GetTypeChildren( std::string repoId, std::string typeId ) :
            m_repositoryId( repoId ),
            m_typeId( typeId )
        {
        }

        ~GetTypeChildren( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class GetTypeChildrenResponse : public SoapResponse
{
    private:
        std::vector< libcmis::ObjectTypePtr > m_children;

        GetTypeChildrenResponse( ) : SoapResponse( ), m_children( ) { }

    public:

        /** Parse cmism:getTypeChildrenResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        std::vector< libcmis::ObjectTypePtr > getChildren( ) { return m_children; }
};

class GetObject : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_id;

    public:
        GetObject( std::string repoId, std::string id ) :
            m_repositoryId( repoId ),
            m_id( id )
        {
        }

        ~GetObject( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class GetObjectResponse : public SoapResponse
{
    private:
        libcmis::ObjectPtr m_object;

        GetObjectResponse( ) : SoapResponse( ), m_object( ) { }

    public:

        /** Parse cmism:getObjectResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        libcmis::ObjectPtr getObject( ) { return m_object; }
};

#endif
