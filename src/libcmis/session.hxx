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
#ifndef _SESSION_HXX_
#define _SESSION_HXX_

#include <string>
#include <boost/shared_ptr.hpp>

#include "object-type.hxx"
#include "object.hxx"
#include "folder.hxx"

namespace libcmis
{
    class AuthProvider 
    {
        public:
            virtual ~AuthProvider() { };

            /** The function implementing it needs to fill the username and password parameters
                and return true. Returning false means that the user cancelled the authentication
                and will fail the query.
              */
            virtual bool authenticationQuery( std::string& username, std::string& password ) = 0;
    };
    typedef ::boost::shared_ptr< AuthProvider > AuthProviderPtr;

    class Session
    {
        public:

            virtual ~Session() { };

            /** Get the Root folder of the repository
              */
            virtual FolderPtr getRootFolder() throw ( Exception )= 0;
            
            /** Get a CMIS object from its ID.
              */
            virtual ObjectPtr getObject( std::string id ) throw ( Exception ) = 0;

            /** Get a CMIS object from one of its path.
              */
            virtual ObjectPtr getObjectByPath( std::string path ) throw ( Exception ) = 0;

            /** Get a CMIS folder from its ID.
              */
            virtual libcmis::FolderPtr getFolder( std::string id ) throw ( Exception ) = 0;

            /** Get a CMIS object type from its ID.
              */
            virtual ObjectTypePtr getType( std::string id ) throw ( Exception ) = 0;

            /** Set an authentication provider for providing authentication interactively.
              */
            virtual void setAuthenticationProvider( AuthProviderPtr provider ) = 0;
    };
}

#endif
