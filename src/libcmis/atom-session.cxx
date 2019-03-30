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

#include "atom-session.hxx"

#include <string>

#include <boost/algorithm/string.hpp>
#include <boost/shared_ptr.hpp>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include <libcmis/xml-utils.hxx>

#include "atom-document.hxx"
#include "atom-folder.hxx"
#include "atom-object-type.hxx"

using namespace std;

AtomPubSession::AtomPubSession( string atomPubUrl, string repositoryId,
        string username, string password, bool noSslCheck,
        libcmis::OAuth2DataPtr oauth2, bool verbose ) :
    BaseSession( atomPubUrl, repositoryId, username, password, noSslCheck, oauth2, verbose ),
    m_repository( )
{
    libcmis::HttpResponsePtr response;
    initialize( response );
}

AtomPubSession::AtomPubSession( string atomPubUrl, string repositoryId,
        const HttpSession& httpSession, libcmis::HttpResponsePtr response ) :
    BaseSession( atomPubUrl, repositoryId, httpSession ),
    m_repository( )
{
    initialize( response );
}

AtomPubSession::AtomPubSession( ) :
    BaseSession( ),
    m_repository( )
{
}

AtomPubSession::~AtomPubSession( )
{
}

void AtomPubSession::parseServiceDocument( const string& buf )
{
    // parse the content
    const boost::shared_ptr< xmlDoc > doc( xmlReadMemory( buf.c_str(), buf.size(), m_bindingUrl.c_str(), NULL, 0 ), xmlFreeDoc );

    if ( bool( doc ) )
    {
        // Check that we have an AtomPub service document
        xmlNodePtr root = xmlDocGetRootElement( doc.get() );
        if ( !xmlStrEqual( root->name, BAD_CAST( "service" ) ) )
            throw libcmis::Exception( "Not an atompub service document" );

        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc.get() );

        // Register the Service Document namespaces
        libcmis::registerNamespaces( xpathCtx );

        if ( NULL != xpathCtx )
        {
            string workspacesXPath( "//app:workspace" );
            xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( workspacesXPath.c_str() ), xpathCtx );

            if ( xpathObj != NULL )
            {
                int nbWorkspaces = 0;
                if ( xpathObj->nodesetval )
                    nbWorkspaces = xpathObj->nodesetval->nodeNr;

                for ( int i = 0; i < nbWorkspaces; i++ )
                {
                    try
                    {
                        AtomRepositoryPtr ws( new AtomRepository( xpathObj->nodesetval->nodeTab[i] ) );

                        // Check if we have a repository set
                        if ( m_repositoryId.empty( ) && i == 0 )
                            m_repositoryId = ws->getId( );

                        // SharePoint is case insensitive for the id...
                        if ( boost::to_lower_copy( ws->getId( ) ) == boost::to_lower_copy( m_repositoryId ) )
                            m_repository = ws;

                        m_repositories.push_back( ws );
                    }
                    catch ( const libcmis::Exception& )
                    {
                        // Invalid repository, don't take care of this
                    }
                }
            }
            xmlXPathFreeObject( xpathObj );
        }
        xmlXPathFreeContext( xpathCtx );
    }
    else
        throw libcmis::Exception( "Failed to parse service document" );
}

void AtomPubSession::initialize( libcmis::HttpResponsePtr response )
{
    if ( m_repositories.empty() )
    {
        // Pull the content from sAtomPubUrl
        string buf;
        if ( response )
        {
            buf = response->getStream( )->str( );
        }
        else
        {
            try
            {
                buf = httpGetRequest( m_bindingUrl )->getStream( )->str( );
            }
            catch ( const CurlException& e )
            {
                throw e.getCmisException( );
            }
        }

        parseServiceDocument( buf );
    }
}

AtomRepositoryPtr AtomPubSession::getAtomRepository( )
{
    return m_repository;
}

libcmis::RepositoryPtr AtomPubSession::getRepository( )
{
    return getAtomRepository( );
}

bool AtomPubSession::setRepository( string repositoryId )
{
    vector< libcmis::RepositoryPtr > repos = getRepositories( );
    bool found = false;
    for ( vector< libcmis::RepositoryPtr >::iterator it = repos.begin();
            it != repos.end() && !found; ++it )
    {
        libcmis::RepositoryPtr repo = *it;
        if ( repo->getId() == repositoryId )
        {
            AtomRepositoryPtr atomRepo = boost::dynamic_pointer_cast< AtomRepository >( repo );
            m_repository = atomRepo;
            m_repositoryId = repositoryId;
            found = true;
        }
    }
    return found;
}

