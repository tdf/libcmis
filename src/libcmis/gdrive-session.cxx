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
                                        clientId, clientSecret, NULL ) );
    setOAuth2Data( data );
}

GDriveSession::~GDriveSession()
{
}

GDriveSession::GDriveSession() :
        BaseSession()
{
}

string findStringBetween( string str, string str1, string str2)
{

    int start = str.find ( str1 ) ;

    if ( start == (int) string::npos ) return string ( );
    //find the closing \" of the Url
    int end = str.find ( str2, start + str1.length( ) + 1  ) ;

    start += str1.length( );

    string result = str.substr( start, end - start );
    return result;
}

char* GDriveSession::oauth2Authenticate ( const char* url, const char* username, const char* password ) {

    //grab the visit cookie
    libcmis::HttpResponsePtr resp = this-> httpGetRequest( url );
    string loginCookie = resp->getHeaders( )[ "Set-Cookie" ];

    if ( loginCookie.empty( ) ) return NULL;

    //only take the first cookie
    int pos = loginCookie.find(';');
    string firstCookie = loginCookie.substr(0, pos);
    //login
    string post =
        "continue=" +
        libcmis::escape( url) +
        libcmis::escape ( "&from_login=1" ) +
        "&" + firstCookie    +
        "&Email="            + username +
        "&Passwd="           + password;

    istringstream is( post );

    libcmis::HttpResponsePtr loginResp = this->httpPostRequest ( GOOGLE_LOGIN_URL, is,
            "application/x-www-form-urlencoded", firstCookie, true);

    //the login cookie
    string authenticatedCookie = loginResp->getHeaders( )[ "Set-Cookie" ];
    if ( authenticatedCookie.empty( ) ) return NULL;

    string loginRes = loginResp->getStream( )->str( );

    //approve, parse stateWrapper, approveURL from login response page

    //the approveUrl is found as the action link of the post form
    string approveUrl = findStringBetween( loginRes, "form action=\"", "\"");

    //"remove "amp;" from the URL, it's only validated as HTML
    string removeAmp = "amp;";
    do
    {
        int firstPos =  approveUrl.find( removeAmp );
        if ( firstPos == (int) string::npos ) break;
        approveUrl.erase( firstPos, removeAmp.length( ) );
    } while ( true );

    //the state_wrapper parameter is found from the state_wrapper tag
    string stateWrapper = findStringBetween (loginRes, "name=\"state_wrapper\" value=\"", "\"");

    //submit allow access
    post = "state_wrapper=" +
            stateWrapper + "&" +
           "submit_access=true";

    istringstream appproveIs( post );

    libcmis::HttpResponsePtr approveResp = this->httpPostRequest ( approveUrl, appproveIs,
               "application/x-www-form-urlencoded", authenticatedCookie, true);

    string approveRes = approveResp->getStream( )->str( );

    string authCode = findStringBetween (approveRes, "\"readonly\" value=\"", "\"");

    return (char*) authCode.c_str( );
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
