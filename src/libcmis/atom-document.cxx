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


AtomDocument::AtomDocument( AtomPubSession* session ) :
    AtomObject( session ),
    m_contentUrl( ),
    m_contentType( ),
    m_contentFilename( ),
    m_contentLength( 0 ),
    m_contentStream( )
{
}


AtomDocument::AtomDocument( AtomPubSession* session, xmlNodePtr entryNd ) :
    AtomObject( session ),
    m_contentUrl( ),
    m_contentType( ),
    m_contentFilename( ),
    m_contentLength( 0 ),
    m_contentStream( )
{
    xmlDocPtr doc = atom::wrapInDoc( entryNd );
    refreshImpl( doc );
    xmlFreeDoc( doc );
}

AtomDocument::~AtomDocument( )
{
}

vector< libcmis::FolderPtr > AtomDocument::getParents( ) throw ( libcmis::Exception )
{
    AtomLink* parentsLink = getLink( "up", "" );

    if ( ( NULL == parentsLink ) ||
            ( getAllowableActions( ).get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::GetObjectParents ) ) )
        throw libcmis::Exception( string( "GetObjectParents not allowed on node " ) + getId() );

    vector< libcmis::FolderPtr > parents;
    
    string buf;
    try
    {
        buf = getSession()->httpGetRequest( parentsLink->getHref( ) )->str( );
    }
    catch ( const atom::CurlException& e )
    {
        throw e.getCmisException( );
    }

    xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), parentsLink->getHref( ).c_str(), NULL, 0 );
    if ( NULL != doc )
    {
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
        atom::registerNamespaces( xpathCtx );
        if ( NULL != xpathCtx )
        {
            const string& entriesReq( "//atom:entry" );
            xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( entriesReq.c_str() ), xpathCtx );

            if ( NULL != xpathObj && NULL != xpathObj->nodesetval )
            {
                int size = xpathObj->nodesetval->nodeNr;
                for ( int i = 0; i < size; i++ )
                {
                    xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];
                    xmlDocPtr entryDoc = atom::wrapInDoc( node );
                    libcmis::ObjectPtr object = getSession()->createObjectFromEntryDoc( entryDoc );
                    libcmis::FolderPtr folder = boost::dynamic_pointer_cast< libcmis::Folder >( object );

                    if ( folder.get() )
                        parents.push_back( folder );
                    xmlFreeDoc( entryDoc );
                }
            }

            xmlXPathFreeObject( xpathObj );
        }

        xmlXPathFreeContext( xpathCtx );
    }
    else
    {
        throw new libcmis::Exception( "Failed to parse folder infos" );
    }
    xmlFreeDoc( doc );

    return parents;
}

vector< string > AtomDocument::getPaths( )
{
    vector< string > paths;
    vector< libcmis::FolderPtr > parents = getParents( );
    for ( vector< libcmis::FolderPtr >::iterator it = parents.begin( );
         it != parents.end(); ++it )
    {
        string path = ( *it )->getPath( );
        if ( path[path.size() - 1] != '/' )
            path += "/";
        path += getName( );
        paths.push_back( path );
    }
    return paths;
}

boost::shared_ptr< istream > AtomDocument::getContentStream( ) throw ( libcmis::Exception )
{
    if ( getAllowableActions().get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::GetContentStream ) )
        throw libcmis::Exception( string( "GetContentStream is not allowed on document " ) + getId() );

    boost::shared_ptr< istream > stream;
    try
    {
        stream = getSession()->httpGetRequest( m_contentUrl );
    }
    catch ( const atom::CurlException& e )
    {
        throw e.getCmisException( );
    }

    return stream;
}

void AtomDocument::setContentStream( boost::shared_ptr< ostream > os, string contentType, bool overwrite ) throw ( libcmis::Exception )
{
    if ( !os.get( ) )
        throw libcmis::Exception( "Missing stream" );

    if ( 0 == getRefreshTimestamp( ) )
    {
        m_contentStream = os;
        m_contentType = contentType;
    }
    else
    {
        if ( getAllowableActions().get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::GetContentStream ) )
            throw libcmis::Exception( string( "SetContentStream is not allowed on document " ) + getId() );

        string overwriteStr( "false" );
        if ( overwrite )
            overwriteStr = "true";

        string urlPattern( m_contentUrl );
        if ( urlPattern.find( '?' ) != string::npos )
            urlPattern += "&";
        else
            urlPattern += "?";
        urlPattern += "overwriteFlag={overwriteFlag}";

        map< string, string > params;
        params["overwriteFlag"] = overwriteStr;

        // Use the changeToken if set on the object
        if ( !getChangeToken().empty() )
        {
            urlPattern += "&changeToken={changeToken}";
            params["changeToken"] = getChangeToken();
        }

        string putUrl = getSession()->createUrl( urlPattern, params );       

        bool tryBase64 = false;
        do
        {
            try
            {
                boost::shared_ptr< istream> is ( new istream ( os->rdbuf( ) ) );
                if ( tryBase64 )
                {
                    tryBase64 = false;

                    // Encode the content
                    stringstream* encodedIn = new stringstream( );
                    atom::EncodedData encoder( encodedIn );
                    encoder.setEncoding( "base64" );

                    int bufLength = 1000;
                    char* buf = new char[ bufLength ];
                    do
                    {
                        is->read( buf, bufLength );
                        int size = is->gcount( );
                        encoder.encode( buf, 1, size );
                    } while ( !is->eof( ) && !is->fail( ) );
                    delete[] buf;
                    encoder.finish( );

                    encodedIn->seekg( 0, ios_base::beg );
                    encodedIn->clear( );

                    is.reset( encodedIn );
                }
                getSession()->httpPutRequest( putUrl, *is, contentType );

                long httpStatus = getSession( )->getHttpStatus( );
                if ( httpStatus < 200 || httpStatus >= 300 )
                    throw libcmis::Exception( "Document content wasn't set for some reason" );
                refresh( );
            }
            catch ( const atom::CurlException& e )
            {
                // SharePoint wants base64 encoded content... let's try to figure out
                // if we falled in that case.
                if ( !tryBase64 && e.getHttpStatus() == 400 )
                    tryBase64 = true;
                else
                    throw e.getCmisException( );
            }
        }
        while ( tryBase64 );
    }
}

