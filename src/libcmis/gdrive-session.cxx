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
string findStringBetween( string str, string str1, string str2, int startPos = 0, int* resultPos = NULL)
{

    int start = str.find ( str1, startPos) ;

    if ( start == (int) string::npos ) return string ( );

    int end = str.find ( str2, start + str1.length( ) ) ;

    if ( end == (int) string::npos) return string ( );

    start += str1.length( );

    string result = str.substr( start, end - start );
    
    if ( resultPos != NULL )
        *resultPos = start;

    return result;
}

/*
 * Remove all sub string from a string
 */

string removeSubstr ( string str, string removeStr)
{
    string res = str;
    do
    {
        int pos = res.find( removeStr );
        if ( pos == (int) string::npos ) break;
        res.erase( pos, removeStr.length( ) );
    } while ( true );

    return res;
}

/*
 * Parse input values from response page
 */

map < string, string > getInputs ( string response )
{

    map < string, string > result;

    int resultPos = 0;

    int startPos;

    do
    {
        startPos = resultPos + 1;
        string input = findStringBetween ( response, "<input", ">" , startPos, &resultPos );
        if ( input.empty ( ) ) break;
        
        string name = findStringBetween ( input, "name=\"", "\"", 0);

        if ( name.empty ( ) ) continue;

        string value = findStringBetween ( input, "value=\"", "\"", 0);

        if ( value.empty ( ) ) continue;

        //remove amp;, it's only validated as HTML, not post form
        value = removeSubstr ( value, "amp;");

        result [name] = value;

    } while ( true );

    return result;
}

char* GDriveSession::oauth2Authenticate ( const char* url, const char* username, const char* password )
    throw ( CurlException )
{
    const string CONTENT_TYPE ( "application/x-www-form-urlencoded" );

    // STEP 1: authenticate to grab the cookie
    libcmis::HttpResponsePtr resp = this-> httpGetRequest( url );
    string res = resp->getStream()->str();
 
    map < string, string> inputs = getInputs ( res );

    // Parse login form to post
    string post;
    for ( map < string, string >::iterator it = inputs.begin( ); it != inputs.end( ); ++it)
    {
        if (it != inputs.begin( )) post += "&"; 
        post += it->first + "=" + libcmis::escape( it->second ) ;
    }    

    post += string ("&Email=" )   + username    +
            string ("&Passwd=")  + password;

    istringstream is( post );

    libcmis::HttpResponsePtr loginResp = this->httpPostRequest ( GOOGLE_LOGIN_URL, is, CONTENT_TYPE);

    string loginRes = loginResp->getStream( )->str( );

    // STEP 2: allow libcmis to access google drive

    // The approve redirect Url is found as the action link of the post form
    string approveUrl = findStringBetween( loginRes, "form action=\"", "\"" );

    // Remove "amp;" from the URL, it's only validated as HTML
    approveUrl = removeSubstr( approveUrl, "amp;");
    
    // Bad authentication, or parser fails
    if ( approveUrl.empty( ) ) return NULL;

    // Parse input values from the approve response page
    inputs = getInputs ( loginRes );
    string approvePost;
    for ( map < string, string >::iterator it = inputs.begin( ); it != inputs.end( ); ++it)
    {
        if (it != inputs.begin( )) approvePost += "&"; 
        approvePost += it->first + "=" + libcmis::escape( it->second ) ;
    }

    // Submit allow access
    approvePost += "&submit_access=true";

    istringstream approveIs( approvePost );

    libcmis::HttpResponsePtr approveResp = this->httpPostRequest ( approveUrl, approveIs, CONTENT_TYPE);

    string approveRes = approveResp->getStream( )->str( );

    // STEP 3: Take the authentication code from the text bar
    string inputString = findStringBetween ( approveRes, "<input", ">");
    string code = findStringBetween ( inputString, " value=\"", "\"");

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
