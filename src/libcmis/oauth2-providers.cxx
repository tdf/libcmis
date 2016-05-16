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
 * Copyright (C) 2013 Cao Cuong Ngo <cao.cuong.ngo@gmail.com>
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

#include <libxml/HTMLparser.h>
#include <libxml/xmlreader.h>

#include "session-factory.hxx"
#include "oauth2-providers.hxx"
#include "http-session.hxx"

using namespace std;

string OAuth2Providers::OAuth2Gdrive( HttpSession* session, const string& authUrl,
                                      const string& username, const string& password )
{
    /* This member function implements 'Google OAuth 2.0'
     *
     * The interaction is carried out by libcmis, with no web browser involved.
     *
     * Normal sequence (without 2FA) is:
     * 1) a get to activate login page
     *    receive first login page, html format
     * 2) subsequent post to sent email
     *    receive html page for password input
     * 3) subsequent post to send password
     *    receive html page for application consent
     * 4) subsequent post to send a consent for the application
     *    receive a single-use authorization code
     *    this code is returned as a string
     *
     * Sequence with 2FA is:
     * 1) a get to activate login page
     *    receive first login page, html format
     * 2) subsequent post to sent email
     *    receive html page for password input
     * 3) subsequent post to send password
     *    receive html page for pin input
     * 3b) subsequent post to send pin number
     *    receive html page for application consent
     * 4) subsequent post to send a consent for the application
     *    receive a single-use authorization code
     *    this code is returned as a string
     */

    static const string CONTENT_TYPE( "application/x-www-form-urlencoded" );
    // STEP 1: get login page
    string res;
    try
    {
        // send the first get, receive the html login page
        res = session->httpGetRequest( authUrl )->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        return string( );
    }

    // STEP 2: send email

    string loginEmailPost, loginEmailLink;
    if ( !parseResponse( res.c_str( ), loginEmailPost, loginEmailLink ) )
        return string( );

    loginEmailPost += "Email=";
    loginEmailPost += string( username );

    istringstream loginEmailIs( loginEmailPost );
    string loginEmailRes;
    try
    {
        // send a post with user email, receive the html page for password input
        loginEmailRes = session->httpPostRequest ( loginEmailLink, loginEmailIs, CONTENT_TYPE )
                        ->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        return string( );
    }

    // STEP 3: password page

    string loginPasswdPost, loginPasswdLink;
    if ( !parseResponse( loginEmailRes.c_str( ), loginPasswdPost, loginPasswdLink ) )
        return string( );

    loginPasswdPost += "Passwd=";
    loginPasswdPost += string( password );

    istringstream loginPasswdIs( loginPasswdPost );
    string loginPasswdRes;
    try
    {
        // send a post with user password, receive the application consent page
        loginPasswdRes = session->httpPostRequest ( loginPasswdLink, loginPasswdIs, CONTENT_TYPE )
                        ->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        return string( );
    }

    string approvalPost, approvalLink;
    if ( !parseResponse( loginPasswdRes. c_str( ), approvalPost, approvalLink) )
        return string( );

    // when 2FA is enabled, link doesn't start with 'http'
    if ( approvalLink.compare(0, 4, "http") != 0 )
    {
        // STEP 3b: 2 Factor Authentication, pin code request

        string loginChallengePost( approvalPost );
        string loginChallengeLink( approvalLink );

        libcmis::OAuth2AuthCodeProvider fallbackProvider = libcmis::SessionFactory::getOAuth2AuthCodeProvider( );
        string pin( fallbackProvider( "", "", "" ) );

        loginChallengeLink = "https://accounts.google.com" + loginChallengeLink;
        loginChallengePost += "Pin=";
        loginChallengePost += string( pin );

        istringstream loginChallengeIs( loginChallengePost );
        string loginChallengeRes;
        try
        {
            // send a post with pin, receive the application consent page
            loginChallengeRes = session->httpPostRequest ( loginChallengeLink, loginChallengeIs, CONTENT_TYPE )
                            ->getStream( )->str( );
        }
        catch ( const CurlException& e )
        {
            return string( );
        }

        approvalPost = string();
        approvalLink = string();

        if ( !parseResponse( loginChallengeRes. c_str( ), approvalPost, approvalLink) )
            return string( );
    }
    else if( approvalLink.compare( "https://accounts.google.com/ServiceLoginAuth" ) == 0 )
    {
        // wrong password,
        // unset OAuth2AuthCode Provider to avoid showing pin request again in the HttpSession::oauth2Authenticate
        libcmis::SessionFactory::setOAuth2AuthCodeProvider( NULL );
        return string( );
    }

    // STEP 4: allow libcmis to access google drive
    approvalPost += "submit_access=true";

    istringstream approvalIs( approvalPost );
    string approvalRes;
    try
    {
        // send a post with application consent
        approvalRes = session->httpPostRequest ( approvalLink, approvalIs,
                            CONTENT_TYPE) ->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    // Take the authentication code from the text bar
    string code = parseCode( approvalRes.c_str( ) );

    return code;
}

string OAuth2Providers::OAuth2Onedrive( HttpSession* /*session*/, const string& /*authUrl*/,
                                      const string& /*username*/, const string& /*password*/ )
{
    return string( );
}

string OAuth2Providers::OAuth2Alfresco( HttpSession* session, const string& authUrl,
                                        const string& username, const string& password )
{
    static const string CONTENT_TYPE( "application/x-www-form-urlencoded" );
   
    // Log in
    string res;
    try
    {
        res = session->httpGetRequest( authUrl )->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        return string( );
    }

    string loginPost, loginLink;

    if ( !parseResponse( res.c_str( ), loginPost, loginLink ) ) 
        return string( );
    
    loginPost += "username=";  
    loginPost += string( username );
    loginPost += "&password=";
    loginPost += string( password );
    loginPost += "&action=Grant";

    istringstream loginIs( loginPost );
   
    libcmis::HttpResponsePtr resp;

    // Get the code
    try 
    {
        // Alfresco code is in the redirect link
        resp = session->httpPostRequest( loginLink, loginIs, CONTENT_TYPE, false ); 
    }
    catch ( const CurlException& e )
    {
        return string( );
    }

    string header = resp->getHeaders()["Location"];
    string code;
    int start = header.find("code=");
    int notFound = ( int ) ( string::npos );
    if ( start != notFound )
    {   
        start += 5;
        int end = header.find("&");
        if ( end != notFound )
            code = header.substr( start, end - start );
        else code = header.substr( start );
    }

    return code;
}

OAuth2Parser OAuth2Providers::getOAuth2Parser( const std::string& url )
{
    if ( url.find( "https://api.alfresco.com/" ) == 0 )
        // For Alfresco in the cloud, only match the hostname as there can be several
        // binding URLs created with it.
        return OAuth2Alfresco;
    else if ( url.find( "https://www.googleapis.com/drive/v2" ) == 0 )
        return OAuth2Gdrive;
    else if ( url.find( "https://apis.live.net/v5.0" ) == 0 )
        return OAuth2Onedrive;

    return OAuth2Gdrive;
}

int OAuth2Providers::parseResponse ( const char* response, string& post, string& link )
{
    xmlDoc *doc = htmlReadDoc ( BAD_CAST( response ), NULL, 0,
            HTML_PARSE_NOWARNING | HTML_PARSE_RECOVER | HTML_PARSE_NOERROR );
    if ( doc == NULL ) return 0;
    xmlTextReaderPtr reader =   xmlReaderWalker( doc );
    if ( reader == NULL ) return 0;

    bool readInputField = false;

    while ( true )
    {
        // Go to the next node, quit if not found
        if ( xmlTextReaderRead ( reader ) != 1) break;
        xmlChar* nodeName = xmlTextReaderName ( reader );
        if ( nodeName == NULL ) continue;
        // Find the redirect link
        if ( xmlStrEqual( nodeName, BAD_CAST( "form" ) ) )
        {
            xmlChar* action = xmlTextReaderGetAttribute( reader, 
                                                         BAD_CAST( "action" ));

            // GDrive pin code page contains 3 forms.
            // First form resends pin code, we need to omit its input fields.
            // Also we mustn't parse action field from the 'select challenge' form.
            if ( action != NULL )
            {
                if ( strcmp((char*)action, "/signin/challenge") != 0
                   && strcmp((char*)action, "/signin/selectchallenge") != 0
                   && xmlStrlen(action) > 0 )
                {
                    link = string ( (char*) action);
                    readInputField = true;
                }
                xmlFree (action);
            }
        }
        // Find input values
        if ( readInputField && !xmlStrcmp( nodeName, BAD_CAST( "input" ) ) )
        {
            xmlChar* name = xmlTextReaderGetAttribute( reader, 
                                                       BAD_CAST( "name" ));
            xmlChar* value = xmlTextReaderGetAttribute( reader, 
                                                        BAD_CAST( "value" ));
            if ( ( name != NULL ) && ( value!= NULL ) )
            {
                if ( ( xmlStrlen( name ) > 0) && ( xmlStrlen( value ) > 0) )
                {
                    post += libcmis::escape( ( char * ) name ); 
                    post += string ( "=" ); 
                    post += libcmis::escape( ( char * ) value ); 
                    post += string ( "&" );
                }
            }
            xmlFree( name );
            xmlFree( value );
        }
        xmlFree( nodeName );
    }
    xmlFreeTextReader( reader );              
    xmlFreeDoc( doc );
    if ( link.empty( ) || post.empty () ) 
        return 0;
    return 1;
}

string OAuth2Providers::parseCode( const char* response )
{
    string authCode;
    xmlDoc *doc = htmlReadDoc ( BAD_CAST( response ), NULL, 0,
            HTML_PARSE_NOWARNING | HTML_PARSE_RECOVER | HTML_PARSE_NOERROR );
    if ( doc == NULL ) return authCode;
    xmlTextReaderPtr reader = xmlReaderWalker( doc );
    if ( reader == NULL ) return authCode;

    while ( true )
    {
        // Go to the next node, quit if not found
        if ( xmlTextReaderRead ( reader ) != 1) break;
        xmlChar* nodeName = xmlTextReaderName ( reader );
        if ( nodeName == NULL ) continue;
        // Find the code 
        if ( xmlStrEqual( nodeName, BAD_CAST ( "input" ) ) )
        { 
            xmlChar* id = xmlTextReaderGetAttribute( reader, BAD_CAST( "id" ));
            if ( id != NULL )
            {
                if ( xmlStrEqual( id, BAD_CAST ( "code" ) ) )
                {
                    xmlChar* code = xmlTextReaderGetAttribute( 
                        reader, BAD_CAST("value") );
                    if ( code!= NULL )
                    {
                        authCode = string ( (char*) code );
                        xmlFree( code );
                    }
                }
                xmlFree ( id );
            }
        }
        xmlFree( nodeName );
    }
    xmlFreeTextReader( reader );              
    xmlFreeDoc( doc );

    return authCode;
}

