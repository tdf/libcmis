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
 * Copyright (C) 2011 Cédric Bosdonnat <cbosdo@users.sourceforge.net>
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

#include "atom-workspace.hxx"
#include "xml-utils.hxx"

using namespace std;

AtomRepository::AtomRepository( xmlNodePtr wsNode ):
    Repository( ),
    m_collections( ),
    m_uriTemplates( )
{
    if ( wsNode != NULL )
    {
        xmlDocPtr doc = libcmis::wrapInDoc( wsNode );
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
        libcmis::registerNamespaces( xpathCtx );

        if ( NULL != xpathCtx )
        {
            // Get the collections
            xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( "//app:collection" ), xpathCtx );
            if ( NULL != xpathObj )
                readCollections( xpathObj->nodesetval );
            xmlXPathFreeObject( xpathObj );

            // Get the URI templates
            xpathObj = xmlXPathEvalExpression( BAD_CAST( "//cmisra:uritemplate" ), xpathCtx );
            if ( NULL != xpathObj )
                readUriTemplates( xpathObj->nodesetval );
            xmlXPathFreeObject( xpathObj );

            // Get the repository infos 
            xpathObj = xmlXPathEvalExpression( BAD_CAST( "//cmisra:repositoryInfo" ), xpathCtx );
            if ( NULL != xpathObj )
                initializeFromNode( xpathObj->nodesetval->nodeTab[0] );
            xmlXPathFreeObject( xpathObj );
            
        }
        xmlXPathFreeContext( xpathCtx );
        xmlFreeDoc( doc );
    }
}

AtomRepository::AtomRepository( const AtomRepository& rCopy ) :
    Repository( rCopy ),
    m_collections( rCopy.m_collections ),
    m_uriTemplates( rCopy.m_uriTemplates )
{
}

AtomRepository::~AtomRepository( )
{
    m_collections.clear( );
    m_uriTemplates.clear( );
}

AtomRepository& AtomRepository::operator= ( const AtomRepository& rCopy )
{
    if ( this != &rCopy )
    {
        m_collections = rCopy.m_collections;
        m_uriTemplates = rCopy.m_uriTemplates;
    }

    return *this;
}

string AtomRepository::getCollectionUrl( Collection::Type type )
{
    return m_collections[ type ];
}

string AtomRepository::getUriTemplate( UriTemplate::Type type )
{
    return m_uriTemplates[ type ];
}

void AtomRepository::readCollections( xmlNodeSetPtr nodeSet )
{
    int size = 0;
    if ( nodeSet )
        size = nodeSet->nodeNr;

    for ( int i = 0; i < size; i++ )
    {
        xmlNodePtr node = nodeSet->nodeTab[i];

        // Look for the href property
        xmlChar* href = xmlGetProp( node, BAD_CAST( "href" ) );
        if ( href )
        {
            string collectionRef( ( char* )href );
            xmlFree( href );

            // Look for the cmisra:collectionType child
            for ( xmlNodePtr child = node->children; child; child = child->next )
            {
                // SharePoint CMIS implementation doesn't follow the spec:
                // the cmisra namespace is omitted
                bool isCollectionType = xmlStrEqual( child->name, BAD_CAST( "collectionType" ) );
                if ( isCollectionType )
                {
                    xmlChar* content = xmlNodeGetContent( child );
                    Collection::Type type = Collection::Root;
                    bool typeDefined = false;

                    if ( xmlStrEqual( content, BAD_CAST( "root" ) ) )
                    {
                        type = Collection::Root;
                        typeDefined = true;
                    }
                    else if ( xmlStrEqual( content, BAD_CAST( "types" ) ) )
                    {
                        type = Collection::Types;
                        typeDefined = true;
                    }
                    else if ( xmlStrEqual( content, BAD_CAST( "query" ) ) )
                    {
                        type = Collection::Query;
                        typeDefined = true;
                    }
                    else if ( xmlStrEqual( content, BAD_CAST( "checkedout" ) ) )
                    {
                        type = Collection::CheckedOut;
                        typeDefined = true;
                    }
                    else if ( xmlStrEqual( content, BAD_CAST( "unfiled" ) ) )
                    {
                        type = Collection::Unfiled;
                        typeDefined = true;
                    }

                    if ( typeDefined )
                        m_collections[ type ] = collectionRef;

                    xmlFree( content );
                }
            }
        }
    }
}

void AtomRepository::readUriTemplates( xmlNodeSetPtr nodeSet )
{
    int size = 0;
    if ( nodeSet )
        size = nodeSet->nodeNr;

    for ( int i = 0; i < size; i++ )
    {
        xmlNodePtr node = nodeSet->nodeTab[i];

        string templateUri;
        UriTemplate::Type type = UriTemplate::ObjectById;
        bool typeDefined = false;

        // Look for the cmisra:template and cmisra:type children
        for ( xmlNodePtr child = node->children; child; child = child->next )
        {
            bool isTemplate = xmlStrEqual( child->name, BAD_CAST( "template" ) );
            bool isType = xmlStrEqual( child->name, BAD_CAST( "type" ) );

            if ( isTemplate )
            {
                xmlChar* content = xmlNodeGetContent( child );
                if ( content != NULL )
                    templateUri = string( ( char * )content );
                xmlFree( content );
            }
            else if ( isType )
            {
                xmlChar* content = xmlNodeGetContent( child );
                if ( xmlStrEqual( content, BAD_CAST( "objectbyid" ) ) )
                {
                    type = UriTemplate::ObjectById;
                    typeDefined = true;
                }
                else if ( xmlStrEqual( content, BAD_CAST( "objectbypath" ) ) )
                {
                    type = UriTemplate::ObjectByPath;
                    typeDefined = true;
                }
                else if ( xmlStrEqual( content, BAD_CAST( "query" ) ) )
                {
                    type = UriTemplate::Query;
                    typeDefined = true;
                }
                else if ( xmlStrEqual( content, BAD_CAST( "typebyid" ) ) )
                {
                    type = UriTemplate::TypeById;
                    typeDefined = true;
                }
                xmlFree( content );
            }
        }

        if ( !templateUri.empty() && typeDefined )
            m_uriTemplates[ type ] = templateUri;
    }
}
