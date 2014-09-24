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
#ifndef _REPOSITORY_HXX_
#define _REPOSITORY_HXX_

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>
#include <libxml/tree.h>

namespace libcmis
{
    /** Class representing a repository and its infos.

        \sa 2.2.2.2 section of the CMIS specifications
      */
    class Repository
    {
        public:

            enum Capability
            {
                ACL,
                AllVersionsSearchable,
                Changes,
                ContentStreamUpdatability,
                GetDescendants,
                GetFolderTree,
                OrderBy,
                Multifiling,
                PWCSearchable,
                PWCUpdatable,
                Query,
                Renditions,
                Unfiling,
                VersionSpecificFiling,
                Join
            };

        protected:
            std::string m_id;
            std::string m_name;
            std::string m_description;
            std::string m_vendorName;
            std::string m_productName;
            std::string m_productVersion;
            std::string m_rootId;
            std::string m_cmisVersionSupported;
            boost::shared_ptr< std::string > m_thinClientUri;
            boost::shared_ptr< std::string > m_principalAnonymous;
            boost::shared_ptr< std::string > m_principalAnyone;

            std::map< Capability, std::string > m_capabilities ;

            Repository( );
            void initializeFromNode( xmlNodePtr node );

        public:
            Repository( xmlNodePtr node );
            virtual ~Repository( ) { };

            std::string getId( ) const;
            std::string getName( ) const;
            std::string getDescription( ) const;
            std::string getVendorName( ) const;
            std::string getProductName( ) const;
            std::string getProductVersion( ) const;
            std::string getRootId( ) const;
            std::string getCmisVersionSupported( ) const;
            boost::shared_ptr< std::string > getThinClientUri( ) const;
            boost::shared_ptr< std::string > getPrincipalAnonymous( ) const;
            boost::shared_ptr< std::string > getPrincipalAnyone( ) const;

            std::string getCapability( Capability capability ) const;

            /** Wrapper function providing the capability as a boolean value.
                If the capability value is not a boolean, returns false.
             */
            bool getCapabilityAsBool( Capability capability ) const;

            std::string toString( ) const;

        private:

            static std::map< Capability, std::string > parseCapabilities( xmlNodePtr node );
    };
    
    typedef ::boost::shared_ptr< Repository > RepositoryPtr;
}

#endif
