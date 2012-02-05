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

#include <sstream>

#include "atom-object.hxx"
#include "atom-session.hxx"
#include "atom-utils.hxx"
#include "xml-utils.hxx"

using namespace boost;
using namespace std;


/** Constructor for the object, the url provided url should point to the object
    CMIS properties. The content of the URL isn't extracted and parsed by the constructor:
    this task is left to the class children.
  */
AtomObject::AtomObject( AtomPubSession* session, string url ) throw ( libcmis::Exception ) :
    m_session( session ),
    m_refreshTimestamp( 0 ),
    m_infosUrl( url ),
    m_properties( ),
    m_allowableActions( )
{
}

AtomObject::AtomObject( const AtomObject& copy ) :
    m_session( copy.m_session ),
    m_refreshTimestamp( copy.m_refreshTimestamp ),
    m_infosUrl( copy.m_infosUrl ),
    m_properties( copy.m_properties ),
    m_allowableActions( copy.m_allowableActions )
{
}

AtomObject& AtomObject::operator=( const AtomObject& copy )
{
    m_session = copy.m_session;
    m_refreshTimestamp = copy.m_refreshTimestamp;
    m_infosUrl = copy.m_infosUrl;
    m_properties = copy.m_properties;
    m_allowableActions = copy.m_allowableActions;

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

posix_time::ptime AtomObject::getCreationDate( )
{
    posix_time::ptime value;
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

posix_time::ptime AtomObject::getLastModificationDate( )
{
    posix_time::ptime value;
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

shared_ptr< libcmis::AllowableActions > AtomObject::getAllowableActions( )
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
            buf  = getSession()->httpGetRequest( getInfosUrl() );
        }
        catch ( const atom::CurlException& e )
        {
            throw e.getCmisException( );
        }

        doc = xmlReadMemory( buf.c_str(), buf.size(), getInfosUrl().c_str(), NULL, 0 );

        if ( NULL == doc )
            throw libcmis::Exception( "Failed to parse object infos" );

    }

    extractInfos( doc );
    m_refreshTimestamp = time( NULL );

    if ( createdDoc )
        xmlFreeDoc( doc );
}

string AtomObject::toString( )
{
    stringstream buf;

    buf << "Id: " << getId() << endl;
    buf << "Name: " << getName() << endl;
    buf << "Type: " << getType() << endl;
    buf << "Base type: " << getBaseType() << endl;
    buf << "Created on " << posix_time::to_simple_string( getCreationDate() )
        << " by " << getCreatedBy() << endl;
    buf << "Last modified on " << posix_time::to_simple_string( getLastModificationDate() )
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
            buf << prop->getDisplayName( ) << "( " << prop->getId( ) << " ): " << endl;
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

void AtomObject::extractInfos( xmlDocPtr doc )
{
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );

    atom::registerNamespaces( xpathCtx );

    if ( NULL != xpathCtx )
    {
        // Get the infos URL as we may not have it
        string selfReq( "//atom:link[@rel='self']/@href" );
        m_infosUrl = atom::getXPathValue( xpathCtx, selfReq ) ;
        
        // Get the URL to the allowableActions
        string allowableActionsReq( "//atom:link[@rel='http://docs.oasis-open.org/ns/cmis/link/200908/allowableactions']/attribute::href" );
        string allowableActionsUrl = atom::getXPathValue( xpathCtx, allowableActionsReq );
        if ( !allowableActionsUrl.empty() )
        {
            shared_ptr< AtomAllowableActions > allowableActions( new AtomAllowableActions( m_session, allowableActionsUrl ) );
            m_allowableActions.swap( allowableActions );
        }

        string propertiesReq( "//cmis:properties/*" );
        xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( propertiesReq.c_str() ), xpathCtx );
        if ( NULL != xpathObj && NULL != xpathObj->nodesetval )
        {
            int size = xpathObj->nodesetval->nodeNr;
            for ( int i = 0; i < size; i++ )
            {
                xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];
                libcmis::PropertyPtr property = libcmis::parseProperty( node );
                m_properties.insert( std::pair< string, libcmis::PropertyPtr >( property->getId(), property ) );
            }
        }
        xmlXPathFreeObject( xpathObj );
    }

    xmlXPathFreeContext( xpathCtx );
}

