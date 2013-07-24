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

#include "gdrive-document.hxx"
#include "gdrive-folder.hxx"
#include "gdrive-session.hxx"
#include "json-utils.hxx"
#include "rendition.hxx"
#include "gdrive-utils.hxx"

using namespace std;
using namespace libcmis;

GDriveDocument::GDriveDocument( GDriveSession* session ) :
    libcmis::Object( session),
    libcmis::Document( session ),
    GDriveObject( session ),
    m_isGoogleDoc( false )
{
}

GDriveDocument::GDriveDocument( GDriveSession* session, Json json, string id, string name ) :
    libcmis::Object( session),
    libcmis::Document( session ),
    GDriveObject( session, json, id, name ),
    m_isGoogleDoc( false )
{
    m_isGoogleDoc = getContentType( ).find( "google" ) != string::npos;
    getRenditions( );   
}

GDriveDocument::~GDriveDocument( )
{
}

string GDriveDocument::getDownloadUrl( string streamId )
{
    string streamUrl;
    vector< RenditionPtr > renditions = getRenditions( );    
    
    if ( renditions.empty( ) )
        return streamUrl;    

    if ( !streamId.empty( ) )
    {
        // Find the rendition associated with the streamId
        for ( vector< RenditionPtr >::iterator it = renditions.begin( ) ; 
            it != renditions.end(); ++it )
        {
            if ( (*it)->getStreamId( ) == streamId )
            {
                streamUrl = (*it)->getUrl( );
                break;
            }
        }
    }
    else
    {
        // Automatically find the rendition

        // Prefer ODF format
        for ( vector< RenditionPtr >::iterator it = renditions.begin( ) ; 
            it != renditions.end(); ++it )
            if ( (*it)->getMimeType( ).find( "opendocument") != string::npos )
                return (*it)->getUrl( );

        // Then MS format
        for ( vector< RenditionPtr >::iterator it = renditions.begin( ) ; 
            it != renditions.end(); ++it )
            if ( (*it)->getMimeType( ).find( "officedocument") != string::npos )
                return (*it)->getUrl( );

        // If not found, take the first one
        streamUrl = renditions.front( )->getUrl( );

    }

    return streamUrl;
}

vector< libcmis::FolderPtr > GDriveDocument::getParents( ) 
    throw ( libcmis::Exception )
{
    vector< libcmis::FolderPtr > parents;

    vector< string > parentsId = getMultiStringProperty( "cmis:parentId" );
   
    // Create folder objects from parent IDs
    for ( vector< string >::iterator it = parentsId.begin( ); it != parentsId.end( ); ++it)
	{
        string parentId = ( *it );
		libcmis::ObjectPtr obj = getSession( )->getObject( parentId );
        libcmis::FolderPtr parent = boost::dynamic_pointer_cast< libcmis::Folder >( obj );
        parents.push_back( parent );
	}
    return parents;
}

boost::shared_ptr< istream > GDriveDocument::getContentStream( string streamId )
    throw ( libcmis::Exception )
{
    boost::shared_ptr< istream > stream;
    string streamUrl = getDownloadUrl( streamId );
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

void GDriveDocument::uploadStream( boost::shared_ptr< ostream > os, 
                                   string contentType )
                                throw ( libcmis::Exception )
{
    if ( !os.get( ) )
        throw libcmis::Exception( "Missing stream" );
    if ( !isImmutable( ) )
        throw libcmis::Exception( string ( "Document " + getId( )+ 
                                    " is not editable" ) );
    string putUrl = getUploadUrl( ) + getId( );
    
    // If it's a Google document, convert it 
    if ( isGoogleDoc( ) )
        putUrl  += "?convert=true";

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
        throw libcmis::Exception( "Document content wasn't set for"
                "some reason" );
    refresh( );
}

void GDriveDocument::setContentStream( boost::shared_ptr< ostream > os, 
                                       string contentType, 
                                       string fileName, 
                                       bool /*overwrite*/ ) 
                                            throw ( libcmis::Exception )
{
    if ( !os.get( ) )
        throw libcmis::Exception( "Missing stream" );
    
    if ( !isImmutable( ) )
        throw libcmis::Exception( string ( "Document " + getId( )+ 
                                    " is not editable" ) );
    string metaUrl = getUrl( );

    // If it's a Google document, convert it 
    if ( isGoogleDoc( ) )
        metaUrl += "?convert=true";

    // Update file name meta information
    if ( !fileName.empty( ) && fileName != getContentFilename( ) )
    {
        Json metaJson;
        Json fileJson( fileName.c_str( ) );
        metaJson.add("title", fileJson );

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

    // Upload stream
    uploadStream( os, contentType );
}

libcmis::DocumentPtr GDriveDocument::checkOut( ) throw ( libcmis::Exception )
{
    // GDrive doesn't have CheckOut, so just return the same document here
    libcmis::ObjectPtr obj = getSession( )->getObject( getId( ) );
    libcmis::DocumentPtr checkout =
        boost::dynamic_pointer_cast< libcmis::Document > ( obj );
    return checkout;
}

void GDriveDocument::cancelCheckout( ) throw ( libcmis::Exception )
{
    // Don't do anything since we don't have CheckOut
}

libcmis::DocumentPtr GDriveDocument::checkIn( 
    bool /*isMajor*/, 
    std::string /*comment*/,
    const PropertyPtrMap& properties,
    boost::shared_ptr< std::ostream > stream,
    std::string contentType, 
    std::string fileName ) 
        throw ( libcmis::Exception )
{     
    // GDrive doesn't have CheckIn, so just upload the properties, 
    // the content stream and fetch the new document resource.
    updateProperties( properties );
    setContentStream( stream, contentType, fileName );
    libcmis::ObjectPtr obj = getSession( )->getObject( getId( ) );
    libcmis::DocumentPtr checkin = 
        boost::dynamic_pointer_cast< libcmis::Document > ( obj );
    return checkin;
}
        

vector< libcmis::DocumentPtr > GDriveDocument::getAllVersions( ) 
    throw ( libcmis::Exception )
{   
    vector< libcmis::DocumentPtr > revisions;
    string versionUrl = getUrl( ) + "/revisions";
    // Run the http request to get the properties definition
    string res;
    try
    {
        res = getSession()->httpGetRequest( versionUrl )->getStream()->str();
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    Json jsonRes = Json::parse( res );        
    Json::JsonVector objs = jsonRes["items"].getList( );
   
    string parentId = getStringProperty( "cmis:parentId" );

    // Create document objects from Json objects
    for(unsigned int i = 0; i < objs.size(); i++)
	{     
        objs[i].add( "parents", GdriveUtils::createJsonFromParentId( parentId ) );
		libcmis::DocumentPtr revision( 
            new GDriveDocument( getSession(), objs[i], getId( ), getName( ) ) );
        
        revisions.push_back( revision );
	}
    return revisions;
}

