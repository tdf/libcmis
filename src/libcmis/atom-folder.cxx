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
    string buf  = atom::httpGetRequest( getInfosUrl() );
    
    xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), getInfosUrl().c_str(), NULL, 0 );
    if ( NULL != doc )
        extractInfos( doc );
    else
    {
        fprintf( stderr, "Failed to parse folder infos\n" );
    }
    xmlFreeDoc( doc );
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
    
    string buf = atom::httpGetRequest( m_childrenUrl );

    xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), m_childrenUrl.c_str(), NULL, 0 );
    if ( NULL != doc )
    {
        xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( doc );
        atom::registerNamespaces( pXPathCtx );
        if ( NULL != pXPathCtx )
        {
            const string& entriesReq( "//atom:entry" );
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

                    if ( resource.get() )
                        children.push_back( resource );
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
    }
    xmlFreeDoc( doc );

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
