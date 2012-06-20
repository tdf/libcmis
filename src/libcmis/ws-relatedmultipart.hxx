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
#ifndef _WS_RELATEDMULTIPART_HXX_
#define _WS_RELATEDMULTIPART_HXX_

#include <exception>
#include <map>
#include <string>
#include <sstream>
#include <vector>

#include <boost/shared_ptr.hpp>

class RelatedPart
{
    private:
        std::string m_name;
        std::string m_contentType;
        std::string m_content;

    public:
        RelatedPart( std::string& name, std::string& type, std::string& content );
        ~RelatedPart( ) { };

        std::string getName( ) { return m_name; }
        std::string getContentType( ) { return m_contentType; }
        std::string getContent( ) { return m_content; }

        /** Create the string to place between the boundaries in the multipart.

            \param cid the content Id to output
          */
        std::string toString( std::string cid );
};
typedef boost::shared_ptr< RelatedPart > RelatedPartPtr;

/** Represents a multipart/related content as specified by RFC-2387.
  */
class RelatedMultipart
{
    private:

        std::string m_startId;
        std::string m_startInfo;
        std::map< std::string, RelatedPartPtr > m_parts;
        std::string m_boundary;

    public:

        /** Create a multipart/related from scratch (most probably
            to output it later).
          */
        RelatedMultipart( );

        /** Parse a multipart body to extract the entries from it.
          */
        RelatedMultipart( const std::string& body, const std::string& contentType );

        /** Get the Content ids of all the parts;
          */
        std::vector< std::string > getIds( );

        /** Get the entry corresponding to the given ID.
          */
        RelatedPartPtr getPart( std::string& cid );

        /** Get the id of the multipart start entry.
          */
        std::string& getStartId( ) { return m_startId; }

        std::string& getStartInfo( ) { return m_startInfo; }

        /** Add an entry to the multipart and returns the content ID that
            was created for it.
          */
        std::string addPart( RelatedPartPtr part );

        /** Define the start of the multipart. That method needs to be
            called before running toString(): the output order of the entries
            is not guaranteed.

            \param cid the Content-Id of the start entry
            \param startInfo the type to use as start-info in the Content-Type
          */
        void setStart( std::string& cid, std::string& startInfo );

        /** Compute the content type for the multipart object to set as the
            Content-Type HTTP header.
          */
        std::string getContentType( );

        /** Dump the multipart to an input stream: this can be provided as is as
            an HTTP post request body.
          */
        boost::shared_ptr< std::istringstream > toString( );

        /** Provide an access to the boundary token for the unit tests.
          */
        std::string getBoundary( ) { return m_boundary; }

    private:

        /** Generate a content id, using an entry name and some random uuid.
          */
        std::string createPartId( const std::string& name );
};

#endif
