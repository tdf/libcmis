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

#include "oauth2-providers.hxx"
#include "http-session.hxx"

using namespace std;

string OAuth2Providers::OAuth2Gdrive( HttpSession* session, const string& authUrl,
                                      const string& username, const string& password )
{
    static const string CONTENT_TYPE( "application/x-www-form-urlencoded" );
    // STEP 1: Log in
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
    
    loginPost += "Email=";  
    loginPost += string( username );
    loginPost += "&Passwd=";
    loginPost += string( password );
    
    istringstream loginIs( loginPost );
    string loginRes;
    try 
    {
        loginRes = session->httpPostRequest ( loginLink, loginIs, CONTENT_TYPE )
                        ->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        return string( );
    }

    // STEP 2: allow libcmis to access google drive
    string approvalPost, approvalLink; 
    if ( !parseResponse( loginRes. c_str( ), approvalPost, approvalLink) )
        return string( );
    approvalPost += "submit_access=true";

    istringstream approvalIs( approvalPost );
    string approvalRes;
    try
    {
        approvalRes = session->httpPostRequest ( approvalLink, approvalIs, 
                            CONTENT_TYPE) ->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    // STEP 3: Take the authentication code from the text bar
    string code = parseCode( approvalRes.c_str( ) );

    return code;
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

    return OAuth2Gdrive;
}

int OAuth2Providers::parseResponse ( const char* response, string& post, string& link )
{
    xmlDoc *doc = htmlReadDoc ( BAD_CAST( response ), NULL, 0,
            HTML_PARSE_NOWARNING | HTML_PARSE_RECOVER | HTML_PARSE_NOERROR );
    if ( doc == NULL ) return 0;
    xmlTextReaderPtr reader =   xmlReaderWalker( doc );
    if ( reader == NULL ) return 0;
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
            if ( action != NULL )
            {
                if ( xmlStrlen(action) > 0)
                    link = string ( (char*) action);
                xmlFree (action);
            }
        }
        // Find input values
        if ( !xmlStrcmp( nodeName, BAD_CAST( "input" ) ) )
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

