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
#include "atom-session.hxx"
#include "atom-utils.hxx"

using namespace std;

string UriTemplate::createUrl( const string& pattern, map< string, string > variables )
{
    string url( pattern );

    // Decompose the pattern and replace the variables by their values
    map< string, string >::iterator it = variables.begin( );
    while ( it != variables.end( ) )
    {
        string name = "{";
        name += it->first;
        name += "}";
        string value = it->second;

        // Search and replace the variable
        size_t pos = url.find( name );
        if ( pos != string::npos )
            url.replace( pos, name.size(), value );

        ++it;
    }

    // Cleanup the remaining unset variables
    size_t pos1 = url.find( '{' );
    while ( pos1 != string::npos )
    {
        // look for the closing bracket
        size_t pos2 = url.find( '}', pos1 );
        if ( pos2 != string::npos )
            url.erase( pos1, pos2 - pos1 + 1 );

        pos1 = url.find( '{', pos1 - 1 );
    }
    
    return url;
}

AtomPubSession::AtomPubSession( string atomPubUrl, string repository, string username, string password ) throw ( libcmis::Exception ) :
    Session( ),
    m_sAtomPubUrl( atomPubUrl ),
    m_sRepository( repository ),
    m_username( username ),
    m_password( password ),
    m_sRootId( ),
    m_aCollections( ),
    m_aUriTemplates( )
{
    // Pull the content from sAtomPubUrl and parse it
    string buf = atom::httpGetRequest( m_sAtomPubUrl, m_username, m_password );
    
    xmlDocPtr pDoc = xmlReadMemory( buf.c_str(), buf.size(), m_sAtomPubUrl.c_str(), NULL, 0 );

    if ( NULL != pDoc )
    {
        xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( pDoc );

        // Register the Service Document namespaces
        atom::registerNamespaces( pXPathCtx );

        if ( NULL != pXPathCtx )
        {
            // Get the collections
            xmlXPathObjectPtr pXPathObj = xmlXPathEvalExpression( BAD_CAST( "//app:collection" ), pXPathCtx );
            if ( NULL != pXPathObj )
                readCollections( pXPathObj->nodesetval );
            xmlXPathFreeObject( pXPathObj );

            // Get the URI templates
            pXPathObj = xmlXPathEvalExpression( BAD_CAST( "//cmisra:uritemplate" ), pXPathCtx );
            if ( NULL != pXPathObj )
                readUriTemplates( pXPathObj->nodesetval );
            xmlXPathFreeObject( pXPathObj );
            
            // Get the root node id
            string infosXPath( "//cmisra:repositoryInfo[cmis:repositoryId='" );
            infosXPath += m_sRepository;
            infosXPath += "']/cmis:rootFolderId/text()";
            m_sRootId = atom::getXPathValue( pXPathCtx, infosXPath );
        }
        xmlXPathFreeContext( pXPathCtx );
    }
    else
    {
        fprintf( stderr, "Failed to parse service document\n" );
    }

    xmlFreeDoc( pDoc );
}

AtomPubSession::~AtomPubSession( )
{
}

list< string > AtomPubSession::getRepositories( string url, string username, string password ) throw ( libcmis::Exception )
{
    list< string > repos;

    // Parse the service document and get the workspaces
    string buf = atom::httpGetRequest( url, username, password );
   
    xmlDocPtr pDoc = xmlReadMemory( buf.c_str(), buf.size(), url.c_str(), NULL, 0 );
    if ( NULL != pDoc )
    {
        xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( pDoc );

        // Register the Service Document namespaces
        atom::registerNamespaces( pXPathCtx );

        if ( NULL != pXPathCtx )
        {
            xmlXPathObjectPtr pXPathObj = xmlXPathEvalExpression( BAD_CAST( "//cmis:repositoryId/text()" ), pXPathCtx );
            if ( NULL != pXPathObj )
            {
                int size = 0;
                if ( pXPathObj->nodesetval )
                    size = pXPathObj->nodesetval->nodeNr;
                
                for ( int i = 0; i < size; i++ )
                {
                    xmlNodePtr pNode = pXPathObj->nodesetval->nodeTab[i];
                    string workspaceId( ( char* )pNode->content );
                    repos.push_back( workspaceId );
                }
            }

            xmlXPathFreeObject( pXPathObj );
        }
        xmlXPathFreeContext( pXPathCtx );
        
    }
    else
    {
        fprintf( stderr, "Failed to parse service document\n" );
    }
    xmlFreeDoc( pDoc );

    return repos;
}

string AtomPubSession::getCollectionUrl( Collection::Type type )
{
    return m_aCollections[ type ];
}

string AtomPubSession::getUriTemplate( UriTemplate::Type type )
{
    return m_aUriTemplates[ type ];
}

libcmis::FolderPtr AtomPubSession::getRootFolder()
{
    return getFolder( m_sRootId );
}

libcmis::ObjectPtr AtomPubSession::createObjectFromEntryDoc( xmlDocPtr doc )
{
    libcmis::ObjectPtr cmisObject;

    if ( NULL != doc )
    {
        // Get the atom:entry node
        xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( doc );
        atom::registerNamespaces( pXPathCtx );
        if ( NULL != pXPathCtx )
        {
            const string& entriesReq( "//atom:entry" );
            xmlXPathObjectPtr pXPathObj = xmlXPathEvalExpression( BAD_CAST( entriesReq.c_str() ), pXPathCtx );

            if ( NULL != pXPathObj && NULL != pXPathObj->nodesetval && ( 0 < pXPathObj->nodesetval->nodeNr ) )
            {
                xmlNodePtr node = pXPathObj->nodesetval->nodeTab[0];
                if ( !AtomFolder::getChildrenUrl( doc ).empty() )
                {
                    libcmis::ObjectPtr folder( new AtomFolder( this, node ) );
                    cmisObject.swap( folder );
                }
                else
                {
                    libcmis::ObjectPtr content( new AtomDocument( this, node ) );
                    cmisObject.swap( content );
                }
            }
            xmlXPathFreeObject( pXPathObj );
        }
        xmlXPathFreeContext( pXPathCtx );
    }

    return cmisObject;
}

