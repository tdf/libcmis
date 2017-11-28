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

#include "onedrive-document.hxx"
#include "onedrive-folder.hxx"
#include "onedrive-session.hxx"
#include "onedrive-utils.hxx"
#include "json-utils.hxx"
#include "rendition.hxx"

using namespace std;
using namespace libcmis;

OneDriveDocument::OneDriveDocument( OneDriveSession* session ) :
    libcmis::Object( session),
    libcmis::Document( session ),
    OneDriveObject( session )
{
}

OneDriveDocument::OneDriveDocument( OneDriveSession* session, Json json, string id, string name ) :
    libcmis::Object( session),
    libcmis::Document( session ),
    OneDriveObject( session, json, id, name )
{
}

OneDriveDocument::~OneDriveDocument( )
{
}

vector< libcmis::FolderPtr > OneDriveDocument::getParents( ) 
{
    vector< libcmis::FolderPtr > parents;

    string parentId = getStringProperty( "cmis:parentId" );
   
    libcmis::ObjectPtr obj = getSession( )->getObject( parentId );
    libcmis::FolderPtr parent = boost::dynamic_pointer_cast< libcmis::Folder >( obj );
    parents.push_back( parent );
    return parents;
}

boost::shared_ptr< istream > OneDriveDocument::getContentStream( string /*streamId*/ )
{
    boost::shared_ptr< istream > stream;
    string streamUrl = getStringProperty( "source" );
    if ( streamUrl.empty( ) )
        throw libcmis::Exception( "can not found stream url" );

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

void OneDriveDocument::setContentStream( boost::shared_ptr< ostream > os, 
                                         string /*contentType*/, 
                                         string fileName, 
                                         bool /*overwrite*/ ) 
{
    if ( !os.get( ) )
        throw libcmis::Exception( "Missing stream" );
    
    string metaUrl = getUrl( );

    // Update file name meta information
    if ( !fileName.empty( ) && fileName != getContentFilename( ) )
    {
        Json metaJson;
        Json fileJson( fileName.c_str( ) );
        metaJson.add("name", fileJson );

        std::istringstream is( metaJson.toString( ) );
        vector<string> headers;
        headers.push_back( "Content-Type: application/json" );
        try
        {
            getSession()->httpPutRequest( metaUrl, is, headers );
        }
        catch ( const CurlException& e )
        {
            throw e.getCmisException( );
        }
    }

    fileName = libcmis::escape( getStringProperty( "cmis:name" ) );
    string putUrl = getSession( )->getBindingUrl( ) + "/" + 
                    getStringProperty( "cmis:parentId" ) + "/files/" +
                    fileName + "?overwrite=true";
    
    // Upload stream
    boost::shared_ptr< istream> is ( new istream ( os->rdbuf( ) ) );
    vector <string> headers;
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
        throw libcmis::Exception( "Document content wasn't set for"
                "some reason" );
    refresh( );
}

libcmis::DocumentPtr OneDriveDocument::checkOut( )
{
    // OneDrive doesn't have CheckOut, so just return the same document here
    libcmis::ObjectPtr obj = getSession( )->getObject( getId( ) );
    libcmis::DocumentPtr checkout =
        boost::dynamic_pointer_cast< libcmis::Document > ( obj );
    return checkout;
}

void OneDriveDocument::cancelCheckout( )
{
    // Don't do anything since we don't have CheckOut
}

libcmis::DocumentPtr OneDriveDocument::checkIn( bool /*isMajor*/, 
                                                std::string /*comment*/,
                                                const PropertyPtrMap& properties,
                                                boost::shared_ptr< std::ostream > stream,
                                                std::string contentType, 
                                                std::string fileName ) 
{     
    // OneDrive doesn't have CheckIn, so just upload the properties, 
    // the content stream and fetch the new document resource.
    updateProperties( properties );
    setContentStream( stream, contentType, fileName );
    libcmis::ObjectPtr obj = getSession( )->getObject( getId( ) );
    libcmis::DocumentPtr checkin = 
        boost::dynamic_pointer_cast< libcmis::Document > ( obj );
    return checkin;
}

vector< libcmis::DocumentPtr > OneDriveDocument::getAllVersions( ) 
{   
    return vector< libcmis::DocumentPtr > ( );
}
