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
 * Parse XML nodes to find the redirect link and input form
 */
void parse ( const xmlNodePtr node, string& post, string& link )
{
    xmlNodePtr current = NULL;

    for ( current = node; current; current = current->next)
    {
        if ( ((!xmlStrcmp(current->name, ( const xmlChar* ) "form" ))) )
        {
            char* actionLink  = (char*) xmlGetProp( current, ( const xmlChar*) "action");
            if ( actionLink != NULL)
                link = string( actionLink );

            delete actionLink;
        }

        if ( ((!xmlStrcmp(current->name, ( const xmlChar* ) "input" ))) )
        {
            char* name  = (char*) xmlGetProp( current, ( const xmlChar*) "name");
            char* value = (char*) xmlGetProp( current, ( const xmlChar*) "value");

            if ( name != NULL &&  value!= NULL)
                if ( strlen(value) > 0)
                post += string (name ) + string ("=") + 
                    libcmis::escape(string (value)) + string ("&");

            delete name;
            delete value;
        }
        parse ( current->children, post, link );
    }     
}

/*
 * Parse input values and redirect link from response page
 */

void parseResponse ( const char* response, string& post, string& link )
{
    xmlDoc *doc = htmlReadDoc ( ( const xmlChar* ) response, NULL, 0, 0);
    xmlNodePtr root =   xmlDocGetRootElement ( doc );
    parse( root, post, link );
    xmlFreeDoc( doc );
}

void findCode ( const xmlNodePtr node, string& authCode)
{
    xmlNodePtr current = NULL;
    for ( current = node; current; current = current->next)
    {
        if ( ((!xmlStrcmp(current->name, ( const xmlChar* ) "input" ))) )
        {
            char* id  = (char*) xmlGetProp( current, ( const xmlChar*) "id");
            if ( id != NULL )
            if ( !strcmp (id, "code") )
            {
                char* code  = (char*) xmlGetProp( current, ( const xmlChar*) "value");
                if ( code != NULL )
                    authCode = string( code );
                delete code;
            }
            delete id;
        }
        findCode ( current->children, authCode);
    }
}

/*
 * Parse authorization code from the response page
 */

string parseCode ( const char* response)
{
    xmlDoc *doc = htmlReadDoc ( ( const xmlChar* ) response, NULL, 0, 0);
    xmlNodePtr root = xmlDocGetRootElement ( doc );
    string code ;
    findCode ( root, code);
    xmlFreeDoc( doc );
    return code;
}

char* GDriveSession::oauth2Authenticate ( const char* url, const char* username, const char* password )
    throw ( CurlException )
{
    const string CONTENT_TYPE ( "application/x-www-form-urlencoded" );
    // STEP 1: Log in
    string res = httpGetRequest( url )->getStream( )->str( );
 
    string loginPost, loginLink; 
    
    parseResponse ( res.c_str( ), loginPost, loginLink );

    loginPost += "Email="  + string( username ) + "&Passwd=" + string( password );

    istringstream is( loginPost );

    string loginRes = httpPostRequest ( loginLink, is, CONTENT_TYPE)->getStream( ) ->str( );

    // STEP 2: allow libcmis to access google drive
    
    string approvalPost, approvalLink; 
    parseResponse( loginRes.c_str( ), approvalPost, approvalLink);

    approvalPost += "submit_access=true";

    istringstream approvalIs( approvalPost );

    string approvalRes = httpPostRequest ( approvalLink, approvalIs, CONTENT_TYPE) ->getStream( )->str( );

    // STEP 3: Take the authentication code from the text bar

    string code = parseCode ( approvalRes.c_str( ) );
    if ( code.empty( )) return NULL;
    char* authCode = new char [ code.length( )];
    strcpy ( authCode, code.c_str( ));

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
