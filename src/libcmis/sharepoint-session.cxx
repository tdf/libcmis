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
 * Copyright (C) 2014 Mihai Varga <mihai.mv13@gmail.com>
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

#include "sharepoint-document.hxx"
#include "sharepoint-folder.hxx"
#include "sharepoint-object.hxx"
#include "sharepoint-object-type.hxx"
#include "sharepoint-repository.hxx"
#include "sharepoint-session.hxx"
#include "sharepoint-utils.hxx"
#include "session-factory.hxx"

using namespace std;

SharePointSession::SharePointSession ( string baseUrl,
                               string username,
                               string password,
                               bool verbose )
                                    throw ( libcmis::Exception ) :
    BaseSession( baseUrl, string(), username, password, false,
                 libcmis::OAuth2DataPtr(), verbose ),
    m_digestCode( string( ) ) 

{
    setAuthMethod( CURLAUTH_NTLM );
    libcmis::HttpResponsePtr response;
    try
    {
        response = httpGetRequest( baseUrl + "/currentuser" );
    }
    catch ( const CurlException& e )
    {
        // It's not SharePoint or wrong username/passwd provided
        throw e.getCmisException( );
    }

    // Add the dummy repository
    m_repositories.push_back( getRepository( ) );
    fetchDigestCode( );
}

SharePointSession::SharePointSession( string baseUrl,
                                      const HttpSession& httpSession,
                                      libcmis::HttpResponsePtr response )
                                            throw ( libcmis::Exception ) :
    BaseSession( baseUrl, string(), httpSession ),
    m_digestCode( string( ) ) 
{
    boost::shared_ptr< libcmis::Exception > exception;
    if ( !SharePointUtils::isSharePoint( response->getStream( )->str( ) ) )
    {
        exception.reset( new libcmis::Exception( "Not a SharePoint service" ) );
        throw *exception.get( );
    }
    // Add the dummy repository
    m_repositories.push_back( getRepository( ) );
    fetchDigestCode( );
}

SharePointSession::SharePointSession( const SharePointSession& copy ) :
    BaseSession( copy ), m_digestCode( copy.m_digestCode )
{
}

SharePointSession::SharePointSession() :
    BaseSession(), m_digestCode( string( ) )
{
}

SharePointSession::~SharePointSession()
{
}

libcmis::RepositoryPtr SharePointSession::getRepository( )
    throw ( libcmis::Exception )
{
    // Return a dummy repository since SharePoint doesn't have that notion
    libcmis::RepositoryPtr repo( new SharePointRepository( ) );
    return repo;
}

libcmis::ObjectPtr SharePointSession::getObject( string objectId )
    throw ( libcmis::Exception )
{
    // objectId is uri for the file
    string res;
    try
    {
        res = httpGetRequest( objectId )->getStream()->str();
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    Json jsonRes = Json::parse( res );
    return getObjectFromJson( jsonRes );
}

libcmis::ObjectPtr SharePointSession::getObjectFromJson( Json& jsonRes, string parentId ) 
            throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object;
    if ( !jsonRes["d"].toString( ).empty( ) ) {
        jsonRes = jsonRes["d"];
    }
    string kind = jsonRes["__metadata"]["type"].toString( );
    // only SharePointObject available for now
    if ( kind == "SP.Folder" )
    {
        object.reset( new SharePointFolder( this, jsonRes, parentId ) );
    }
    else if ( kind == "SP.File" || kind == "SP.FileVersion" )
    {
        object.reset( new SharePointDocument( this, jsonRes, parentId ) );
    }
    else
    {
        object.reset( new SharePointObject( this, jsonRes, parentId ) );
    }
    return object;
}

libcmis::ObjectPtr SharePointSession::getObjectByPath( string path )
    throw ( libcmis::Exception )
{
    return getObject( path );
}

libcmis::ObjectTypePtr SharePointSession::getType( string id )
    throw ( libcmis::Exception )
{
    libcmis::ObjectTypePtr type( new SharePointObjectType( id ) );
    return type;
}

vector< libcmis::ObjectTypePtr > SharePointSession::getBaseTypes( )
    throw ( libcmis::Exception )
{
    vector< libcmis::ObjectTypePtr > types;
    return types;
}

