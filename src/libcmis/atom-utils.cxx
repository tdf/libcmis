#include <sstream>

#include <curl/curl.h>

#include "atom-utils.hxx"

using namespace std;

namespace
{
    size_t lcl_bufferData( void* buffer, size_t size, size_t nmemb, void* data )
    {
        stringstream& out = *( static_cast< stringstream* >( data ) );
        out.write( ( const char* ) buffer, size * nmemb );
        return nmemb;
    }
}

namespace atom
{
    void registerNamespaces( xmlXPathContextPtr pXPathCtx )
    {
        xmlXPathRegisterNs( pXPathCtx, BAD_CAST( "app" ),  NS_APP_URL );
        xmlXPathRegisterNs( pXPathCtx, BAD_CAST( "atom" ),  NS_ATOM_URL );
        xmlXPathRegisterNs( pXPathCtx, BAD_CAST( "cmis" ),  NS_CMIS_URL );
        xmlXPathRegisterNs( pXPathCtx, BAD_CAST( "cmisra" ),  NS_CMISRA_URL );
    }
    
    string getXPathValue( xmlXPathContextPtr pXPathCtx, string req )
    {
        string value;
        xmlXPathObjectPtr pXPathObj = xmlXPathEvalExpression( BAD_CAST( req.c_str() ), pXPathCtx );
        if ( pXPathObj && pXPathObj->nodesetval && pXPathObj->nodesetval->nodeNr > 0 )
        {
            xmlChar* pContent = xmlNodeGetContent( pXPathObj->nodesetval->nodeTab[0] );
            value = string( ( char* )pContent );
            xmlFree( pContent );
        }
        xmlXPathFreeObject( pXPathObj );

        return value;
    }

    xmlDocPtr wrapInDoc( xmlNodePtr entryNd )
    {
        xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
        xmlNodePtr entryCopy = xmlCopyNode( entryNd, 1 );

        xmlDocSetRootElement( doc, entryCopy );
        return doc;
    }

    string httpGetRequest( string url )
    {
        stringstream stream;

        curl_global_init( CURL_GLOBAL_ALL );
        CURL* pHandle = curl_easy_init( );

        // Grab something from the web
        curl_easy_setopt( pHandle, CURLOPT_URL, url.c_str() );
        curl_easy_setopt( pHandle, CURLOPT_WRITEFUNCTION, lcl_bufferData );
        curl_easy_setopt( pHandle, CURLOPT_WRITEDATA, &stream );

        // Perform the query
        curl_easy_perform( pHandle );

        curl_easy_cleanup( pHandle );

        return stream.str();
    }
}
