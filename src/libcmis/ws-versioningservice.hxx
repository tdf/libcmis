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
#ifndef _WS_VERSIONINGSERVICE_HXX_
#define _WS_VERSIONINGSERVICE_HXX_

#include <string>

#include <libcmis/document.hxx>

class WSSession;

class VersioningService
{
    private:
        WSSession* m_session;
        std::string m_url;

    public:

        VersioningService( WSSession* session );
        VersioningService( const VersioningService& copy );
        ~VersioningService( );

        VersioningService& operator=( const VersioningService& copy );

        libcmis::DocumentPtr checkOut( const std::string& repoId, const std::string& documentId );

        void cancelCheckOut( const std::string& repoId, const std::string& documentId );

        libcmis::DocumentPtr checkIn( std::string repoId, std::string objectId, bool isMajor,
                const std::map< std::string, libcmis::PropertyPtr >& properties,
                boost::shared_ptr< std::ostream > stream, std::string contentType,
                std::string fileName, std::string comment );

        std::vector< libcmis::DocumentPtr > getAllVersions( const std::string& repoId, const std::string& objectId );

    private:

        VersioningService( );
};

#endif
