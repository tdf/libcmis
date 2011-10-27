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

#include "atom-folder.hxx"
#include "atom-session.hxx"
#include "atom-utils.hxx"

using namespace std;

namespace
{
}

AtomFolder::AtomFolder( AtomPubSession* session, string url ) :
    AtomObject( session, url ),
    m_path( ),
    m_childrenUrl( )
{
    refresh();
}

AtomFolder::AtomFolder( AtomPubSession* session, xmlNodePtr entryNd ) :
    AtomObject( session, string() ),
    m_path( ),
    m_childrenUrl( )
{
    xmlDocPtr doc = atom::wrapInDoc( entryNd );
    refreshImpl( doc );
    xmlFreeDoc( doc );
}


AtomFolder::~AtomFolder( )
{
}

vector< libcmis::ObjectPtr > AtomFolder::getChildren( ) throw ( libcmis::Exception )
{
    vector< libcmis::ObjectPtr > children;
    
    string buf = getSession()->httpGetRequest( m_childrenUrl );

    xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), m_childrenUrl.c_str(), NULL, 0 );
    if ( NULL != doc )
    {
        xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( doc );
        atom::registerNamespaces( pXPathCtx );
        if ( NULL != pXPathCtx )
        {
            const string& entriesReq( "//atom:entry" );
            xmlXPathObjectPtr pXPathObj = xmlXPathEvalExpression( BAD_CAST( entriesReq.c_str() ), pXPathCtx );

            if ( NULL != pXPathObj && NULL != pXPathObj->nodesetval )
            {
                int size = pXPathObj->nodesetval->nodeNr;
                for ( int i = 0; i < size; i++ )
                {
                    xmlNodePtr node = pXPathObj->nodesetval->nodeTab[i];
                    xmlDocPtr entryDoc = atom::wrapInDoc( node );
                    libcmis::ObjectPtr cmisObject = getSession()->createObjectFromEntryDoc( entryDoc );

                    if ( cmisObject.get() )
                        children.push_back( cmisObject );
                    xmlFreeDoc( entryDoc );
                }
            }

            xmlXPathFreeObject( pXPathObj );
        }

        xmlXPathFreeContext( pXPathCtx );
    }
    else
    {
        fprintf( stderr, "Failed to parse folder infos\n" );
    }
    xmlFreeDoc( doc );

    return children;
}

string AtomFolder::getPath( )
{
    return m_path;
}

string AtomFolder::toString( )
{
    stringstream buf;

    buf << "Folder Object:" << endl << endl;
    buf << AtomObject::toString();
    buf << "Path: " << getPath() << endl;
    buf << "Children [Name (Id)]:" << endl;

    vector< libcmis::ObjectPtr > children = getChildren( );
    for ( vector< libcmis::ObjectPtr >::iterator it = children.begin( );
            it != children.end(); it++ )
    {
        libcmis::ObjectPtr child = *it;
        buf << "    " << child->getName() << " (" << child->getId() << ")" << endl;
    }

    return buf.str();
}

void AtomFolder::extractInfos( xmlDocPtr doc )
{
    AtomObject::extractInfos( doc );
    m_childrenUrl = AtomFolder::getChildrenUrl( doc );

    xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( doc );

    // Register the Service Document namespaces
    atom::registerNamespaces( pXPathCtx );

    if ( NULL != pXPathCtx )
    {
        // Get the path
        string pathReq( "//cmis:propertyString[@propertyDefinitionId='cmis:path']/cmis:value/text()" );
        m_path = atom::getXPathValue( pXPathCtx, pathReq );
    }
    xmlXPathFreeContext( pXPathCtx );
}

string AtomFolder::getChildrenUrl( xmlDocPtr doc )
{
    string childrenUrl;

    xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( doc );
    atom::registerNamespaces( pXPathCtx );

    if ( NULL != pXPathCtx )
    {
        // Get the children collection url
        string downReq( "//atom:link[@rel='down' and @type='application/atom+xml;type=feed']//attribute::href" );
        childrenUrl = atom::getXPathValue( pXPathCtx, downReq );
    }
    xmlXPathFreeContext( pXPathCtx );

    return childrenUrl;
}
