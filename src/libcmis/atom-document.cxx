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
#include "xml-utils.hxx"

using namespace std;
using namespace libcmis;

AtomDocument::AtomDocument( AtomPubSession* session ) :
    libcmis::Object( session ),
    libcmis::Document( session ),
    AtomObject( session ),
    m_contentUrl( )
{
}


AtomDocument::AtomDocument( AtomPubSession* session, xmlNodePtr entryNd ) :
    libcmis::Object( session ),
    libcmis::Document( session ),
    AtomObject( session ),
    m_contentUrl( )
{
    xmlDocPtr doc = libcmis::wrapInDoc( entryNd );
    refreshImpl( doc );
    xmlFreeDoc( doc );
}

AtomDocument::~AtomDocument( )
{
}

vector< libcmis::FolderPtr > AtomDocument::getParents( )
{
    AtomLink* parentsLink = getLink( "up", "" );

    if ( ( NULL == parentsLink ) ||
            ( getAllowableActions( ).get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::GetObjectParents ) ) )
        throw libcmis::Exception( string( "GetObjectParents not allowed on node " ) + getId() );

    vector< libcmis::FolderPtr > parents;
    
    string buf;
    try
    {
        buf = getSession()->httpGetRequest( parentsLink->getHref( ) )->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), parentsLink->getHref( ).c_str(), NULL, 0 );
    if ( NULL != doc )
    {
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
        libcmis::registerNamespaces( xpathCtx );
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
                    xmlDocPtr entryDoc = libcmis::wrapInDoc( node );
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
        throw libcmis::Exception( "Failed to parse folder infos" );
    }
    xmlFreeDoc( doc );

    return parents;
}

boost::shared_ptr< istream > AtomDocument::getContentStream( string /*streamId*/ )
{
    if ( getAllowableActions().get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::GetContentStream ) )
        throw libcmis::Exception( string( "GetContentStream is not allowed on document " ) + getId() );

    boost::shared_ptr< istream > stream;
    try
    {
        stream = getSession()->httpGetRequest( m_contentUrl )->getStream( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    return stream;
}

void AtomDocument::setContentStream( boost::shared_ptr< ostream > os, string contentType, string fileName, bool overwrite )
{
    if ( !os.get( ) )
        throw libcmis::Exception( "Missing stream" );

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
                libcmis::EncodedData encoder( encodedIn );
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
            vector< string > headers;
            headers.push_back( string( "Content-Type: " ) + contentType );
            if ( !fileName.empty( ) )
                headers.push_back( string( "Content-Disposition: attachment; filename=" ) + fileName );
            getSession()->httpPutRequest( putUrl, *is, headers );

            long httpStatus = getSession( )->getHttpStatus( );
            if ( httpStatus < 200 || httpStatus >= 300 )
                throw libcmis::Exception( "Document content wasn't set for some reason" );
            refresh( );
        }
        catch ( const CurlException& e )
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

libcmis::DocumentPtr AtomDocument::checkOut( )
{
    if ( ( getAllowableActions( ).get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::CheckOut ) ) )
        throw libcmis::Exception( string( "CanCheckout not allowed on document " ) + getId() );
    
    xmlBufferPtr buf = xmlBufferCreate( );
    xmlTextWriterPtr writer = xmlNewTextWriterMemory( buf, 0 );

    xmlTextWriterStartDocument( writer, NULL, NULL, NULL );

    // Create a document with only the needed properties
    PropertyPtrMap props; 
    PropertyPtrMap::iterator it = getProperties( ).find( string( "cmis:objectId" ) );
    if ( it != getProperties( ).end( ) )
    {
        props.insert( *it );
    }

    boost::shared_ptr< ostream > stream;
    AtomObject::writeAtomEntry( writer, props, stream, string( ) );

    xmlTextWriterEndDocument( writer );
    string str( ( const char * )xmlBufferContent( buf ) );
    istringstream is( str );

    xmlFreeTextWriter( writer );
    xmlBufferFree( buf );

    libcmis::HttpResponsePtr resp;
    string urlPattern = getSession()->getAtomRepository( )->getCollectionUrl( Collection::CheckedOut );
    if ( urlPattern.find( "?" ) != string::npos )
        urlPattern += "&";
    else
        urlPattern += "?";
    urlPattern += "objectId={objectId}";

    map< string, string > params;
    params[ "objectId" ] = getId( );
    string checkedOutUrl = getSession( )->createUrl( urlPattern, params );

    try
    {
        resp = getSession( )->httpPostRequest( checkedOutUrl, is, "application/atom+xml;type=entry" );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    string respBuf = resp->getStream( )->str();
    xmlDocPtr doc = xmlReadMemory( respBuf.c_str(), respBuf.size(), checkedOutUrl.c_str(), NULL, 0 );
    if ( NULL == doc )
        throw libcmis::Exception( "Failed to parse object infos" );

    libcmis::ObjectPtr created = getSession( )->createObjectFromEntryDoc( doc, AtomPubSession::RESULT_DOCUMENT );
    xmlFreeDoc( doc );

    libcmis::DocumentPtr pwc = boost::dynamic_pointer_cast< libcmis::Document >( created );
    if ( !pwc.get( ) )
        throw libcmis::Exception( string( "Created object is not a document: " ) + created->getId( ) );

    return pwc;
}

void AtomDocument::cancelCheckout( )
{
    if ( ( getAllowableActions( ).get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::CancelCheckOut ) ) )
        throw libcmis::Exception( string( "CanCancelCheckout not allowed on document " ) + getId() );

    string url = getInfosUrl( );

    // Use working-copy link if provided as a workaround
    // for some non-compliant repositories
    AtomLink* link = getLink( "working-copy", "application/atom+xml;type=entry" );
    if ( link )
        url = link->getHref( );

    try
    {
        getSession( )->httpDeleteRequest( url );
    }
    catch ( CurlException const& e )
    {
        throw e.getCmisException( );
    }
}

