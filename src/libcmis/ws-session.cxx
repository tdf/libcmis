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

#include "ws-session.hxx"

#include <sstream>

#include <boost/date_time.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <libcmis/xml-utils.hxx>

#include "ws-requests.hxx"

using namespace std;

WSSession::WSSession( string bindingUrl, string repositoryId, string username,
        string password, bool noSslCheck, libcmis::OAuth2DataPtr oauth2,
        bool verbose ) :
    BaseSession( bindingUrl, repositoryId, username, password, noSslCheck, oauth2, verbose ),
    m_servicesUrls( ),
    m_navigationService( NULL ),
    m_objectService( NULL ),
    m_repositoryService( NULL ),
    m_versioningService( NULL ),
    m_responseFactory( )
{
    // We don't want to have the HTTP exceptions as the errors are coming
    // back as SoapFault elements.
    setNoHttpErrors( true );
    initialize( );
}

WSSession::WSSession( string bindingUrl, string repositoryId,
                      const HttpSession& httpSession,
                      libcmis::HttpResponsePtr response ) :
    BaseSession( bindingUrl, repositoryId, httpSession ),
    m_servicesUrls( ),
    m_navigationService( NULL ),
    m_objectService( NULL ),
    m_repositoryService( NULL ),
    m_versioningService( NULL ),
    m_responseFactory( )
{
    // We don't want to have the HTTP exceptions as the errors are coming
    // back as SoapFault elements.
    setNoHttpErrors( true );
    initialize( response );
}

WSSession::WSSession( const WSSession& copy ) :
    BaseSession( copy ),
    m_servicesUrls( copy.m_servicesUrls ),
    m_navigationService( NULL ),
    m_objectService( NULL ),
    m_repositoryService( NULL ),
    m_versioningService( NULL ),
    m_responseFactory( copy.m_responseFactory )
{
}

WSSession::WSSession( ) :
    BaseSession( ),
    m_servicesUrls( ),
    m_navigationService( NULL ),
    m_objectService( NULL ),
    m_repositoryService( NULL ),
    m_versioningService( NULL ),
    m_responseFactory( )
{
    setNoHttpErrors( true );
}


WSSession& WSSession::operator=( const WSSession& copy )
{
    if ( this != &copy )
    {
        BaseSession::operator=( copy );
        m_servicesUrls = copy.m_servicesUrls;
        m_responseFactory = copy.m_responseFactory;
        delete m_navigationService;
        m_navigationService = NULL;
        delete m_objectService;
        m_objectService = NULL;
        delete m_repositoryService;
        m_repositoryService = NULL;
        delete m_versioningService;
        m_versioningService = NULL;
    }

    return *this;
}

WSSession::~WSSession( )
{
    delete m_navigationService;
    delete m_objectService;
    delete m_repositoryService;
    delete m_versioningService;
}

string WSSession::getWsdl( string url, libcmis::HttpResponsePtr response )
{
    string buf;
    if ( response )
        buf = response->getStream( )->str( );
    else
        buf = httpGetRequest( url )->getStream( )->str( );

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
            xmlXPathFreeObject( xpathObj );
        }
        xmlXPathFreeContext( xpathCtx );
    }
    xmlFreeDoc( doc );

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

vector< SoapResponsePtr > WSSession::soapRequest( string& url, SoapRequest& request )
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
            if ( string::npos != responseType.find( "multipart/related" ) )
            {
                RelatedMultipart answer( response->getStream( )->str( ), responseType );

                responses = getResponseFactory( ).parseResponse( answer );
            }
            else if ( string::npos != responseType.find( "text/xml" ) )
            {
                // Parse the envelope
                string xml = response->getStream( )->str( );
                responses = getResponseFactory( ).parseResponse( xml );
            }
        }
    }
    catch ( const SoapFault& fault )
    {
        boost::shared_ptr< libcmis::Exception > cmisException = getCmisException( fault );
        if ( cmisException )
        {
            throw *cmisException;
        }
        throw libcmis::Exception( fault.what( ), "runtime" );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    return responses;
}

