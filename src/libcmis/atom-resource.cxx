#include "atom-resource.hxx"
#include "atom-utils.hxx"

using namespace std;

/** Constructor for the resource, the url provided url should point to the resource
    CMIS properties. The content of the URL isn't extracted and parsed by the constructor:
    this task is left to the class children.
  */
AtomResource::AtomResource( string url ) :
    m_infosUrl( url ),
    m_name( )
{
}

AtomResource::~AtomResource( )
{
}

string AtomResource::getName( )
{
    return m_name;
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
    }
    xmlXPathFreeContext( pXPathCtx );
}
