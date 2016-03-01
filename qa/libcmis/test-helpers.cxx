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

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "test-helpers.hxx"
#include "xml-utils.hxx"

using namespace std;
using libcmis::PropertyPtrMap;

namespace test
{

    XmlNodeRef::XmlNodeRef( xmlNodePtr node, boost::shared_ptr< xmlDoc > doc )
        : m_node( node )
        , m_doc( doc )
    {
    }

    XmlNodeRef::XmlNodeRef( const XmlNodeRef& other )
        : m_node( other.m_node )
        , m_doc( other.m_doc )
    {
    }

    XmlNodeRef& XmlNodeRef::operator=( const XmlNodeRef& other )
    {
        m_node = other.m_node;
        m_doc = other.m_doc;
        return *this;
    }

    XmlNodeRef::operator xmlNodePtr( ) const
    {
        return m_node;
    }

    XmlNodeRef getXmlNode( string str )
    {
        xmlNodePtr node = NULL;
        const boost::shared_ptr< xmlDoc > doc( xmlReadMemory( str.c_str( ), str.size( ), "tester", NULL, 0 ), xmlFreeDoc );
        if ( bool( doc ) )
            node = xmlDocGetRootElement( doc.get() );

        return XmlNodeRef( node, doc );
    }

    const char* getXmlns( )
    {
        return "xmlns:cmis=\"http://docs.oasis-open.org/ns/cmis/core/200908/\" xmlns:cmisra=\"http://docs.oasis-open.org/ns/cmis/restatom/200908/\" ";
    }

    string writeXml( boost::shared_ptr< libcmis::XmlSerializable > serializable )
    {
        xmlBufferPtr buf = xmlBufferCreate( );
        xmlTextWriterPtr writer = xmlNewTextWriterMemory( buf, 0 );

        xmlTextWriterStartDocument( writer, NULL, NULL, NULL );
        serializable->toXml( writer );
        xmlTextWriterEndDocument( writer );

        string str( ( const char * )xmlBufferContent( buf ) );

        xmlFreeTextWriter( writer );
        xmlBufferFree( buf );

        return str;
    }

    string getXmlNodeAsString( const string& xmlDoc, const string& xpath )
    {
        string result;
        xmlDocPtr doc = xmlReadMemory( xmlDoc.c_str(), xmlDoc.size(), "", NULL, 0 );

        if ( NULL != doc )
        {
            xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
            libcmis::registerNamespaces( xpathCtx );
            libcmis::registerCmisWSNamespaces( xpathCtx );

            if ( NULL != xpathCtx )
            {
                xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( xpath.c_str() ), xpathCtx );

                if ( xpathObj != NULL )
                {
                    int nbResults = 0;
                    if ( xpathObj->nodesetval )
                        nbResults = xpathObj->nodesetval->nodeNr;

                    for ( int i = 0; i < nbResults; ++i )
                    {
                        xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];
                        xmlBufferPtr buf = xmlBufferCreate( );
                        xmlNodeDump( buf, doc, node, 0, 0 );
                        result += string( ( char * )xmlBufferContent( buf ) );
                        xmlBufferFree( buf );
                    }
                }
                xmlXPathFreeObject( xpathObj);
            }
            xmlXPathFreeContext( xpathCtx );
        }
        else
            throw libcmis::Exception( "Failed to parse service document" );

        xmlFreeDoc( doc );

        return result;
    }

    libcmis::DocumentPtr createVersionableDocument( libcmis::Session* session, string docName )
    {
        libcmis::FolderPtr parent = session->getRootFolder( );

        // Prepare the properties for the new object, object type is cmis:folder
        PropertyPtrMap props;
        libcmis::ObjectTypePtr type = session->getType( "VersionableType" );
        map< string, libcmis::PropertyTypePtr > propTypes = type->getPropertiesTypes( );

        // Set the object name
        map< string, libcmis::PropertyTypePtr >::iterator it = propTypes.find( string( "cmis:name" ) );
        vector< string > nameValues;
        nameValues.push_back( docName );
        libcmis::PropertyPtr nameProperty( new libcmis::Property( it->second, nameValues ) );
        props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );

        // set the object type
        it = propTypes.find( string( "cmis:objectTypeId" ) );
        vector< string > typeValues;
        typeValues.push_back( "VersionableType" );
        libcmis::PropertyPtr typeProperty( new libcmis::Property( it->second, typeValues ) );
        props.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeProperty ) );

        // Actually send the document creation request
        string contentStr = "Some content";
        boost::shared_ptr< ostream > os ( new stringstream( contentStr ) );
        string contentType = "text/plain";
        string filename( "name.txt" );

        return parent->createDocument( props, os, contentType, filename );
    }

    void loadFromFile( const char* path, string& buf )
    {
        ifstream in( path );

        in.seekg( 0, ios::end );
        int length = in.tellg( );
        in.seekg( 0, ios::beg );

        char* buffer = new char[length];
        in.read( buffer, length );
        in.close( );

        buf = string( buffer, length );
        delete[] buffer;
    }
}
