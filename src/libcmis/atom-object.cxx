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
#include <sstream>

#include <boost/date_time/posix_time/posix_time.hpp>

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

                // Some implementation (SharePoint) are omitting the type attribute
                bool matchesType = m_type.empty( ) || link.getType().empty() || ( link.getType( ) == m_type );
                return matchesRel && matchesType;
            }
    };
}

AtomObject::AtomObject( AtomPubSession* session ) throw ( libcmis::Exception ) :
    m_session( session ),
    m_refreshTimestamp( 0 ),
    m_typeId( ),
    m_typeDescription( ),
    m_properties( ),
    m_allowableActions( ),
    m_links( )
{
}

AtomObject::AtomObject( const AtomObject& copy ) :
    m_session( copy.m_session ),
    m_refreshTimestamp( copy.m_refreshTimestamp ),
    m_typeId( copy.m_typeId ),
    m_typeDescription( copy.m_typeDescription ),
    m_properties( copy.m_properties ),
    m_allowableActions( copy.m_allowableActions ),
    m_links( copy.m_links )
{
}

AtomObject& AtomObject::operator=( const AtomObject& copy )
{
    if ( this != &copy )
    {
        m_session = copy.m_session;
        m_refreshTimestamp = copy.m_refreshTimestamp;
        m_typeId = copy.m_typeId;
        m_typeDescription = copy.m_typeDescription;
        m_properties = copy.m_properties;
        m_allowableActions = copy.m_allowableActions;
        m_links = copy.m_links;
    }

    return *this;
}

AtomObject::~AtomObject( )
{
}

string AtomObject::getId( )
{
    string name;
    map< string, libcmis::PropertyPtr >::iterator it = getProperties( ).find( string( "cmis:objectId" ) );
    if ( it != getProperties( ).end( ) && !it->second->getStrings( ).empty( ) )
        name = it->second->getStrings( ).front( );
    return name;
}

string AtomObject::getName( )
{
    string name;
    map< string, libcmis::PropertyPtr >::iterator it = getProperties( ).find( string( "cmis:name" ) );
    if ( it != getProperties( ).end( ) && !it->second->getStrings( ).empty( ) )
        name = it->second->getStrings( ).front( );
    return name;
}

vector< string > AtomObject::getPaths( )
{
    return vector< string > ( );
}

string AtomObject::getBaseType( )
{
    string value;
    map< string, libcmis::PropertyPtr >::iterator it = getProperties( ).find( string( "cmis:baseTypeId" ) );
    if ( it != getProperties( ).end( ) && !it->second->getStrings( ).empty( ) )
        value = it->second->getStrings( ).front( );
    return value;
}

string AtomObject::getType( )
{
    string value;
    map< string, libcmis::PropertyPtr >::iterator it = getProperties( ).find( string( "cmis:objectTypeId" ) );
    if ( it != getProperties( ).end( ) && !it->second->getStrings( ).empty( ) )
        value = it->second->getStrings( ).front( );
    return value;
}

string AtomObject::getCreatedBy( )
{
    string value;
    map< string, libcmis::PropertyPtr >::iterator it = getProperties( ).find( string( "cmis:createdBy" ) );
    if ( it != getProperties( ).end( ) && !it->second->getStrings( ).empty( ) )
        value = it->second->getStrings( ).front( );
    return value;
}

boost::posix_time::ptime AtomObject::getCreationDate( )
{
    boost::posix_time::ptime value;
    map< string, libcmis::PropertyPtr >::iterator it = getProperties( ).find( string( "cmis:creationDate" ) );
    if ( it != getProperties( ).end( ) && !it->second->getDateTimes( ).empty( ) )
        value = it->second->getDateTimes( ).front( );
    return value;
}

string AtomObject::getLastModifiedBy( )
{
    string value;
    map< string, libcmis::PropertyPtr >::iterator it = getProperties( ).find( string( "cmis:lastModifiedBy" ) );
    if ( it != getProperties( ).end( ) && !it->second->getStrings( ).empty( ) )
        value = it->second->getStrings( ).front( );
    return value;
}

boost::posix_time::ptime AtomObject::getLastModificationDate( )
{
    boost::posix_time::ptime value;
    map< string, libcmis::PropertyPtr >::iterator it = getProperties( ).find( string( "cmis:lastModificationDate" ) );
    if ( it != getProperties( ).end( ) && !it->second->getDateTimes( ).empty( ) )
        value = it->second->getDateTimes( ).front( );
    return value;
}

bool AtomObject::isImmutable( )
{
    bool value = false;
    map< string, libcmis::PropertyPtr >::iterator it = getProperties( ).find( string( "cmis:isImmutable" ) );
    if ( it != getProperties( ).end( ) && !it->second->getBools( ).empty( ) )
        value = it->second->getBools( ).front( );
    return value;
}