libcmis::ObjectPtr AtomPubSession::getObject( string id ) throw ( libcmis::Exception )
{
    string pattern = getUriTemplate( UriTemplate::ObjectById );
    map< string, string > vars;
    vars[URI_TEMPLATE_VAR_ID] = id;
    string url = UriTemplate::createUrl( pattern, vars );

    string buf = atom::httpGetRequest( url, m_username, m_password );
    xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), url.c_str(), NULL, 0 );
    libcmis::ObjectPtr cmisObject = createObjectFromEntryDoc( doc );
    xmlFreeDoc( doc );

    return cmisObject;
}

void AtomPubSession::readCollections( xmlNodeSetPtr pNodeSet )
{
    int size = 0;
    if ( pNodeSet )
        size = pNodeSet->nodeNr;

    for ( int i = 0; i < size; i++ )
    {
        xmlNodePtr pNode = pNodeSet->nodeTab[i];

        // Look for the href property
        xmlChar* pHref = xmlGetProp( pNode, BAD_CAST( "href" ) );
        if ( pHref )
        {
            string collectionRef( ( char* )pHref );
            xmlFree( pHref );

            // Look for the cmisra:collectionType child
            for ( xmlNodePtr pChild = pNode->children; pChild; pChild = pChild->next )
            {
                // SharePoint CMIS implementation doesn't follow the spec:
                // the cmisra namespace is omitted
                bool isCollectionType = xmlStrEqual( pChild->name, BAD_CAST( "collectionType" ) );
                if ( isCollectionType )
                {
                    xmlChar* pContent = xmlNodeGetContent( pChild );
                    Collection::Type type = Collection::Root;
                    bool typeDefined = false;

                    if ( xmlStrEqual( pContent, BAD_CAST( "root" ) ) )
                    {
                        type = Collection::Root;
                        typeDefined = true;
                    }
                    else if ( xmlStrEqual( pContent, BAD_CAST( "types" ) ) )
                    {
                        type = Collection::Types;
                        typeDefined = true;
                    }
                    else if ( xmlStrEqual( pContent, BAD_CAST( "query" ) ) )
                    {
                        type = Collection::Query;
                        typeDefined = true;
                    }
                    else if ( xmlStrEqual( pContent, BAD_CAST( "checkedout" ) ) )
                    {
                        type = Collection::CheckedOut;
                        typeDefined = true;
                    }
                    else if ( xmlStrEqual( pContent, BAD_CAST( "unfiled" ) ) )
                    {
                        type = Collection::Unfiled;
                        typeDefined = true;
                    }

                    if ( typeDefined )
                        m_aCollections[ type ] = collectionRef;

                    xmlFree( pContent );
                }
            }
        }
    }
}

void AtomPubSession::readUriTemplates( xmlNodeSetPtr pNodeSet )
{
    int size = 0;
    if ( pNodeSet )
        size = pNodeSet->nodeNr;

    for ( int i = 0; i < size; i++ )
    {
        xmlNodePtr pNode = pNodeSet->nodeTab[i];

        string templateUri;
        UriTemplate::Type type = UriTemplate::ObjectById;
        bool typeDefined = false;

        // Look for the cmisra:template and cmisra:type children
        for ( xmlNodePtr pChild = pNode->children; pChild; pChild = pChild->next )
        {
            bool bIsTemplate = xmlStrEqual( pChild->name, BAD_CAST( "template" ) );
            bool bIsType = xmlStrEqual( pChild->name, BAD_CAST( "type" ) );

            if ( bIsTemplate )
            {
                xmlChar* pContent = xmlNodeGetContent( pChild );
                templateUri = string( ( char * )pContent );
                xmlFree( pContent );
            }
            else if ( bIsType )
            {
                xmlChar* pContent = xmlNodeGetContent( pChild );
                if ( xmlStrEqual( pContent, BAD_CAST( "objectbyid" ) ) )
                {
                    type = UriTemplate::ObjectById;
                    typeDefined = true;
                }
                else if ( xmlStrEqual( pContent, BAD_CAST( "objectbypath" ) ) )
                {
                    type = UriTemplate::ObjectByPath;
                    typeDefined = true;
                }
                else if ( xmlStrEqual( pContent, BAD_CAST( "query" ) ) )
                {
                    type = UriTemplate::Query;
                    typeDefined = true;
                }
                else if ( xmlStrEqual( pContent, BAD_CAST( "typebyid" ) ) )
                {
                    type = UriTemplate::TypeById;
                    typeDefined = true;
                }
                xmlFree( pContent );
            }
        }

        if ( !templateUri.empty() && typeDefined )
            m_aUriTemplates[ type ] = templateUri;
    }
}

libcmis::FolderPtr AtomPubSession::getFolder( string id )
{
    libcmis::ObjectPtr object = getObject( id );
    libcmis::FolderPtr folder = boost::dynamic_pointer_cast< libcmis::Folder >( object );
    return folder;
}
