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
#include <algorithm>
#include <stdlib.h>
#include <sstream>

#include <curl/curl.h>

#include "atom-document.hxx"
#include "atom-session.hxx"
#include "atom-utils.hxx"

using namespace std;

namespace
{
    size_t lcl_getEncoding( void *ptr, size_t size, size_t nmemb, void *userdata )
    {
        atom::EncodedData* data = static_cast< atom::EncodedData* >( userdata );

        string buf( ( const char* ) ptr, size * nmemb );

        size_t sepPos = buf.find( ':' );
        if ( sepPos != string::npos )
        {
            string name( buf, 0, sepPos );
            if ( "Content-Transfer-Encoding" == name )
            {
                string encoding = buf.substr( sepPos + 1 );
                encoding.erase( remove_if( encoding.begin(), encoding.end(), ptr_fun< int, int> ( isspace ) ), encoding.end() );

                data->setEncoding( encoding );
            }
        }
        
        return nmemb;
    }

    size_t lcl_getData( void* ptr, size_t size, size_t nmemb, void* data )
    {
        atom::EncodedData* encoded = static_cast< atom::EncodedData* >( data );
        encoded->decode( ptr, size, nmemb );
        return nmemb;
    }
}


AtomDocument::AtomDocument( AtomPubSession* session, string url ) :
    AtomObject( session, url ),
    m_contentUrl( ),
    m_contentType( ),
    m_contentFilename( ),
    m_contentLength( 0 )
{
    refresh( );
}

AtomDocument::AtomDocument( AtomPubSession* session, xmlNodePtr entryNd ) :
    AtomObject( session, string() ),
    m_contentUrl( ),
    m_contentType( ),
    m_contentFilename( ),
    m_contentLength( 0 )
{
    xmlDocPtr doc = atom::wrapInDoc( entryNd );
    refreshImpl( doc );
    xmlFreeDoc( doc );
}

AtomDocument::~AtomDocument( )
{
}

FILE* AtomDocument::getContent( const char* path )
{
    curl_global_init( CURL_GLOBAL_ALL );
    CURL* handle = curl_easy_init( );

    FILE* res = NULL;
    if ( NULL == path )
        res = tmpfile();
    else
        res = fopen( path, "w+b" );

    atom::EncodedData* data = new atom::EncodedData( res );

    curl_easy_setopt( handle, CURLOPT_URL, m_contentUrl.c_str() );
    curl_easy_setopt( handle, CURLOPT_WRITEFUNCTION, &lcl_getData );
    curl_easy_setopt( handle, CURLOPT_WRITEDATA, data );

    curl_easy_setopt( handle, CURLOPT_HEADERFUNCTION, lcl_getEncoding );
    curl_easy_setopt( handle, CURLOPT_WRITEHEADER, data );

    try
    {
        getSession()->httpRunRequest( handle, m_contentUrl );
        
        data->finish();
        rewind( res );

        delete data;
        curl_easy_cleanup( handle );
    }
    catch ( const atom::CurlException& e )
    {
        fclose( res );
        res = NULL;

        delete data;
        curl_easy_cleanup( handle );

        throw e.getCmisException( );
    }

    return res;
}

boost::shared_ptr< istream > AtomDocument::getContentStream( ) throw ( libcmis::Exception )
{
    if ( getAllowableActions().get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::GetContentStream ) )
        throw libcmis::Exception( string( "GetContentStream is not allowed on document " ) + getId() );
    curl_global_init( CURL_GLOBAL_ALL );
    CURL* handle = curl_easy_init( );

    boost::shared_ptr< stringstream > stream( new stringstream( ios_base::out | ios_base::in | ios_base::binary ) );

    atom::EncodedData* data = new atom::EncodedData( stream.get() );

    curl_easy_setopt( handle, CURLOPT_WRITEFUNCTION, &lcl_getData );
    curl_easy_setopt( handle, CURLOPT_WRITEDATA, data );

    curl_easy_setopt( handle, CURLOPT_HEADERFUNCTION, lcl_getEncoding );
    curl_easy_setopt( handle, CURLOPT_WRITEHEADER, data );

    try
    {
        getSession()->httpRunRequest( handle, m_contentUrl );
        data->finish();
    
        delete data;
        curl_easy_cleanup( handle );
    }
    catch ( const atom::CurlException& e )
    {
        delete data;
        curl_easy_cleanup( handle );

        throw e.getCmisException( );
    }

    return stream;
}

void AtomDocument::setContentStream( istream& is, string contentType, bool overwrite ) throw ( libcmis::Exception )
{
    if ( getAllowableActions().get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::GetContentStream ) )
        throw libcmis::Exception( string( "SetContentStream is not allowed on document " ) + getId() );

    string overwriteStr( "false" );
    if ( overwrite )
        overwriteStr = "true";

    string putUrl( m_contentUrl );
    if ( putUrl.find( '?' ) != string::npos )
        putUrl += "&";
    else
        putUrl += "?";
    putUrl += "overwriteFlag=" + overwriteStr;

    // Use the changeToken if set on the object
    if ( !getChangeToken().empty() )
        putUrl += "changeToken=" + getChangeToken();

    try
    {
        getSession()->httpPutRequest( putUrl, is, contentType );
        refresh( );
    }
    catch ( const atom::CurlException& e )
    {
        throw e.getCmisException( );
    }
}

string AtomDocument::toString( )
{
    stringstream buf;

    buf << "Document Object:" << endl << endl;
    buf << AtomObject::toString();
    buf << "Content Type: " << getContentType( ) << endl;
    buf << "Content Length: " << getContentLength( ) << endl;
    buf << "Content Filename: " << getContentFilename( ) << endl;

    return buf.str();
}

void AtomDocument::extractInfos( xmlDocPtr doc )
{
    AtomObject::extractInfos( doc );
   
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

                // Get the content filename
                string filenameReq( "//cmis:propertyString[@propertyDefinitionId='cmis:contentStreamFileName']/cmis:value/text()" );
                m_contentFilename = atom::getXPathValue( pXPathCtx, filenameReq );

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
