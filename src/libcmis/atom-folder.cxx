#include "atom-folder.hxx"
#include "atom-utils.hxx"

using namespace std;

namespace
{
}

AtomFolder::AtomFolder( string url ) :
    AtomResource( url )
{
    fprintf( stderr, "URL: %s\n", getInfosUrl().c_str() );
    // Get the infos
    atom::http_request( getInfosUrl(), &AtomFolder::parseFolderInfos, this );
}

AtomFolder::~AtomFolder( )
{
}

vector< Resource > AtomFolder::getChildren( )
{
    // TODO Implement me
}

string AtomFolder::getName( )
{
    return AtomResource::getName( );
}

string AtomFolder::getPath( )
{
    return AtomResource::getPath( );
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

    return readSize;
}

void AtomFolder::extractInfos( xmlDocPtr doc )
{
    AtomResource::extractInfos( doc );

    xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( doc );

    // Register the Service Document namespaces
    atom::registerNamespaces( pXPathCtx );

    if ( NULL != pXPathCtx )
    {
        // Get the children collection url
        string downReq( "//atom:link[@rel='down' and @type='application/atom+xml;type=feed']" );
        m_childrenUrl = atom::getXPathValue( pXPathCtx, downReq );
    }
    xmlXPathFreeContext( pXPathCtx );
}
