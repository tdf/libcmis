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
#include <locale>
#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>

#include "atom-document.hxx"
#include "atom-folder.hxx"
#include "atom-object.hxx"
#include "atom-object-type.hxx"
#include "atom-session.hxx"
#include "xml-utils.hxx"

using namespace std;

namespace
{
    class MatchLink
    {
        private:
            string m_rel;
            string m_type;

        public:
            MatchLink( string rel, string type ) : m_rel( rel ), m_type( type ) { }
            bool operator() ( AtomLink link )
            {
                bool matchesRel = link.getRel( ) == m_rel;

                // Some implementations (xcmis) put extra spaces into the type attribute
                // (e.g. "application/atom+xml; type=feed" instead of "application/atom+xml;type=feed")
                string linkType = link.getType( );
                linkType.erase( remove_if( linkType.begin(), linkType.end(), ::isspace ), linkType.end() );

                // Some implementation (SharePoint) are omitting the type attribute
                bool matchesType = m_type.empty( ) || linkType.empty() || ( linkType == m_type );
                return matchesRel && matchesType;
            }
    };
}

AtomObject::AtomObject( AtomPubSession* session ) throw ( libcmis::Exception ) :
    libcmis::Object( session ),
    m_links( )
{
}

AtomObject::AtomObject( const AtomObject& copy ) :
    libcmis::Object( copy ),
    m_links( copy.m_links )
{
}

AtomObject& AtomObject::operator=( const AtomObject& copy )
{
    if ( this != &copy )
    {
        libcmis::Object::operator=( copy );
        m_links = copy.m_links;
    }

    return *this;
}

AtomObject::~AtomObject( )
{
}

libcmis::ObjectPtr AtomObject::updateProperties( ) throw ( libcmis::Exception )
{
    if ( getAllowableActions().get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::UpdateProperties ) )
        throw libcmis::Exception( string( "UpdateProperties is not allowed on object " ) + getId() );

    xmlBufferPtr buf = xmlBufferCreate( );
    xmlTextWriterPtr writer = xmlNewTextWriterMemory( buf, 0 );

    xmlTextWriterStartDocument( writer, NULL, NULL, NULL );

    // Copy and remove the readonly properties before serializing
    AtomObject copy( *this );
    map< string, libcmis::PropertyPtr >& props = copy.getProperties( );
    for ( map< string, libcmis::PropertyPtr >::iterator it = props.begin( ); it != props.end( ); )
    {
        if ( !it->second->getPropertyType( )->isUpdatable( ) )
            props.erase( it++ );
        else
            ++it;
    }
    copy.toXml( writer );

    xmlTextWriterEndDocument( writer );
    string str( ( const char * )xmlBufferContent( buf ) );
    istringstream is( str );

    xmlFreeTextWriter( writer );
    xmlBufferFree( buf );

    libcmis::HttpResponsePtr response;
    try
    {
        response = getSession( )->httpPutRequest( getInfosUrl( ), is, "application/atom+xml;type=entry" );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    string respBuf = response->getStream( )->str( );
    xmlDocPtr doc = xmlReadMemory( respBuf.c_str(), respBuf.size(), getInfosUrl().c_str(), NULL, 0 );
    if ( NULL == doc )
        throw libcmis::Exception( "Failed to parse object infos" );

    libcmis::ObjectPtr updated = getSession( )->createObjectFromEntryDoc( doc );
    if ( updated->getId( ) == getId( ) )
        refreshImpl( doc );
    xmlFreeDoc( doc );

    return updated;
}

void AtomObject::refreshImpl( xmlDocPtr doc ) throw ( libcmis::Exception )
{
    bool createdDoc = ( NULL == doc );
    if ( createdDoc )
    {
        string buf;
        try
        {
            buf  = getSession()->httpGetRequest( getInfosUrl() )->getStream( )->str( );
        }
        catch ( const CurlException& e )
        {
            throw e.getCmisException( );
        }

        doc = xmlReadMemory( buf.c_str(), buf.size(), getInfosUrl().c_str(), NULL, 0 );

        if ( NULL == doc )
            throw libcmis::Exception( "Failed to parse object infos" );

    }

    // Cleanup the structures before setting them again
    m_typeDescription.reset( );
    m_properties.clear( );
    m_allowableActions.reset( );
    m_links.clear( );


    extractInfos( doc );

    if ( createdDoc )
        xmlFreeDoc( doc );
}

void AtomObject::remove( bool allVersions ) throw ( libcmis::Exception )
{
    if ( getAllowableActions( ).get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::DeleteObject ) )
        throw libcmis::Exception( string( "DeleteObject not allowed on object " ) + getId() );

    try
    {
        string deleteUrl = getInfosUrl( );
        if ( deleteUrl.find( '?' ) != string::npos )
            deleteUrl += "&";
        else
            deleteUrl += "?";
        
        string allVersionsStr = "TRUE";
        if ( !allVersions )
            allVersionsStr = "FALSE";
        deleteUrl += "allVersions=" + allVersionsStr;

        getSession( )->httpDeleteRequest( deleteUrl );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
}

