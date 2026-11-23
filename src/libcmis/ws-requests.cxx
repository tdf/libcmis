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

#include "ws-requests.hxx"

#include <utility>

#include <libcmis/xml-utils.hxx>

#include "ws-document.hxx"
#include "ws-folder.hxx"
#include "ws-object.hxx"
#include "ws-object-type.hxx"

using namespace std;
using libcmis::PropertyPtrMap;

CmisSoapFaultDetail::CmisSoapFaultDetail( xmlNodePtr node ) :
    SoapFaultDetail( ),
    m_type( ),
    m_code( 0 ),
    m_message( )
{
    // Extract the type, code and message
    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        xmlChar* content = xmlNodeGetContent( child );
        if ( content == NULL )
            continue;
        string value( ( char * )content );
        xmlFree( content );

        if ( xmlStrEqual( child->name, BAD_CAST( "type" ) ) )
        {
            m_type = std::move(value);
        }
        else if ( xmlStrEqual( child->name, BAD_CAST( "code" ) ) )
        {
            try
            {
                m_code = libcmis::parseInteger( std::move(value) );
            }
            catch ( const libcmis::Exception& )
            {
                // Simply leave the default error code if unparsable
            }
        }
        else if ( xmlStrEqual( child->name, BAD_CAST( "message" ) ) )
        {
            m_message = std::move(value);
        }
    }
}

libcmis::Exception CmisSoapFaultDetail::toException( )
{
    libcmis::Exception e( m_message, m_type );
    return e;
}

boost::shared_ptr< libcmis::Exception > getCmisException( const SoapFault& fault )
{
    boost::shared_ptr< libcmis::Exception > exception;

    vector< SoapFaultDetailPtr > details = fault.getDetail( );
    for ( vector< SoapFaultDetailPtr >::iterator it = details.begin( );
            it != details.end( ) && exception.get( ) == NULL; ++ it )
    {
        boost::shared_ptr< CmisSoapFaultDetail > cmisDetail = boost::dynamic_pointer_cast< CmisSoapFaultDetail >( *it );
        if ( cmisDetail.get( ) != NULL )
            exception.reset( new libcmis::Exception( cmisDetail->toException( ) ) );
    }

    return exception;
}

void writeCmismStream( xmlTextWriterPtr writer, RelatedMultipart& multipart, boost::shared_ptr< ostream > os, string& contentType, const string& filename )
{
    // Get the stream as a string
    istream is( os->rdbuf( ) );
    is.seekg( 0, ios::end );
    long size = is.tellg( );
    is.seekg( 0, ios::beg );

    char* buf = new char[ size ];
    is.read( buf, size );
    string content( buf, size );
    delete[ ] buf;

    (void)xmlTextWriterWriteFormatElement( writer, BAD_CAST( "cmism:length" ), "%ld", static_cast<long int>(content.size( )) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:mimeType" ), BAD_CAST( contentType.c_str( ) ) );
    if ( !filename.empty( ) )
        (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:filename" ), BAD_CAST( filename.c_str( ) ) );
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:stream" ) );

    string name( "stream" );
    RelatedPartPtr streamPart( new RelatedPart( name, contentType, content ) );
    string partHref = "cid:";
    partHref += multipart.addPart( streamPart );

    (void)xmlTextWriterStartElement( writer, BAD_CAST( "xop:Include" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:xop" ), BAD_CAST( "http://www.w3.org/2004/08/xop/include" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "href" ), BAD_CAST( partHref.c_str( ) ) );
    (void)xmlTextWriterEndElement( writer ); // xop:Include
    (void)xmlTextWriterEndElement( writer ); // cmism:stream
}

SoapFaultDetailPtr CmisSoapFaultDetail::create( xmlNodePtr node )
{
    return SoapFaultDetailPtr( new CmisSoapFaultDetail( node ) );
}

void GetRepositoriesRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getRepositories" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );
    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr GetRepositoriesResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* )
{
    GetRepositoriesResponse* response = new GetRepositoriesResponse( );

    // Look for the cmiss:repositories children
    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "repositories" ) ) )
        {
            string id;
            string name;

            // Look for repositoryId and repositoryName
            for ( xmlNodePtr repoChild = child->children; repoChild; repoChild = repoChild->next )
            {
                xmlChar* content = xmlNodeGetContent( repoChild );
                if ( content == NULL )
                    continue;
                string value( ( char* ) content );
                xmlFree( content );

                if ( xmlStrEqual( repoChild->name, BAD_CAST( "repositoryId" ) ) )
                {
                    id = std::move(value);
                }
                else if ( xmlStrEqual( repoChild->name, BAD_CAST( "repositoryName" ) ) )
                {
                    name = std::move(value);
                }

            }

            if ( !id.empty( ) )
                response->m_repositories[ id ] = std::move(name);
        }
    }

    return SoapResponsePtr( response );
}

void GetRepositoryInfoRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getRepositoryInfo" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_id.c_str( ) ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr GetRepositoryInfoResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* )
{
    GetRepositoryInfoResponse* response = new GetRepositoryInfoResponse( );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "repositoryInfo" ) ) )
        {
            libcmis::RepositoryPtr repository( new libcmis::Repository( child ) );
            response->m_repository = repository;
        }
    }

    return SoapResponsePtr( response );
}

void GetTypeDefinitionRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getTypeDefinition" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:typeId" ), BAD_CAST( m_typeId.c_str( ) ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr GetTypeDefinitionResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* session )
{
    GetTypeDefinitionResponse* response = new GetTypeDefinitionResponse( );
    WSSession* wsSession = dynamic_cast< WSSession* >( session );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "type" ) ) )
        {
            libcmis::ObjectTypePtr type( new WSObjectType( wsSession, child ) );
            response->m_type = type;
        }
    }

    return SoapResponsePtr( response );
}

void GetTypeChildrenRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getTypeChildren" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:typeId" ), BAD_CAST( m_typeId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:includePropertyDefinitions" ), BAD_CAST( "true" ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr GetTypeChildrenResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* session )
{
    GetTypeChildrenResponse* response = new GetTypeChildrenResponse( );
    WSSession* wsSession = dynamic_cast< WSSession* >( session );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "types" ) ) )
        {
            for ( xmlNodePtr gdchild = child->children; gdchild; gdchild = gdchild->next )
            {
                if ( xmlStrEqual( gdchild->name, BAD_CAST( "types" ) ) )
                {
                    libcmis::ObjectTypePtr type( new WSObjectType( wsSession, gdchild ) );
                    response->m_children.push_back( type );
                }
            }
        }
    }

    return SoapResponsePtr( response );
}

void GetObjectRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getObject" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:objectId" ), BAD_CAST( m_id.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:includeAllowableActions" ), BAD_CAST( "true" ) );

    // Ask for renditions... some servers like Alfresco are providing them only this way
    // and it saves time (another HTTP request) anyway.
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:renditionFilter" ), BAD_CAST( "*" ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr GetObjectResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* session )
{
    GetObjectResponse* response = new GetObjectResponse( );
    WSSession* wsSession = dynamic_cast< WSSession* >( session );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "object" ) ) )
        {
            libcmis::ObjectPtr object;
            WSObject tmp( wsSession, child );
            if ( tmp.getBaseType( ) == "cmis:folder" )
            {
                object.reset( new WSFolder( tmp ) );
            }
            else if ( tmp.getBaseType( ) == "cmis:document" )
            {
                object.reset( new WSDocument( tmp ) );
            }
            else
            {
                // This should never happen... but who knows if the standard is 100% repected?
                object.reset( new WSObject( wsSession, child ) );
            }
            response->m_object = object;
        }
    }

    return SoapResponsePtr( response );
}

void GetObjectByPathRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getObjectByPath" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:path" ), BAD_CAST( m_path.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:includeAllowableActions" ), BAD_CAST( "true" ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:renditionFilter" ), BAD_CAST( "*" ) );

    (void)xmlTextWriterEndElement( writer );
}

void UpdatePropertiesRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:updateProperties" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:objectId" ), BAD_CAST( m_objectId.c_str( ) ) );

    if ( !m_changeToken.empty( ) )
        (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:changeToken" ), BAD_CAST( m_changeToken.c_str( ) ) );

    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:properties" ) );
    for ( PropertyPtrMap::const_iterator it = m_properties.begin( );
            it != m_properties.end( ); ++it )
    {
        libcmis::PropertyPtr property = it->second;
        if( property->getPropertyType( )->isUpdatable( ) )
            property->toXml( writer );
    }
    (void)xmlTextWriterEndElement( writer ); // cmis:properties


    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr UpdatePropertiesResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* )
{
    UpdatePropertiesResponse* response = new UpdatePropertiesResponse( );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "objectId" ) ) )
        {
            xmlChar* content = xmlNodeGetContent( child );
            if ( content != NULL )
            {
                string value( ( char* ) content );
                xmlFree( content );
                response->m_id = std::move(value);
            }
        }
    }

    return SoapResponsePtr( response );
}

void DeleteObjectRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:deleteObject" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:objectId" ), BAD_CAST( m_objectId.c_str( ) ) );

    string allVersionsStr( "false" );
    if ( m_allVersions )
        allVersionsStr = "true";
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:allVersions" ), BAD_CAST( allVersionsStr.c_str( ) ) );

    (void)xmlTextWriterEndElement( writer );
}

void DeleteTreeRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:deleteTree" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:folderId" ), BAD_CAST( m_folderId.c_str( ) ) );

    string allVersionsStr( "false" );
    if ( m_allVersions )
        allVersionsStr = "true";
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:allVersions" ), BAD_CAST( allVersionsStr.c_str( ) ) );

    string unfileStr( "" );
    switch ( m_unfile )
    {
        case libcmis::UnfileObjects::Unfile:
            unfileStr = "unfile";
            break;
        case libcmis::UnfileObjects::DeleteSingleFiled:
            unfileStr = "deletesinglefiled";
            break;
        case libcmis::UnfileObjects::Delete:
            unfileStr = "delete";
            break;
        default:
            break;
    }
    if ( !unfileStr.empty( ) )
        (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:unfileObjects" ), BAD_CAST( unfileStr.c_str( ) ) );

    string continueStr( "false" );
    if ( m_continueOnFailure )
        continueStr = "true";
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:continueOnFailure" ), BAD_CAST( continueStr.c_str( ) ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr DeleteTreeResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* )
{
    DeleteTreeResponse* response = new DeleteTreeResponse( );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "failedToDelete" ) ) )
        {
            for ( xmlNodePtr gdchild = child->children; gdchild; gdchild = gdchild->next )
            {
                if ( xmlStrEqual( gdchild->name, BAD_CAST( "objectIds" ) ) )
                {
                    xmlChar* content = xmlNodeGetContent( gdchild );
                    if ( content != NULL )
                    {
                        string value( ( char* ) content );
                        xmlFree( content );
                        response->m_failedIds.push_back( std::move(value) );
                    }
                }
            }
        }
    }

    return SoapResponsePtr( response );
}

void MoveObjectRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:moveObject" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:objectId" ), BAD_CAST( m_objectId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:targetFolderId" ), BAD_CAST( m_destId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:sourceFolderId" ), BAD_CAST( m_srcId.c_str( ) ) );

    (void)xmlTextWriterEndElement( writer );
}

void GetContentStreamRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getContentStream" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:objectId" ), BAD_CAST( m_objectId.c_str( ) ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr GetContentStreamResponse::create( xmlNodePtr node, RelatedMultipart& multipart, SoapSession* )
{
    GetContentStreamResponse* response = new GetContentStreamResponse( );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "contentStream" ) ) )
        {
            for ( xmlNodePtr gdchild = child->children; gdchild; gdchild = gdchild->next )
            {
                if ( xmlStrEqual( gdchild->name, BAD_CAST( "stream" ) ) )
                {
                    xmlChar* content = xmlNodeGetContent( gdchild );
                    if ( content != NULL )
                    {
                        // We can either have directly the base64 encoded data or
                        // an <xop:Include> pointing to another part of the multipart
                        response->m_stream = getStreamFromNode( gdchild, multipart );
                    }
                    xmlFree( content );
                }
            }
        }
    }

    return SoapResponsePtr( response );
}

void GetObjectParentsRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getObjectParents" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:objectId" ), BAD_CAST( m_objectId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:includeAllowableActions" ), BAD_CAST( "true" ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:renditionFilter" ), BAD_CAST( "*" ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr GetObjectParentsResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* session )
{
    GetObjectParentsResponse* response = new GetObjectParentsResponse( );
    WSSession* wsSession = dynamic_cast< WSSession* >( session );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "parents" ) ) )
        {
            for ( xmlNodePtr gdchild = child->children; gdchild; gdchild = gdchild->next )
            {
                if ( xmlStrEqual( gdchild->name, BAD_CAST( "object" ) ) )
                {
                    libcmis::FolderPtr parent;
                    WSObject tmp( wsSession, gdchild );
                    if ( tmp.getBaseType( ) == "cmis:folder" )
                    {
                        parent.reset( new WSFolder( tmp ) );
                        response->m_parents.push_back( parent );
                    }
                }
            }
        }
    }

    return SoapResponsePtr( response );
}

void GetChildrenRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getChildren" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:folderId" ), BAD_CAST( m_folderId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:includeAllowableActions" ), BAD_CAST( "true" ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:renditionFilter" ), BAD_CAST( "*" ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr GetChildrenResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* session )
{
    GetChildrenResponse* response = new GetChildrenResponse( );
    WSSession* wsSession = dynamic_cast< WSSession* >( session );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "objects" ) ) )
        {
            for ( xmlNodePtr gdchild = child->children; gdchild; gdchild = gdchild->next )
            {
                if ( xmlStrEqual( gdchild->name, BAD_CAST( "objects" ) ) )
                {
                    for ( xmlNodePtr gdgdchild = gdchild->children; gdgdchild; gdgdchild = gdgdchild->next )
                    {
                        if ( xmlStrEqual( gdgdchild->name, BAD_CAST( "object" ) ) )
                        {
                            libcmis::ObjectPtr object;
                            WSObject tmp( wsSession, gdgdchild );
                            if ( tmp.getBaseType( ) == "cmis:folder" )
                            {
                                object.reset( new WSFolder( tmp ) );
                            }
                            else if ( tmp.getBaseType( ) == "cmis:document" )
                            {
                                object.reset( new WSDocument( tmp ) );
                            }
                            else
                            {
                                // This should never happen... but who knows if the standard is 100% repected?
                                object.reset( new WSObject( wsSession, gdgdchild ) );
                            }
                            response->m_children.push_back( object );
                        }
                    }
                }
            }
        }
    }

    return SoapResponsePtr( response );
}

void CreateFolderRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:createFolder" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );

    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:properties" ) );
    for ( PropertyPtrMap::const_iterator it = m_properties.begin( );
            it != m_properties.end( ); ++it )
    {
        libcmis::PropertyPtr property = it->second;
        property->toXml( writer );
    }
    (void)xmlTextWriterEndElement( writer ); // cmis:properties

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:folderId" ), BAD_CAST( m_folderId.c_str( ) ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr CreateFolderResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* )
{
    CreateFolderResponse* response = new CreateFolderResponse( );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "objectId" ) ) )
        {
            xmlChar* content = xmlNodeGetContent( child );
            if ( content != NULL )
            {
                string value( ( char* ) content );
                xmlFree( content );
                response->m_id = std::move(value);
            }
        }
    }

    return SoapResponsePtr( response );
}

void CreateDocumentRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:createDocument" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );

    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:properties" ) );
    for ( PropertyPtrMap::const_iterator it = m_properties.begin( );
            it != m_properties.end( ); ++it )
    {
        libcmis::PropertyPtr property = it->second;
        property->toXml( writer );
    }
    (void)xmlTextWriterEndElement( writer ); // cmis:properties

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:folderId" ), BAD_CAST( m_folderId.c_str( ) ) );

    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:contentStream" ) );
    writeCmismStream( writer, m_multipart, m_stream, m_contentType, m_filename );
    (void)xmlTextWriterEndElement( writer ); // cmism:contentStream

    (void)xmlTextWriterEndElement( writer );
}

void SetContentStreamRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:setContentStream" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:objectId" ), BAD_CAST( m_objectId.c_str( ) ) );

    string overwrite( "false" );
    if ( m_overwrite )
        overwrite = "true";
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:overwriteFlag" ), BAD_CAST( overwrite.c_str( ) ) );

    if ( !m_changeToken.empty( ) )
        (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:changeToken" ), BAD_CAST( m_changeToken.c_str( ) ) );

    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:contentStream" ) );
    writeCmismStream( writer, m_multipart, m_stream, m_contentType, m_filename );

    (void)xmlTextWriterEndElement( writer ); // cmism:contentStream

    (void)xmlTextWriterEndElement( writer );
}

void GetRenditionsRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getRenditions" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:objectId" ), BAD_CAST( m_objectId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:renditionFilter" ), BAD_CAST( m_filter.c_str( ) ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr GetRenditionsResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* )
{
    GetRenditionsResponse* response = new GetRenditionsResponse( );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "renditions" ) ) )
        {
            libcmis::RenditionPtr rendition( new libcmis::Rendition( child ) );
            response->m_renditions.push_back( rendition );
        }
    }

    return SoapResponsePtr( response );
}

void CheckOutRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:checkOut" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:objectId" ), BAD_CAST( m_objectId.c_str( ) ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr CheckOutResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* )
{
    CheckOutResponse* response = new CheckOutResponse( );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "objectId" ) ) )
        {
            xmlChar* content = xmlNodeGetContent( child );
            if ( content != NULL )
            {
                string value( ( char* ) content );
                xmlFree( content );
                response->m_objectId = std::move(value);
            }
        }
    }

    return SoapResponsePtr( response );
}

void CancelCheckOutRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:cancelCheckOut" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:objectId" ), BAD_CAST( m_objectId.c_str( ) ) );

    (void)xmlTextWriterEndElement( writer );
}

void CheckInRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:checkIn" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:objectId" ), BAD_CAST( m_objectId.c_str( ) ) );

    string major = "false";
    if ( m_isMajor )
        major = "true";
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:major" ), BAD_CAST( major.c_str( ) ) );

    if ( m_properties.empty( ) )
    {
        (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:properties" ) );
        for ( PropertyPtrMap::const_iterator it = m_properties.begin( );
                it != m_properties.end( ); ++it )
        {
            libcmis::PropertyPtr property = it->second;
            property->toXml( writer );
        }
        (void)xmlTextWriterEndElement( writer ); // cmis:properties
    }

    if ( m_stream.get( ) )
    {
        (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:contentStream" ) );
        writeCmismStream( writer, m_multipart, m_stream, m_contentType, m_fileName );
        (void)xmlTextWriterEndElement( writer ); // cmism:contentStream
    }

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:checkinComment" ), BAD_CAST( m_comment.c_str( ) ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr CheckInResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* )
{
    CheckInResponse* response = new CheckInResponse( );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "objectId" ) ) )
        {
            xmlChar* content = xmlNodeGetContent( child );
            if ( content != NULL )
            {
                string value( ( char* ) content );
                xmlFree( content );
                response->m_objectId = std::move(value);
            }
        }
    }

    return SoapResponsePtr( response );
}

void GetAllVersionsRequest::toXml( xmlTextWriterPtr writer )
{
    (void)xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getAllVersions" ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    (void)xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:objectId" ), BAD_CAST( m_objectId.c_str( ) ) );
    (void)xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:includeAllowableActions" ), BAD_CAST( "true" ) );

    (void)xmlTextWriterEndElement( writer );
}

SoapResponsePtr GetAllVersionsResponse::create( xmlNodePtr node, RelatedMultipart&, SoapSession* session )
{
    GetAllVersionsResponse* response = new GetAllVersionsResponse( );
    WSSession* wsSession = dynamic_cast< WSSession* >( session );

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "objects" ) ) )
        {
            WSObject tmp( wsSession, child );
            if ( tmp.getBaseType( ) == "cmis:document" )
            {
                libcmis::DocumentPtr object( new WSDocument( tmp ) );
                response->m_objects.push_back( object );
            }
        }
    }

    return SoapResponsePtr( response );
}