void WSSession::parseWsdl( string buf )
{
    // parse the content
    const boost::shared_ptr< xmlDoc > doc( xmlReadMemory( buf.c_str(), buf.size(), m_bindingUrl.c_str(), NULL, 0 ), xmlFreeDoc );

    if ( bool( doc ) )
    {
        // Check that we have a WSDL document
        xmlNodePtr root = xmlDocGetRootElement( doc.get() );
        if ( !xmlStrEqual( root->name, BAD_CAST( "definitions" ) ) )
            throw libcmis::Exception( "Not a WSDL document" );

        // Get all the services soap URLs
        m_servicesUrls.clear( );

        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc.get() );
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
            xmlXPathFreeObject( xpathObj );
        }
        xmlXPathFreeContext( xpathCtx );
    }
    else
        throw libcmis::Exception( "Failed to parse service document" );
}

void WSSession::initializeResponseFactory( )
{
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
}

void WSSession::initializeRepositories( map< string, string > repositories )
{
    for ( map< string, string >::iterator it = repositories.begin( );
          it != repositories.end( ); ++it )
    {
        string repoId = it->first;
        m_repositories.push_back( getRepositoryService( ).getRepositoryInfo( repoId ) );
    }
}

void WSSession::initialize( libcmis::HttpResponsePtr response )
{
    if ( m_repositories.empty() )
    {
        // Get the wsdl file
        string buf;
        try
        {
            buf = getWsdl( m_bindingUrl, response );
        }
        catch ( const CurlException& e )
        {
            throw e.getCmisException( );
        }

        parseWsdl( buf );
        initializeResponseFactory( );
        map< string, string > repositories = getRepositoryService( ).getRepositories( );
        initializeRepositories( repositories );
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
    mapping[ "{" + string( NS_CMISM_URL ) + "}getObjectParentsResponse" ] = &GetObjectParentsResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}getChildrenResponse" ] = &GetChildrenResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}createFolderResponse" ] = &CreateFolderResponse::create;
    // Use the same response object than folders as it contains the same elements
    mapping[ "{" + string( NS_CMISM_URL ) + "}createDocumentResponse" ] = &CreateFolderResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}checkOutResponse" ] = &CheckOutResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}checkInResponse" ] = &CheckInResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}getAllVersionsResponse" ] = &GetAllVersionsResponse::create;
    mapping[ "{" + string( NS_CMISM_URL ) + "}getRenditionsResponse" ] = &GetRenditionsResponse::create;

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

RepositoryService& WSSession::getRepositoryService( )
{
    if ( m_repositoryService == NULL )
        m_repositoryService = new RepositoryService( this );
    return *m_repositoryService;
}

ObjectService& WSSession::getObjectService( )
{
    if ( m_objectService == NULL )
        m_objectService = new ObjectService( this );
    return *m_objectService;
}

NavigationService& WSSession::getNavigationService( )
{
    if ( m_navigationService == NULL )
        m_navigationService = new NavigationService( this );
    return *m_navigationService;
}

VersioningService& WSSession::getVersioningService( )
{
    if ( m_versioningService == NULL )
        m_versioningService = new VersioningService( this );
    return *m_versioningService;
}

libcmis::RepositoryPtr WSSession::getRepository( )
{
    // Check if we already have the repository
    libcmis::RepositoryPtr repo;
    vector< libcmis::RepositoryPtr >::iterator it = m_repositories.begin();
    while ( !repo && it != m_repositories.end() )
    {
        if ( ( *it )->getId() == m_repositoryId )
            repo = *it;
        ++it;
    }

    // We found nothing cached, so try to get it from the server
    if ( !repo )
    {
        repo = getRepositoryService( ).getRepositoryInfo( m_repositoryId );
        if ( repo )
            m_repositories.push_back( repo );
    }

    return repo;
}

bool WSSession::setRepository( string repositoryId )
{
    bool success = false;
    try
    {
        libcmis::RepositoryPtr repo = getRepositoryService( ).getRepositoryInfo( repositoryId );
        if (repo && repo->getId( ) == repositoryId )
            m_repositoryId = repositoryId;
        success = true;
    }
    catch ( const libcmis::Exception& )
    {
    }
    return success;
}

libcmis::ObjectPtr WSSession::getObject( string id )
{
    return getObjectService( ).getObject( getRepositoryId( ), id );
}

libcmis::ObjectPtr WSSession::getObjectByPath( string path )
{
    return getObjectService( ).getObjectByPath( getRepositoryId( ), path );
}

libcmis::ObjectTypePtr WSSession::getType( string id )
{
    return getRepositoryService( ).getTypeDefinition( m_repositoryId, id );
}

vector< libcmis::ObjectTypePtr > WSSession::getBaseTypes( )
{
    return getRepositoryService().getTypeChildren( m_repositoryId, "" );
}
