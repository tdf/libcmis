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

#include <sstream>

#include "rendition.hxx"
#include "xml-utils.hxx"

using namespace std;

namespace libcmis{

Rendition::Rendition( ):
    m_streamId( ),
    m_mimeType( ),
    m_kind( ),
    m_href( ),
    m_title( ),
    m_length( -1 ),
    m_width ( -1 ),
    m_height( -1 ),
    m_renditionDocumentId( )
{   
}

Rendition::Rendition( string streamId, string mimeType, 
                      string kind, string href, string title, long length,
                      long width, long height, string renditionDocumentId ):
    m_streamId( streamId ),
    m_mimeType( mimeType ),
    m_kind( kind ),
    m_href( href ),
    m_title( title ),
    m_length( length ),
    m_width ( width ),
    m_height( height ),
    m_renditionDocumentId( renditionDocumentId )
{  
}

Rendition::Rendition( xmlNodePtr node ):
    m_streamId( ),
    m_mimeType( ),
    m_kind( ),
    m_href( ),
    m_title( ),
    m_length( -1 ),
    m_width ( -1 ),
    m_height( -1 ),
    m_renditionDocumentId( )
{
    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        xmlChar* content = xmlNodeGetContent( child );
        string value( ( char * ) content );
        xmlFree( content );

        if ( xmlStrEqual( child->name, BAD_CAST( "streamId" ) ) )
            m_streamId = value;
        else if ( xmlStrEqual( child->name, BAD_CAST( "mimetype" ) ) )
            m_mimeType = value;
        else if ( xmlStrEqual( child->name, BAD_CAST( "length" ) ) )
            m_length = libcmis::parseInteger( value );
        else if ( xmlStrEqual( child->name, BAD_CAST( "kind" ) ) )
            m_kind = value;
        else if ( xmlStrEqual( child->name, BAD_CAST( "title" ) ) )
            m_title = value;
        else if ( xmlStrEqual( child->name, BAD_CAST( "height" ) ) )
            m_height = libcmis::parseInteger( value );
        else if ( xmlStrEqual( child->name, BAD_CAST( "width" ) ) )
            m_width = libcmis::parseInteger( value );
        else if ( xmlStrEqual( child->name, BAD_CAST( "renditionDocumentId" ) ) )
            m_renditionDocumentId = value;
    }
}

Rendition::~Rendition( )
{
}

string Rendition::toString( )
{
    stringstream buf;

    if ( !getStreamId( ).empty( ) )
        buf << " ID: " << getStreamId( ) << endl;

    if ( !getKind().empty() )
        buf << " Kind: " << getKind( ) << endl;

    if ( !getMimeType( ).empty() )
        buf << " MimeType: " << getMimeType( ) << endl;

    if ( !getUrl().empty( ) )
        buf << " URL: " << getUrl( ) << endl;

    if ( !getTitle().empty( ) )
        buf << " Title: " << getTitle( ) << endl;

    if ( getLength( ) >= 0 )
        buf << " Length: " << getLength( ) << endl;

    if ( getWidth( ) >= 0 )
        buf << " Width: " << getWidth( ) << endl;

    if ( getHeight( ) >= 0 )
        buf << " Height: " << getHeight( ) << endl;

    if ( !getRenditionDocumentId().empty( ) )
        buf << " Rendition Document ID: " << getRenditionDocumentId( ) << endl;

    return buf.str( );
}

}
