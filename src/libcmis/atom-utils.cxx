#include <curl/curl.h>

#include "atom-utils.hxx"

using namespace std;

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
        }
        else
            fprintf( stderr, "Bad request: %s\n", req.c_str() );
        xmlXPathFreeObject( pXPathObj );

        return value;
    }

    xmlDocPtr wrapInDoc( xmlNodePtr entryNd )
    {
        xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
        xmlNodePtr entryCopy = xmlCopyNode( entryNd, 1 );

        if ( !xmlHasProp( entryNd, BAD_CAST( "xmlns:cmis" ) ) )
        {
            // Add the namespaces
            xmlNewProp( entryCopy, BAD_CAST( "xmlns:atom" ),
                        NS_ATOM_URL );
            xmlNewProp( entryCopy, BAD_CAST( "xmlns:app" ),
                        NS_APP_URL );
            xmlNewProp( entryCopy, BAD_CAST( "xmlns:cmis" ),
                        NS_CMIS_URL );
            xmlNewProp( entryCopy, BAD_CAST( "xmlns:cmisra" ),
                        NS_CMISRA_URL );
        }
        xmlDocSetRootElement( doc, entryCopy );
        return doc;
    }

    void http_request( string Url, size_t (*pCallback)( void *, size_t, size_t, void* ), void* pData )
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
}