libcmis::DocumentPtr AtomDocument::checkIn( bool isMajor, string comment,
                            const PropertyPtrMap& properties,
                            boost::shared_ptr< ostream > stream, string contentType, string )
{
    if ( ( getAllowableActions( ).get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::CheckIn ) ) )
        throw libcmis::Exception( string( "CanCheckIn not allowed on document " ) + getId() );

    string urlPattern = getInfosUrl( );

    // Use working-copy link if provided as a workaround
    // for some non-compliant repositories
    AtomLink* link = getLink( "working-copy", "application/atom+xml;type=entry" );
    if ( link )
        urlPattern = link->getHref( );

    if ( urlPattern.find( "?" ) != string::npos )
        urlPattern += "&";
    else
        urlPattern += "?";
    urlPattern += "checkin=true&major={major}&checkinComment={checkinComment}";

    map< string, string > params;

    string majorStr = "false";
    if ( isMajor )
        majorStr = "true";
    params[ "major" ] = majorStr;
    params[ "checkinComment" ] = comment;
    string checkInUrl = getSession( )->createUrl( urlPattern, params );

    xmlBufferPtr buf = xmlBufferCreate( );
    xmlTextWriterPtr writer = xmlNewTextWriterMemory( buf, 0 );

    xmlTextWriterStartDocument( writer, NULL, NULL, NULL );

    AtomObject::writeAtomEntry( writer, properties, stream, contentType );

    xmlTextWriterEndDocument( writer );
    string str( ( const char * )xmlBufferContent( buf ) );
    istringstream is( str );

    xmlFreeTextWriter( writer );
    xmlBufferFree( buf );
    
    // Run the request
    libcmis::HttpResponsePtr response;
    try
    {
        vector< string > headers;
        headers.push_back( string( "Content-Type: application/atom+xml;type=entry" ) );
        response = getSession( )->httpPutRequest( checkInUrl, is, headers );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    
    // Get the returned entry and update using it
    string respBuf = response->getStream( )->str( );
    xmlDocPtr doc = xmlReadMemory( respBuf.c_str(), respBuf.size(), checkInUrl.c_str(), NULL, 0 );
    if ( NULL == doc )
        throw libcmis::Exception( "Failed to parse object infos" );


    libcmis::ObjectPtr newVersion = getSession( )->createObjectFromEntryDoc( doc, AtomPubSession::RESULT_DOCUMENT );

    if ( newVersion->getId( ) == getId( ) )
        refreshImpl( doc );
    xmlFreeDoc( doc );

    return boost::dynamic_pointer_cast< libcmis::Document >( newVersion );
}

vector< libcmis::DocumentPtr > AtomDocument::getAllVersions( )
{
    if ( getAllowableActions( ).get() &&
                !getAllowableActions()->isAllowed( libcmis::ObjectAction::GetAllVersions ) )
        throw libcmis::Exception( string( "GetAllVersions not allowed on node " ) + getId() );

    vector< libcmis::DocumentPtr > versions;
    AtomLink* link = getLink( "version-history", string( ) );
    if ( link != NULL )
    {
        string pageUrl = link->getHref( );

        string buf;
        try
        {
            buf = getSession()->httpGetRequest( pageUrl )->getStream( )->str( );
        }
        catch ( const CurlException& e )
        {
            throw e.getCmisException( );
        }

        xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), pageUrl.c_str(), NULL, 0 );
        if ( NULL != doc )
        {
            xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
            libcmis::registerNamespaces( xpathCtx );
            if ( NULL != xpathCtx )
            {
                // Get the entries
                const string& entriesReq( "//atom:entry" );
                xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( entriesReq.c_str() ), xpathCtx );

                if ( NULL != xpathObj && NULL != xpathObj->nodesetval )
                {
                    int size = xpathObj->nodesetval->nodeNr;
                    for ( int i = 0; i < size; i++ )
                    {
                        xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];
                        xmlDocPtr entryDoc = libcmis::wrapInDoc( node );
                        libcmis::ObjectPtr cmisObject = getSession()->createObjectFromEntryDoc( entryDoc );
                        libcmis::DocumentPtr cmisDoc = boost::dynamic_pointer_cast< libcmis::Document >( cmisObject );

                        if ( cmisDoc.get() )
                            versions.push_back( cmisDoc );
                        xmlFreeDoc( entryDoc );
                    }
                }

                xmlXPathFreeObject( xpathObj );
            }

            xmlXPathFreeContext( xpathCtx );
        }
        else
        {
            throw libcmis::Exception( "Failed to parse versions infos" );
        }
        xmlFreeDoc( doc );

    }
    return versions;
}

void AtomDocument::extractInfos( xmlDocPtr doc )
{
    AtomObject::extractInfos( doc );
   
   // Get the content url 
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
    if ( NULL != doc )
    {
        libcmis::registerNamespaces( xpathCtx );

        if ( NULL != xpathCtx )
        {
            xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( "//atom:content" ), xpathCtx );
            if ( xpathObj && xpathObj->nodesetval && xpathObj->nodesetval->nodeNr > 0 )
            {
                xmlNodePtr contentNd = xpathObj->nodesetval->nodeTab[0];
                xmlChar* src = xmlGetProp( contentNd, BAD_CAST( "src" ) );
                m_contentUrl = string( ( char* ) src );
                xmlFree( src );
            }
            xmlXPathFreeObject( xpathObj );
        }
        xmlXPathFreeContext( xpathCtx );
    }
}
