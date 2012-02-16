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
#include "atom-utils.hxx"

using namespace std;

namespace
{
    size_t lcl_bufferData( void* buffer, size_t size, size_t nmemb, void* data )
    {
        stringstream& out = *( static_cast< stringstream* >( data ) );
        out.write( ( const char* ) buffer, size * nmemb );
        return nmemb;
    }

    size_t lcl_readStream( void* buffer, size_t size, size_t nmemb, void* data )
    {
        istream& is = *( static_cast< istream* >( data ) );
        char* out = ( char * ) buffer;
        is.read( out, size * nmemb );

        return is.gcount( ) / size;
    }
}

string atom::UriTemplate::createUrl( const string& pattern, map< string, string > variables )
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

AtomPubSession::AtomPubSession( string atomPubUrl, string repository, 
        string username, string password, bool verbose ) throw ( libcmis::Exception ) :
    Session( ),
    m_sAtomPubUrl( atomPubUrl ),
    m_sRepository( repository ),
    m_username( username ),
    m_password( password ),
    m_workspace( ),
    m_repositoriesIds( ),
    m_verbose( verbose )
{
    // Pull the content from sAtomPubUrl
    string buf;
    try
    {
        buf = httpGetRequest( m_sAtomPubUrl );
    }
    catch ( const atom::CurlException& e )
    {
        throw e.getCmisException( );
    }
   
    // parse the content
    xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), m_sAtomPubUrl.c_str(), NULL, 0 );

    if ( NULL != doc )
    {
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );

        // Register the Service Document namespaces
        atom::registerNamespaces( xpathCtx );

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
                        atom::Workspace ws( xpathObj->nodesetval->nodeTab[i] );
                        if ( ws.getId( ) == m_sRepository )
                            m_workspace = ws;

                        m_repositoriesIds.push_back( ws.getId() );
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

AtomPubSession::~AtomPubSession( )
{
}

list< string > AtomPubSession::getRepositories( string url, string username, string password, bool verbose ) throw ( libcmis::Exception )
{
    AtomPubSession session( url, string(), username, password, verbose );
    return session.m_repositoriesIds;
}

libcmis::FolderPtr AtomPubSession::getRootFolder()
{
    return getFolder( m_workspace.getRootId() );
}

libcmis::ObjectPtr AtomPubSession::createObjectFromEntryDoc( xmlDocPtr doc )
{
    libcmis::ObjectPtr cmisObject;

    if ( NULL != doc )
    {
        // Get the atom:entry node
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );
        atom::registerNamespaces( xpathCtx );
        if ( NULL != xpathCtx )
        {
            const string& entriesReq( "//atom:entry" );
            xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( entriesReq.c_str() ), xpathCtx );

            if ( NULL != xpathObj && NULL != xpathObj->nodesetval && ( 0 < xpathObj->nodesetval->nodeNr ) )
            {
                xmlNodePtr node = xpathObj->nodesetval->nodeTab[0];
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
            xmlXPathFreeObject( xpathObj );
        }
        xmlXPathFreeContext( xpathCtx );
    }

    return cmisObject;
}

libcmis::ObjectPtr AtomPubSession::getObject( string id ) throw ( libcmis::Exception )
{
    string pattern = m_workspace.getUriTemplate( atom::UriTemplate::ObjectById );
    map< string, string > vars;
    vars[URI_TEMPLATE_VAR_ID] = id;
    string url = atom::UriTemplate::createUrl( pattern, vars );

    try
    {
        string buf = httpGetRequest( url );
        xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), url.c_str(), NULL, 0 );
        libcmis::ObjectPtr cmisObject = createObjectFromEntryDoc( doc );
        xmlFreeDoc( doc );
        return cmisObject;
    }
    catch ( const atom::CurlException& e )
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
    string pattern = m_workspace.getUriTemplate( atom::UriTemplate::ObjectByPath );
    map< string, string > vars;
    vars[URI_TEMPLATE_VAR_PATH] = path;
    string url = atom::UriTemplate::createUrl( pattern, vars );

    try
    {
        string buf = httpGetRequest( url );
        xmlDocPtr doc = xmlReadMemory( buf.c_str(), buf.size(), url.c_str(), NULL, 0 );
        libcmis::ObjectPtr cmisObject = createObjectFromEntryDoc( doc );
        xmlFreeDoc( doc );
        return cmisObject;
    }
    catch ( const atom::CurlException& e )
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