void AtomObject::move( boost::shared_ptr< libcmis::Folder > source, boost::shared_ptr< libcmis::Folder > destination ) throw ( libcmis::Exception )
{
    AtomFolder* atomDestination = dynamic_cast< AtomFolder* > ( destination.get() );

    if ( NULL == atomDestination )
        throw libcmis::Exception( string( "Destination is not an AtomFolder" ) );

    AtomLink* destChildrenLink = atomDestination->getLink( "down", "application/atom+xml;type=feed" );

    if ( ( NULL == destChildrenLink ) || ( getAllowableActions().get() &&
            !getAllowableActions()->isAllowed( libcmis::ObjectAction::MoveObject ) ) )
        throw libcmis::Exception( string( "MoveObject not allowed on object " ) + getId() );

    // create object xml
    xmlBufferPtr buf = xmlBufferCreate( );
    xmlTextWriterPtr writer = xmlNewTextWriterMemory( buf, 0 );
    xmlTextWriterStartDocument( writer, NULL, NULL, NULL );
    toXml( writer );
    xmlTextWriterEndDocument( writer );

    string str( ( const char * )xmlBufferContent( buf ) );
    istringstream is( str );
    xmlFreeTextWriter( writer );
    xmlBufferFree( buf );

    // create post url
    string postUrl = destChildrenLink->getHref();
    if ( postUrl.find( '?' ) != string::npos )
        postUrl += "&";
    else
        postUrl += "?";
    postUrl += "sourceFolderId={sourceFolderId}";
    // Session::CreateUrl is used to properly escape the id
    map< string, string > params;
    params[ "sourceFolderId" ] = source->getId();
    postUrl = getSession( )->createUrl( postUrl, params );

    // post it
    libcmis::HttpResponsePtr response;
    try
    {
        response = getSession( )->httpPostRequest( postUrl, is, "application/atom+xml;type=entry" );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    // refresh self from response
    string respBuf = response->getStream( )->str( );
    xmlDocPtr doc = xmlReadMemory( respBuf.c_str(), respBuf.size(), getInfosUrl().c_str(), NULL, 0 );
    if ( NULL == doc )
        throw libcmis::Exception( "Failed to parse object infos" );
    refreshImpl( doc );
    xmlFreeDoc( doc );
}

void AtomObject::toXml( xmlTextWriterPtr writer )
{
    xmlTextWriterStartElement( writer, BAD_CAST( "atom:entry" ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:atom" ), BAD_CAST( NS_ATOM_URL ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmis" ), BAD_CAST( NS_CMIS_URL ) );
    xmlTextWriterWriteAttribute( writer, BAD_CAST( "xmlns:cmisra" ), BAD_CAST( NS_CMISRA_URL ) );

    if ( !getCreatedBy( ).empty( ) )
    {
        xmlTextWriterStartElement( writer, BAD_CAST( "atom:author" ) );
        xmlTextWriterWriteElement( writer, BAD_CAST( "atom:name" ), BAD_CAST( getCreatedBy( ).c_str( ) ) );
        xmlTextWriterEndElement( writer );
    }

    xmlTextWriterWriteElement( writer, BAD_CAST( "atom:title" ), BAD_CAST( getName( ).c_str( ) ) );

    boost::posix_time::ptime now( boost::posix_time::second_clock::universal_time( ) );
    xmlTextWriterWriteElement( writer, BAD_CAST( "atom:updated" ), BAD_CAST( libcmis::writeDateTime( now ).c_str( ) ) );

    contentToXml( writer );

    xmlTextWriterStartElement( writer, BAD_CAST( "cmisra:object" ) );

    libcmis::Object::toXml( writer );

    xmlTextWriterEndElement( writer ); // cmisra:object

    xmlTextWriterEndElement( writer ); // atom:entry
}

string AtomObject::getInfosUrl( )
{
    AtomLink* selfLink = getLink( "self", "application/atom+xml;type=entry" );
    if ( NULL != selfLink )
        return selfLink->getHref( );
    return string( );
}

void AtomObject::extractInfos( xmlDocPtr doc )
{
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );

    libcmis::registerNamespaces( xpathCtx );

    if ( NULL != xpathCtx )
    {
        m_links.clear( );

        // Get all the atom links
        string linksReq( "//atom:link" );
        xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( linksReq.c_str() ), xpathCtx );
        if ( NULL != xpathObj && NULL != xpathObj->nodesetval )
        {
            int size = xpathObj->nodesetval->nodeNr;
            for ( int i = 0; i < size; i++ )
            {
                xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];
                try
                {
                    AtomLink link( node );
                    m_links.push_back( node );
                }
                catch ( const libcmis::Exception& )
                {
                    // Broken or incomplete link... don't add it
                }
            }
        }
        xmlXPathFreeObject( xpathObj );


        xpathObj = xmlXPathEvalExpression( BAD_CAST( "//cmisra:object" ), xpathCtx );
        if ( xpathObj && xpathObj->nodesetval && xpathObj->nodesetval->nodeNr > 0 )
        {
            xmlNodePtr node = xpathObj->nodesetval->nodeTab[0];
            initializeFromNode( node );
        }
        xmlXPathFreeObject( xpathObj );
    }

    xmlXPathFreeContext( xpathCtx );
}

AtomPubSession* AtomObject::getSession( )
{
    return dynamic_cast< AtomPubSession* >( m_session );
}

void AtomObject::contentToXml( xmlTextWriterPtr )
{
}

AtomLink* AtomObject::getLink( std::string rel, std::string type )
{
    AtomLink* link = NULL;
    vector< AtomLink >::iterator it = find_if( m_links.begin(), m_links.end(), MatchLink( rel, type ) );
    if ( it != m_links.end() )
        link = &( *it );
    return link;
}

AtomLink::AtomLink( xmlNodePtr node ) throw ( libcmis::Exception ):
    m_rel( ),
    m_type( ),
    m_id( ),
    m_href( )
{
    m_rel = libcmis::getXmlNodeAttributeValue( node, "rel" );
    m_href = libcmis::getXmlNodeAttributeValue( node, "href" );
    
    try
    {
        m_type = libcmis::getXmlNodeAttributeValue( node, "type" );
        m_id = libcmis::getXmlNodeAttributeValue( node, "id" );
    }
    catch ( const libcmis::Exception & )
    {
        // id attribute can be missing
        // type attribute is missing in some implementations (SharePoint)
    }
}