Json SharePointSession::getJsonFromUrl( string url )
    throw ( libcmis::Exception )
{
    string response;
    try
    {
        response = httpGetRequest( url )->getStream()->str();
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    return Json::parse( response );
}

/* Overwriting HttpSession::httpRunRequest to add the "accept:application/json" header */
void SharePointSession::httpRunRequest( string url, vector< string > headers, bool redirect ) throw ( CurlException )
{
    // Redirect
    curl_easy_setopt( m_curlHandle, CURLOPT_FOLLOWLOCATION, redirect);

    // Activate the cookie engine
    curl_easy_setopt( m_curlHandle, CURLOPT_COOKIEFILE, "" );

    // Grab something from the web
    curl_easy_setopt( m_curlHandle, CURLOPT_URL, url.c_str() );

    // Set the headers
    struct curl_slist *headers_slist = NULL;
    for ( vector< string >::iterator it = headers.begin( ); it != headers.end( ); ++it )
        headers_slist = curl_slist_append( headers_slist, it->c_str( ) );

    headers_slist = curl_slist_append( headers_slist, "accept:application/json; odata=verbose" );
    headers_slist = curl_slist_append( headers_slist, ( "x-requestdigest:" + m_digestCode ).c_str( ) );

    if ( !getUsername().empty() && !getPassword().empty() )
    {
        curl_easy_setopt( m_curlHandle, CURLOPT_HTTPAUTH, m_authMethod );
#if LIBCURL_VERSION_VALUE >= 0x071301
        curl_easy_setopt( m_curlHandle, CURLOPT_USERNAME, getUsername().c_str() );
        curl_easy_setopt( m_curlHandle, CURLOPT_PASSWORD, getPassword().c_str() );
#else
        string userpwd = getUsername() + ":" + getPassword();
        curl_easy_setopt( m_curlHandle, CURLOPT_USERPWD, userpwd.c_str( ) );
#endif
    }

    curl_easy_setopt( m_curlHandle, CURLOPT_HTTPHEADER, headers_slist );

    // Set the proxy configuration if any
    if ( !libcmis::SessionFactory::getProxy( ).empty() )
    {
        curl_easy_setopt( m_curlHandle, CURLOPT_PROXY, libcmis::SessionFactory::getProxy( ).c_str() );
#if LIBCURL_VERSION_VALUE >= 0x071304
        curl_easy_setopt( m_curlHandle, CURLOPT_NOPROXY, libcmis::SessionFactory::getNoProxy( ).c_str() );
#endif
        const string& proxyUser = libcmis::SessionFactory::getProxyUser( );
        const string& proxyPass = libcmis::SessionFactory::getProxyPass( );
        if ( !proxyUser.empty( ) && !proxyPass.empty( ) )
        {
            curl_easy_setopt( m_curlHandle, CURLOPT_PROXYAUTH, CURLAUTH_ANY );
#if LIBCURL_VERSION_VALUE >= 0X071301
            curl_easy_setopt( m_curlHandle, CURLOPT_PROXYUSERNAME, proxyUser.c_str( ) );
            curl_easy_setopt( m_curlHandle, CURLOPT_PROXYPASSWORD, proxyPass.c_str( ) );
#else
            string userpwd = proxyUser + ":" + proxyPass;
            curl_easy_setopt( m_curlHandle, CURLOPT_PROXYUSERPWD, userpwd.c_str( ) );
#endif
        }
    }

    // Get some feedback when something wrong happens
    char errBuff[CURL_ERROR_SIZE];
    curl_easy_setopt( m_curlHandle, CURLOPT_ERRORBUFFER, errBuff );

    // We want to get the response even if there is an Http error
    if ( !m_noHttpErrors )
        curl_easy_setopt( m_curlHandle, CURLOPT_FAILONERROR, 1 );

    if ( m_verbose )
        curl_easy_setopt( m_curlHandle, CURLOPT_VERBOSE, 1 );

    // We want to get the certificate infos in error cases
#if LIBCURL_VERSION_VALUE >= 0X071301
    curl_easy_setopt( m_curlHandle, CURLOPT_CERTINFO, 1 );
#endif

    if ( m_noSSLCheck )
    {
#if LIBCURL_VERSION_VALUE >= 0x070801
        curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYHOST, 0);
#endif
#if LIBCURL_VERSION_VALUE >= 0x070402
        curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYPEER, 0);
#endif
    }

    // Perform the query
    CURLcode errCode = curl_easy_perform( m_curlHandle );

    // Free the headers list
    curl_slist_free_all( headers_slist );

    // Process the response
    bool isHttpError = errCode == CURLE_HTTP_RETURNED_ERROR;
    if ( CURLE_OK != errCode && !( m_noHttpErrors && isHttpError ) )
    {
        long httpError = 0;
        curl_easy_getinfo( m_curlHandle, CURLINFO_RESPONSE_CODE, &httpError );

        bool errorFixed = false;
#if LIBCURL_VERSION_VALUE >= 0X071301
        // If we had a bad certificate, then try to get more details
        if ( CURLE_SSL_CACERT == errCode )
        {
            vector< string > certificates;

            // We somehow need to rerun the request to get the certificate
            curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYHOST, 0);
            curl_easy_setopt(m_curlHandle, CURLOPT_SSL_VERIFYPEER, 0);
            errCode = curl_easy_perform( m_curlHandle );

            union {
                struct curl_slist    *to_info;
                struct curl_certinfo *to_certinfo;
            } ptr;

            ptr.to_info = NULL;

            CURLcode res = curl_easy_getinfo(m_curlHandle, CURLINFO_CERTINFO, &ptr.to_info);

            if ( !res && ptr.to_info )
            {
                // We need the first certificate in the chain only
                if ( ptr.to_certinfo->num_of_certs > 0 )
                {
                    struct curl_slist *slist;

                    string certStart( "Cert:" );
                    for ( slist = ptr.to_certinfo->certinfo[0]; slist; slist = slist->next )
                    {
                        string data( slist->data );
                        size_t startPos = data.find( certStart );
                        if ( startPos == 0 )
                        {
                            startPos = certStart.length();
                            data = data.substr( startPos );
                            certificates.push_back( data );
                        }
                    }
                }
            }

            if ( !certificates.empty() )
            {
                libcmis::CertValidationHandlerPtr validationHandler =
                    libcmis::SessionFactory::getCertificateValidationHandler( );
                bool ignoreCert = validationHandler && validationHandler->validateCertificate( certificates );
                if ( ignoreCert )
                {
                    m_noSSLCheck = true;

                    isHttpError = errCode == CURLE_HTTP_RETURNED_ERROR;
                    errorFixed = ( CURLE_OK == errCode || ( m_noHttpErrors && isHttpError ) );
                    if ( !errorFixed )
                        curl_easy_getinfo( m_curlHandle, CURLINFO_RESPONSE_CODE, &httpError );
                }
                else
                {
                    throw CurlException( "Invalid SSL certificate" );
                }
            }
        }
