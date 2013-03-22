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

#include "gdrive-folder.hxx"
#include "gdrive-session.hxx"

using namespace std;

GDriveFolder::GDriveFolder( GDriveSession* session ):
    libcmis::Object( session ),
    libcmis::Folder( session ),
    GDriveObject( session )
{
}

GDriveFolder::GDriveFolder( GDriveSession* session, Json json ):
    libcmis::Object( session ),
    libcmis::Folder( session ),
    GDriveObject( session, json )
{
}

GDriveFolder::~GDriveFolder( )
{
}

vector< libcmis::ObjectPtr > GDriveFolder::getChildren( ) throw ( libcmis::Exception )
{
    //TODO implementation
    vector< libcmis::ObjectPtr > result;
    return result;
}

libcmis::FolderPtr GDriveFolder::createFolder( const map< string, libcmis::PropertyPtr >& /*properties*/ ) throw( libcmis::Exception )
{
    //TODO implementation
    libcmis::FolderPtr result;
    return result;
}
    
libcmis::DocumentPtr GDriveFolder::createDocument( const map< string, libcmis::PropertyPtr >& /*properties*/, boost::shared_ptr< ostream > /*os*/, string /*contentType*/, string ) throw ( libcmis::Exception )

{
    //TODO implementation
    libcmis::DocumentPtr result;
    return result;
}

vector< string > GDriveFolder::removeTree( bool /*allVersions*/, libcmis::UnfileObjects::Type /*unfile*/, bool /*continueOnError*/ ) throw ( libcmis::Exception )
{
    //TODO implementation
    vector< string > result;
    return result;
}
