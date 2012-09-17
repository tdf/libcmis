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

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xmlstring.h>

#include "ws-soap.hxx"
#include "xml-utils.hxx"

using namespace std;
using namespace boost::uuids;

SoapFault::SoapFault( xmlNodePtr node, SoapResponseFactory* factory ) :
    exception( ),
    m_faultcode( ),
    m_faultstring( ),
    m_detail( ),
    m_message( )
{
    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "faultcode" ) ) )
        {
            xmlChar* content = xmlNodeGetContent( child );
            xmlChar* prefix = NULL;
            xmlChar* localName = xmlSplitQName2( content, &prefix );
            if (localName == NULL)
                localName = xmlStrdup( content );
            m_faultcode = string( ( char* )localName );
            xmlFree( content );
            xmlFree( localName );
        }
        else if ( xmlStrEqual( child->name, BAD_CAST( "faultstring" ) ) )
        {
            xmlChar* content = xmlNodeGetContent( child );
            m_faultstring = string( ( char* )content );
            xmlFree( content );
        }
        else if ( xmlStrEqual( child->name, BAD_CAST( "detail" ) ) )
        {
            m_detail = factory->parseFaultDetail( child );
        }
    }
    
    m_message = getFaultcode() + ": " + getFaultstring();
    for ( vector< SoapFaultDetailPtr >::const_iterator it = m_detail.begin( ); it != m_detail.end( ); ++it )
    {
        m_message += "\n" + ( *it )->toString( );
    }

}

const char* SoapFault::what( ) const throw ( )
{
    return m_message.c_str( );
}


SoapResponseFactory::SoapResponseFactory( ) :
    m_mapping( ),
    m_namespaces( ),
    m_detailMapping( ),
    m_session( NULL )
{
}

SoapResponseFactory::SoapResponseFactory( const SoapResponseFactory& copy ) :
    m_mapping( copy.m_mapping ),
    m_namespaces( copy.m_namespaces ),
    m_detailMapping( copy.m_detailMapping ),
    m_session( copy.m_session )
{
}

SoapResponseFactory& SoapResponseFactory::operator=( const SoapResponseFactory& copy )
{
    if ( this != &copy )
    {
        m_mapping = copy.m_mapping;
        m_namespaces = copy.m_namespaces;
        m_detailMapping = copy.m_detailMapping;
        m_session = copy.m_session;
    }

    return *this;
}

vector< SoapResponsePtr > SoapResponseFactory::parseResponse( string& xml ) throw ( SoapFault )
{
    // Create a fake multipart
    RelatedMultipart multipart;
    string name = "root";
    string type = "text/xml";
    string info;
    RelatedPartPtr part( new RelatedPart( name, type, xml ) );
    string cid = multipart.addPart( part );
    multipart.setStart( cid, info );

    // Then parse it normally
    return parseResponse( multipart );
}

vector< SoapResponsePtr > SoapResponseFactory::parseResponse( RelatedMultipart& multipart ) throw ( SoapFault )
{
    string xml;
    RelatedPartPtr part = multipart.getPart( multipart.getStartId( ) );
    if ( part.get() != NULL )
        xml = part->getContent( );

    vector< SoapResponsePtr > responses;

    xmlDocPtr doc = xmlReadMemory( xml.c_str(), xml.size(), "", NULL, 0 );

    if ( NULL != doc )
    {
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
        libcmis::registerSoapNamespaces( xpathCtx );

        for ( map< string, string >::iterator it = m_namespaces.begin( );
              it != m_namespaces.end( ); ++it )
        {
            xmlXPathRegisterNs( xpathCtx, BAD_CAST( it->first.c_str() ),  BAD_CAST( it->second.c_str( ) ) );
        }

        if ( NULL != xpathCtx )
        {
            string bodyXPath( "//soap-env:Body/*" );
            xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( bodyXPath.c_str() ), xpathCtx );

            if ( xpathObj != NULL )
            {
                int nbChildren = 0;
                if ( xpathObj->nodesetval )
                    nbChildren = xpathObj->nodesetval->nodeNr;

                for ( int i = 0; i < nbChildren; i++ )
                {
                    xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];

                    // Is it a fault?
                    if ( xmlStrEqual( BAD_CAST( NS_SOAP_ENV_URL ), node->ns->href ) &&
                         xmlStrEqual( BAD_CAST( "Fault" ), node->name ) )
                    {
                        throw SoapFault( node, this );
                    }
                    SoapResponsePtr response = createResponse( node, multipart );
                    if ( NULL != response.get( ) )
                        responses.push_back( response );
                }
            }
        }
        xmlXPathFreeContext( xpathCtx );
    }

    xmlFreeDoc( doc );

    return responses;
}

SoapResponsePtr SoapResponseFactory::createResponse( xmlNodePtr node, RelatedMultipart& multipart )
{
    SoapResponsePtr response;

    string ns( ( const char* ) node->ns->href );
    string name( ( const char* ) node->name );
    string id = "{" + ns + "}" + name;
    map< string, SoapResponseCreator >::iterator it = m_mapping.find( id );

    if ( it != m_mapping.end( ) )
    {
        SoapResponseCreator creator = it->second;
        response = creator( node, multipart, m_session );
    }

    return response;
}

