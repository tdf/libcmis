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
 * Copyright (C) 2014 Mihai Varga <mihai.mv13@gmail.com>
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
#ifndef _ONEDRIVE_SESSION_HXX_
#define _ONEDRIVE_SESSION_HXX_

#include <libcmis/repository.hxx>

#include "base-session.hxx"
#include "json-utils.hxx"

class OneDriveSession : public BaseSession
{
    public:
        OneDriveSession( std::string baseUrl,
                       std::string username, 
                       std::string password,
                       libcmis::OAuth2DataPtr oauth2,
                       bool verbose = false );

        OneDriveSession( const OneDriveSession& copy );

        ~OneDriveSession ( );

        virtual libcmis::RepositoryPtr getRepository( );

        virtual bool setRepository( std::string ) { return true; }

        virtual libcmis::ObjectPtr getObject( std::string id );

        virtual libcmis::ObjectPtr getObjectByPath( std::string path );

        virtual libcmis::ObjectTypePtr getType( std::string id );
        
        virtual std::vector< libcmis::ObjectTypePtr > getBaseTypes( );

       libcmis::ObjectPtr getObjectFromJson( Json& jsonRes );

       bool isAPathMatch( Json objectJson, std::string path );

    private:
        OneDriveSession( );
};

#endif /* _ONEDRIVE_SESSION_HXX_ */
