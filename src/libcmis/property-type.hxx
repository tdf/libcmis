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
#ifndef _PROPERTY_TYPE_HXX_
#define _PROPERTY_TYPE_HXX_

#include <boost/date_time.hpp>
#include <libxml/tree.h>

#include <string>

namespace libcmis
{
    class PropertyType
    {
        public:

            enum Type
            {
                String,
                Integer,
                Decimal,
                Bool,
                DateTime
            };

        private:

            std::string m_id;
            std::string m_localName;
            std::string m_localNamespace;
            std::string m_displayName;
            std::string m_queryName;
            Type m_type;
            std::string m_xmlType;
            bool m_multiValued;
            bool m_updatable;
            bool m_inherited;
            bool m_required;
            bool m_queryable;
            bool m_orderable;
            bool m_openChoice;
            bool m_temporary;

        public:

            /// Default constructor, mostly present for testing.
            PropertyType( );
            PropertyType( xmlNodePtr node );
            PropertyType( const PropertyType& copy );
            /// constructor for temporary type definitions
            PropertyType( std::string type,
                          std::string id,
                          std::string localName,
                          std::string displayName,
                          std::string queryName );
            virtual ~PropertyType( ) { };

            PropertyType& operator=( const PropertyType& copy );

            std::string getId( ) { return m_id; }
            std::string getLocalName( ) { return m_localName; }
            std::string getLocalNamespace( ) { return m_localNamespace; }
            std::string getDisplayName( ) { return m_displayName; }
            std::string getQueryName( ) { return m_queryName; }
            Type getType( ) { return m_type; }
            std::string getXmlType( ) { return m_xmlType; }
            bool isMultiValued( ) { return m_multiValued; }
            bool isUpdatable( ) { return m_updatable; }
            bool isInherited( ) { return m_inherited; }
            bool isRequired( ) { return m_required; }
            bool isQueryable( ) { return m_queryable; }
            bool isOrderable( ) { return m_orderable; }
            bool isOpenChoice( ) { return m_openChoice; }

            void setId( std::string id ) { m_id = id; }
            void setLocalName( std::string localName ) { m_localName = localName; }
            void setLocalNamespace( std::string localNamespace ) { m_localNamespace = localNamespace; }
            void setDisplayName( std::string displayName ) { m_displayName = displayName; }
            void setQueryName( std::string queryName ) { m_queryName = queryName; }
            void setType( Type type ) { m_type = type; }
            void setMultiValued( bool multivalued ) { m_multiValued = multivalued; }
            void setUpdatable( bool updatable ) { m_updatable = updatable; }
            void setInherited( bool inherited ) { m_inherited = inherited; }
            void setRequired( bool required ) { m_required = required; }
            void setQueryable( bool queryable ) { m_queryable = queryable; }
            void setOrderable( bool orderable ) { m_orderable = orderable; }
            void setOpenChoice( bool openChoice ) { m_openChoice = openChoice; }

            void setTypeFromXml( std::string typeStr );
            void setTypeFromJsonType( std::string jsonType );
    };
    typedef ::boost::shared_ptr< PropertyType > PropertyTypePtr;
}

#endif
