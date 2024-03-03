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

#include <libcmis/document.hxx>
#include <libcmis/exception.hxx>
#include <libcmis/folder.hxx>
#include <libcmis/object.hxx>
#include <libcmis/object-type.hxx>
#include <libcmis/repository.hxx>

#include "ws-soap.hxx"

class CmisSoapFaultDetail : public SoapFaultDetail
{
    private:
        std::string m_type;
        long m_code;
        std::string m_message;

        CmisSoapFaultDetail( xmlNodePtr node );

    public:
        ~CmisSoapFaultDetail( ) noexcept { };

        std::string getType( ) { return m_type; }
        int getCode( ) { return m_code; }
        std::string getMessage( ) { return m_message; }

        libcmis::Exception toException( );

        static SoapFaultDetailPtr create( xmlNodePtr node );
};

boost::shared_ptr< libcmis::Exception > getCmisException( const SoapFault& fault );

void writeCmismStream( xmlTextWriterPtr writer, RelatedMultipart& multipart,
        boost::shared_ptr< std::ostream >, std::string& contentType, const std::string& filename );

/** getRepositories request.
  */
class GetRepositoriesRequest : public SoapRequest
{
    public:
        GetRepositoriesRequest( ) { };
        ~GetRepositoriesRequest( ) { };

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

class GetRepositoryInfoRequest : public SoapRequest
{
    private:
        std::string m_id;

    public:
        GetRepositoryInfoRequest( std::string id ) : m_id( id ) { };
        ~GetRepositoryInfoRequest( ) { };

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

class GetTypeDefinitionRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_typeId;

    public:
        GetTypeDefinitionRequest( std::string repoId, std::string typeId ) :
            m_repositoryId( repoId ),
            m_typeId( typeId )
        {
        }

        ~GetTypeDefinitionRequest( ) { }

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

class GetTypeChildrenRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_typeId;

    public:
        GetTypeChildrenRequest( std::string repoId, std::string typeId ) :
            m_repositoryId( repoId ),
            m_typeId( typeId )
        {
        }

        ~GetTypeChildrenRequest( ) { }

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

class GetObjectRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_id;

    public:
        GetObjectRequest( std::string repoId, std::string id ) :
            m_repositoryId( repoId ),
            m_id( id )
        {
        }

        ~GetObjectRequest( ) { }

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

class GetObjectByPathRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_path;

    public:
        GetObjectByPathRequest( std::string repoId, std::string path ) :
            m_repositoryId( repoId ),
            m_path( path )
        {
        }