vector< SoapFaultDetailPtr > SoapResponseFactory::parseFaultDetail( xmlNodePtr node )
{
    vector< SoapFaultDetailPtr > detail;

    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        string ns;
        if ( child->ns != NULL )
            ns = string( ( const char* ) child->ns->href );
        string name( ( const char* ) child->name );
        string id = "{" + ns + "}" + name;
        map< string, SoapFaultDetailCreator >::iterator it = m_detailMapping.find( id );

        if ( it != m_detailMapping.end( ) )
        {
            SoapFaultDetailCreator creator = it->second;
            detail.push_back( creator( child ) );
        }
    }

    return detail;
}

RelatedMultipart& SoapRequest::getMultipart( string& username, string& password )
{
    // Generate the envelope and add it to the multipart
    string envelope = createEnvelope( username, password );
    string name( "root" );
    string type( "application/xop+xml;charset=UTF-8;type=\"text/xml\"" );
    RelatedPartPtr envelopePart( new RelatedPart( name, type, envelope ) );
    string rootId = m_multipart.addPart( envelopePart );

    // Set the envelope as the start part of the multipart
    string startInfo( "text/xml" );
    m_multipart.setStart( rootId, startInfo );

    return m_multipart;
}

string SoapRequest::createEnvelope( string& username, string& password )
{
    xmlBufferPtr buf = xmlBufferCreate( );
    xmlTextWriterPtr writer = xmlNewTextWriterMemory( buf, 0 );

    xmlTextWriterStartDocument( writer, NULL, NULL, NULL );

    /* Sample envelope:
     <S:Envelope xmlns:S="http://schemas.xmlsoap.org/soap/envelope/">
        <S:Header>
            <Security xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd">
                <Timestamp xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">
                    <Created>2012-06-14T09:20:29Z</Created>
                    <Expires>2012-06-15T09:20:29Z</Expires>
                </Timestamp>
                <UsernameToken>
                    <Username>admin</Username>
                    <Password Type="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordText">admin</Password>
                    <Created xmlns="http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd">2012-06-14T09:20:29Z</Created>
                </UsernameToken>
            </Security>
        </S:Header>
        <S:Body>
            <ns2:getRepositories xmlns="http://docs.oasis-open.org/ns/cmis/core/200908/" xmlns:ns2="http://docs.oasis-open.org/ns/cmis/messaging/200908/"/>
        </S:Body>
     </S:Envelope>
     */
    xmlChar* wsseUrl = BAD_CAST( "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd" );
    xmlChar* wsuUrl = BAD_CAST( "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-utility-1.0.xsd" );

    // Use an unsecure password transmission (PasswordText) because some clients can't support the PasswordDigest.
    xmlChar* passTypeStr = BAD_CAST( "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordText" );

    // Created must be a UTC time with no more than 3 digits fractional seconds.
    boost::posix_time::ptime created( boost::posix_time::second_clock::universal_time( ) );
    boost::posix_time::ptime expires( created );
    expires = expires + boost::gregorian::days( 1 );
    string createdStr = libcmis::writeDateTime( created );
    xmlChar* expiresStr = BAD_CAST( libcmis::writeDateTime( expires ).c_str( ) );

    xmlTextWriterStartElement( writer, BAD_CAST( "S:Envelope" ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:S" ), BAD_CAST( NS_SOAP_ENV_URL ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:wsu" ), wsuUrl );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:wsse" ), wsseUrl );
    
    xmlTextWriterStartElement( writer, BAD_CAST( "S:Header" ) );

    // Write out the Basic Security Profile 1.0 compliant headers
    xmlTextWriterStartElement( writer, BAD_CAST( "wsse:Security" ) );

    xmlTextWriterStartElement( writer, BAD_CAST( "wsse:Timestamp" ) );
    xmlTextWriterStartElement( writer, BAD_CAST( "wsse:Created" ) );
    xmlTextWriterWriteRaw( writer, BAD_CAST( createdStr.c_str( ) ) );
    xmlTextWriterEndElement( writer ); // End of Created
    xmlTextWriterStartElement( writer, BAD_CAST( "wsse:Expires" ) );
    xmlTextWriterWriteRaw( writer, expiresStr );
    xmlTextWriterEndElement( writer ); // End of Expires
    xmlTextWriterEndElement( writer ); // End of Timestamp

    xmlTextWriterStartElement( writer, BAD_CAST( "wsse:UsernameToken" ) );
    xmlTextWriterWriteElement( writer, BAD_CAST( "wsse:Username" ), BAD_CAST( username.c_str( ) ) );

    xmlTextWriterStartElement( writer, BAD_CAST( "wsse:Password" ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "Type" ), passTypeStr );
    xmlTextWriterWriteRaw( writer, BAD_CAST( password.c_str( ) ) );
    xmlTextWriterEndElement( writer ); // End of Password
    xmlTextWriterStartElement( writer, BAD_CAST( "wsu:Created" ) );
    xmlTextWriterWriteRaw( writer, BAD_CAST( createdStr.c_str( ) ) );
    xmlTextWriterEndElement( writer ); // End of Created
    xmlTextWriterEndElement( writer ); // End of UsernameToken

    xmlTextWriterEndElement( writer ); // End of Security

    xmlTextWriterEndElement( writer ); // End of S:Header

    xmlTextWriterStartElement( writer, BAD_CAST( "S:Body" ) );
    toXml( writer );
    xmlTextWriterEndElement( writer ); // End of S:Body

    xmlTextWriterEndElement( writer );  // End of S:Envelope
    xmlTextWriterEndDocument( writer );

    string str( ( const char * )xmlBufferContent( buf ) );

    xmlFreeTextWriter( writer );
    xmlBufferFree( buf );

    return str;
}
