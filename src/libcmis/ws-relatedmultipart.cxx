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
 * Copyright (C) 2011 SUSE <cbosdonnat@suse.com>
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

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "ws-relatedmultipart.hxx"
#include "xml-utils.hxx"

using namespace std;
using namespace boost::uuids;

RelatedPart::RelatedPart( string& name, string& type, string& content ) :
    m_name( name ),
    m_contentType( type ),
    m_content( content )
{
}

string RelatedPart::toString( string cid )
{
    string buf;

    buf += "Content-Id: " + cid + "\r\n";
    buf += "Content-Type: " + getContentType( ) + "\r\n";
    buf += "Content-Transfer-Encoding: binary\r\n\r\n";
    buf += getContent( );

    return buf;
}

RelatedMultipart::RelatedMultipart( ) :
    m_startId( ),
    m_startInfo( ),
    m_parts( ),
    m_boundary( )
{
    uuid uuid = random_generator()();
    m_boundary = "--------uuid:" + to_string( uuid );
}

RelatedMultipart::RelatedMultipart( const string& body, const string& contentType ) :
    m_startId( ),
    m_startInfo( ),
    m_parts( ),
    m_boundary( )
{
    // Parse the content-type
    size_t lastPos = 0;
    size_t pos = contentType.find_first_of( ";\"" );
    while ( pos != string::npos )
    {
        bool escaped = contentType[pos] == '"';
        if ( escaped )
        {
            // Look for the closing quote and then look for the ; after it
            pos = contentType.find( "\"", pos + 1 );
            pos = contentType.find( ";", pos + 1 );
        }
       
        string param = contentType.substr( lastPos, pos - lastPos );
        size_t eqPos = param.find( "=" );
        if ( eqPos != string::npos )
        {
            string name = param.substr( 0, eqPos );
            string value = param.substr( eqPos + 1 );
            if ( value[0] == '"' && value[value.length() - 1] == '"' )
                value = value.substr( 1, value.length( ) - 2 );

            if ( name == "start" )
                m_startId = value;
            else if ( name == "boundary" )
                m_boundary = value;
            else if ( name == "start-info" )
                m_startInfo = value;
        }

        if ( pos != string::npos )
        {
            lastPos = pos + 1;
            pos = contentType.find_first_of( ";\"", lastPos );
        }
    }

    // Parse the multipart
    string bodyFixed( body );
    if ( bodyFixed.find( "--" + m_boundary + "\r\n" ) == 0 )
        bodyFixed = "\r\n" + bodyFixed;
    
    string boundaryString( "\r\n--" + m_boundary + "\r\n" );
    string endBoundaryString( "\r\n--" + m_boundary + "--" );
    string headerSeparator( "\r\n\r\n" );
    lastPos = 0;
    pos = bodyFixed.find( boundaryString );

    while ( pos != string::npos )
    {
        string part = bodyFixed.substr( lastPos, pos - lastPos );

        size_t bodyPos = part.find( headerSeparator );
        if ( bodyPos != string::npos )
        { 
            string headers = part.substr( 0, bodyPos );
            string partBody = part.substr( bodyPos + headerSeparator.length( ) );

            string cid;
            string type;

            do
            {
                string headerSep( "\r\n" );
                size_t headerEndPos = headers.find( headerSep );
                string header = headers.substr( 0, headerEndPos );
                if ( headerEndPos != string::npos )
                    headers = headers.substr( headerEndPos + headerSep.length( ) );
                else
                    headers.clear( );

                size_t colonPos = header.find( ":" );
                string headerName = header.substr( 0, colonPos );
                string headerValue = header.substr( colonPos + 1 );
                if ( headerName == "Content-Id" )
                    cid = libcmis::trim( headerValue );
                else if ( headerName == "Content-Type" )
                    type = libcmis::trim( headerValue );
                // TODO Handle the Content-Transfer-Encoding
            }
            while ( !headers.empty( ) );

            if ( !cid.empty() && !type.empty( ) )
            {
                string name;
                RelatedPartPtr relatedPart( new RelatedPart( name, type, partBody ) );
                m_parts[cid] = relatedPart;
            }
        }

        lastPos = pos + boundaryString.length( );
        pos = bodyFixed.find( boundaryString, lastPos );
        if ( pos == string::npos )
            pos = bodyFixed.find( endBoundaryString, lastPos );
    }
}

