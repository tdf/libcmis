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
#ifndef _LIBCMIS_TEST_DUMMIES_HXX_
#define _LIBCMIS_TEST_DUMMIES_HXX_


#include <libcmis/allowable-actions.hxx>
#include <libcmis/object-type.hxx>
#include <libcmis/property-type.hxx>
#include <libcmis/repository.hxx>

/** This namespace contains dummy classes to simulate the libcmis layer
    in the libcmis-c unit tests.
  */
namespace dummies
{
    class Repository : public libcmis::Repository
    {
        public:
            Repository( );
            ~Repository( );
    };

    class PropertyType : public libcmis::PropertyType
    {
        public:
            PropertyType( std::string id, std::string xmlType );
            ~PropertyType( );
    };

    /** Dummy for testing the C API for allowable actions. The dummy has only the
        following actions defined:
        \li \c GetProperties, defined to \c true
        \li \c GetFolderParent, defined to \c false
      */
    class AllowableActions : public libcmis::AllowableActions
    {
        public:
            AllowableActions( );
            ~AllowableActions( );
    };

    class ObjectType : public libcmis::ObjectType
    {
        private:
            std::string m_id;
            std::string m_parentId;
            std::string m_baseId;
            std::vector< std::string > m_childrenIds;
            bool m_triggersFaults;
            std::map< std::string, libcmis::PropertyTypePtr > m_propertyTypes;

            ObjectType( );

        public:
            ObjectType( bool rootType, bool triggersFaults );
            ~ObjectType( );

            virtual std::string getId( );
            virtual std::string getLocalName( );
            virtual std::string getLocalNamespace( );
            virtual std::string getDisplayName( );
            virtual std::string getQueryName( );
            virtual std::string getDescription( );

            virtual boost::shared_ptr< libcmis::ObjectType >  getParentType( ) throw ( libcmis::Exception );
            virtual boost::shared_ptr< libcmis::ObjectType >  getBaseType( ) throw ( libcmis::Exception );
            virtual std::vector< boost::shared_ptr< libcmis::ObjectType > > getChildren( ) throw ( libcmis::Exception );
            
            virtual bool isCreatable( ) { return true; }
            virtual bool isFileable( ) { return true; }
            virtual bool isQueryable( ) { return true; }
            virtual bool isFulltextIndexed( ) { return true; }
            virtual bool isIncludedInSupertypeQuery( ) { return true; }
            virtual bool isControllablePolicy( ) { return true; }
            virtual bool isControllableACL( ) { return true; }
            virtual bool isVersionable( ) { return true; }
            virtual ContentStreamAllowed getContentStreamAllowed( ) { return libcmis::ObjectType::Allowed; }

            virtual std::map< std::string, libcmis::PropertyTypePtr >& getPropertiesTypes( );

            virtual std::string toString( );
    };
}

#endif
