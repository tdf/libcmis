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

#ifndef _RENDITION_HXX_
#define _RENDITION_HXX_

#include <string>

#include <boost/shared_ptr.hpp>
#include <libxml/tree.h>

namespace libcmis
{  
    class Rendition
    {
        private:
            Rendition( );
    
            std::string m_streamId;
            std::string m_mimeType;
            std::string m_kind;
            std::string m_href;
            std::string m_title;
            long m_length;
            long m_width;
            long m_height;
            std::string m_renditionDocumentId;

        public:
            Rendition( std::string streamId, std::string mimeType, 
                       std::string kind, std::string href,
                       std::string title = std::string( ),
                       long length = -1, long width = -1, long height = -1,
                       std::string renditionDocumentId = std::string( ) );

            /** Parse an XML node of type cmisRenditionType
              */
            Rendition( xmlNodePtr node );
            ~Rendition( );
            
            bool isThumbnail( );

            const std::string& getStreamId( ) const;
            const std::string& getMimeType( ) const;
            const std::string& getKind( ) const;
            const std::string& getUrl( ) const;
            const std::string& getTitle( ) const;

            /** Provides the stream length in bytes or a negative value if missing.
              */
            long getLength( ) const;
            long getWidth( ) const;
            long getHeight( ) const;
            const std::string& getRenditionDocumentId( );

            std::string toString( );
    };

    typedef boost::shared_ptr< Rendition > RenditionPtr;
}

#endif

