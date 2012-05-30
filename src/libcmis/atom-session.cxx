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
    size_t lcl_getEncoding( void *ptr, size_t size, size_t nmemb, void *userdata )
    {
        atom::EncodedData* data = static_cast< atom::EncodedData* >( userdata );

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
        atom::EncodedData* encoded = static_cast< atom::EncodedData* >( data );
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
}

AtomPubSession::AtomPubSession( string atomPubUrl, string repository, 
        string username, string password, bool verbose ) throw ( libcmis::Exception ) :
    Session( ),
    m_sAtomPubUrl( atomPubUrl ),
    m_sRepository( repository ),
    m_username( username ),
    m_password( password ),
    m_authProvided( false ),
    m_workspace( ),
    m_repositoriesIds( ),
    m_verbose( verbose ),
    m_authProvider( ),
    m_curlHandle( NULL )
{
    curl_global_init( CURL_GLOBAL_ALL );
    m_curlHandle = curl_easy_init( );
}

AtomPubSession::AtomPubSession( const AtomPubSession& copy ) :
    Session( ),
    m_sAtomPubUrl( copy.m_sAtomPubUrl ),
    m_sRepository( copy.m_sRepository ),
    m_username( copy.m_username ),
    m_password( copy.m_password ),
    m_authProvided( copy.m_authProvided ),
    m_workspace( copy.m_workspace ),
    m_repositoriesIds( copy.m_repositoriesIds ),
    m_verbose( copy.m_verbose ),
    m_authProvider( copy.m_authProvider ),
    m_curlHandle( NULL )
{
    // Not sure how sharing curl handles is safe.
    curl_global_init( CURL_GLOBAL_ALL );
    m_curlHandle = curl_easy_init( );
}


AtomPubSession& AtomPubSession::operator=( const AtomPubSession& copy )
{
    m_sAtomPubUrl = copy.m_sAtomPubUrl;
    m_sRepository = copy.m_sRepository;
    m_username = copy.m_username;
    m_password = copy.m_password;
    m_authProvided = copy.m_authProvided;
    m_workspace = copy.m_workspace;
    m_repositoriesIds = copy.m_repositoriesIds;
    m_verbose = copy.m_verbose;
    m_authProvider = copy.m_authProvider;
    m_curlHandle = NULL;
    
    // Not sure how sharing curl handles is safe.
    curl_global_init( CURL_GLOBAL_ALL );
    m_curlHandle = curl_easy_init( );

    return *this;
}

AtomPubSession::~AtomPubSession( )
{
    if ( NULL != m_curlHandle )
        curl_easy_cleanup( m_curlHandle );
}

void AtomPubSession::initialize( ) throw ( libcmis::Exception )
{
    if ( m_repositoriesIds.empty() )
    {
        // Pull the content from sAtomPubUrl
        string buf;
        try
        {
            buf = httpGetRequest( m_sAtomPubUrl )->str( );
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

}

list< string > AtomPubSession::getRepositories( string url, string username, string password, bool verbose ) throw ( libcmis::Exception )
{
    AtomPubSession session( url, string(), username, password, verbose );
    session.initialize( );
    return session.m_repositoriesIds;
}

atom::Workspace& AtomPubSession::getWorkspace( ) throw ( libcmis::Exception )
{
    initialize( );
    return m_workspace;
}

libcmis::FolderPtr AtomPubSession::getRootFolder() throw ( libcmis::Exception )
{
    return getFolder( getWorkspace().getRootId() );
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
    string pattern = getWorkspace().getUriTemplate( atom::UriTemplate::ObjectById );
    map< string, string > vars;
    vars[URI_TEMPLATE_VAR_ID] = id;
    string url = createUrl( pattern, vars );

    try
    {
        string buf = httpGetRequest( url )->str( );
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
    string pattern = getWorkspace().getUriTemplate( atom::UriTemplate::ObjectByPath );
    map< string, string > vars;
    vars[URI_TEMPLATE_VAR_PATH] = path;
    string url = createUrl( pattern, vars );

    try
    {
        string buf = httpGetRequest( url )->str( );
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

libcmis::FolderPtr AtomPubSession::getFolder( string id ) throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object = getObject( id );
    libcmis::FolderPtr folder = boost::dynamic_pointer_cast< libcmis::Folder >( object );
    return folder;
}

string AtomPubSession::createUrl( const string& pattern, map< string, string > variables )
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
        {
            // Escape the URL by chunks
            char* escaped = curl_easy_escape( m_curlHandle, value.c_str(), value.length() );
            url.replace( pos, name.size(), escaped );
            curl_free( escaped );
        }

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

boost::shared_ptr< stringstream > AtomPubSession::httpGetRequest( string url ) throw ( atom::CurlException )
{
    // Get the response to the stringstream, but take care of the encoding
    // if any is set in the HTTP headers
    boost::shared_ptr< stringstream > stream( new stringstream( ios_base::out | ios_base::in | ios_base::binary ) );

    atom::EncodedData* data = new atom::EncodedData( stream.get() );

    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, &lcl_bufferData );
    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, data );

    curl_easy_setopt( m_curlHandle, CURLOPT_HEADERFUNCTION, &lcl_getEncoding );
    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEHEADER, data );

    try
    {
        httpRunRequest( url );
        data->finish();
    }
    catch ( const atom::CurlException& e )
    {
        delete data;
        throw e;
    }

    delete data;

    return stream;
}

string AtomPubSession::httpPutRequest( string url, istream& is, string contentType ) throw ( atom::CurlException )
{
    // Get the response to the stringstream
    boost::shared_ptr< stringstream > stream( new stringstream( ios_base::out | ios_base::in | ios_base::binary ) );
    atom::EncodedData* data = new atom::EncodedData( stream.get() );

    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, lcl_bufferData );
    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, data );

    // Get the stream length
    is.seekg( 0, ios::end );
    long size = is.tellg( );
    is.seekg( 0, ios::beg );
    curl_easy_setopt( m_curlHandle, CURLOPT_INFILESIZE, size );
    curl_easy_setopt( m_curlHandle, CURLOPT_READDATA, &is );
    curl_easy_setopt( m_curlHandle, CURLOPT_READFUNCTION, lcl_readStream );
    curl_easy_setopt( m_curlHandle, CURLOPT_UPLOAD, 1 );
    curl_easy_setopt( m_curlHandle, CURLOPT_IOCTLFUNCTION, lcl_ioctlStream );
    curl_easy_setopt( m_curlHandle, CURLOPT_IOCTLDATA, &is );

    struct curl_slist *headers_slist = NULL;
    string contentTypeHeader = string( "Content-Type:" ) + contentType;
    headers_slist = curl_slist_append( headers_slist, contentTypeHeader.c_str( ) );
    curl_easy_setopt( m_curlHandle, CURLOPT_HTTPHEADER, headers_slist );

    try
    {
        httpRunRequest( url );
        data->finish();
    }
    catch ( const atom::CurlException& e )
    {
        delete data;
        curl_slist_free_all( headers_slist );
        throw e;
    }

    delete data;
    curl_slist_free_all( headers_slist );

    return stream->str( );
}

