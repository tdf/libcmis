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

#include <cassert>

#include <libxml/HTMLparser.h>
#include <libxml/xmlreader.h>

#include "session-factory.hxx"
#include "oauth2-providers.hxx"
#include "http-session.hxx"

#define CHALLENGE_PAGE_ACTION "/signin"
#define CHALLENGE_PAGE_ACTION_LEN sizeof( CHALLENGE_PAGE_ACTION ) - 1
#define PIN_FORM_ACTION "/signin/challenge/ipp"
#define PIN_FORM_ACTION_LEN sizeof( PIN_FORM_ACTION ) - 1
#define PIN_INPUT_NAME "Pin"

using namespace std;

namespace {

// See <https://url.spec.whatwg.org/#concept-urlencoded-byte-serializer>:
void addXWwwFormUrlencoded(std::string * buffer, std::string const & data) {
    assert(buffer);
    for (string::const_iterator i = data.begin(); i != data.end(); ++i) {
        unsigned char c = static_cast<unsigned char>(*i);
        if (c == ' ' || c == '*' || c == '-' || c == '.' || (c >= '0' && c <= '9')
            || (c >= 'A' && c <= 'Z') || c == '_' || (c >= 'a' && c <= 'z'))
        {
            *buffer += static_cast<char>(c);
        } else {
            static const char hex[16] = {
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
            *buffer += '%';
            *buffer += hex[c >> 4];
            *buffer += hex[c & 0xF];
        }
    }
}

}

string OAuth2Providers::OAuth2Gdrive( HttpSession* session, const string& authUrl,
                                      const string& username, const string& password )
{
fprintf(stderr,"\nDEBUG.1 OAuth2Providers::OAuth2Gdrive with\n authUrl=<%s>\n username<%s>\n password=<%s>\n",authUrl.c_str(),username.c_str(),password.c_str());
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
fprintf(stderr,"\nDEBUG.2 httpGetRequest authUrl=<%s>:\n",authUrl.c_str());
    try
    {
        // send the first get, receive the html login page
        res = session->httpGetRequest( authUrl )->getStream( )->str( );
    }
    catch ( const CurlException& )
    {
fprintf(stderr,"\nDEBUG.3 failed\n");
        return string( );
    }
fprintf(stderr,"\nDEBUG.4 responded res=<%s>\n",res.c_str());

    // STEP 2: send email

    string loginEmailPost, loginEmailLink;
    if ( !parseResponse( res.c_str( ), loginEmailPost, loginEmailLink ) )
{fprintf(stderr,"\nDEBUG.5 parseResponse failed\n");
        return string( );
}fprintf(stderr,"\nDEBUG.6 parseResponse:\n loginEmailPost=<%s>\n loginEmailLink=<%s>\n",loginEmailPost.c_str(),loginEmailLink.c_str());

    loginEmailPost += "Email=";
    addXWwwFormUrlencoded(&loginEmailPost, username);

    istringstream loginEmailIs( loginEmailPost );
    string loginEmailRes;
fprintf(stderr,"\nDEBUG.7 httpPostRequest\n updated loginEmailPost=<%s>:\n",loginEmailPost.c_str());
    try
    {
        // send a post with user email, receive the html page for password input
        loginEmailRes = session->httpPostRequest ( loginEmailLink, loginEmailIs, CONTENT_TYPE )
                        ->getStream( )->str( );
    }
    catch ( const CurlException& )
    {
fprintf(stderr,"\nDEBUG.8 failed\n");
        return string( );
    }
fprintf(stderr,"\nDEBUG.9 responded loginEmailRes=<%s>\n",loginEmailRes.c_str());

    // STEP 3: password page

    string loginPasswdPost, loginPasswdLink;
    if ( !parseResponse( loginEmailRes.c_str( ), loginPasswdPost, loginPasswdLink ) )
{fprintf(stderr,"\nDEBUG.10 parseResponse failed\n");
        return string( );
}fprintf(stderr,"\nDEBUG.11 parseResponse:\n loginPasswdPost=<%s>\n loginPasswdLink=<%s>\n",loginPasswdPost.c_str(),loginPasswdLink.c_str());

    loginPasswdPost += "Passwd=";
    addXWwwFormUrlencoded(&loginPasswdPost, password);

    istringstream loginPasswdIs( loginPasswdPost );
    string loginPasswdRes;
fprintf(stderr,"\nDEBUG.12 httpPostRequest\n updated loginPasswdPost=<%s>:\n",loginPasswdPost.c_str());
    try
    {
        // send a post with user password, receive the application consent page
        loginPasswdRes = session->httpPostRequest ( loginPasswdLink, loginPasswdIs, CONTENT_TYPE )
                        ->getStream( )->str( );
    }
    catch ( const CurlException& )
    {
fprintf(stderr,"\nDEBUG.13 failed\n");
        return string( );
    }
fprintf(stderr,"\nDEBUG.14 responded loginPasswdRes=<%s>\n",loginPasswdRes.c_str());

    string approvalPost, approvalLink;
    if ( !parseResponse( loginPasswdRes. c_str( ), approvalPost, approvalLink) )
{fprintf(stderr,"\nDEBUG.15 parseResponse failed\n");
        return string( );
}fprintf(stderr,"\nDEBUG.16 parseResponse:\n approvalPost=<%s>\n approvalLink=<%s>\n",approvalPost.c_str(),approvalLink.c_str());

    // when 2FA is enabled, link doesn't start with 'http'
    if ( approvalLink.compare(0, 4, "http") != 0 )
    {
fprintf(stderr,"\nDEBUG.17\n");
        // STEP 3b: 2 Factor Authentication, pin code request

        string loginChallengePost( approvalPost );
        string loginChallengeLink( approvalLink );

        libcmis::OAuth2AuthCodeProvider fallbackProvider = libcmis::SessionFactory::getOAuth2AuthCodeProvider( );
        string pin( fallbackProvider( "", "", "" ) );

        if( pin.empty() )
        {
            // unset OAuth2AuthCode Provider to avoid showing pin request again in the HttpSession::oauth2Authenticate
            libcmis::SessionFactory::setOAuth2AuthCodeProvider( NULL );
            return string( );
        }

        loginChallengeLink = "https://accounts.google.com" + loginChallengeLink;
        loginChallengePost += string( PIN_INPUT_NAME ) + "=";
        loginChallengePost += string( pin );

        istringstream loginChallengeIs( loginChallengePost );
        string loginChallengeRes;
        try
        {
            // send a post with pin, receive the application consent page
            loginChallengeRes = session->httpPostRequest ( loginChallengeLink, loginChallengeIs, CONTENT_TYPE )
                            ->getStream( )->str( );
        }
        catch ( const CurlException& )
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
fprintf(stderr,"\nDEBUG.18\n");
        // wrong password,
        // unset OAuth2AuthCode Provider to avoid showing pin request again in the HttpSession::oauth2Authenticate
        libcmis::SessionFactory::setOAuth2AuthCodeProvider( NULL );
        return string( );
    }

    // STEP 4: allow libcmis to access google drive
    approvalPost += "submit_access=true";

    istringstream approvalIs( approvalPost );
    string approvalRes;
fprintf(stderr,"\nDEBUG.19 httpPostRequest\n updated approvalPost=<%s>:\n",approvalPost.c_str());
    try
    {
        // send a post with application consent
        approvalRes = session->httpPostRequest ( approvalLink, approvalIs,
                            CONTENT_TYPE) ->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
fprintf(stderr,"\nDEBUG.20 failed\n");
        throw e.getCmisException( );
    }
fprintf(stderr,"\nDEBUG.21 responded approvalRes=<%s>\n",approvalRes.c_str());

    // Take the authentication code from the text bar
    string code = parseCode( approvalRes.c_str( ) );

fprintf(stderr,"\nDEBUG.22 code=<%s>\n",code.c_str());
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
    catch ( const CurlException& )
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
    catch ( const CurlException& )
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
    bool bIsRightForm = false;
    bool bHasPinField = false;

    while ( true )
    {
        // Go to the next node, quit if not found
        if ( xmlTextReaderRead ( reader ) != 1) break;
        xmlChar* nodeName = xmlTextReaderName ( reader );
        if ( nodeName == NULL ) continue;
        // Find the redirect link
        if ( xmlStrEqual( nodeName, BAD_CAST( "form" ) ) )
        {
            // 2FA: Don't add fields form other forms not having pin field
            if ( bIsRightForm && !bHasPinField )
                post = string( "" );
            if ( bIsRightForm && bHasPinField )
                break;

            xmlChar* action = xmlTextReaderGetAttribute( reader, 
                                                         BAD_CAST( "action" ));

            // GDrive pin code page contains many forms.
            // We have to parse only the form with pin field.
            if ( action != NULL )
            {
                bool bChallengePage = ( strncmp( (char*)action,
                                                 CHALLENGE_PAGE_ACTION,
                                                 CHALLENGE_PAGE_ACTION_LEN ) == 0 );
                bIsRightForm = ( strncmp( (char*)action,
                                                 PIN_FORM_ACTION,
                                                 PIN_FORM_ACTION_LEN ) == 0 );
                if ( ( xmlStrlen( action ) > 0 )
                    && ( ( bChallengePage && bIsRightForm ) || !bChallengePage ) )
                {
                    link = string ( (char*) action);
                    readInputField = true;
                }
                else
                    readInputField = false;
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
            if ( name != NULL && strcmp( (char*)name, PIN_INPUT_NAME ) == 0 )
                bHasPinField = true;
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