#endif

        if ( !errorFixed )
            throw CurlException( string( errBuff ), errCode, url, httpError );
    }
}

libcmis::HttpResponsePtr SharePointSession::httpPutRequest( std::string url,
                                         std::istream& is,
                                         std::vector< std::string > headers )
    throw ( CurlException )
{
    libcmis::HttpResponsePtr response;
    try
    {
        response = HttpSession::httpPutRequest( url, is, headers );
    }
    catch ( const CurlException& e )
    {
        fetchDigestCode( );
        response = HttpSession::httpPutRequest( url, is, headers );
    }
    return response;
}
        
libcmis::HttpResponsePtr SharePointSession::httpPostRequest( const std::string& url,
                                          std::istream& is,
                                          const std::string& contentType,
                                          bool redirect )
    throw ( CurlException )
{
    libcmis::HttpResponsePtr response;
    try
    {
        response = HttpSession::httpPostRequest( url, is, contentType, redirect );
    }
    catch ( const CurlException& e )
    {
        fetchDigestCode( );
        response = HttpSession::httpPostRequest( url, is, contentType, redirect );
    }
    return response;
}

void SharePointSession::httpDeleteRequest( std::string url )
    throw ( CurlException )
{
    try
    {
        HttpSession::httpDeleteRequest( url );
    }
    catch ( const CurlException& e )
    {
        fetchDigestCode( );
        HttpSession::httpDeleteRequest( url );
    }
}

void SharePointSession::fetchDigestCode( )
    throw ( libcmis::Exception )
{
    istringstream is( "empty" );
    libcmis::HttpResponsePtr response;
    // url = http://host/_api/contextinfo, first we remove the '/web' part
    string url = m_bindingUrl.substr( 0, m_bindingUrl.size( ) - 4 ) + "/contextinfo";
    try 
    {   
        response = HttpSession::httpPostRequest( url, is, "" );
    }
    catch ( const CurlException& e )
    {   
        throw e.getCmisException( );
    }
    string res = response->getStream( )->str( );
    Json jsonRes = Json::parse( res );
    m_digestCode = jsonRes["d"]["GetContextWebInformation"]["FormDigestValue"].toString( );
}
