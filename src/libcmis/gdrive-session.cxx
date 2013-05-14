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

#include "oauth2-handler.hxx"
#include "gdrive-session.hxx"
#include "gdrive-document.hxx"
#include "gdrive-folder.hxx"
#include "object-type.hxx"
#include "gdrive-repository.hxx"

using std::string;
using std::istringstream;

GDriveSession::GDriveSession ( string baseUrl, 
                               string username, 
                               string password, 
                               libcmis::OAuth2DataPtr oauth2, 
                               bool verbose )
                                    throw ( libcmis::Exception ) :
    BaseSession( baseUrl, string(), username, password, 
                 libcmis::OAuth2DataPtr(), verbose )

{
    if ( oauth2 && oauth2->isComplete( ) ){
        setOAuth2Data( oauth2 );
        // Add the repository
        m_repositories.push_back( getRepository( ) );
    }
}

GDriveSession::GDriveSession( const GDriveSession& copy ) :
    BaseSession( copy )
{
}

GDriveSession::GDriveSession() :
    BaseSession()
{
}

GDriveSession::~GDriveSession()
{
}

/*
 * Parse input values and redirect link from the response page
 */
int parseResponse ( const char* response, string& post, string& link )
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

/*
 * Parse the authorization code from the response page
 * in the input tag, with id = code
 */
string parseCode ( const char* response )
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

char* GDriveSession::oauth2Authenticate ( ) throw ( CurlException )
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
        throw e.getCmisException( );
    }

    string loginPost, loginLink; 
    if ( !parseResponse ( res.c_str( ), loginPost, loginLink ) ) 
        return NULL;
    
    loginPost += "Email=";  
    loginPost += string( getUsername( ) );
    loginPost += "&Passwd=";
    loginPost += string( getPassword( ) );
    
    istringstream loginIs( loginPost );
    string loginRes;
    try 
    {
        loginRes = httpPostRequest ( loginLink, loginIs, CONTENT_TYPE )
                        ->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    // STEP 2: allow libcmis to access google drive
    string approvalPost, approvalLink; 
    if ( !parseResponse( loginRes. c_str( ), approvalPost, approvalLink) )
        return NULL;
    approvalPost += "submit_access=true";

    istringstream approvalIs( approvalPost );
    string approvalRes;
    try
    {
        approvalRes = httpPostRequest ( approvalLink, approvalIs, 
                            CONTENT_TYPE) ->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    // STEP 3: Take the authentication code from the text bar
    string code = parseCode ( approvalRes.c_str( ) );
    if ( code.empty( )) 
        return NULL;
    char* authCode = new char [ code.length( ) ];
    strcpy( authCode, code.c_str( ));

    return authCode;
}

libcmis::RepositoryPtr GDriveSession::getRepository( ) 
    throw ( libcmis::Exception )
{
    // Return a dummy repository since GDrive doesn't have that notion
    libcmis::RepositoryPtr repo( new GdriveRepository( ) );
    return repo;
}

libcmis::ObjectPtr GDriveSession::getObject( string objectId )
    throw ( libcmis::Exception )
{
    // Run the http request to get the properties definition
    string res;
    string objectLink = m_bindingUrl + "/files/" + objectId;
    try
    {
        res = httpGetRequest( objectLink )->getStream()->str();
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    Json jsonRes = Json::parse( res );

    // If we have a folder, then convert the object
    // into a GDriveFolder otherwise, convert it
    // into a GDriveDocument
    libcmis::ObjectPtr object;
    string kind = jsonRes["kind"].toString( );
    if ( kind == "drive#file" )
    {
        string mimeType = jsonRes["mimeType"].toString( );
        // Folder is a file with a special mimeType
        if ( mimeType == "application/vnd.google-apps.folder" )
            object.reset( new GDriveFolder( this, jsonRes ) );
        else 
            object.reset( new GDriveDocument( this, jsonRes ) );
    } else if ( kind == "drive#revision" ) // A revision is a document too
    {
        object.reset( new GDriveDocument( this, jsonRes ) );
    }
    else // not a folder nor file, maybe a permission or changes,...
        object.reset( new GDriveObject( this, jsonRes ) );

    return object;
}

libcmis::ObjectPtr GDriveSession::getObjectByPath( string /*path*/ ) 
    throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object;
    return object;
}

libcmis::ObjectTypePtr GDriveSession::getType( string id ) 
    throw ( libcmis::Exception )
{
    libcmis::ObjectTypePtr type( new libcmis::DummyObjectType( id ) );
    return type;
}
