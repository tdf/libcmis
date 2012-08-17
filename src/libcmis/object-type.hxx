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
#ifndef _OBJECT_TYPE_HXX_
#define _OBJECT_TYPE_HXX_

#include <boost/shared_ptr.hpp>
#include <libxml/tree.h>

#include <string>
#include <vector>

#include "exception.hxx"
#include "property-type.hxx"

namespace libcmis
{
    /** Class representing a CMIS object type definition.
      */
    class ObjectType
    {
        public:

            enum ContentStreamAllowed
            {
                NotAllowed,
                Allowed,
                Required
            };

        protected:
            time_t m_refreshTimestamp;

            std::string m_id;
            std::string m_localName;
            std::string m_localNamespace;
            std::string m_displayName;
            std::string m_queryName;
            std::string m_description;

            std::string m_parentTypeId;
            std::string m_baseTypeId;

            bool m_creatable;
            bool m_fileable;
            bool m_queryable;
            bool m_fulltextIndexed;
            bool m_includedInSupertypeQuery;
            bool m_controllablePolicy;
            bool m_controllableAcl;
            bool m_versionable;
            libcmis::ObjectType::ContentStreamAllowed m_contentStreamAllowed;

            std::map< std::string, libcmis::PropertyTypePtr > m_propertiesTypes;

            ObjectType( );
            void initializeFromNode( xmlNodePtr node );

        public:

            ObjectType( xmlNodePtr node );
            ObjectType( const ObjectType& copy );
            virtual ~ObjectType() { }

            ObjectType& operator=( const ObjectType& copy );

            /** Reload the data from the server.

                \attention 
                    This method needs to be implemented in subclasses or it will
                    do nothing
             */
            virtual void refresh( ) throw ( Exception );
            virtual time_t getRefreshTimestamp( ) { return m_refreshTimestamp; }

            std::string getId( ) { return m_id; }
            std::string getLocalName( ) { return m_localName; }
            std::string getLocalNamespace( ) { return m_localNamespace; }
            std::string getDisplayName( ) { return m_displayName; }
            std::string getQueryName( ) { return m_queryName; }
            std::string getDescription( ) { return m_description; }

            virtual boost::shared_ptr< ObjectType >  getParentType( ) throw ( Exception );
            virtual boost::shared_ptr< ObjectType >  getBaseType( ) throw ( Exception );
            virtual std::vector< boost::shared_ptr< ObjectType > > getChildren( ) throw ( Exception );
            
            bool isCreatable( ) { return m_creatable; }
            bool isFileable( ) { return m_fileable; }
            bool isQueryable( ) { return m_queryable; }
            bool isFulltextIndexed( ) { return m_fulltextIndexed; }
            bool isIncludedInSupertypeQuery( ) { return m_includedInSupertypeQuery; }
            bool isControllablePolicy( ) { return m_controllablePolicy; }
            bool isControllableACL( ) { return m_controllableAcl; }
            bool isVersionable( ) { return m_versionable; }
            ContentStreamAllowed getContentStreamAllowed( ) { return m_contentStreamAllowed; }

            std::map< std::string, PropertyTypePtr >& getPropertiesTypes( ) { return m_propertiesTypes; }

            virtual std::string toString( ); 
    };

    typedef ::boost::shared_ptr< ObjectType > ObjectTypePtr;
}

#endif
