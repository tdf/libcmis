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

#include "oauth2-providers.hxx"
#include "base-session.hxx"
#include "gdrive-utils.hxx"

using namespace std;

string OAuth2Providers::OAuth2Gdrive( BaseSession* session, const string& authUrl,
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
    if ( !GdriveUtils::parseResponse( res.c_str( ), loginPost, loginLink ) ) 
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
    if ( !GdriveUtils::parseResponse( loginRes. c_str( ), approvalPost, approvalLink) )
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
    string code = GdriveUtils::parseCode( approvalRes.c_str( ) );

    return code;
}

string OAuth2Providers::OAuth2Alfresco( BaseSession* session, const string& authUrl,
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

    if ( !GdriveUtils::parseResponse( res.c_str( ), loginPost, loginLink ) ) 
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

OAuth2Parser OAuth2Providers::getOAuth2Parser( const std::string& bindingUrl )
{
    if ( bindingUrl == "https://api.alfresco.com/cmis/versions/1.0/atom/" )
        return OAuth2Alfresco;
    else if ( bindingUrl == "https://www.googleapis.com/drive/v2" )
        return OAuth2Gdrive;

    return OAuth2Gdrive;
}

