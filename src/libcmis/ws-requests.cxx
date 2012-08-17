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

#include "ws-object-type.hxx"
#include "ws-requests.hxx"
#include "xml-utils.hxx"

using namespace std;

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
        string value( ( char * )content );
        xmlFree( content );

        if ( xmlStrEqual( child->name, BAD_CAST( "type" ) ) )
        {
            m_type = value;
        }
        else if ( xmlStrEqual( child->name, BAD_CAST( "code" ) ) )
        {
            try
            {
                m_code = libcmis::parseInteger( value );
            }
            catch ( const libcmis::Exception& )
            {
                // Simply leave the default error code if unparsable
            }
        }
        else if ( xmlStrEqual( child->name, BAD_CAST( "message" ) ) )
        {
            m_message = value;
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

SoapFaultDetailPtr CmisSoapFaultDetail::create( xmlNodePtr node )
{
    return SoapFaultDetailPtr( new CmisSoapFaultDetail( node ) );
}

void GetRepositories::toXml( xmlTextWriterPtr writer )
{
    xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getRepositories" ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns" ), BAD_CAST( NS_CMIS_URL ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );
    xmlTextWriterEndElement( writer );
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
                string value( ( char* ) content );
                xmlFree( content );

                if ( xmlStrEqual( repoChild->name, BAD_CAST( "repositoryId" ) ) )
                {
                    id = value;
                }
                else if ( xmlStrEqual( repoChild->name, BAD_CAST( "repositoryName" ) ) )
                {
                    name = value;
                }

            }

            if ( !id.empty( ) )
                response->m_repositories[ id ] = name;
        }
    }

    return SoapResponsePtr( response );
}

void GetRepositoryInfo::toXml( xmlTextWriterPtr writer )
{
    xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getRepositoryInfo" ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns" ), BAD_CAST( NS_CMIS_URL ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_id.c_str( ) ) );

    xmlTextWriterEndElement( writer );
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

void GetTypeDefinition::toXml( xmlTextWriterPtr writer )
{
    xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getTypeDefinition" ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns" ), BAD_CAST( NS_CMIS_URL ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:typeId" ), BAD_CAST( m_typeId.c_str( ) ) );

    xmlTextWriterEndElement( writer );
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

void GetTypeChildren::toXml( xmlTextWriterPtr writer )
{
    xmlTextWriterStartElement( writer, BAD_CAST( "cmism:getTypeChildren" ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns" ), BAD_CAST( NS_CMIS_URL ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmism" ), BAD_CAST( NS_CMISM_URL ) );

    xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:repositoryId" ), BAD_CAST( m_repositoryId.c_str( ) ) );
    xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:typeId" ), BAD_CAST( m_typeId.c_str( ) ) );
    xmlTextWriterWriteElement( writer, BAD_CAST( "cmism:includePropertyDefinitions" ), BAD_CAST( "true" ) );

    xmlTextWriterEndElement( writer );
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
