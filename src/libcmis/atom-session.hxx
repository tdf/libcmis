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
#ifndef _ATOM_SESSION_HXX_
#define _ATOM_SESSION_HXX_

#include "base-session.hxx"
#include "atom-workspace.hxx"

class AtomPubSession : public BaseSession
{
    private:
        AtomRepositoryPtr m_repository;

    public:
        AtomPubSession( std::string sAtomPubUrl, std::string repositoryId,
                        std::string username, std::string password,
                        bool verbose =false ) throw ( libcmis::Exception );
        AtomPubSession( const AtomPubSession& copy );
        ~AtomPubSession( );

        AtomPubSession& operator=( const AtomPubSession& copy );

        static std::list< libcmis::RepositoryPtr > getRepositories( std::string url,
                        std::string username, std::string password,
                        bool verbose = false ) throw ( libcmis::Exception );

        AtomRepositoryPtr getAtomRepository( ) throw ( libcmis::Exception );

        // Utility methods

        libcmis::ObjectPtr createObjectFromEntryDoc( xmlDocPtr doc );

        // Override session methods

        virtual libcmis::RepositoryPtr getRepository( ) throw ( libcmis::Exception );

        virtual libcmis::ObjectPtr getObject( std::string id ) throw ( libcmis::Exception );
        
        virtual libcmis::ObjectPtr getObjectByPath( std::string path ) throw ( libcmis::Exception );

        virtual libcmis::ObjectTypePtr getType( std::string id ) throw ( libcmis::Exception );

    private:

        void initialize( ) throw ( libcmis::Exception );
};

#endif
