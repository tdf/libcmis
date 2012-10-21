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
#ifndef _ALLOWABLE_ACTIONS_HXX_
#define _ALLOWABLE_ACTIONS_HXX_

#include <map>
#include <string>

#include <boost/shared_ptr.hpp>
#include <libxml/tree.h>

#include "exception.hxx"

namespace libcmis
{
    class Object;

    class ObjectAction
    {
        public:
            enum Type
            {
                DeleteObject,
                UpdateProperties,
                GetFolderTree,
                GetProperties,
                GetObjectRelationships,
                GetObjectParents,
                GetFolderParent,
                GetDescendants,
                MoveObject,
                DeleteContentStream,
                CheckOut,
                CancelCheckOut,
                CheckIn,
                SetContentStream,
                GetAllVersions,
                AddObjectToFolder,
                RemoveObjectFromFolder,
                GetContentStream,
                ApplyPolicy,
                GetAppliedPolicies,
                RemovePolicy,
                GetChildren,
                CreateDocument,
                CreateFolder,
                CreateRelationship,
                DeleteTree,
                GetRenditions,
                GetACL,
                ApplyACL
            };

        private:
            Type m_type;
            bool m_enabled;
            bool m_valid;

        public:
            ObjectAction( xmlNodePtr node );
            virtual ~ObjectAction( ){ }

            Type getType( ) { return m_type; }
            bool isEnabled( ) { return m_enabled; }
            bool isValid( ) { return m_valid; }

            /** Parses the permission name into one of the enum values or throws
                an exception for invalid input strings.
              */
            static Type parseType( std::string type ) throw ( Exception );

    };

    /** Class providing access to the allowed actions on an object.
      */
    class AllowableActions
    {
        protected:
            std::map< ObjectAction::Type, bool > m_states;

        public:
            /** Default constructor for testing purpose
              */
            AllowableActions( );
            AllowableActions( xmlNodePtr node );
            AllowableActions( const AllowableActions& copy );
            virtual ~AllowableActions( );

            AllowableActions& operator=( const AllowableActions& copy );

            /** Returns the permissions for the corresponding actions.
              */
            bool isAllowed( ObjectAction::Type action );

            /** Returns true if the action was defined, false if the default
                value is used.
              */
            bool isDefined( ObjectAction::Type action );

            std::string toString( );
    };
    typedef boost::shared_ptr< AllowableActions > AllowableActionsPtr;
}

#endif
