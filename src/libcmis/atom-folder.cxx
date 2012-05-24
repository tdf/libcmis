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

#include "atom-document.hxx"
#include "atom-folder.hxx"
#include "atom-session.hxx"
#include "atom-utils.hxx"

using namespace std;

namespace
{
}

AtomFolder::AtomFolder( AtomPubSession* session, xmlNodePtr entryNd ) :
    AtomObject( session ),
    m_path( ),
    m_childrenUrl( ),
    m_parentId( )
{
    xmlDocPtr doc = atom::wrapInDoc( entryNd );
    refreshImpl( doc );
    xmlFreeDoc( doc );
}


AtomFolder::~AtomFolder( )
{
}

libcmis::FolderPtr AtomFolder::getFolderParent( ) throw ( libcmis::Exception )
{
    if ( getAllowableActions( ).get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::GetFolderParent ) )
        throw libcmis::Exception( string( "GetFolderParent not allowed on node " ) + getId() );

    return getSession()->getFolder( m_parentId ); 
}

vector< libcmis::ObjectPtr > AtomFolder::getChildren( ) throw ( libcmis::Exception )
{
    vector< libcmis::ObjectPtr > children;

    if ( getAllowableActions( ).get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::GetChildren ) )
        throw libcmis::Exception( string( "GetChildren not allowed on node " ) + getId() );

    string buf;
    try
    {
        buf = getSession()->httpGetRequest( m_childrenUrl );
    }
    catch ( const atom::CurlException& e )
    {
        throw e.getCmisException( );
    }

    xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), m_childrenUrl.c_str(), NULL, 0 );
    if ( NULL != doc )
    {
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
        atom::registerNamespaces( xpathCtx );
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
                    xmlDocPtr entryDoc = atom::wrapInDoc( node );
                    libcmis::ObjectPtr cmisObject = getSession()->createObjectFromEntryDoc( entryDoc );

                    if ( cmisObject.get() )
                        children.push_back( cmisObject );
                    xmlFreeDoc( entryDoc );
                }
            }

            xmlXPathFreeObject( xpathObj );
        }

        xmlXPathFreeContext( xpathCtx );
    }
    else
    {
        throw new libcmis::Exception( "Failed to parse folder infos" );
    }
    xmlFreeDoc( doc );

    return children;
}

string AtomFolder::getPath( )
{
    return m_path;
}

bool AtomFolder::isRootFolder( )
{
    return m_parentId.empty( );
}

libcmis::FolderPtr AtomFolder::createFolder( map< string, libcmis::PropertyPtr >& properties )
{
    if ( getAllowableActions( ).get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::CreateFolder ) )
        throw libcmis::Exception( string( "CreateFolder not allowed on folder " ) + getId() );

    // Actually create the folder
    AtomObject object( getSession() );
    object.getProperties( ).swap( properties );
    
    xmlBufferPtr buf = xmlBufferCreate( );
    xmlTextWriterPtr writer = xmlNewTextWriterMemory( buf, 0 );

    xmlTextWriterStartDocument( writer, NULL, NULL, NULL );

    // Copy and remove the readonly properties before serializing
    object.toXml( writer );

    xmlTextWriterEndDocument( writer );
    string str( ( const char * )xmlBufferContent( buf ) );
    istringstream is( str );

    xmlFreeTextWriter( writer );
    xmlBufferFree( buf );

    string respBuf;
    try
    {
        respBuf = getSession( )->httpPostRequest( m_childrenUrl, is, "application/atom+xml;type=entry" );
    }
    catch ( const atom::CurlException& e )
    {
        throw e.getCmisException( );
    }

    xmlDocPtr doc = xmlReadMemory( respBuf.c_str(), respBuf.size(), getInfosUrl().c_str(), NULL, 0 );
    if ( NULL == doc )
        throw libcmis::Exception( "Failed to parse object infos" );

    libcmis::ObjectPtr created = getSession( )->createObjectFromEntryDoc( doc );
    xmlFreeDoc( doc );

    libcmis::FolderPtr newFolder = boost::dynamic_pointer_cast< libcmis::Folder >( created );
    if ( !newFolder.get( ) )
        throw libcmis::Exception( string( "Created object is not a folder: " ) + created->getId( ) );

    return newFolder;
}

libcmis::DocumentPtr AtomFolder::createDocument( map< string, libcmis::PropertyPtr >& properties,
        boost::shared_ptr< ostream > os, string contentType ) throw ( libcmis::Exception )
{
    if ( getAllowableActions( ).get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::CreateDocument ) )
        throw libcmis::Exception( string( "CreateDocument not allowed on folder " ) + getId() );

    // Actually create the document
    AtomDocument document( getSession() );
    document.getProperties( ).swap( properties );
    document.setContentStream( os, contentType );
   
    xmlBufferPtr buf = xmlBufferCreate( );
    xmlTextWriterPtr writer = xmlNewTextWriterMemory( buf, 0 );

    xmlTextWriterStartDocument( writer, NULL, NULL, NULL );

    // Copy and remove the readonly properties before serializing
    document.toXml( writer );

    xmlTextWriterEndDocument( writer );
    string str( ( const char * )xmlBufferContent( buf ) );
    istringstream is( str );

    xmlFreeTextWriter( writer );
    xmlBufferFree( buf );

    string respBuf;
    try
    {
        respBuf = getSession( )->httpPostRequest( m_childrenUrl, is, "application/atom+xml;type=entry" );
    }
    catch ( const atom::CurlException& e )
    {
        throw e.getCmisException( );
    }

    xmlDocPtr doc = xmlReadMemory( respBuf.c_str(), respBuf.size(), getInfosUrl().c_str(), NULL, 0 );
    if ( NULL == doc )
        throw libcmis::Exception( "Failed to parse object infos" );

    libcmis::ObjectPtr created = getSession( )->createObjectFromEntryDoc( doc );
    xmlFreeDoc( doc );

    libcmis::DocumentPtr newDocument = boost::dynamic_pointer_cast< libcmis::Document >( created );
    if ( !newDocument.get( ) )
        throw libcmis::Exception( string( "Created object is not a document: " ) + created->getId( ) );

    return newDocument;
}

string AtomFolder::toString( )
{
    stringstream buf;

    buf << "Folder Object:" << endl << endl;
    buf << AtomObject::toString();
    buf << "Path: " << getPath() << endl;
    buf << "Folder Parent Id: " << m_parentId << endl;
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

    xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );

    // Register the Service Document namespaces
    atom::registerNamespaces( xpathCtx );

    if ( NULL != xpathCtx )
    {
        // Get the path
        string pathReq( "//cmis:propertyString[@propertyDefinitionId='cmis:path']/cmis:value/text()" );
        m_path = atom::getXPathValue( xpathCtx, pathReq );
        
        // Get the parent id
        string parentIdReq( "//cmis:propertyId[@propertyDefinitionId='cmis:parentId']/cmis:value/text()" );
        m_parentId = atom::getXPathValue( xpathCtx, parentIdReq );
    }
    xmlXPathFreeContext( xpathCtx );
}

string AtomFolder::getChildrenUrl( xmlDocPtr doc )
{
    string childrenUrl;

    xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
    atom::registerNamespaces( xpathCtx );

    if ( NULL != xpathCtx )
    {
        // Get the children collection url
        string downReq( "//atom:link[@rel='down' and @type='application/atom+xml;type=feed']/attribute::href" );
        childrenUrl = atom::getXPathValue( xpathCtx, downReq );
    }
    xmlXPathFreeContext( xpathCtx );

    return childrenUrl;
}
