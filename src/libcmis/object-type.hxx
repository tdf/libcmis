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

            virtual ~ObjectType() { };

            virtual std::string getId( ) = 0;
            virtual std::string getLocalName( ) = 0;
            virtual std::string getLocalNamespace( ) = 0;
            virtual std::string getDisplayName( ) = 0;
            virtual std::string getQueryName( ) = 0;
            virtual std::string getDescription( ) = 0;

            virtual boost::shared_ptr< ObjectType>  getParentType( ) throw ( Exception ) = 0;
            virtual boost::shared_ptr< ObjectType>  getBaseType( ) throw ( Exception ) = 0;
            virtual std::vector< boost::shared_ptr< ObjectType > > getChildren( ) throw ( Exception ) = 0;
            
            virtual bool isCreatable( ) = 0;
            virtual bool isFileable( ) = 0;
            virtual bool isQueryable( ) = 0;
            virtual bool isFulltextIndexed( ) = 0;
            virtual bool isIncludedInSupertypeQuery( ) = 0;
            virtual bool isControllablePolicy( ) = 0;
            virtual bool isControllableACL( ) = 0;
            virtual bool isVersionable( ) = 0;
            virtual ContentStreamAllowed getContentStreamAllowed( ) = 0;

            virtual std::map< std::string, PropertyTypePtr >& getPropertiesTypes( ) = 0;

            virtual std::string toString( ) = 0; 
    };

    typedef ::boost::shared_ptr< ObjectType > ObjectTypePtr;
}

#endif
