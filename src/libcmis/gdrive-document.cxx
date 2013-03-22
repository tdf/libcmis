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
#include "gdrive-session.hxx"

using namespace std;

GDriveDocument::GDriveDocument( GDriveSession* session ) :
    libcmis::Object( session),
    libcmis::Document( session ),
    GDriveObject( session ),
    m_revisionId( ),
    m_isGoogleDoc( false ),
    m_downloadUrl( )
{
}

GDriveDocument::GDriveDocument( GDriveSession* session, Json json ) :
    libcmis::Object( session),
    libcmis::Document( session ),
    GDriveObject( session, json ),
    m_revisionId( ),
    m_isGoogleDoc( false ),
    m_downloadUrl( )
{
    m_downloadUrl = json["downloadUrl"].toString( );
    if ( m_downloadUrl.empty( ) )
    {
        // If it's a Google document, we download the export link and then 
        // automatically convert when upload
        Json exportLinks = json["exportLinks"];
        Json::JsonObject objs = exportLinks.getObjects( );
        Json::JsonObject::iterator it; 
        for ( it = objs.begin( ); it != objs.end( ); it++)
        { 
            string link = it->first;
            // We get the ODF link
            if ( link.find( "opendocument" ) != string::npos )
            {
                m_downloadUrl = it->second.toString( );
                break;
            }
        }
        m_isGoogleDoc = true;
    }
}

GDriveDocument::~GDriveDocument( )
{
}


vector< libcmis::FolderPtr > GDriveDocument::getParents( ) throw ( libcmis::Exception )
{
    //TODO implementation
    vector< libcmis::FolderPtr > result;
    return result;
}

boost::shared_ptr< istream > GDriveDocument::getContentStream( ) throw ( libcmis::Exception )
{
    boost::shared_ptr< istream > stream;
    try
    {
        stream = getSession( )->httpGetRequest( m_downloadUrl )->getStream( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    return stream;
}

void GDriveDocument::setContentStream( boost::shared_ptr< ostream > os, 
                                       string contentType, 
                                       string fileName, 
                                       bool /*overwrite*/ ) 
                                            throw ( libcmis::Exception )
{
    // Google define a specific URL to update media
    static const string uploadBaseUrl = 
        "https://www.googleapis.com/upload/drive/v2/files/";
    if ( !os.get( ) )
        throw libcmis::Exception( "Missing stream" );

    string putUrl = uploadBaseUrl + getId( );
    string metaUrl = getSession()->getBaseUrl() + "/files/" + getId( );

    // If we downloaded a Google document, convert it back now
    if ( m_isGoogleDoc )
    {
        putUrl  += "?convert=true";
        metaUrl += "?convert=true";
    }

    // Upload properties
   // if ( !contentType.empty( ) || !fileName.empty( ) )
    {
        string uploadStr;
        uploadStr += "{\n";
        //if ( !contentType.empty( ) )
        //    uploadStr += "\"mimeType\": \"" + contentType + "\"";
       // if ( !contentType.empty( ) && !fileName.empty( ) )
       //     uploadStr +=",\n";
        if ( !fileName.empty( ) )
            uploadStr += "\"title\": \"" + fileName + "\"";
        uploadStr += "\n}";

        std::istringstream is( uploadStr );
        vector<string> headers;
        headers.push_back( string( "Content-Type: " ) + "application/json" );
        getSession()->httpPutRequest( metaUrl, is, headers );
    }

    // Upload stream
    bool tryBase64 = false;
    do
    {
        try
        {
            boost::shared_ptr< istream> is ( new istream ( os->rdbuf( ) ) );
            if ( tryBase64 )
            {
                tryBase64 = false;

                // Encode the content
                stringstream* encodedIn = new stringstream( );
                libcmis::EncodedData encoder( encodedIn );
                encoder.setEncoding( "base64" );

                int bufLength = 1000;
                char* buf = new char[ bufLength ];
                do
                {
                    is->read( buf, bufLength );
                    int size = is->gcount( );
                    encoder.encode( buf, 1, size );
                } while ( !is->eof( ) && !is->fail( ) );
                delete[] buf;
                encoder.finish( );

                encodedIn->seekg( 0, ios_base::beg );
                encodedIn->clear( );

                is.reset( encodedIn );
            }
            vector <string> headers;
            headers.push_back( string( "Content-Type: " ) + contentType );

            getSession()->httpPutRequest( putUrl, *is, headers );

            long httpStatus = getSession( )->getHttpStatus( );
            if ( httpStatus < 200 || httpStatus >= 300 )
                throw libcmis::Exception( "Document content wasn't set for some reason" );
            refresh( );
        }
        catch ( const CurlException& e )
        {
            // Try base64 encoded content.
            if ( !tryBase64 && e.getHttpStatus() == 400 )
                tryBase64 = true;
            else
                throw e.getCmisException( );
        }
    }
    while ( tryBase64 );    

    
    
}

libcmis::DocumentPtr GDriveDocument::checkOut( ) throw ( libcmis::Exception )
{
    libcmis::DocumentPtr ptr;
    // TODO implementation
    return ptr;
}

void GDriveDocument::cancelCheckout( ) throw ( libcmis::Exception )
{
    
}

libcmis::DocumentPtr GDriveDocument::checkIn( bool /*isMajor*/, 
                                              std::string /*comment*/,
                                              const std::map< std::string, 
                                                libcmis::PropertyPtr >& /*properties*/,
                                              boost::shared_ptr< std::ostream > /*stream*/,
                                              std::string /*contentType*/, 
                                              std::string /*fileName*/ ) 
                                                throw ( libcmis::Exception )
{
    libcmis::DocumentPtr ptr;
    // TODO implementation
    return ptr;
}
        

vector< libcmis::DocumentPtr > GDriveDocument::getAllVersions( ) throw ( libcmis::Exception )
{
    //TODO implementation
    vector< libcmis::DocumentPtr > result;
    return result;
}
