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
    // We don't want to have the HTTP exceptions as the errors are coming
    // back as SoapFault elements.
    setNoHttpErrors( true );
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
    if ( this != &copy )
    {
        BaseSession::operator=( copy );
        m_servicesUrls = copy.m_servicesUrls;
        m_responseFactory = copy.m_responseFactory;
    }
    
    return *this;
}

WSSession::~WSSession( )
{
}

string WSSession::getWsdl( string url ) throw ( CurlException )
{
    string buf = httpGetRequest( url )->getStream( )->str( );

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

        buf = httpGetRequest( url )->getStream( )->str( );
    }

    return buf;
}

vector< SoapResponsePtr > WSSession::soapRequest( string& url, SoapRequest& request ) throw ( libcmis::Exception )
{
    vector< SoapResponsePtr > responses;

    try
    {
        // Place the request in an envelope
        RelatedMultipart& multipart = request.getMultipart( getUsername( ), getPassword( ) );
        libcmis::HttpResponsePtr response = httpPostRequest( url, *multipart.toStream( ).get( ), multipart.getContentType( ) );

        string responseType;
        map< string, string >::iterator it = response->getHeaders( ).find( "Content-Type" );
        if ( it != response->getHeaders( ).end( ) )
        {
            responseType = it->second;
            RelatedMultipart answer( response->getStream( )->str( ), responseType );
        
            responses = getResponseFactory( ).parseResponse( answer );
        }
    }
    catch ( const SoapFault& fault )
    {
        boost::shared_ptr< libcmis::Exception > cmisException = getCmisException( fault );
        if ( !cmisException.get( ) )
        {
            cmisException.reset( new libcmis::Exception( fault.what( ), "runtime" ) );
        }
        throw *cmisException.get( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    return responses;
}

void WSSession::initialize( ) throw ( libcmis::Exception )
{
    if ( m_repositories.empty() )
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
        m_responseFactory.setDetailMapping( getDetailMapping( ) );
        m_responseFactory.setSession( this );

        // Get all repositories
        map< string, string > repositories = getRepositoryService( ).getRepositories( );
        for ( map< string, string >::iterator it = repositories.begin( );
              it != repositories.end( ); ++it )
        {
            string repoId = it->first;
            m_repositories.push_back( getRepositoryService( ).getRepositoryInfo( repoId ) );
        }
    }
}

map< string, SoapResponseCreator > WSSession::getResponseMapping( )
{
    map< string, SoapResponseCreator > mapping;

    mapping[ "{" + string( NS_CMISM_URL ) + "}getRepositoriesResponse" ] = &GetRepositoriesResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}getRepositoryInfoResponse" ] = &GetRepositoryInfoResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}getTypeDefinitionResponse" ] = &GetTypeDefinitionResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}getTypeChildrenResponse" ] = &GetTypeChildrenResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}getObjectResponse" ] = &GetObjectResponse::create;
    // No need to create a GetObjectByPathResponse as it would do the same than GetObjectResponse
    mapping[ "{" + string( NS_CMISM_URL ) + "}getObjectByPathResponse" ] = &GetObjectResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}updatePropertiesResponse" ] = &UpdatePropertiesResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}deleteTreeResponse" ] = &DeleteTreeResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}getContentStreamResponse" ] = &GetContentStreamResponse::create;

    return mapping;
}

map< string, SoapFaultDetailCreator > WSSession::getDetailMapping( )
{
    map< string, SoapFaultDetailCreator > mapping;

    mapping[ "{" + string( NS_CMISM_URL ) + "}cmisFault" ] = &CmisSoapFaultDetail::create;

    return mapping;
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

ObjectService WSSession::getObjectService( )
{
    return ObjectService( this );
}

list< libcmis::RepositoryPtr > WSSession::getRepositories( string url, string username, string password, bool verbose ) throw ( libcmis::Exception )
{
    WSSession session( url, string(), username, password, verbose );
    session.initialize( );
    return session.m_repositories;
}

libcmis::RepositoryPtr WSSession::getRepository( ) throw ( libcmis::Exception )
{
    return getRepositoryService( ).getRepositoryInfo( m_repositoryId );
}

libcmis::ObjectPtr WSSession::getObject( string id ) throw ( libcmis::Exception )
{
    return getObjectService( ).getObject( getRepositoryId( ), id );
}

libcmis::ObjectPtr WSSession::getObjectByPath( string path ) throw ( libcmis::Exception )
{
    return getObjectService( ).getObjectByPath( getRepositoryId( ), path );
}

libcmis::ObjectTypePtr WSSession::getType( string id ) throw ( libcmis::Exception )
{
    return getRepositoryService( ).getTypeDefinition( m_repositoryId, id );
}