libcmis::ObjectPtr AtomPubSession::createObjectFromEntryDoc( xmlDocPtr doc, ResultObjectType res )
{
    libcmis::ObjectPtr cmisObject;

    if ( NULL != doc )
    {
        // Get the atom:entry node
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
        libcmis::registerNamespaces( xpathCtx );
        if ( NULL != xpathCtx )
        {
            const string& entriesReq( "//atom:entry" );
            xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( entriesReq.c_str() ), xpathCtx );

            if ( NULL != xpathObj && NULL != xpathObj->nodesetval && ( 0 < xpathObj->nodesetval->nodeNr ) )
            {
                // Get the entry's base type
                string baseTypeReq = "//atom:entry[1]//cmis:propertyId[@propertyDefinitionId='cmis:baseTypeId']/cmis:value/text()";
                string baseType = libcmis::getXPathValue( xpathCtx, baseTypeReq );

                xmlNodePtr node = xpathObj->nodesetval->nodeTab[0];
                if ( res == RESULT_FOLDER || baseType == "cmis:folder" )
                {
                    cmisObject.reset( new AtomFolder( this, node ) );
                }
                else if ( res == RESULT_DOCUMENT || baseType == "cmis:document" )
                {
                    cmisObject.reset( new AtomDocument( this, node ) );
                }
                else
                {
                    // Not a valid CMIS atom entry... weird
                }
            }
            xmlXPathFreeObject( xpathObj );
        }
        xmlXPathFreeContext( xpathCtx );
    }

    return cmisObject;
}

libcmis::ObjectPtr AtomPubSession::getObject( string id )
{
    string pattern = getAtomRepository()->getUriTemplate( UriTemplate::ObjectById );
    map< string, string > vars;
    vars[URI_TEMPLATE_VAR_ID] = id;
    vars[string( "includeAllowableActions" )] = string( "true" );
    string url = createUrl( pattern, vars );

    try
    {
        string buf = httpGetRequest( url )->getStream( )->str( );
        xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), url.c_str(), NULL, 0 );
        libcmis::ObjectPtr cmisObject = createObjectFromEntryDoc( doc );
        xmlFreeDoc( doc );
        return cmisObject;
    }
    catch ( const CurlException& e )
    {
        if ( ( e.getErrorCode( ) == CURLE_HTTP_RETURNED_ERROR ) &&
             ( e.getHttpStatus( ) == 404 ) )
        {
            string msg = "No such node: ";
            msg += id;
            throw libcmis::Exception( msg, "objectNotFound" );
        }
        else
            throw e.getCmisException();
    }
}

libcmis::ObjectPtr AtomPubSession::getObjectByPath( string path )
{
    string pattern = getAtomRepository()->getUriTemplate( UriTemplate::ObjectByPath );
    map< string, string > vars;
    vars[URI_TEMPLATE_VAR_PATH] = path;
    vars[string( "includeAllowableActions" )] = string( "true" );
    string url = createUrl( pattern, vars );

    try
    {
        string buf = httpGetRequest( url )->getStream( )->str( );
        xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), url.c_str(), NULL, 0 );
        libcmis::ObjectPtr cmisObject = createObjectFromEntryDoc( doc );
        xmlFreeDoc( doc );
        return cmisObject;
    }
    catch ( const CurlException& e )
    {
        if ( ( e.getErrorCode( ) == CURLE_HTTP_RETURNED_ERROR ) &&
             ( e.getHttpStatus( ) == 404 ) )
        {
            string msg = "No node corresponding to path: ";
            msg += path;
            throw libcmis::Exception( msg, "objectNotFound" );
        }
        else
            throw e.getCmisException();
    }
}

libcmis::ObjectTypePtr AtomPubSession::getType( string id )
{
    libcmis::ObjectTypePtr type( new AtomObjectType( this, id ) );
    return type;
}

vector< libcmis::ObjectTypePtr > AtomPubSession::getBaseTypes( )
{
    string url = getAtomRepository( )->getCollectionUrl( Collection::Types );
    return getChildrenTypes( url );
}

vector< libcmis::ObjectTypePtr > AtomPubSession::getChildrenTypes( string url )
{
    vector< libcmis::ObjectTypePtr > children;
    string buf;
    try
    {
        buf = httpGetRequest( url )->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), url.c_str(), NULL, 0 );
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
                    libcmis::ObjectTypePtr type( new AtomObjectType( this, node ) );
                    children.push_back( type );
                }
            }

            xmlXPathFreeObject( xpathObj );
        }

        xmlXPathFreeContext( xpathCtx );
    }
    else
    {
        throw libcmis::Exception( "Failed to parse type children infos" );
    }
    xmlFreeDoc( doc );

    return children;
}
