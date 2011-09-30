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
#ifndef _ATOM_CONTENT_HXX_
#define _ATOM_CONTENT_HXX_

#include <string>

#include "content.hxx"
#include "atom-object.hxx"

class AtomContent : public Content, public AtomCmisObject
{
    private:
        std::string m_contentUrl;
        std::string m_contentType;
        std::string m_contentFilename;
        long m_contentLength;

    public:
        AtomContent( AtomPubSession* session, std::string url );
        AtomContent( AtomPubSession* session, xmlNodePtr entryNd );
        ~AtomContent( );

        // Override content methods
        virtual void getContent( size_t (*pCallback)( void *, size_t, size_t, void* ), void* userData );
        virtual std::string getContentType( ) { return m_contentType; }
        virtual std::string getContentFilename( ) { return m_contentFilename; }
        virtual long getContentLength( ){ return m_contentLength; }
        
        virtual std::string toString( );
    
    protected:
        virtual void extractInfos( xmlDocPtr doc );
};

#endif
