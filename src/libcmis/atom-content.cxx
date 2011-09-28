/* libcmis
 * Version: MPL 1.1 / GPLv2+ / LGPLv2+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * Copyright (C) 2011 SUSE <cbosdonnat@suse.com>
 *
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPLv2+"), or
 * the GNU Lesser General Public License Version 2 or later (the "LGPLv2+"),
 * in which case the provisions of the GPLv2+ or the LGPLv2+ are applicable
 * instead of those above.
 */
#include <stdlib.h>

#include <curl/curl.h>

#include "atom-content.hxx"
#include "atom-session.hxx"
#include "atom-utils.hxx"

using namespace std;

AtomContent::AtomContent( AtomPubSession* session, string url ) :
    AtomCmisObject( session, url ),
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

AtomContent::AtomContent( AtomPubSession* session, xmlNodePtr entryNd ) :
    AtomCmisObject( session, string() ),
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

void AtomContent::getContent( size_t (*pCallback)( void *, size_t, size_t, void* ), void* userData )
{
    curl_global_init( CURL_GLOBAL_ALL );
    CURL* pHandle = curl_easy_init( );

    // Grab something from the web
    curl_easy_setopt( pHandle, CURLOPT_URL, m_contentUrl.c_str() );
    curl_easy_setopt( pHandle, CURLOPT_WRITEFUNCTION, pCallback );
    curl_easy_setopt( pHandle, CURLOPT_WRITEDATA, userData );

    // Perform the query
    curl_easy_perform( pHandle );

    curl_easy_cleanup( pHandle );
}

void AtomContent::extractInfos( xmlDocPtr doc )
{
    AtomCmisObject::extractInfos( doc );
   
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

                // Get the content length
                string lengthReq( "//cmis:propertyInteger[@propertyDefinitionId='cmis:contentStreamLength']/cmis:value/text()" );
                string bytes = atom::getXPathValue( pXPathCtx, lengthReq );
                m_contentLength = atol( bytes.c_str() );

            }
            xmlXPathFreeObject( pXPathObj );
        }
        xmlXPathFreeContext( pXPathCtx );
    }
}
