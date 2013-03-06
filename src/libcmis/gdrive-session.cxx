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
 * Copyright (C) 2013 Cao Cuong Ngo <cao.cuong.ngo@gmail.com>
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

#include "gdrive-session.hxx"
#include "gdrive-common.hxx"

using namespace std;

GDriveSession::GDriveSession ( string clientId, string clientSecret,
        string username, string password, bool verbose )
    throw ( libcmis::Exception ) :
        BaseSession( "", "", username, password, libcmis::OAuth2DataPtr(), verbose )
{
    libcmis::OAuth2DataPtr data( new libcmis::OAuth2Data( DRIVE_AUTH_URL, DRIVE_TOKEN_URL,
                                        DRIVE_SCOPE_FULL, DRIVE_REDIRECT_URI,
                                        clientId, clientSecret, &GDriveSession::authenticate ) );
    setOAuth2Data( data );
}

GDriveSession::~GDriveSession()
{
}

GDriveSession::GDriveSession() :
        BaseSession()
{
}

char* GDriveSession::authenticate( const char* /*url*/, const char* /*username*/, const char* /*password*/ )
{
    char* authCode = NULL;

    // TODO Try to run the authentication

    return authCode;
}

libcmis::RepositoryPtr GDriveSession::getRepository( ) throw ( libcmis::Exception )
{
    libcmis::RepositoryPtr ptr;
    return ptr;
}

libcmis::ObjectPtr GDriveSession::getObject( string /*id*/ ) throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object;
    return object;
}

libcmis::ObjectPtr GDriveSession::getObjectByPath( string /*path*/ ) throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object;
    return object;
}

libcmis::ObjectTypePtr GDriveSession::getType( string /*id*/ ) throw ( libcmis::Exception )
{
    libcmis::ObjectTypePtr ptr;
    return ptr;

}
