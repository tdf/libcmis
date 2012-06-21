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
#include <cctype>
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

namespace
{
    size_t lcl_getEncoding( void *ptr, size_t size, size_t nmemb, void *userdata )
    {
        libcmis::EncodedData* data = static_cast< libcmis::EncodedData* >( userdata );

        string buf( ( const char* ) ptr, size * nmemb );

        size_t sepPos = buf.find( ':' );
        if ( sepPos != string::npos )
        {
            string name( buf, 0, sepPos );
            if ( "Content-Transfer-Encoding" == name )
            {
                string encoding = buf.substr( sepPos + 1 );
                encoding.erase( remove_if( encoding.begin(), encoding.end(), ptr_fun< int, int> ( isspace ) ), encoding.end() );

                data->setEncoding( encoding );
            }
        }
        
        return nmemb;
    }

    size_t lcl_bufferData( void* buffer, size_t size, size_t nmemb, void* data )
    {
        libcmis::EncodedData* encoded = static_cast< libcmis::EncodedData* >( data );
        encoded->decode( buffer, size, nmemb );
        return nmemb;
    }

    size_t lcl_readStream( void* buffer, size_t size, size_t nmemb, void* data )
    {
        istream& is = *( static_cast< istream* >( data ) );
        char* out = ( char * ) buffer;
        is.read( out, size * nmemb );

        return is.gcount( ) / size;
    }

    curlioerr lcl_ioctlStream( CURL* /*handle*/, int cmd, void* data )
    {
        curlioerr errCode = CURLIOE_OK;

        switch ( cmd )
        {
            case CURLIOCMD_RESTARTREAD:
                {
                    istream& is = *( static_cast< istream* >( data ) );
                    is.clear( );
                    is.seekg( 0, ios::beg );

                    if ( !is.good() )
                    {
                        fprintf ( stderr, "rewind failed\n" );
                        errCode = CURLIOE_FAILRESTART;
                    }
                }
                break;
            case CURLIOCMD_NOP:
                break;
            default:
                errCode = CURLIOE_UNKNOWNCMD;
        }
        return errCode;
    }

    string lcl_tolower( string sText )
    {
        string lower( sText );
        for ( size_t i = 0; i < sText.size(); ++i )
        {
            lower[i] = ::tolower( sText[i] );
        }
        return lower;
    }
}

AtomPubSession::AtomPubSession( string atomPubUrl, string repositoryId, 
        string username, string password, bool verbose ) throw ( libcmis::Exception ) :
    BaseSession( atomPubUrl, repositoryId, username, password, verbose ),
    m_repository( )
{
}

AtomPubSession::AtomPubSession( const AtomPubSession& copy ) :
    BaseSession( copy ),
    m_repository( copy.m_repository )
{
}


AtomPubSession& AtomPubSession::operator=( const AtomPubSession& copy )
{
    BaseSession::operator=( copy );
    m_repository = copy.m_repository;
    
    return *this;
}

AtomPubSession::~AtomPubSession( )
{
}

void AtomPubSession::initialize( ) throw ( libcmis::Exception )
{
    if ( m_repositoriesIds.empty() )
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
                            if ( lcl_tolower( ws->getId( ) ) == lcl_tolower( m_repositoryId ) )
                                m_repository = ws;

                            m_repositoriesIds.push_back( ws->getId() );
                        }
                        catch ( const libcmis::Exception& e )
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

list< string > AtomPubSession::getRepositories( string url, string username, string password, bool verbose ) throw ( libcmis::Exception )
{
    AtomPubSession session( url, string(), username, password, verbose );
    session.initialize( );
    return session.m_repositoriesIds;
}

AtomRepositoryPtr AtomPubSession::getAtomRepository( ) throw ( libcmis::Exception )
{
    initialize( );
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
