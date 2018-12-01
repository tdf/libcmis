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

#include "atom-object-type.hxx"
#include "xml-utils.hxx"

using namespace std;
using namespace boost;


AtomObjectType::AtomObjectType( AtomPubSession* session, string id ) :
    libcmis::ObjectType( ),
    m_session( session ),
    m_selfUrl( ),
    m_childrenUrl( )
{
    m_id = id;
    refresh( );
}

AtomObjectType::AtomObjectType( AtomPubSession* session, xmlNodePtr entryNd ) :
    libcmis::ObjectType( ),
    m_session( session ),
    m_selfUrl( ),
    m_childrenUrl( )
{
    xmlDocPtr doc = libcmis::wrapInDoc( entryNd );
    refreshImpl( doc );
    xmlFreeDoc( doc );
}

AtomObjectType::AtomObjectType( const AtomObjectType& copy ) :
    libcmis::ObjectType( copy ),
    m_session( copy.m_session ),
    m_selfUrl( copy.m_selfUrl ),
    m_childrenUrl( copy.m_childrenUrl )
{
}

AtomObjectType::~AtomObjectType( )
{
}

AtomObjectType& AtomObjectType::operator=( const AtomObjectType& copy )
{
    if ( this != &copy )
    {
        libcmis::ObjectType::operator=( copy );
        m_session = copy.m_session;
        m_selfUrl = copy.m_selfUrl;
        m_childrenUrl = copy.m_childrenUrl;
    }

    return *this;
}

libcmis::ObjectTypePtr AtomObjectType::getParentType( )
{
    return m_session->getType( m_parentTypeId );
}

libcmis::ObjectTypePtr AtomObjectType::getBaseType( )
{
    return m_session->getType( m_baseTypeId );
}

vector< libcmis::ObjectTypePtr > AtomObjectType::getChildren( )
{
    return m_session->getChildrenTypes( m_childrenUrl );
}

void AtomObjectType::refreshImpl( xmlDocPtr doc )
{
    bool createdDoc = ( NULL == doc );
    if ( createdDoc )
    {
        string pattern = m_session->getAtomRepository()->getUriTemplate( UriTemplate::TypeById );
        map< string, string > vars;
        vars[URI_TEMPLATE_VAR_ID] = getId( );
        string url = m_session->createUrl( pattern, vars );

        string buf;
        try
        {
            buf  = m_session->httpGetRequest( url )->getStream()->str();
        }
        catch ( const CurlException& e )
        {
            if ( ( e.getErrorCode( ) == CURLE_HTTP_RETURNED_ERROR ) &&
                 ( e.getHttpStatus( ) == 404 ) )
            {
                string msg = "No such type: ";
                msg += getId( );
                throw libcmis::Exception( msg, "objectNotFound" );
            }
            else
                throw e.getCmisException( );
        }

        doc = xmlReadMemory( buf.c_str(), buf.size(), m_selfUrl.c_str(), NULL, 0 );

        if ( NULL == doc )
            throw libcmis::Exception( "Failed to parse object infos" );
    }

    extractInfos( doc );
    
    if ( createdDoc )
        xmlFreeDoc( doc );
}

void AtomObjectType::extractInfos( xmlDocPtr doc )
{
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );

    // Register the Service Document namespaces
    libcmis::registerNamespaces( xpathCtx );

    if ( NULL != xpathCtx )
    {
        // Get the self URL
        string selfUrlReq( "//atom:link[@rel='self']/attribute::href" );
        m_selfUrl = libcmis::getXPathValue( xpathCtx, selfUrlReq );
        
        // Get the children URL
        string childrenUrlReq( "//atom:link[@rel='down' and @type='application/atom+xml;type=feed']/attribute::href" );
        m_childrenUrl = libcmis::getXPathValue( xpathCtx, childrenUrlReq );
        
        // Get the cmisra:type node
        xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( "//cmisra:type" ), xpathCtx );
        if ( NULL != xpathObj && NULL != xpathObj->nodesetval && xpathObj->nodesetval->nodeNr )
        {
            xmlNodePtr typeNode = xpathObj->nodesetval->nodeTab[0];
            initializeFromNode( typeNode );
        }
        xmlXPathFreeObject( xpathObj );
    }
    xmlXPathFreeContext( xpathCtx );
}
