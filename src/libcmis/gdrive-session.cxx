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

#include "gdrive-session.hxx"
#include "gdrive-common.hxx"

using namespace std;

GDriveSession::GDriveSession ( string clientId, string clientSecret,
        string username, string password, bool verbose )
    throw ( libcmis::Exception ) :
        BaseSession( "", "", username, password, libcmis::OAuth2DataPtr(), verbose )
{
    libcmis::OAuth2DataPtr data( new libcmis::OAuth2Data( DRIVE_AUTH_URL, DRIVE_TOKEN_URL,
                                        DRIVE_SCOPE_FULL, DRIVE_REDIRECT_URI,
                                        clientId, clientSecret ) );
    setOAuth2Data( data );
}

GDriveSession::~GDriveSession()
{
}

GDriveSession::GDriveSession() :
        BaseSession()
{
}

/*
 * Extract a sub string from str, between str1 and str2
 */
string findStringBetween( string str, string str1, string str2)
{

    int start = str.find ( str1 ) ;

    if ( start == (int) string::npos ) return string ( );

    int end = str.find ( str2, start + str1.length( ) + 1  ) ;

    start += str1.length( );

    string result = str.substr( start, end - start );
    return result;
}

char* GDriveSession::oauth2Authenticate ( const char* url, const char* username, const char* password )
    throw ( CurlException )
{
    // STEP 1: authenticate to grab the visit cookie
    libcmis::HttpResponsePtr resp = this-> httpGetRequest( url );
    string loginCookie = resp->getHeaders( )[ "Set-Cookie" ];

    if ( loginCookie.empty( ) ) return NULL;

    // Take the GALX cookie
    string galxCookie = findStringBetween ( loginCookie, "GALX", ";" );
    galxCookie = "GALX" + galxCookie;

    //Login
    string post =
        "continue=" +
        libcmis::escape( url) +
        libcmis::escape ( "&from_login=1" ) +
        "&" + galxCookie    +
        "&Email="            + username +
        "&Passwd="           + password;

    istringstream is( post );

    libcmis::HttpResponsePtr loginResp = this->httpPostRequest ( GOOGLE_LOGIN_URL, is,
            "application/x-www-form-urlencoded", galxCookie, true);

    // The login cookie
    string authenticatedCookie = loginResp->getHeaders( )[ "Set-Cookie" ];
    if ( authenticatedCookie.empty( ) ) return NULL;

    string loginRes = loginResp->getStream( )->str( );

    // STEP 2: allow libcmis to access google drive
    // Get stateWrapper and approveURL from login response page to go to the approve page

    // The approve redirect Url is found as the action link of the post form
    string approveUrl = findStringBetween( loginRes, "form action=\"", "\"" );

    // Remove "amp;" from the URL, it's only validated as HTML
    string removeAmp = "amp;";
    do
    {
        int firstPos =  approveUrl.find( removeAmp );
        if ( firstPos == (int) string::npos ) break;
        approveUrl.erase( firstPos, removeAmp.length( ) );
    } while ( true );

    // Bad authentication, or parser fails
    if ( approveUrl.empty( ) ) return NULL;

    // The state_wrapper parameter is found inside the state_wrapper tag
    string stateWrapper = findStringBetween ( loginRes, "name=\"state_wrapper\" value=\"", "\"");

    // Bad authentication, or parser fails
    if ( stateWrapper.empty( ) ) return NULL;

    // Submit allow access
    post = "state_wrapper=" +
            stateWrapper + "&" +
           "submit_access=true";

    istringstream approveIs( post );

    libcmis::HttpResponsePtr approveResp = this->httpPostRequest ( approveUrl, approveIs,
               "application/x-www-form-urlencoded", authenticatedCookie, true);

    string approveRes = approveResp->getStream( )->str( );

    // STEP 3: Take the authentication code from the text bar
    string code = findStringBetween (approveRes, "\"readonly\" value=\"", "\"");

    char* authCode = new char[ code.length( ) ];
    strcpy ( authCode, code.c_str( ) );

    return authCode;
}

libcmis::RepositoryPtr GDriveSession::getRepository( ) throw ( libcmis::Exception )
{
    libcmis::RepositoryPtr ptr;
    return ptr;
}

libcmis::ObjectPtr GDriveSession::getObject( string /*id*/ ) throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object;
    return object;
}

libcmis::ObjectPtr GDriveSession::getObjectByPath( string /*path*/ ) throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object;
    return object;
}

libcmis::ObjectTypePtr GDriveSession::getType( string /*id*/ ) throw ( libcmis::Exception )
{
    libcmis::ObjectTypePtr ptr;
    return ptr;

}
