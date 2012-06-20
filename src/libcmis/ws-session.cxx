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

#include <sstream>

#include <boost/date_time.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "ws-requests.hxx"
#include "ws-session.hxx"
#include "xml-utils.hxx"

using namespace std;

WSSession::WSSession( string bindingUrl, string repositoryId, 
        string username, string password, bool verbose ) throw ( libcmis::Exception ) :
    BaseSession( bindingUrl, repositoryId, username, password, verbose ),
    m_servicesUrls( ),
    m_responseFactory( )
{
    initialize( );
}

WSSession::WSSession( const WSSession& copy ) :
    BaseSession( copy ),
    m_servicesUrls( copy.m_servicesUrls ),
    m_responseFactory( copy.m_responseFactory )
{
}


WSSession& WSSession::operator=( const WSSession& copy )
{
    BaseSession::operator=( copy );
    m_servicesUrls = copy.m_servicesUrls;
    m_responseFactory = copy.m_responseFactory;
    
    return *this;
}

WSSession::~WSSession( )
{
}

string WSSession::getWsdl( string url ) throw ( CurlException )
{
    string buf = httpGetRequest( url )->str( );

    // Do we have a wsdl file?
    bool isWsdl = false;

    xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), m_bindingUrl.c_str(), NULL, 0 );
    if ( NULL != doc )
    {
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
        libcmis::registerCmisWSNamespaces( xpathCtx );

        if ( NULL != xpathCtx )
        {
            string definitionsXPath( "/wsdl:definitions" );
            xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( definitionsXPath.c_str() ), xpathCtx );

            isWsdl = ( xpathObj != NULL ) && ( xpathObj->nodesetval != NULL ) && ( xpathObj->nodesetval->nodeNr > 0 );
        }
        xmlXPathFreeContext( xpathCtx );
    }

    // If we don't have a wsdl file we may have received an HTML explanation for it,
    // try to add ?wsdl to the URL (last chance to get something)
    if ( !isWsdl )
    {
        if ( url.find( "?" ) == string::npos )
            url += "?";
        else
            url += "&";
        url += "wsdl";

        buf = httpGetRequest( url )->str( );
    }

    return buf;
}

vector< SoapResponsePtr > WSSession::soapRequest( string& url, SoapRequest& request ) throw ( SoapFault, CurlException )
{
#if 0
    // Place the request in an envelope
    string xml = createEnvelope( request );
    
    // TODO Embed the xml in a multipart/related body.
    // Do it manually as libcurl doesn't implement it!
    // See ObjectService.createDocument.request.xml for an example of request with
    // several related parts. The parts ID, should contain a UUID (see boost::uuid).
    // An idea of id to generate: rootpart*uuid@libcmis.sourceforge.net

    string response = httpPostRequest( url, multipart, contentType );

    // TODO Extract the response from the multipart/related body of the response

    // Get the responses
   return getResponseFactory( ).parseResponse( response );
#endif
   // TODO Implement me
   return vector< SoapResponsePtr >( );
}

void WSSession::initialize( ) throw ( libcmis::Exception )
{
    if ( m_repositoriesIds.empty() )
    {
        // Get the wsdl file
        string buf;
        try
        {
            buf = getWsdl( m_bindingUrl );
        }
        catch ( const CurlException& e )
        {
            throw e.getCmisException( );
        }
       
        // parse the content
        xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), m_bindingUrl.c_str(), NULL, 0 );

        if ( NULL != doc )
        {
            // Get all the services soap URLs
            m_servicesUrls.clear( );

            xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
            libcmis::registerCmisWSNamespaces( xpathCtx );

            if ( NULL != xpathCtx )
            {
                string serviceXPath( "//wsdl:service" );
                xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( serviceXPath.c_str() ), xpathCtx );

                if ( xpathObj != NULL )
                {
                    int nbServices = 0;
                    if ( xpathObj->nodesetval )
                        nbServices = xpathObj->nodesetval->nodeNr;

                    for ( int i = 0; i < nbServices; i++ )
                    {
                        // What service do we have here?
                        xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];
                        string name = libcmis::getXmlNodeAttributeValue( node, "name" );

                        // Gimme you soap:address location attribute
                        string locationXPath = serviceXPath + "[@name='" + name + "']/wsdl:port/soap:address/attribute::location";
                        string location = libcmis::getXPathValue( xpathCtx, locationXPath );

                        m_servicesUrls[name] = location;
                    }
                }
            }
            xmlXPathFreeContext( xpathCtx );
        }
        else
            throw libcmis::Exception( "Failed to parse service document" );

        xmlFreeDoc( doc );

        // Initialize the response factory
        map< string, string > ns;
        ns[ "wsssecurity" ] = "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-wssecurity-secext-1.0.xsd";
        ns[ NS_SOAP_ENV_PREFIX ] = NS_SOAP_ENV_URL;
        ns[ "cmism" ] = NS_CMISM_URL;
        ns[ "cmisw" ] = NS_CMISW_URL;
        ns[ "cmis" ] = NS_CMIS_URL;
        m_responseFactory.setNamespaces( ns );
        m_responseFactory.setMapping( getResponseMapping() );

        // Get all repositories Ids
        map< string, string > repositories = getRepositoryService( ).getRepositories( );
        for ( map< string, string >::iterator it = repositories.begin( );
              it != repositories.end( ); ++it )
        {
            m_repositoriesIds.push_back( it->first );
        }
    }
}

