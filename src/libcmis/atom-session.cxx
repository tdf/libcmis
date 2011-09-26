#include <string>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "atom-folder.hxx"
#include "atom-session.hxx"
#include "atom-utils.hxx"

using namespace std;

namespace
{
    size_t lcl_getXmlWorkspaces( void* pBuffer, size_t size, size_t nmemb, void* pUserData )
    {
        size_t readSize = size;

        list< string >& ids = *( static_cast< list< string >* >( pUserData ) );
        xmlDocPtr pDoc = xmlReadMemory( ( const char * )pBuffer, size * nmemb, "atompub.xml", NULL, 0 );

        if ( NULL != pDoc )
        {
            xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( pDoc );

            // Register the Service Document namespaces
            atom::registerNamespaces( pXPathCtx );

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
    
string UriTemplate::createUrl( const string& pattern, map< string, string > variables )
{
    string url( pattern );

    // Decompose the pattern and replace the variables by their values
    map< string, string >::iterator it = variables.begin( );
    while ( it != variables.end( ) )
    {
        string name = "{";
        name += it->first;
        name += "}";
        string value = it->second;

        // Search and replace the variable
        size_t pos = url.find( name );
        if ( pos != string::npos )
            url.replace( pos, name.size(), value );

        ++it;
    }

    // Cleanup the remaining unset variables
    size_t pos1 = url.find( '{' );
    while ( pos1 != string::npos )
    {
        // look for the closing bracket
        size_t pos2 = url.find( '}', pos1 );
        if ( pos2 != string::npos )
            url.erase( pos1, pos2 - pos1 + 1 );

        pos1 = url.find( '{', pos1 - 1 );
    }
    
    return url;
}

AtomPubSession::AtomPubSession( string atomPubUrl, string repository ) :
    Session( ),
    m_sAtomPubUrl( atomPubUrl ),
    m_sRepository( repository )
{
    // Pull the content from sAtomPubUrl and parse it
    atom::http_request( m_sAtomPubUrl, &AtomPubSession::parseServiceDocument, this );
}

AtomPubSession::~AtomPubSession( )
{
}

list< string > AtomPubSession::getRepositories( string url )
{
    list< string > repos;

    // Parse the service document and get the workspaces
    atom::http_request( url, lcl_getXmlWorkspaces, &repos );

    return repos;
}

string AtomPubSession::getCollectionUrl( Collection::Type type )
{
    return m_aCollections[ type ];
}

string AtomPubSession::getUriTemplate( UriTemplate::Type type )
{
    return m_aUriTemplates[ type ];
}

Folder* AtomPubSession::getRootFolder()
{
    return getFolder( m_sRootId );
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
                bool isCmisra =  xmlStrEqual( pChild->ns->href, NS_CMISRA_URL );
                bool isCollectionType = xmlStrEqual( pChild->name, BAD_CAST( "collectionType" ) );
                if ( isCmisra && isCollectionType )
                {
                    xmlChar* pContent = xmlNodeGetContent( pChild );
                    Collection::Type type = Collection::Root;
                    bool typeDefined = false;

                    if ( xmlStrEqual( pContent, BAD_CAST( "root" ) ) )
                    {
                        type = Collection::Root;
                        typeDefined = true;
                    }
                    else if ( xmlStrEqual( pContent, BAD_CAST( "types" ) ) )
                    {
                        type = Collection::Types;
                        typeDefined = true;
                    }
                    else if ( xmlStrEqual( pContent, BAD_CAST( "query" ) ) )
                    {
                        type = Collection::Query;
                        typeDefined = true;
                    }
                    else if ( xmlStrEqual( pContent, BAD_CAST( "checkedout" ) ) )
                    {
                        type = Collection::Checkedout;
                        typeDefined = true;
                    }
                    else if ( xmlStrEqual( pContent, BAD_CAST( "unfiled" ) ) )
                    {
                        type = Collection::Unfiled;
                        typeDefined = true;
                    }

                    if ( typeDefined )
                        m_aCollections[ type ] = collectionRef;

                    xmlFree( pContent );
                }
            }
        }
    }
}

void AtomPubSession::readUriTemplates( xmlNodeSetPtr pNodeSet )
{
    int size = 0;
    if ( pNodeSet )
        size = pNodeSet->nodeNr;

    for ( int i = 0; i < size; i++ )
    {
        xmlNodePtr pNode = pNodeSet->nodeTab[i];

        string templateUri;
        UriTemplate::Type type = UriTemplate::ObjectById;
        bool typeDefined = false;

        // Look for the cmisra:template and cmisra:type children
        for ( xmlNodePtr pChild = pNode->children; pChild; pChild = pChild->next )
        {
            bool bIsCmisra =  xmlStrEqual( pChild->ns->href, NS_CMISRA_URL );
            bool bIsTemplate = xmlStrEqual( pChild->name, BAD_CAST( "template" ) );
            bool bIsType = xmlStrEqual( pChild->name, BAD_CAST( "type" ) );

            if ( bIsCmisra && bIsTemplate )
            {
                xmlChar* pContent = xmlNodeGetContent( pChild );
                templateUri = string( ( char * )pContent );
                xmlFree( pContent );
            }
            else if ( bIsCmisra && bIsType )
            {
                xmlChar* pContent = xmlNodeGetContent( pChild );
                if ( xmlStrEqual( pContent, BAD_CAST( "objectbyid" ) ) )
                {
                    type = UriTemplate::ObjectById;
                    typeDefined = true;
                }
                else if ( xmlStrEqual( pContent, BAD_CAST( "ObjectByPath" ) ) )
                {
                    type = UriTemplate::ObjectByPath;
                    typeDefined = true;
                }
                else if ( xmlStrEqual( pContent, BAD_CAST( "query" ) ) )
                {
                    type = UriTemplate::Query;
                    typeDefined = true;
                }
                else if ( xmlStrEqual( pContent, BAD_CAST( "TypeById" ) ) )
                {
                    type = UriTemplate::TypeById;
                    typeDefined = true;
                }
                xmlFree( pContent );
            }
        }

        if ( !templateUri.empty() && typeDefined )
            m_aUriTemplates[ type ] = templateUri;
    }
}

Folder* AtomPubSession::getFolder( string id )
{
    string pattern = getUriTemplate( UriTemplate::ObjectById );
    map< string, string > vars;
    vars[URI_TEMPLATE_VAR_ID] = id;
    AtomFolder* folder = new AtomFolder( UriTemplate::createUrl( pattern, vars ) );
    return folder;
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
        atom::registerNamespaces( pXPathCtx );

        if ( NULL != pXPathCtx )
        {
            // Get the collections
            xmlXPathObjectPtr pXPathObj = xmlXPathEvalExpression( BAD_CAST( "//app:collection" ), pXPathCtx );
            if ( NULL != pXPathObj )
                session.readCollections( pXPathObj->nodesetval );
            xmlXPathFreeObject( pXPathObj );

            // Get the URI templates
            pXPathObj = xmlXPathEvalExpression( BAD_CAST( "//cmisra:uritemplate" ), pXPathCtx );
            if ( NULL != pXPathObj )
                session.readUriTemplates( pXPathObj->nodesetval );
            xmlXPathFreeObject( pXPathObj );
            
            // Get the root node id
            string infosXPath( "//cmisra:repositoryInfo[cmis:repositoryId='" );
            infosXPath += session.m_sRepository;
            infosXPath += "']/cmis:rootFolderId/text()";
            session.m_sRootId = atom::getXPathValue( pXPathCtx, infosXPath );
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
