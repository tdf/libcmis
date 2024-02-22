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

#include "ws-folder.hxx"

using namespace std;
using libcmis::PropertyPtrMap;

WSFolder::WSFolder( const WSObject& object ) :
    libcmis::Object( object ),
    WSObject( object )
{
}

WSFolder::~WSFolder( )
{
}

vector< libcmis::ObjectPtr > WSFolder::getChildren( )
{
    string repoId = getSession( )->getRepositoryId( );
    return getSession( )->getNavigationService( ).getChildren( repoId, getId( ) );
}

libcmis::FolderPtr WSFolder::createFolder( const PropertyPtrMap& properties )
{
    string repoId = getSession( )->getRepositoryId( );
    return getSession( )->getObjectService( ).createFolder( repoId, properties, getId( ) );
}

libcmis::DocumentPtr WSFolder::createDocument( const PropertyPtrMap& properties,
                        boost::shared_ptr< ostream > os, string contentType, string fileName )
{
    string repoId = getSession( )->getRepositoryId( );
    return getSession( )->getObjectService( ).createDocument( repoId, properties, getId( ), os, contentType, fileName );
}

vector< string > WSFolder::removeTree( bool allVersion, libcmis::UnfileObjects::Type unfile, bool continueOnError )
{
    string repoId = getSession( )->getRepositoryId( );
    return getSession( )->getObjectService( ).deleteTree( repoId, getId( ), allVersion, unfile, continueOnError );
}
