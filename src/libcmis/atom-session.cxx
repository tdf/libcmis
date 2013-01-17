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
#include <string>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "atom-document.hxx"
#include "atom-folder.hxx"
#include "atom-object-type.hxx"
#include "atom-session.hxx"
#include "xml-utils.hxx"

using namespace std;

AtomPubSession::AtomPubSession( string atomPubUrl, string repositoryId,
        string username, string password, bool verbose ) throw ( libcmis::Exception ) :
    BaseSession( atomPubUrl, repositoryId, username, password, verbose ),
    m_repository( )
{
    initialize( );
}

AtomPubSession::AtomPubSession( const AtomPubSession& copy ) :
    BaseSession( copy ),
    m_repository( copy.m_repository )
{
}


AtomPubSession& AtomPubSession::operator=( const AtomPubSession& copy )
{
    if ( this != &copy )
    {
        BaseSession::operator=( copy );
        m_repository = copy.m_repository;
    }
    
    return *this;
}

AtomPubSession::~AtomPubSession( )
{
}

void AtomPubSession::initialize( ) throw ( libcmis::Exception )
{
    if ( m_repositories.empty() )
    {
        // Pull the content from sAtomPubUrl
        string buf;
        try
        {
            buf = httpGetRequest( m_bindingUrl )->getStream( )->str( );
        }
        catch ( const CurlException& e )
        {
            throw e.getCmisException( );
        }
       
        // parse the content
        xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), m_bindingUrl.c_str(), NULL, 0 );

        if ( NULL != doc )
        {
            // Check that we have an AtomPub service document
            xmlNodePtr root = xmlDocGetRootElement( doc );
            if ( !xmlStrEqual( root->name, BAD_CAST( "service" ) ) )
                throw libcmis::Exception( "Not an atompub service document" );

            xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );

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

                            // SharePoint is case insensitive for the id...
                            if ( libcmis::tolower( ws->getId( ) ) == libcmis::tolower( m_repositoryId ) )
                                m_repository = ws;

                            m_repositories.push_back( ws );
                        }
                        catch ( const libcmis::Exception& )
                        {
                            // Invalid repository, don't take care of this
                        }
                    }
                }
            }
            xmlXPathFreeContext( xpathCtx );
        }
        else
            throw libcmis::Exception( "Failed to parse service document" );

        xmlFreeDoc( doc );
    }

}

list< libcmis::RepositoryPtr > AtomPubSession::getRepositories( string url, string username,
        string password, bool verbose ) throw ( libcmis::Exception )
{
    AtomPubSession session( url, string(), username, password, verbose );
    return session.m_repositories;
}

AtomRepositoryPtr AtomPubSession::getAtomRepository( ) throw ( libcmis::Exception )
{
    return m_repository;
}

libcmis::RepositoryPtr AtomPubSession::getRepository( ) throw ( libcmis::Exception )
{
    return getAtomRepository( );
}

libcmis::ObjectPtr AtomPubSession::createObjectFromEntryDoc( xmlDocPtr doc )
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
                if ( baseType == "cmis:folder" )
                {
                    cmisObject.reset( new AtomFolder( this, node ) );
                }
                else if ( baseType == "cmis:document" )
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

libcmis::ObjectPtr AtomPubSession::getObject( string id ) throw ( libcmis::Exception )
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
             ( string::npos != e.getErrorMessage( ).find( "404" ) ) )
        {
            string msg = "No such node: ";
            msg += id;
            throw libcmis::Exception( msg );
        }
        else
            throw e.getCmisException();
    }
}

libcmis::ObjectPtr AtomPubSession::getObjectByPath( string path ) throw ( libcmis::Exception )
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
             ( string::npos != e.getErrorMessage( ).find( "404" ) ) )
        {
            string msg = "No node corresponding to path: ";
            msg += path;
            throw libcmis::Exception( msg );
        }
        else
            throw e.getCmisException();
    }
}

libcmis::ObjectTypePtr AtomPubSession::getType( string id ) throw ( libcmis::Exception )
{
    libcmis::ObjectTypePtr type( new AtomObjectType( this, id ) );
    return type;
}