string AtomObject::getChangeToken( )
{
    string value;
    map< string, libcmis::PropertyPtr >::iterator it = getProperties( ).find( string( "cmis:changeToken" ) );
    if ( it != getProperties( ).end( ) && !it->second->getStrings( ).empty( ) )
        value = it->second->getStrings( ).front( );
    return value;
}

std::map< std::string, libcmis::PropertyPtr >& AtomObject::getProperties( )
{
    return m_properties;
}

void AtomObject::updateProperties( ) throw ( libcmis::Exception )
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

    refreshImpl( doc );
    xmlFreeDoc( doc );
}

libcmis::ObjectTypePtr AtomObject::getTypeDescription( )
{
    // Don't use the type from the properties as it may not be read yet.
    if ( !m_typeDescription.get( ) )
        m_typeDescription.reset( new AtomObjectType( m_session, m_typeId ) );

    return m_typeDescription;
}

boost::shared_ptr< libcmis::AllowableActions > AtomObject::getAllowableActions( )
{
    return m_allowableActions;
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
    m_refreshTimestamp = time( NULL );

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

        m_session->httpDeleteRequest( deleteUrl );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
}

string AtomObject::toString( )
{
    stringstream buf;

    buf << "Id: " << getId() << endl;
    buf << "Name: " << getName() << endl;
    buf << "Type: " << getType() << endl;
    buf << "Base type: " << getBaseType() << endl;
    buf << "Created on " << boost::posix_time::to_simple_string( getCreationDate() )
        << " by " << getCreatedBy() << endl;
    buf << "Last modified on " << boost::posix_time::to_simple_string( getLastModificationDate() )
        << " by " << getLastModifiedBy() << endl;
    buf << "Change token: " << getChangeToken() << endl;

    // Write remaining properties
    static const char* skippedProps[] = {
        "cmis:name", "cmis:baseTypeId", "cmis:objectTypeId", "cmis:createdBy",
        "cmis:creationDate", "cmis:lastModifiedBy", "cmis:lastModificationDate",
        "cmis::changeToken"
    };
    int skippedCount = sizeof( skippedProps ) / sizeof( char* );

    for ( map< string, libcmis::PropertyPtr >::iterator it = getProperties( ).begin();
            it != getProperties( ).end( ); ++it )
    {
        string propId = it->first;
        bool toSkip = false;
        for ( int i = 0; i < skippedCount && !toSkip; ++i )
        {
            toSkip = propId == skippedProps[i];
        }

        if ( !toSkip )
        {
            libcmis::PropertyPtr prop = it->second;
            buf << prop->getPropertyType( )->getDisplayName( ) << "( " << prop->getPropertyType()->getId( ) << " ): " << endl;
            vector< string > strValues = prop->getStrings( );
            for ( vector< string >::iterator valueIt = strValues.begin( );
                  valueIt != strValues.end( ); ++valueIt )
            {
                buf << "\t" << *valueIt << endl; 
            }
        }
    }

    return buf.str();
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

    // Output the properties
    xmlTextWriterStartElement( writer, BAD_CAST( "cmis:properties" ) );
    for ( map< string, libcmis::PropertyPtr >::iterator it = getProperties( ).begin( );
            it != getProperties( ).end( ); ++it )
    {
        it->second->toXml( writer );
    }
    xmlTextWriterEndElement( writer ); // cmis:properties
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

        // Get the allowableActions
        xpathObj = xmlXPathEvalExpression( BAD_CAST( "//cmis:allowableActions" ), xpathCtx );
        if ( xpathObj && xpathObj->nodesetval && xpathObj->nodesetval->nodeNr > 0 )
        {
            xmlNodePtr node = xpathObj->nodesetval->nodeTab[0];
            m_allowableActions.reset( new libcmis::AllowableActions( node ) );
        }
        xmlXPathFreeObject( xpathObj );

        // First get the type id as it will give us the property definitions
        string typeIdReq( "//cmis:propertyId[@propertyDefinitionId='cmis:objectTypeId']/cmis:value/text()" );
        m_typeId = libcmis::getXPathValue( xpathCtx, typeIdReq );

        string propertiesReq( "//cmis:properties/*" );
        xpathObj = xmlXPathEvalExpression( BAD_CAST( propertiesReq.c_str() ), xpathCtx );
        if ( NULL != xpathObj && NULL != xpathObj->nodesetval )
        {
            int size = xpathObj->nodesetval->nodeNr;
            for ( int i = 0; i < size; i++ )
            {
                xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];
                libcmis::PropertyPtr property = libcmis::parseProperty( node, getTypeDescription( ) );
                if ( property.get( ) )
                    m_properties.insert(
                            std::pair< string, libcmis::PropertyPtr >(
                                property->getPropertyType( )->getId(),
                                property ) );
            }
        }
        xmlXPathFreeObject( xpathObj );
    }

    xmlXPathFreeContext( xpathCtx );
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
