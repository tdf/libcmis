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

#include <istream>
#include <map>
#include <ostream>
#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <libxml/tree.h>

#include "exception.hxx"
#include "folder.hxx"
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

void writeCmismStream( xmlTextWriterPtr writer, RelatedMultipart& multipart,
        boost::shared_ptr< std::ostream >, std::string& contentType );

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

class GetObjectByPath : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_path;

    public:
        GetObjectByPath( std::string repoId, std::string path ) :
            m_repositoryId( repoId ),
            m_path( path )
        {
        }

        ~GetObjectByPath( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class UpdateProperties : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;
        const std::map< std::string, libcmis::PropertyPtr >& m_properties;
        std::string m_changeToken;

    public:
        UpdateProperties( std::string repoId, std::string objectId,
                const std::map< std::string, libcmis::PropertyPtr >& properties,
                std::string changeToken ) :
            m_repositoryId( repoId ),
            m_objectId( objectId ),
            m_properties( properties ),
            m_changeToken( changeToken )
        {
        }

        ~UpdateProperties( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class UpdatePropertiesResponse : public SoapResponse
{
    private:
        std::string m_id;

        UpdatePropertiesResponse( ) : SoapResponse( ), m_id( ) { }

    public:

        /** Parse cmism:updatePropertiesResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        std::string getObjectId( ) { return m_id; }
};

class DeleteObject : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;
        bool m_allVersions;

    public:
        DeleteObject( std::string repoId, std::string objectId, bool allVersions ) :
            m_repositoryId( repoId ),
            m_objectId( objectId ),
            m_allVersions( allVersions )
        {
        }

        ~DeleteObject( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class DeleteTree : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_folderId;
        bool m_allVersions;
        libcmis::UnfileObjects::Type m_unfile;
        bool m_continueOnFailure;

    public:
        DeleteTree( std::string repoId,
                std::string folderId,
                bool allVersions,
                libcmis::UnfileObjects::Type unfile,
                bool continueOnFailure ) :
            m_repositoryId( repoId ),
            m_folderId( folderId ),
            m_allVersions( allVersions ),
            m_unfile( unfile ),
            m_continueOnFailure( continueOnFailure )
        {
        }

        ~DeleteTree( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class DeleteTreeResponse : public SoapResponse
{
    private:
        std::vector< std::string > m_failedIds;

        DeleteTreeResponse( ) : SoapResponse( ), m_failedIds( ) { }

    public:

        /** Parse cmism:deleteTreeResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        std::vector< std::string > getFailedIds( ) { return m_failedIds; }
};

class MoveObject : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;
        std::string m_destId;
        std::string m_srcId;

    public:
        MoveObject( std::string repoId, std::string objectId, std::string destId, std::string srcId ) :
            m_repositoryId( repoId ),
            m_objectId( objectId ),
            m_destId( destId ),
            m_srcId( srcId )
        {
        }

        ~MoveObject( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class GetContentStream : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;

    public:
        GetContentStream( std::string repoId, std::string objectId ) :
            m_repositoryId( repoId ),
            m_objectId( objectId )
        {
        }

        ~GetContentStream( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class GetContentStreamResponse : public SoapResponse
{
    private:
        boost::shared_ptr< std::istream > m_stream;

        GetContentStreamResponse( ) : SoapResponse( ), m_stream( ) { }

    public:

        /** Parse cmism:getContentStreamResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        boost::shared_ptr< std::istream> getStream( ) { return m_stream; }
};

class GetObjectParents : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;

    public:
        GetObjectParents( std::string repoId,
                std::string objectId ) :
            m_repositoryId( repoId ),
            m_objectId( objectId )
        {
        }

        ~GetObjectParents( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class GetObjectParentsResponse : public SoapResponse
{
    private:
        std::vector< libcmis::FolderPtr > m_parents;

        GetObjectParentsResponse( ) : SoapResponse( ), m_parents( ) { }

    public:

        /** Parse cmism:getObjectParentsResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        std::vector< libcmis::FolderPtr > getParents( ) { return m_parents; }
};

class GetChildren : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_folderId;

    public:
        GetChildren( std::string repoId,
                std::string folderId ) :
            m_repositoryId( repoId ),
            m_folderId( folderId )
        {
        }

        ~GetChildren( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class GetChildrenResponse : public SoapResponse
{
    private:
        std::vector< libcmis::ObjectPtr > m_children;

        GetChildrenResponse( ) : SoapResponse( ), m_children( ) { }

    public:

        /** Parse cmism:getChildrenResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        std::vector< libcmis::ObjectPtr > getChildren( ) { return m_children; }
};

class CreateFolder : public SoapRequest
{
    private:
        std::string m_repositoryId;
        const std::map< std::string, libcmis::PropertyPtr >& m_properties;
        std::string m_folderId;

    public:
        CreateFolder( std::string repoId,
                const std::map< std::string, libcmis::PropertyPtr >& properties,
                std::string folderId ) :
            m_repositoryId( repoId ),
            m_properties( properties ),
            m_folderId( folderId )
        {
        }

        ~CreateFolder( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class CreateFolderResponse : public SoapResponse
{
    private:
        std::string m_id;

        CreateFolderResponse( ) : SoapResponse( ), m_id( ) { }

    public:

        /** Parse cmism:createFolderResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        std::string getObjectId( ) { return m_id; }
};

class CreateDocument : public SoapRequest
{
    private:
        std::string m_repositoryId;
        const std::map< std::string, libcmis::PropertyPtr >& m_properties;
        std::string m_folderId;
        boost::shared_ptr< std::ostream > m_stream;
        std::string m_contentType;

    public:
        CreateDocument( std::string repoId,
                const std::map< std::string, libcmis::PropertyPtr >& properties,
                std::string folderId, boost::shared_ptr< std::ostream > stream,
                std::string contentType ) :
            m_repositoryId( repoId ),
            m_properties( properties ),
            m_folderId( folderId ),
            m_stream( stream ),
            m_contentType( contentType )
        {
        }

        ~CreateDocument( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class SetContentStream : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;
        bool m_overwrite;
        std::string m_changeToken;
        boost::shared_ptr< std::ostream > m_stream;
        std::string m_contentType;

    public:
        SetContentStream( std::string repoId,
                std::string objectId,
                bool overwrite,
                std::string changeToken,
                boost::shared_ptr< std::ostream > stream,
                std::string contentType ) :
            m_repositoryId( repoId ),
            m_objectId( objectId ),
            m_overwrite( overwrite ),
            m_changeToken( changeToken ),
            m_stream( stream ),
            m_contentType( contentType )
        {
        }

        ~SetContentStream( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class CheckOut : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;

    public:
        CheckOut( std::string repoId,
                std::string objectId ) :
            m_repositoryId( repoId ),
            m_objectId( objectId )
        {
        }

        ~CheckOut( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class CheckOutResponse : public SoapResponse
{
    private:
        std::string m_objectId;

        CheckOutResponse( ) : SoapResponse( ), m_objectId( ) { }

    public:

        /** Parse cmism:checkOutResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        std::string getObjectId( ) { return m_objectId; }
};

class CancelCheckOut : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;

    public:
        CancelCheckOut( std::string repoId,
                std::string objectId ) :
            m_repositoryId( repoId ),
            m_objectId( objectId )
        {
        }

        ~CancelCheckOut( ) { }

        void toXml( xmlTextWriterPtr writer );
};

#endif
