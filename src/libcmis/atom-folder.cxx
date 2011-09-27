#include "atom-folder.hxx"
#include "atom-utils.hxx"

using namespace std;

namespace
{
}

AtomFolder::AtomFolder( string url ) :
    AtomResource( url ),
    m_childrenUrl( )
{
    atom::http_request( getInfosUrl(), &AtomFolder::parseFolderInfos, this );
}

AtomFolder::AtomFolder( xmlNodePtr entryNd ) :
    AtomResource( string() ),
    m_childrenUrl( )
{
    xmlDocPtr doc = atom::wrapInDoc( entryNd );
    extractInfos( doc );
    xmlFreeDoc( doc );
}


AtomFolder::~AtomFolder( )
{
}

vector< ResourcePtr > AtomFolder::getChildren( )
{
    vector< ResourcePtr > children;
    atom::http_request( m_childrenUrl, &AtomFolder::parseXmlChildren, &children );
    return children;
}

string AtomFolder::getName( )
{
    return AtomResource::getName( );
}

string AtomFolder::getPath( )
{
    return AtomResource::getPath( );
}

void AtomFolder::extractInfos( xmlDocPtr doc )
{
    AtomResource::extractInfos( doc );
    m_childrenUrl = AtomFolder::getChildrenUrl( doc );
}

size_t AtomFolder::parseFolderInfos( void* pBuffer, size_t size, size_t nmemb, void* pUserData )
{
    AtomFolder& folder = *( static_cast< AtomFolder* >( pUserData ) );

    size_t readSize = size;
    xmlDocPtr pDoc = xmlReadMemory( ( const char * )pBuffer, size * nmemb, folder.getInfosUrl().c_str(), NULL, 0 );
    if ( NULL != pDoc )
        folder.extractInfos( pDoc );
    else
    {
        fprintf( stderr, "Failed to parse folder infos\n" );
        readSize = 0;
    }
    xmlFreeDoc( pDoc );

    return readSize;
}

string AtomFolder::getChildrenUrl( xmlDocPtr doc )
{
    string childrenUrl;

    xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( doc );
    atom::registerNamespaces( pXPathCtx );

    if ( NULL != pXPathCtx )
    {
        // Get the children collection url
        string downReq( "//atom:link[@rel='down' and @type='application/atom+xml;type=feed']//attribute::href" );
        childrenUrl = atom::getXPathValue( pXPathCtx, downReq );
    }
    xmlXPathFreeContext( pXPathCtx );

    return childrenUrl;
}

size_t AtomFolder::parseXmlChildren( void* pBuffer, size_t size, size_t nmemb, void* pUserData )
{
    vector< ResourcePtr > children = *( static_cast< vector< ResourcePtr >* >( pUserData ) );
    size_t readSize = size;
    
    xmlDocPtr doc = xmlReadMemory( ( const char * )pBuffer, size * nmemb, "children.xml", NULL, 0 );
    if ( NULL != doc )
    {
        xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( doc );
        if ( NULL != pXPathCtx )
        {
            string entriesReq( "//atom:entry" );
            xmlXPathObjectPtr pXPathObj = xmlXPathEvalExpression( BAD_CAST( entriesReq.c_str() ), pXPathCtx );

            if ( NULL != pXPathObj && NULL != pXPathObj->nodesetval )
            {
                int size = pXPathObj->nodesetval->nodeNr;
                for ( int i = 0; i < size; i++ )
                {
                    xmlNodePtr node = pXPathObj->nodesetval->nodeTab[i];

                    ResourcePtr resource;

                    xmlDocPtr entryDoc = atom::wrapInDoc( node );
                    if ( !AtomFolder::getChildrenUrl( entryDoc ).empty() )
                    {
                        ResourcePtr folder( new AtomFolder( node ) );
                        resource.swap( folder );
                    }
                    else
                    {
#if 0
                        ResourcePtr content( new AtomContent( node ) );
                        resource.swap( content );
#endif
                    }
                    xmlFreeDoc( entryDoc );
                }
            }

            xmlXPathFreeObject( pXPathObj );
        }

        xmlXPathFreeContext( pXPathCtx );
    }
    else
    {
        fprintf( stderr, "Failed to parse folder infos\n" );
        readSize = 0;
    }
    xmlFreeDoc( doc );

    return readSize;
}
