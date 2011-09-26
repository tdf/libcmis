#include "atom-resource.hxx"
#include "atom-utils.hxx"

using namespace std;

/** Constructor for the resource, the url provided url should point to the resource
    CMIS properties. The content of the URL isn't extracted and parsed by the constructor:
    this task is left to the class children.
  */
AtomResource::AtomResource( string url ) :
    m_infosUrl( url ),
    m_name( ),
    m_path( )
{
}

AtomResource::~AtomResource( )
{
}

string AtomResource::getName( )
{
    return m_name;
}

string AtomResource::getPath( )
{
    return m_path;
}

void AtomResource::extractInfos( xmlDocPtr doc )
{
    xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( doc );

    // Register the Service Document namespaces
    atom::registerNamespaces( pXPathCtx );

    if ( NULL != pXPathCtx )
    {
        // Get the name
        string nameReq( "//cmis:propertyString[@queryName='cmis:name']/cmis:value/text()" );
        m_name = atom::getXPathValue( pXPathCtx, nameReq );

        // Get the path
        string pathReq( "//cmis:propertyString[@queryName='cmis:path']/cmis:value/text()" );
        m_path = atom::getXPathValue( pXPathCtx, pathReq );
    }
    xmlXPathFreeContext( pXPathCtx );
}
