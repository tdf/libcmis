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

#include "oauth2-handler.hxx"
#include "alfresco-session.hxx"
#include "gdrive-utils.hxx"
#include "xml-utils.hxx"

using std::string;
using std::istringstream;

AlfrescoSession::AlfrescoSession( string baseUrl, 
                                  string repositoryId,
                                  string username, 
                                  string password,
                                  libcmis::OAuth2DataPtr oauth2, 
                                  bool verbose )
                                    throw ( libcmis::Exception ) :
    AtomPubSession( baseUrl, repositoryId, username, password, 
                 oauth2, verbose )

{
    if ( oauth2 && oauth2->isComplete( ) ){
        setOAuth2Data( oauth2 );
    }
    initialize( );
}

AlfrescoSession::AlfrescoSession() :
    AtomPubSession()
{
}

string AlfrescoSession::oauth2Authenticate ( ) throw ( CurlException )
{
    static const string CONTENT_TYPE( "application/x-www-form-urlencoded" );
    // STEP 1: Log in
    string res;
    try
    {
        res = httpGetRequest( m_oauth2Handler->getAuthURL( ) )
                                    ->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        return string( );
    }

    string loginPost, loginLink;

    if ( !GdriveUtils::parseResponse( res.c_str( ), loginPost, loginLink ) ) 
        return string( );
    
    loginPost += "username=";  
    loginPost += string( getUsername( ) );
    loginPost += "&password=";
    loginPost += string( getPassword( ) );
    loginPost += "&action=Grant";

    istringstream loginIs( loginPost );
   
    libcmis::HttpResponsePtr resp;
    try 
    {
        // Alfresco code is in the redirect link
        resp = httpPostRequest( loginLink, loginIs, CONTENT_TYPE, false ); 
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