        ~GetObjectByPathRequest( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class UpdatePropertiesRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;
        const std::map< std::string, libcmis::PropertyPtr >& m_properties;
        std::string m_changeToken;

    public:
        UpdatePropertiesRequest( std::string repoId, std::string objectId,
                const std::map< std::string, libcmis::PropertyPtr >& properties,
                std::string changeToken ) :
            m_repositoryId( repoId ),
            m_objectId( objectId ),
            m_properties( properties ),
            m_changeToken( changeToken )
        {
        }

        ~UpdatePropertiesRequest( ) { }

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

class DeleteObjectRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;
        bool m_allVersions;

    public:
        DeleteObjectRequest( std::string repoId, std::string objectId, bool allVersions ) :
            m_repositoryId( repoId ),
            m_objectId( objectId ),
            m_allVersions( allVersions )
        {
        }

        ~DeleteObjectRequest( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class DeleteTreeRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_folderId;
        bool m_allVersions;
        libcmis::UnfileObjects::Type m_unfile;
        bool m_continueOnFailure;

    public:
        DeleteTreeRequest( std::string repoId,
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

        ~DeleteTreeRequest( ) { }

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

class MoveObjectRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;
        std::string m_destId;
        std::string m_srcId;

    public:
        MoveObjectRequest( std::string repoId, std::string objectId, std::string destId, std::string srcId ) :
            m_repositoryId( repoId ),
            m_objectId( objectId ),
            m_destId( destId ),
            m_srcId( srcId )
        {
        }

        ~MoveObjectRequest( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class GetContentStreamRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;

    public:
        GetContentStreamRequest( std::string repoId, std::string objectId ) :
            m_repositoryId( repoId ),
            m_objectId( objectId )
        {
        }

        ~GetContentStreamRequest( ) { }

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

class GetObjectParentsRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;

    public:
        GetObjectParentsRequest( std::string repoId,
                std::string objectId ) :
            m_repositoryId( repoId ),
            m_objectId( objectId )
        {
        }

        ~GetObjectParentsRequest( ) { }

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

class GetChildrenRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_folderId;

    public:
        GetChildrenRequest( std::string repoId,
                std::string folderId ) :
            m_repositoryId( repoId ),
            m_folderId( folderId )
        {
        }

        ~GetChildrenRequest( ) { }

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

class CreateFolderRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        const std::map< std::string, libcmis::PropertyPtr >& m_properties;
        std::string m_folderId;

    public:
        CreateFolderRequest( std::string repoId,
                const std::map< std::string, libcmis::PropertyPtr >& properties,
                std::string folderId ) :
            m_repositoryId( repoId ),
            m_properties( properties ),
            m_folderId( folderId )
        {
        }

        ~CreateFolderRequest( ) { }

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

class CreateDocumentRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        const std::map< std::string, libcmis::PropertyPtr >& m_properties;
        std::string m_folderId;
        boost::shared_ptr< std::ostream > m_stream;
        std::string m_contentType;
        std::string m_filename;

    public:
        CreateDocumentRequest( std::string repoId,
                const std::map< std::string, libcmis::PropertyPtr >& properties,
                std::string folderId, boost::shared_ptr< std::ostream > stream,
                std::string contentType,
                std::string filename ) :
            m_repositoryId( repoId ),
            m_properties( properties ),
            m_folderId( folderId ),
            m_stream( stream ),
            m_contentType( contentType ),
            m_filename( filename )
        {
        }

        ~CreateDocumentRequest( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class SetContentStreamRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;
        bool m_overwrite;
        std::string m_changeToken;
        boost::shared_ptr< std::ostream > m_stream;
        std::string m_contentType;
        std::string m_filename;

    public:
        SetContentStreamRequest( std::string repoId,
                std::string objectId,
                bool overwrite,
                std::string changeToken,
                boost::shared_ptr< std::ostream > stream,
                std::string contentType,
                std::string filename ) :
            m_repositoryId( repoId ),
            m_objectId( objectId ),
            m_overwrite( overwrite ),
            m_changeToken( changeToken ),
            m_stream( stream ),
            m_contentType( contentType ),
            m_filename( filename )
        {
        }

        ~SetContentStreamRequest( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class GetRenditionsRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;
        std::string m_filter;

    public:
        GetRenditionsRequest( std::string repoId, std::string objectId, std::string filter ) :
            m_repositoryId( repoId ),
            m_objectId( objectId ),
            m_filter( filter )
        {
        }

        ~GetRenditionsRequest( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class GetRenditionsResponse : public SoapResponse
{
    private:
        std::vector< libcmis::RenditionPtr > m_renditions;

        GetRenditionsResponse( ) : SoapResponse( ), m_renditions( ) { }

    public:

        /** Parse cmism:getRenditionsResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        std::vector< libcmis::RenditionPtr > getRenditions( ) { return m_renditions; }
};

class CheckOutRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;

    public:
        CheckOutRequest( std::string repoId,
                std::string objectId ) :
            m_repositoryId( repoId ),
            m_objectId( objectId )
        {
        }

        ~CheckOutRequest( ) { }

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

class CancelCheckOutRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;

    public:
        CancelCheckOutRequest( std::string repoId,
                std::string objectId ) :
            m_repositoryId( repoId ),
            m_objectId( objectId )
        {
        }

        ~CancelCheckOutRequest( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class CheckInRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;
        bool m_isMajor;
        const std::map< std::string, libcmis::PropertyPtr >& m_properties;
        boost::shared_ptr< std::ostream > m_stream;
        std::string m_contentType;
        std::string m_fileName;
        std::string m_comment;

    public:
        CheckInRequest( std::string repoId,
                std::string objectId, bool isMajor,
                const std::map< std::string, libcmis::PropertyPtr >& properties,
                boost::shared_ptr< std::ostream > stream,
                std::string contentType, std::string fileName, std::string comment ) :
            m_repositoryId( repoId ),
            m_objectId( objectId ),
            m_isMajor( isMajor ),
            m_properties( properties ),
            m_stream( stream ),
            m_contentType( contentType ),
            m_fileName( fileName ),
            m_comment( comment )
        {
        }

        ~CheckInRequest( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class CheckInResponse : public SoapResponse
{
    private:
        std::string m_objectId;

        CheckInResponse( ) : SoapResponse( ), m_objectId( ) { }

    public:

        /** Parse cmism:checkInResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        std::string getObjectId( ) { return m_objectId; }
};

class GetAllVersionsRequest : public SoapRequest
{
    private:
        std::string m_repositoryId;
        std::string m_objectId;

    public:
        GetAllVersionsRequest( std::string repoId, std::string objectId ) :
            m_repositoryId( repoId ),
            m_objectId( objectId )
        {
        }

        ~GetAllVersionsRequest( ) { }

        void toXml( xmlTextWriterPtr writer );
};

class GetAllVersionsResponse : public SoapResponse
{
    private:
        std::vector< libcmis::DocumentPtr > m_objects;

        GetAllVersionsResponse( ) : SoapResponse( ), m_objects( ) { }

    public:

        /** Parse cmism:getAllVersionsResponse. This function
            assumes that the node is the expected one: this is
            normally ensured by the SoapResponseFactory.
          */
        static SoapResponsePtr create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* session );

        std::vector< libcmis::DocumentPtr > getObjects( ) { return m_objects; }
};

#endif