string AtomPubSession::httpPostRequest( string url, istringstream& is, string contentType ) throw ( atom::CurlException )
{
    // Get the response to the stringstream
    boost::shared_ptr< stringstream > stream( new stringstream( ios_base::out | ios_base::in | ios_base::binary ) );
    atom::EncodedData* data = new atom::EncodedData( stream.get() );

    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEFUNCTION, lcl_bufferData );
    curl_easy_setopt( m_curlHandle, CURLOPT_WRITEDATA, data );

    // Get the stream length
    is.seekg( 0, ios::end );
    long size = is.tellg( );
    is.seekg( 0, ios::beg );
    curl_easy_setopt( m_curlHandle, CURLOPT_POSTFIELDSIZE, size );
    curl_easy_setopt( m_curlHandle, CURLOPT_READDATA, &is );
    curl_easy_setopt( m_curlHandle, CURLOPT_READFUNCTION, lcl_readStream );
    curl_easy_setopt( m_curlHandle, CURLOPT_POST, 1 );
    curl_easy_setopt( m_curlHandle, CURLOPT_IOCTLFUNCTION, lcl_ioctlStream );
    curl_easy_setopt( m_curlHandle, CURLOPT_IOCTLDATA, &is );

    struct curl_slist *headers_slist = NULL;
    string contentTypeHeader = string( "Content-Type:" ) + contentType;
    headers_slist = curl_slist_append( headers_slist, contentTypeHeader.c_str( ) );
    curl_easy_setopt( m_curlHandle, CURLOPT_HTTPHEADER, headers_slist );

    try
    {
        httpRunRequest( url );
        data->finish();
    }
    catch ( const atom::CurlException& e )
    {
        delete data;
        curl_slist_free_all( headers_slist );
        throw e;
    }

    delete data;
    curl_slist_free_all( headers_slist );

    return stream->str( );
}

void AtomPubSession::httpDeleteRequest( string url ) throw ( atom::CurlException )
{
    curl_easy_setopt( m_curlHandle, CURLOPT_CUSTOMREQUEST, "DELETE" );
    httpRunRequest( url );
}

void AtomPubSession::httpRunRequest( string url ) throw ( atom::CurlException )
{
    // Grab something from the web
    curl_easy_setopt( m_curlHandle, CURLOPT_URL, url.c_str() );

    // Set the credentials
    if ( m_authProvider.get() && !m_authProvided && ( m_username.empty() || m_password.empty() ) )
    {
        m_authProvided = m_authProvider->authenticationQuery( m_username, m_password );
        if ( !m_authProvided )
        {
            throw atom::CurlException( "User cancelled authentication request" );
        }
    }

    if ( !m_username.empty() && !m_password.empty() )
    {
        curl_easy_setopt( m_curlHandle, CURLOPT_HTTPAUTH, CURLAUTH_ANY );
        curl_easy_setopt( m_curlHandle, CURLOPT_USERNAME, m_username.c_str() );
        curl_easy_setopt( m_curlHandle, CURLOPT_PASSWORD, m_password.c_str() );
    }

    // Get some feedback when something wrong happens
    char errBuff[CURL_ERROR_SIZE];
    curl_easy_setopt( m_curlHandle, CURLOPT_ERRORBUFFER, errBuff );
    curl_easy_setopt( m_curlHandle, CURLOPT_FAILONERROR, 1 );

    if ( m_verbose )
        curl_easy_setopt( m_curlHandle, CURLOPT_VERBOSE, 1 );

    // Perform the query
    CURLcode errCode = curl_easy_perform( m_curlHandle );
    
    // Reset the handle for the next request
    curl_easy_reset( m_curlHandle );

    if ( CURLE_OK != errCode )
        throw atom::CurlException( string( errBuff ), errCode, url );
}

namespace atom
{
    const char* CurlException::what( ) const throw ()
    {
        stringstream buf;
        if ( !isCancelled( ) )
            buf << "CURL error - " << m_code << ": ";
        buf << m_message;

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
            if ( !isCancelled( ) )
                msg += ": " + m_url;
        }

        return libcmis::Exception( msg );
    }

}