libcmis::FolderPtr AtomPubSession::getFolder( string id )
{
    libcmis::ObjectPtr object = getObject( id );
    libcmis::FolderPtr folder = boost::dynamic_pointer_cast< libcmis::Folder >( object );
    return folder;
}

string AtomPubSession::httpGetRequest( string url ) throw ( atom::CurlException )
{
    stringstream stream;

    curl_global_init( CURL_GLOBAL_ALL );
    CURL* pHandle = curl_easy_init( );

    // Grab something from the web
    curl_easy_setopt( pHandle, CURLOPT_WRITEFUNCTION, lcl_bufferData );
    curl_easy_setopt( pHandle, CURLOPT_WRITEDATA, &stream );

    httpRunRequest( pHandle, url );

    curl_easy_cleanup( pHandle );

    return stream.str();
}

void AtomPubSession::httpPutRequest( string url, istream& is, string contentType ) throw ( atom::CurlException )
{
    curl_global_init( CURL_GLOBAL_ALL );
    CURL* handle = curl_easy_init( );
   
    // Get the stream length
    is.seekg( 0, ios::end );
    long size = is.tellg( );
    is.seekg( 0, ios::beg );
    curl_easy_setopt( handle, CURLOPT_INFILESIZE, size );
    curl_easy_setopt( handle, CURLOPT_READDATA, &is );
    curl_easy_setopt( handle, CURLOPT_READFUNCTION, lcl_readStream );
    curl_easy_setopt( handle, CURLOPT_UPLOAD, 1 );

    struct curl_slist *headers_slist = NULL;
    string contentTypeHeader = string( "Content-Type:" ) + contentType;
    headers_slist = curl_slist_append( headers_slist, contentTypeHeader.c_str( ) );
    curl_easy_setopt( handle, CURLOPT_HTTPHEADER, headers_slist );

    // TODO Define a CURLOPT_IOCTLFUNCTION callback to rewind in
    // multipass authentication cases (like for SharePoint)

    httpRunRequest( handle, url );

    curl_slist_free_all( headers_slist );
    curl_easy_cleanup( handle );
}

void AtomPubSession::httpRunRequest( CURL* pHandle, string url ) throw ( atom::CurlException )
{
    // Grab something from the web
    curl_easy_setopt( pHandle, CURLOPT_URL, url.c_str() );

    // Set the credentials
    if ( !m_username.empty() && !m_password.empty() )
    {
        curl_easy_setopt( pHandle, CURLOPT_HTTPAUTH, CURLAUTH_ANY );
        curl_easy_setopt( pHandle, CURLOPT_USERNAME, m_username.c_str() );
        curl_easy_setopt( pHandle, CURLOPT_PASSWORD, m_password.c_str() );
    }

    // Get some feedback when something wrong happens
    char errBuff[CURL_ERROR_SIZE];
    curl_easy_setopt( pHandle, CURLOPT_ERRORBUFFER, errBuff );
    curl_easy_setopt( pHandle, CURLOPT_FAILONERROR, 1 );

    if ( m_verbose )
        curl_easy_setopt( pHandle, CURLOPT_VERBOSE, 1 );

    // Perform the query
    CURLcode errCode = curl_easy_perform( pHandle );
    if ( CURLE_OK != errCode )
        throw atom::CurlException( string( errBuff ), errCode, url );
}

namespace atom
{
    const char* CurlException::what( ) const throw ()
    {
        stringstream buf;
        buf << "CURL error - " << m_code << ": " << m_message;

        return buf.str( ).c_str( );
    }

    libcmis::Exception CurlException::getCmisException( ) const
    {
        string msg;

        if ( ( CURLE_HTTP_RETURNED_ERROR == m_code ) &&
             ( string::npos != m_message.find( "403" ) ) )
        {
            msg = "Invalid credentials";
        }
        else if ( ( CURLE_HTTP_RETURNED_ERROR == m_code ) &&
             ( string::npos != m_message.find( "404" ) ) )
        {
            msg = "Invalid URL: " + m_url;
        }
        else if ( ( CURLE_HTTP_RETURNED_ERROR == m_code ) && 
             ( string::npos != m_message.find( "409" ) ) )
        {
            msg = "Editing conflict error";
        }
        else
        {
            msg = what();
            msg += ": " + m_url;
        }

        return libcmis::Exception( msg );
    }

}