libcmis::DocumentPtr AtomDocument::checkOut( ) throw ( libcmis::Exception )
{
    if ( ( getAllowableActions( ).get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::CheckOut ) ) )
        throw libcmis::Exception( string( "CanCheckout not allowed on document " ) + getId() );
    
    xmlBufferPtr buf = xmlBufferCreate( );
    xmlTextWriterPtr writer = xmlNewTextWriterMemory( buf, 0 );

    xmlTextWriterStartDocument( writer, NULL, NULL, NULL );

    // Copy and remove the readonly properties before serializing
    toXml( writer );

    xmlTextWriterEndDocument( writer );
    string str( ( const char * )xmlBufferContent( buf ) );
    istringstream is( str );

    xmlFreeTextWriter( writer );
    xmlBufferFree( buf );

    string respBuf;
    string checkedOutUrl = getSession()->getWorkspace( ).getCollectionUrl( atom::Collection::CheckedOut );
    try
    {
        respBuf = getSession( )->httpPostRequest( checkedOutUrl, is, "application/atom+xml;type=entry" );
    }
    catch ( const atom::CurlException& e )
    {
        throw e.getCmisException( );
    }

    xmlDocPtr doc = xmlReadMemory( respBuf.c_str(), respBuf.size(), checkedOutUrl.c_str(), NULL, 0 );
    if ( NULL == doc )
        throw libcmis::Exception( "Failed to parse object infos" );

    libcmis::ObjectPtr created = getSession( )->createObjectFromEntryDoc( doc );
    xmlFreeDoc( doc );

    libcmis::DocumentPtr pwc = boost::dynamic_pointer_cast< libcmis::Document >( created );
    if ( !pwc.get( ) )
        throw libcmis::Exception( string( "Created object is not a document: " ) + created->getId( ) );

    return pwc;
}

string AtomDocument::toString( )
{
    stringstream buf;

    buf << "Document Object:" << endl << endl;
    buf << AtomObject::toString();
    buf << "Parents ids: ";
    vector< libcmis::FolderPtr > parents = getParents( );
    for ( vector< libcmis::FolderPtr >::iterator it = parents.begin(); it != parents.end(); ++it )
        buf << "'" << ( *it )->getId( ) << "' ";
    buf << endl;
    buf << "Content Type: " << getContentType( ) << endl;
    buf << "Content Length: " << getContentLength( ) << endl;
    buf << "Content Filename: " << getContentFilename( ) << endl;

    return buf.str();
}

void AtomDocument::extractInfos( xmlDocPtr doc )
{
    AtomObject::extractInfos( doc );
   
   // Get the content url and type 
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
    if ( NULL != doc )
    {
        atom::registerNamespaces( xpathCtx );

        if ( NULL != xpathCtx )
        {
            xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( "//atom:content" ), xpathCtx );
            if ( xpathObj && xpathObj->nodesetval && xpathObj->nodesetval->nodeNr > 0 )
            {
                xmlNodePtr contentNd = xpathObj->nodesetval->nodeTab[0];
                xmlChar* src = xmlGetProp( contentNd, BAD_CAST( "src" ) );
                m_contentUrl = string( ( char* ) src );
                xmlFree( src );
                
                xmlChar* type = xmlGetProp( contentNd, BAD_CAST( "type" ) );
                m_contentType = string( ( char* ) type );
                xmlFree( type );

                // Get the content filename
                string filenameReq( "//cmis:propertyString[@propertyDefinitionId='cmis:contentStreamFileName']/cmis:value/text()" );
                m_contentFilename = atom::getXPathValue( xpathCtx, filenameReq );

                // Get the content length
                string lengthReq( "//cmis:propertyInteger[@propertyDefinitionId='cmis:contentStreamLength']/cmis:value/text()" );
                string bytes = atom::getXPathValue( xpathCtx, lengthReq );
                m_contentLength = atol( bytes.c_str() );
            }
            xmlXPathFreeObject( xpathObj );
        }
        xmlXPathFreeContext( xpathCtx );
    }
}

void AtomDocument::contentToXml( xmlTextWriterPtr writer )
{
    if ( m_contentStream.get( ) )
    {
        xmlTextWriterStartElement( writer, BAD_CAST( "cmisra:content" ) );
        xmlTextWriterWriteElement( writer, BAD_CAST( "cmisra:mediatype" ), BAD_CAST( m_contentType.c_str() ) );

        ostringstream encodedStream;
        atom::EncodedData encoder( &encodedStream );
        encoder.setEncoding( "base64" );
        istream is( m_contentStream->rdbuf( ) );
        int bufLength = 1000;
        char* buf = new char[ bufLength ];
        do
        {
            is.read( buf, bufLength );
            int size = is.gcount( );
            encoder.encode( buf, 1, size );
        } while ( !is.eof( ) && !is.fail( ) );
        delete[] buf;
        encoder.finish( );
        xmlTextWriterWriteElement( writer, BAD_CAST( "cmisra:base64" ), BAD_CAST( encodedStream.str().c_str() ) );

        xmlTextWriterEndElement( writer );
    }
}
