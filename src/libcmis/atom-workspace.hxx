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
 * Copyright (C) 2011 Cédric Bosdonnat <cbosdo@users.sourceforge.net>
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
#ifndef _ATOM_WORKSPACE_HXX_
#define _ATOM_WORKSPACE_HXX_

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>
#include <curl/curl.h>
#include <libxml/xpath.h>

#include "exception.hxx"
#include "repository.hxx"

#define URI_TEMPLATE_VAR_ID std::string( "id" )
#define URI_TEMPLATE_VAR_PATH std::string( "path" )

struct Collection {
    enum Type
    {
        Root,
        Types,
        Query,
        CheckedOut,
        Unfiled
    };
};

struct UriTemplate {
    enum Type
    {
        ObjectById,
        ObjectByPath,
        TypeById,
        Query
    };
};

class AtomRepository : public libcmis::Repository
{
    private:
        std::string m_id;
        std::string m_rootId;

        /// Collections URLs
        std::map< Collection::Type, std::string > m_collections;

        /// URI templates
        std::map< UriTemplate::Type, std::string > m_uriTemplates;

    public:
        AtomRepository( xmlNodePtr wsNode = NULL ) throw ( libcmis::Exception );
        AtomRepository( const AtomRepository& rCopy );
        ~AtomRepository( );

        AtomRepository& operator= ( const AtomRepository& rCopy );

        std::string getCollectionUrl( Collection::Type );
        std::string getUriTemplate( UriTemplate::Type );

        // Repository methods

        virtual std::string getRootId( ) { return m_rootId; }
        virtual std::string getId( ) { return m_id; }

    private:
        void readCollections( xmlNodeSetPtr pNodeSet );
        void readUriTemplates( xmlNodeSetPtr pNodeSet );
};

typedef boost::shared_ptr< AtomRepository > AtomRepositoryPtr;

#endif
