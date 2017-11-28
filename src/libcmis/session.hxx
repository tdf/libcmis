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

#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

#include "object-type.hxx"
#include "object.hxx"
#include "folder.hxx"
#include "repository.hxx"

namespace libcmis
{
    class Session
    {
        public:

            virtual ~Session() { };

            /** Get the current repository.
              */
            virtual RepositoryPtr getRepository( ) = 0;

            virtual std::vector< RepositoryPtr > getRepositories( ) = 0;

            /** Change the current repository.

                \return
                    false if no repository with the provided id can be found on the server,
                    true otherwise
              */
            virtual bool setRepository( std::string repositoryId ) = 0;

            /** Get the Root folder of the repository
              */
            virtual FolderPtr getRootFolder()= 0;
            
            /** Get a CMIS object from its ID.
              */
            virtual ObjectPtr getObject( std::string id ) = 0;

            /** Get a CMIS object from one of its path.
              */
            virtual ObjectPtr getObjectByPath( std::string path ) = 0;

            /** Get a CMIS folder from its ID.
              */
            virtual libcmis::FolderPtr getFolder( std::string id ) = 0;

            /** Get a CMIS object type from its ID.
              */
            virtual ObjectTypePtr getType( std::string id ) = 0;

            /** Get all the CMIS base object types known by the server.
              */
            virtual std::vector< ObjectTypePtr > getBaseTypes( ) = 0;

            /** Enable or disable the SSL certificate verification.

                By default, SSL certificates are verified and errors are thrown in case of
                one is invalid. The user may decide to ignore the checks for this CMIS session
                to workaround self-signed certificates or other similar problems.

                As each session only handles the connection to one CMIS server, it should
                concern only one SSL certificate and should provide the same feature as the
                certificate exception feature available on common web browser.
              */
            virtual void setNoSSLCertificateCheck( bool noCheck ) = 0;
    };
}

#endif
