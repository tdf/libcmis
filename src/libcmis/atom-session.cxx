#include <string>

#include <curl/curl.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

#include "atom-session.hxx"

#define NS_APP_URL BAD_CAST( "http://www.w3.org/2007/app" )
#define NS_ATOM_URL BAD_CAST( "http://www.w3.org/2005/Atom" )
#define NS_CMIS_URL BAD_CAST( "http://docs.oasis-open.org/ns/cmis/core/200908/" )
#define NS_CMISRA_URL BAD_CAST( "http://docs.oasis-open.org/ns/cmis/restatom/200908/" )

using namespace std;

namespace
{
    void lcl_RegisterNamespaces( xmlXPathContextPtr pXPathCtx )
    {
        xmlXPathRegisterNs( pXPathCtx, BAD_CAST( "app" ),  NS_APP_URL );
        xmlXPathRegisterNs( pXPathCtx, BAD_CAST( "atom" ),  NS_ATOM_URL );
        xmlXPathRegisterNs( pXPathCtx, BAD_CAST( "cmis" ),  NS_CMIS_URL );
        xmlXPathRegisterNs( pXPathCtx, BAD_CAST( "cmisra" ),  NS_CMISRA_URL );
    }

    size_t lcl_getXmlWorkspaces( void* pBuffer, size_t size, size_t nmemb, void* pUserData )
    {
        size_t readSize = size;

        list< string >& ids = *( static_cast< list< string >* >( pUserData ) );
        xmlDocPtr pDoc = xmlReadMemory( ( const char * )pBuffer, size * nmemb, "atompub.xml", NULL, 0 );

        if ( NULL != pDoc )
        {
            xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( pDoc );

            // Register the Service Document namespaces
            lcl_RegisterNamespaces( pXPathCtx );

            if ( NULL != pXPathCtx )
            {
                xmlXPathObjectPtr pXPathObj = xmlXPathEvalExpression( BAD_CAST( "//cmis:repositoryId/text()" ), pXPathCtx );
                if ( NULL != pXPathObj )
                {
                    int size = 0;
                    if ( pXPathObj->nodesetval )
                        size = pXPathObj->nodesetval->nodeNr;
                    
                    for ( int i = 0; i < size; i++ )
                    {
                        xmlNodePtr pNode = pXPathObj->nodesetval->nodeTab[i];
                        string workspaceId( ( char* )pNode->content );
                        ids.push_back( workspaceId );
                    }
                }

                xmlXPathFreeObject( pXPathObj );
            }
            xmlXPathFreeContext( pXPathCtx );
            
        }
        else
        {
            fprintf( stderr, "Failed to parse service document\n" );
            readSize = 0;
        }

        return readSize;
    }
}

AtomPubSession::AtomPubSession( string atomPubUrl, string repository ) :
    Session( ),
    m_sAtomPubUrl( atomPubUrl ),
    m_sRepository( repository )
{
    // Pull the content from sAtomPubUrl and parse it
    http_request( m_sAtomPubUrl, &AtomPubSession::parseServiceDocument, this );
}

AtomPubSession::~AtomPubSession( )
{
}

list< string > AtomPubSession::getRepositories( string url )
{
    list< string > repos;

    // TODO Parse the document service and get the workspaces
    http_request( url, lcl_getXmlWorkspaces, &repos );

    return repos;
}

string AtomPubSession::getCollectionUrl( CollectionType type )
{
    return m_aCollections[ type ];
}

Folder AtomPubSession::getRootFolder()
{
    return getFolder( getCollectionUrl( Root ) );
}

void AtomPubSession::readCollections( xmlNodeSetPtr pNodeSet )
{
    int size = 0;
    if ( pNodeSet )
        size = pNodeSet->nodeNr;

    for ( int i = 0; i < size; i++ )
    {
        xmlNodePtr pNode = pNodeSet->nodeTab[i];

        // Look for the href property
        xmlChar* pHref = xmlGetProp( pNode, BAD_CAST( "href" ) );
        if ( pHref )
        {
            string collectionRef( ( char* )pHref );
            xmlFree( pHref );

            // Look for the cmisra:collectionType child
            for ( xmlNodePtr pChild = pNode->children; pChild; pChild = pChild->next )
            {
                bool bIsCmisra =  xmlStrEqual( pChild->ns->href, NS_CMISRA_URL );
                bool bIsCollectionType = xmlStrEqual( pChild->name, BAD_CAST( "collectionType" ) );
                if ( bIsCmisra && bIsCollectionType )
                {
                    xmlChar* pContent = xmlNodeGetContent( pChild );
                    CollectionType type = Unknown;
                    if ( xmlStrEqual( pContent, BAD_CAST( "root" ) ) )
                        type = Root;
                    else if ( xmlStrEqual( pContent, BAD_CAST( "types" ) ) )
                        type = Types;
                    else if ( xmlStrEqual( pContent, BAD_CAST( "query" ) ) )
                        type = Query;
                    else if ( xmlStrEqual( pContent, BAD_CAST( "checkedout" ) ) )
                        type = Checkedout;
                    else if ( xmlStrEqual( pContent, BAD_CAST( "unfiled" ) ) )
                        type = Unfiled;

                    if ( type != Unknown )
                        m_aCollections[ type ] = collectionRef;

                    xmlFree( pContent );
                }
            }
        }
    }
}

void AtomPubSession::http_request( string Url, size_t (*pCallback)( void *, size_t, size_t, void* ), void* pData )
{
    curl_global_init( CURL_GLOBAL_ALL );
    CURL* pHandle = curl_easy_init( );

    // Grab something from the web
    curl_easy_setopt( pHandle, CURLOPT_URL, Url.c_str() );
    curl_easy_setopt( pHandle, CURLOPT_WRITEFUNCTION, pCallback );
    curl_easy_setopt( pHandle, CURLOPT_WRITEDATA, pData );

    // Perform the query
    curl_easy_perform( pHandle );

    curl_easy_cleanup( pHandle );
}

Folder AtomPubSession::getFolder( string urlGet )
{
    Folder aFolder;
    http_request( urlGet, &AtomPubSession::parseFolder, &aFolder );
    // TODO Handle errors

    return aFolder;
}

size_t AtomPubSession::parseServiceDocument( void* pBuffer, size_t size, size_t nmemb, void* pUserData )
{
    AtomPubSession& session = *( static_cast< AtomPubSession* >( pUserData ) );

    size_t readSize = size;
    xmlDocPtr pDoc = xmlReadMemory( ( const char * )pBuffer, size * nmemb, session.m_sAtomPubUrl.c_str(), NULL, 0 );

    if ( NULL != pDoc )
    {
        xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( pDoc );

        // Register the Service Document namespaces
        lcl_RegisterNamespaces( pXPathCtx );

        if ( NULL != pXPathCtx )
        {
            xmlXPathObjectPtr pXPathObj = xmlXPathEvalExpression( BAD_CAST( "//app:collection" ), pXPathCtx );
            if ( NULL != pXPathObj )
                session.readCollections( pXPathObj->nodesetval );

            xmlXPathFreeObject( pXPathObj );
        }
        xmlXPathFreeContext( pXPathCtx );
        
    }
    else
    {
        fprintf( stderr, "Failed to parse service document\n" );
        readSize = 0;
    }

    xmlFreeDoc( pDoc );
    return readSize;
}

size_t AtomPubSession::parseFolder( void* pBuffer, size_t size, size_t nmemb, void* pUserData )
{
    size_t readSize = size;

    // TODO Implement me

    return readSize;
}
