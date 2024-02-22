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
 * Copyright (C) 2014 Mihai Varga <mihai.mv13@gmail.com>
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

#include "sharepoint-document.hxx"

#include "sharepoint-session.hxx"
#include "sharepoint-utils.hxx"
#include "json-utils.hxx"

using namespace std;
using namespace libcmis;

SharePointDocument::SharePointDocument( SharePointSession* session ) :
    libcmis::Object( session),
    SharePointObject( session )
{
}

SharePointDocument::SharePointDocument( SharePointSession* session, Json json, string parentId, string name ) :
    libcmis::Object( session),
    SharePointObject( session, json, parentId, name )
{
}

SharePointDocument::~SharePointDocument( )
{
}

vector< libcmis::FolderPtr > SharePointDocument::getParents( ) 
{
    vector< libcmis::FolderPtr > parents;

    string parentId = getStringProperty( "cmis:parentId" );
   
    libcmis::ObjectPtr obj = getSession( )->getObject( parentId );
    libcmis::FolderPtr parent = boost::dynamic_pointer_cast< libcmis::Folder >( obj );
    parents.push_back( parent );
    return parents;
}

boost::shared_ptr< istream > SharePointDocument::getContentStream( string /*streamId*/ )
{
    boost::shared_ptr< istream > stream;
    // file uri + /$value
    string streamUrl = getId( ) + "/%24value";
    try
    {
        stream = getSession( )->httpGetRequest( streamUrl )->getStream( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    return stream;
}

void SharePointDocument::setContentStream( boost::shared_ptr< ostream > os, 
                                           string contentType, 
                                           string /*fileName*/, 
                                           bool /*overwrite*/ ) 
{
    if ( !os.get( ) )
        throw libcmis::Exception( "Missing stream" );

     // file uri + /$value
    string putUrl = getId( ) + "/%24value";
    // Upload stream
    boost::shared_ptr< istream> is ( new istream ( os->rdbuf( ) ) );
    vector <string> headers;
    headers.push_back( string( "Content-Type: " ) + contentType );
    try
    {
        getSession()->httpPutRequest( putUrl, *is, headers );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    long httpStatus = getSession( )->getHttpStatus( );
    if ( httpStatus < 200 || httpStatus >= 300 )
    {
        throw libcmis::Exception( "Document content wasn't set for"
                "some reason" );
    }
    refresh( );
}

libcmis::DocumentPtr SharePointDocument::checkOut( )
{
    istringstream is( "" );
    string url = getId( ) + "/checkout";
    try 
    {   
        getSession( )->httpPostRequest( url, is, "" );
    }
    catch ( const CurlException& e )
    {   
        throw e.getCmisException( );
    }
    libcmis::ObjectPtr obj = getSession( )->getObject( getId( ) );
    libcmis::DocumentPtr checkout =
        boost::dynamic_pointer_cast< libcmis::Document > ( obj );
    return checkout;
}

void SharePointDocument::cancelCheckout( )
{
    istringstream is( "" );
    string url = getId( ) + "/undocheckout";
    try 
    {   
        getSession( )->httpPostRequest( url, is, "" );
    }
    catch ( const CurlException& e )
    {   
        throw e.getCmisException( );
    }
}

libcmis::DocumentPtr SharePointDocument::checkIn( bool isMajor, 
                                                  std::string comment,
                                                  const PropertyPtrMap& /*properties*/,
                                                  boost::shared_ptr< std::ostream > stream,
                                                  std::string contentType, 
                                                  std::string fileName ) 
{     
    setContentStream( stream, contentType, fileName );
    comment = libcmis::escape( comment );
    string url = getId( ) + "/checkin(comment='" + comment + "'";
    if ( isMajor )
    {
        url += ",checkintype=1)";
    }
    else
    {
        url += ",checkintype=0)";
    }
    istringstream is( "" );
    try 
    {   
        getSession( )->httpPostRequest( url, is, "" );
    }
    catch ( const CurlException& e )
    {   
        throw e.getCmisException( );
    }

    libcmis::ObjectPtr obj = getSession( )->getObject( getId( ) );
    libcmis::DocumentPtr checkin =
        boost::dynamic_pointer_cast< libcmis::Document > ( obj );
    return checkin;
}

vector< libcmis::DocumentPtr > SharePointDocument::getAllVersions( ) 
{   
    string res;
    string url = getStringProperty( "Versions" );
    vector< libcmis::DocumentPtr > allVersions;
    try
    {
        res = getSession( )->httpGetRequest( url )->getStream( )->str( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }

    // adding the latest version
    libcmis::ObjectPtr obj = getSession( )->getObject( getId( ) );
    libcmis::DocumentPtr doc =
        boost::dynamic_pointer_cast< libcmis::Document > ( obj );
    allVersions.push_back( doc );

    Json jsonRes = Json::parse( res );
    Json::JsonVector objs = jsonRes["d"]["results"].getList( );
    for ( unsigned int i = 0; i < objs.size( ); i++) 
    {
        string versionNumber = objs[i]["ID"].toString( );
        string versionId = getId( ) + "/Versions(" + versionNumber + ")";
        obj = getSession( )->getObject( versionId );
        doc = boost::dynamic_pointer_cast< libcmis::Document > ( obj );
        allVersions.push_back( doc );
    }

    return allVersions;
}