map< string, SoapResponseCreator > WSSession::getResponseMapping( )
{
    map< string, SoapResponseCreator > mapping;

    mapping[ "getRepositoriesResponse" ] = &GetRepositoriesResponse::create;

    return mapping;
}

string WSSession::createEnvelope( SoapRequest& request )
{
    xmlBufferPtr buf = xmlBufferCreate( );
    xmlTextWriterPtr writer = xmlNewTextWriterMemory( buf, 0 );

    xmlTextWriterStartDocument( writer, NULL, NULL, NULL );

    /**
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

    // TODO Use a more secure password transmission (PasswordDigest). See Basic Security Profile 1.0 section 11.1.3
    xmlChar* passTypeStr = BAD_CAST( "http://docs.oasis-open.org/wss/2004/01/oasis-200401-wss-username-token-profile-1.0#PasswordText" );

    // Created must be a UTC time with no more than 3 digits fractional seconds.
    boost::posix_time::ptime created( boost::posix_time::second_clock::universal_time( ) );
    boost::posix_time::ptime expires( created );
    expires = expires + boost::gregorian::days( 1 );
    xmlChar* createdStr = BAD_CAST( libcmis::writeDateTime( created ).c_str( ) );
    xmlChar* expiresStr = BAD_CAST( libcmis::writeDateTime( expires ).c_str( ) );

    xmlTextWriterStartElement( writer, BAD_CAST( "S:Envelope" ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "S" ), BAD_CAST( NS_SOAP_ENV_URL ) );
    
    xmlTextWriterStartElement( writer, BAD_CAST( "S:Header" ) );

    // Write out the Basic Security Profile 1.0 compliant headers
    xmlTextWriterStartElement( writer, BAD_CAST( "Security" ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns" ), wsseUrl );

    xmlTextWriterStartElement( writer, BAD_CAST( "Timestamp" ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns" ), wsuUrl );
    xmlTextWriterStartElement( writer, BAD_CAST( "Created" ) );
    xmlTextWriterWriteRaw( writer, createdStr );
    xmlTextWriterEndElement( writer ); // End of Created
    xmlTextWriterStartElement( writer, BAD_CAST( "Expires" ) );
    xmlTextWriterWriteRaw( writer, expiresStr );
    xmlTextWriterEndElement( writer ); // End of Expires
    xmlTextWriterEndElement( writer ); // End of Timestamp

    xmlTextWriterStartElement( writer, BAD_CAST( "UsernameToken" ) );
    xmlTextWriterWriteElement( writer, BAD_CAST( "Username" ), BAD_CAST( getUsername( ).c_str( ) ) );
    xmlTextWriterStartElement( writer, BAD_CAST( "Password" ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "Type" ), passTypeStr );
    xmlTextWriterWriteRaw( writer, BAD_CAST( getPassword( ).c_str( ) ) );
    xmlTextWriterEndElement( writer ); // End of Password
    xmlTextWriterStartElement( writer, BAD_CAST( "Created" ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns" ), wsuUrl );
    xmlTextWriterWriteRaw( writer, createdStr );
    xmlTextWriterEndElement( writer ); // End of Created
    xmlTextWriterEndElement( writer ); // End of UsernameToken

    xmlTextWriterEndElement( writer ); // End of Security

    xmlTextWriterEndElement( writer ); // End of S:Header

    xmlTextWriterStartElement( writer, BAD_CAST( "S:Body" ) );
    request.toXml( writer );
    xmlTextWriterEndElement( writer ); // End of S:Body

    xmlTextWriterEndElement( writer );  // End of S:Envelope
    xmlTextWriterEndDocument( writer );

    string str( ( const char * )xmlBufferContent( buf ) );

    xmlFreeTextWriter( writer );
    xmlBufferFree( buf );

    return str;
}

string WSSession::getServiceUrl( string name )
{
    string url;

    map< string, string >::iterator it = m_servicesUrls.find( name );
    if ( it != m_servicesUrls.end( ) )
        url = it->second;

    return url;
}

RepositoryService WSSession::getRepositoryService( )
{
    return RepositoryService( this );
}

list< string > WSSession::getRepositories( string url, string username, string password, bool verbose ) throw ( libcmis::Exception )
{
    WSSession session( url, string(), username, password, verbose );
    session.initialize( );
    return session.m_repositoriesIds;
}

libcmis::RepositoryPtr WSSession::getRepository( ) throw ( libcmis::Exception )
{
    libcmis::RepositoryPtr empty;

    // TODO Implement me
    return empty;
}

libcmis::ObjectPtr WSSession::getObject( string id ) throw ( libcmis::Exception )
{
    libcmis::ObjectPtr empty;

    // TODO Implement me
    return empty;
}

libcmis::ObjectPtr WSSession::getObjectByPath( string path ) throw ( libcmis::Exception )
{
    libcmis::ObjectPtr empty;

    // TODO Implement me
    return empty;
}

libcmis::ObjectTypePtr WSSession::getType( string id ) throw ( libcmis::Exception )
{
    libcmis::ObjectTypePtr empty;

    // TODO Implement me
    return empty;
}

