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

#include "document.hxx"
#include "folder.hxx"

using namespace std;
using libcmis::PropertyPtrMap;

namespace libcmis
{
    vector< string > Document::getPaths( )
    {
        vector< string > paths;
        try
        {
            vector< libcmis::FolderPtr > parents = getParents( );
            for ( vector< libcmis::FolderPtr >::iterator it = parents.begin( );
                 it != parents.end(); ++it )
            {
                string path = ( *it )->getPath( );
                if ( path[path.size() - 1] != '/' )
                    path += "/";
                path += getName( );
                paths.push_back( path );
            }
        }
        catch ( const libcmis::Exception& )
        {
            // We may not have the permission to get the parents
        }
        return paths;
    }

    string Document::getContentType( )
    {
        return getStringProperty( "cmis:contentStreamMimeType" );
    }

    string Document::getContentFilename( )
    {
        return getStringProperty( "cmis:contentStreamFileName" );
    }

    long Document::getContentLength( )
    {
        long contentLength = 0;
        PropertyPtrMap::const_iterator it = getProperties( ).find( string( "cmis:contentStreamLength" ) );
        if ( it != getProperties( ).end( )  && it->second != NULL && !it->second->getLongs( ).empty( ) )
            contentLength = it->second->getLongs( ).front( );
        return contentLength;
    }

    string Document::toString( )
    {
        stringstream buf;

        buf << "Document Object:" << endl << endl;
        buf << Object::toString();
        try
        {
            vector< libcmis::FolderPtr > parents = getParents( );
            buf << "Parents ids: ";
            for ( vector< libcmis::FolderPtr >::iterator it = parents.begin(); it != parents.end(); ++it )
                buf << "'" << ( *it )->getId( ) << "' ";
            buf << endl;
        }
        catch ( const libcmis::Exception& )
        {
        }
        buf << "Content Type: " << getContentType( ) << endl;
        buf << "Content Length: " << getContentLength( ) << endl;
        buf << "Content Filename: " << getContentFilename( ) << endl;
        
        vector< libcmis::Rendition > renditions = getRenditions( );
        buf << "Renditions: " << endl;
        for ( vector< libcmis::Rendition >::iterator it = renditions.begin(); 
               it != renditions.end(); ++it )
        {
            buf << " ID: " << ( *it ).getStreamId( ) << endl;
            buf << " Kind: " << ( *it ).getKind( ) << endl;
            buf << " MimeType: " << ( *it ).getMimeType( ) << endl;
            buf << " URL: " << ( *it ).getUrl( ) << endl << endl;
        }

        return buf.str();
    }
}
