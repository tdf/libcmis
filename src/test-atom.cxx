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
#include <libcmis/session-factory.hxx>

#include <cstdio>
#include <map>
#include <string>

using namespace std;

int main ( int argc, char* argv[] )
{
    string sAtomUrl( argv[1] );

    // Test the getRepositories
    map< int, string > params;
    params[ATOMPUB_URL] = sAtomUrl;
    list< string > ids = SessionFactory::getRepositories( params );
    if ( ids.size( ) == 0 )
    {
        fprintf( stderr, "There should be at least one repository ID\n" );
        return 1;
    }
    string firstId = ids.front( );
    fprintf( stdout, "Using repository %s\n", firstId.c_str() );
   
    params[REPOSITORY_ID] = firstId; 
    Session* session = SessionFactory::createSession( params );

    FolderPtr folder = session->getRootFolder( );
    fprintf( stdout, "Root name: %s\n", folder->getName( ).c_str() );
    fprintf( stdout, "Root path: %s\n",folder->getPath( ).c_str() );
    if ( folder->getName().empty() || folder->getPath().empty() )
    {
        fprintf( stderr, "Missing root folder name or path\n" );
        return 1;
    }

    // Look for the children of the folder
    fprintf( stdout, "Children:\n" );
    vector< ResourcePtr > children = folder->getChildren( );
    vector< ResourcePtr >::iterator it = children.begin( );
    while ( it != children.end( ) )
    {
        ResourcePtr res = *it;
        fprintf( stdout, "  + %s\n", res->getName( ).c_str() );
        ++it;
    }

    delete session;

    return 0;
}
