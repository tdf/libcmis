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
#include <libxml/xpath.h>
#include <libxml/xmlstring.h>

#include "ws-soap.hxx"
#include "xml-utils.hxx"

using namespace std;

SoapFault::SoapFault( xmlNodePtr node ) :
    exception( ),
    m_faultcode( ),
    m_faultstring( )
{
    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "faultcode" ) ) )
        {
            xmlChar* content = xmlNodeGetContent( child );
            xmlChar* prefix = NULL;
            xmlChar* localName = xmlSplitQName2( content, &prefix );
            if (localName == NULL)
                localName = xmlStrdup( content );
            m_faultcode = string( ( char* )localName );
            xmlFree( content );
            xmlFree( localName );
        }
        else if ( xmlStrEqual( child->name, BAD_CAST( "faultstring" ) ) )
        {
            xmlChar* content = xmlNodeGetContent( child );
            m_faultstring = string( ( char* )content );
            xmlFree( content );
        }
    }   
}

const char* SoapFault::what( ) const throw ( )
{
    return string( getFaultcode() + ": " + getFaultstring() ).c_str( );
}


SoapResponseFactory::SoapResponseFactory( ) :
    m_mapping( ),
    m_namespaces( )
{
}

vector< SoapResponsePtr > SoapResponseFactory::parseResponse( string xml ) throw ( SoapFault )
{
    vector< SoapResponsePtr > responses;

    xmlDocPtr doc = xmlReadMemory( xml.c_str(), xml.size(), "", NULL, 0 );

    if ( NULL != doc )
    {
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
        libcmis::registerSoapNamespaces( xpathCtx );

        for ( map< string, string >::iterator it = m_namespaces.begin( );
              it != m_namespaces.end( ); ++it )
        {
            xmlXPathRegisterNs( xpathCtx, BAD_CAST( it->first.c_str() ),  BAD_CAST( it->second.c_str( ) ) );
        }

        if ( NULL != xpathCtx )
        {
            string bodyXPath( "//soap-env:Body/*" );
            xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( bodyXPath.c_str() ), xpathCtx );

            if ( xpathObj != NULL )
            {
                int nbChildren = 0;
                if ( xpathObj->nodesetval )
                    nbChildren = xpathObj->nodesetval->nodeNr;

                for ( int i = 0; i < nbChildren; i++ )
                {
                    xmlNodePtr node = xpathObj->nodesetval->nodeTab[i];

                    // Is it a fault?
                    if ( xmlStrEqual( NS_SOAP_ENV_URL, node->ns->href ) &&
                         xmlStrEqual( BAD_CAST( "Fault" ), node->name ) )
                    {
                        throw SoapFault( node );
                    }
                    SoapResponsePtr response = createResponse( node );
                    if ( NULL != response.get( ) )
                        responses.push_back( response );
                }
            }
        }
        xmlXPathFreeContext( xpathCtx );
    }

    xmlFreeDoc( doc );

    return responses;
}

SoapResponsePtr SoapResponseFactory::createResponse( xmlNodePtr node )
{
    SoapResponsePtr response;

    // Implement me
    string ns( ( const char* ) node->ns->href );
    string name( ( const char* ) node->name );
    string id = "{" + ns + "}" + name;
    map< string, SoapResponseCreator >::iterator it = m_mapping.find( id );

    if ( it != m_mapping.end( ) )
    {
        SoapResponseCreator creator = it->second;
        response = creator( node );
    }

    return response;
}