vector< string > RelatedMultipart::getIds( )
{
    vector< string > ids;

    for ( map< string, RelatedPartPtr >::iterator it = m_parts.begin( );
            it != m_parts.end( ); ++it )
    {
        ids.push_back( it->first );
    }

    return ids;
}

RelatedPartPtr RelatedMultipart::getPart( string& cid )
{
    RelatedPartPtr part;
    map< string, RelatedPartPtr >::iterator it = m_parts.find( cid );
    if ( it != m_parts.end( ) )
        part = it->second;

    return part;
}

string RelatedMultipart::addPart( RelatedPartPtr part )
{
    string cid = createPartId( part->getName( ) );
    m_parts[cid] = part;
    return cid;
}

void RelatedMultipart::setStart( string& cid, string& startInfo )
{
    RelatedPartPtr start = getPart( cid );

    if ( start.get( ) != NULL )
    {
        m_startId = cid;
        m_startInfo = startInfo;
    }
}

string RelatedMultipart::getContentType( )
{
    string type = "multipart/related;";

    RelatedPartPtr start = getPart( getStartId( ) );
    if ( start.get( ) != NULL )
    {
        type += "start=\"" + getStartId( ) + "\";";

        string startType = start->getContentType( );
        size_t pos = startType.find( ";" );
        if ( pos != string::npos )
            startType = startType.substr( 0, pos );

        type += "type=\"" + startType + "\";";
    }
    type += "boundary=\"" + m_boundary + "\";";
    type += "start-info=\"" + m_startInfo + "\"";

    return type;
}

boost::shared_ptr< istringstream > RelatedMultipart::toStream( )
{
    string buf;

    // Output the start part first
    buf += "\r\n--" + m_boundary + "\r\n";
    RelatedPartPtr part = getPart( getStartId( ) );
    if ( part.get( ) != NULL )
    {
        buf += part->toString( getStartId( ) );
    }

    for ( map< string, RelatedPartPtr >::iterator it = m_parts.begin( );
            it != m_parts.end( ); ++it )
    {
        if ( it->first != getStartId( ) )
        {
            buf += "\r\n--" + m_boundary + "\r\n";
            buf += it->second->toString( it->first );
        }
    }

    buf += "\r\n--" + m_boundary + "--\r\n";

    boost::shared_ptr< istringstream > is( new istringstream( buf ) );
    return is;
}

string RelatedMultipart::createPartId( const string& name )
{
    string cid( name + "*" );

    // Generate the UUID part of the id
    uuid uuid = random_generator()();
    cid += to_string( uuid );

    cid += "@libcmis.sourceforge.net";

    return cid;
}

boost::shared_ptr< istream > getStreamFromNode( xmlNodePtr node, RelatedMultipart& multipart )
{
    boost::shared_ptr< stringstream > stream;
    for ( xmlNodePtr child = node->children; child; child = child->next )
    {
        if ( xmlStrEqual( child->name, BAD_CAST( "Include" ) ) )
        {
            // Get the content from the multipart
            xmlChar* value = xmlGetProp( child, BAD_CAST( "href" ) );
            string href( ( char* )value );
            xmlFree( value );
            // Get the Content ID from the href (cid:content-id)
            string id = href;
            if ( href.substr( 0, 4 ) == "cid:" )
                id = href.substr( 4 );
            RelatedPartPtr part = multipart.getPart( id );
            if ( part != NULL )
                stream.reset( new stringstream( part->getContent( ) ) );
        }
    }

    // If there was no xop:Include, then use the content as base64 data
    if ( stream.get( ) == NULL )
    {
        xmlChar* content = xmlNodeGetContent( node );

        stream.reset( new stringstream( ) );
        libcmis::EncodedData decoder( stream.get( ) );
        decoder.setEncoding( "base64" );
        decoder.decode( ( void* )content, 1, xmlStrlen( content ) );
        decoder.finish( );
        
        xmlFree( content );
    }
    return stream;
}
