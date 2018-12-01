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
#ifndef _WS_REPOSITORYSERVICE_HXX_
#define _WS_REPOSITORYSERVICE_HXX_

#include <map>
#include <string>
#include <vector>

#include "base-session.hxx"
#include "repository.hxx"
#include "ws-soap.hxx"

class WSSession;

class RepositoryService
{
    private:
        WSSession* m_session;
        std::string m_url;

    public:

        RepositoryService( WSSession* session );
        RepositoryService( const RepositoryService& copy );
        ~RepositoryService( );

        RepositoryService& operator=( const RepositoryService& copy );

        std::map< std::string, std::string > getRepositories( );

        /** Get the repository information based on its identifier.
          */
        libcmis::RepositoryPtr getRepositoryInfo( std::string id );

        libcmis::ObjectTypePtr getTypeDefinition( std::string repoId, std::string typeId );

        std::vector< libcmis::ObjectTypePtr > getTypeChildren( std::string repoId, std::string typeId );

    private:

        RepositoryService();
};

#endif
