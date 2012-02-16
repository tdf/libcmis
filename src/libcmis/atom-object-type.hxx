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
#ifndef _ATOM_OBJECT_TYPE_HXX_
#define _ATOM_OBJECT_TYPE_HXX_

#include <libxml/tree.h>

#include "atom-session.hxx"
#include "object-type.hxx"

class AtomObjectType : public libcmis::ObjectType
{
    private:
        AtomPubSession* m_session;
        time_t m_refreshTimestamp;

        std::string m_selfUrl;

        std::string m_id;
        std::string m_localName;
        std::string m_localNamespace;
        std::string m_displayName;
        std::string m_queryName;
        std::string m_description;

        std::string m_parentTypeId;
        std::string m_baseTypeId;
        std::string m_childrenUrl;

        bool m_creatable;
        bool m_fileable;
        bool m_queryable;
        bool m_fulltextIndexed;
        bool m_includedInSupertypeQuery;
        bool m_controllablePolicy;
        bool m_controllableAcl;

    public:
        AtomObjectType( AtomPubSession* session, std::string id ) throw ( libcmis::Exception );
        AtomObjectType( AtomPubSession* session, xmlNodePtr node ) throw ( libcmis::Exception );
        AtomObjectType( const AtomObjectType& copy );
        virtual ~AtomObjectType( );

        AtomObjectType& operator=( const AtomObjectType& copy );
        
        /** Reload the data from the server.
              */
        virtual void refresh( ) throw ( libcmis::Exception ) { refreshImpl( NULL ); }
        virtual time_t getRefreshTimestamp( ) { return m_refreshTimestamp; }

        virtual std::string getId( ) { return m_id; }
        virtual std::string getLocalName( ) { return m_localName; }
        virtual std::string getLocalNamespace( ) { return m_localNamespace; }
        virtual std::string getDisplayName( ) { return m_displayName; }
        virtual std::string getQueryName( ) { return m_queryName; }
        virtual std::string getDescription( ) { return m_description; }

        virtual libcmis::ObjectTypePtr getParentType( ) throw ( libcmis::Exception );
        virtual libcmis::ObjectTypePtr getBaseType( ) throw ( libcmis::Exception );
        virtual std::vector< libcmis::ObjectTypePtr > getChildren( ) throw ( libcmis::Exception );
        
        virtual bool isCreatable( ) { return m_creatable; }
        virtual bool isFileable( ) { return m_fileable; }
        virtual bool isQueryable( ) { return m_queryable; }
        virtual bool isFulltextIndexed( ) { return m_fulltextIndexed; }
        virtual bool isIncludedInSupertypeQuery( ) { return m_includedInSupertypeQuery; }
        virtual bool isControllablePolicy( ) { return m_controllablePolicy; }
        virtual bool isControllableACL( ) { return m_controllableAcl; }

        virtual std::string toString( );

    private:

        void refreshImpl( xmlDocPtr doc ) throw ( libcmis::Exception );
        void extractInfos( xmlDocPtr doc ) throw ( libcmis::Exception );
};

#endif
