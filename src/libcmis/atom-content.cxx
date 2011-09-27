#include "atom-content.hxx"
#include "atom-utils.hxx"

using namespace std;

AtomContent::AtomContent( string url ) :
    AtomResource( url ),
    m_contentUrl( ),
    m_contentType( )
{
    string buf  = atom::httpGetRequest( getInfosUrl() );
    
    xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), getInfosUrl().c_str(), NULL, 0 );
    if ( NULL != doc )
        extractInfos( doc );
    else
    {
        fprintf( stderr, "Failed to parse content infos\n" );
    }
    xmlFreeDoc( doc );
}

AtomContent::AtomContent( xmlNodePtr entryNd ) :
    AtomResource( string() ),
    m_contentUrl( ),
    m_contentType( )
{
    xmlDocPtr doc = atom::wrapInDoc( entryNd );
    extractInfos( doc );
    xmlFreeDoc( doc );
}

AtomContent::~AtomContent( )
{
}

void AtomContent::getContent( size_t (*pCallback)( void *, size_t, size_t, void* ) )
{
}

void AtomContent::extractInfos( xmlDocPtr doc )
{
    AtomResource::extractInfos( doc );
   
   // Get the content url and type 
    xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( doc );
    if ( NULL != doc )
    {
        atom::registerNamespaces( pXPathCtx );

        if ( NULL != pXPathCtx )
        {
            // Get the children collection url
            xmlXPathObjectPtr pXPathObj = xmlXPathEvalExpression( BAD_CAST( "//atom:content" ), pXPathCtx );
            if ( pXPathObj && pXPathObj->nodesetval && pXPathObj->nodesetval->nodeNr > 0 )
            {
                xmlNodePtr contentNd = pXPathObj->nodesetval->nodeTab[0];
                xmlChar* src = xmlGetProp( contentNd, BAD_CAST( "src" ) );
                m_contentUrl = string( ( char* ) src );
                xmlFree( src );
                
                xmlChar* type = xmlGetProp( contentNd, BAD_CAST( "type" ) );
                m_contentType = string( ( char* ) type );
                xmlFree( type );
            }
            xmlXPathFreeObject( pXPathObj );
        }
        xmlXPathFreeContext( pXPathCtx );
    }
}
