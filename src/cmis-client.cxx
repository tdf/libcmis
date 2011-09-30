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

#include <iostream>
#include <map>
#include <string>

#include <boost/program_options.hpp>

#include <libcmis/session-factory.hxx>

using namespace std;
using namespace ::boost::program_options;

int main ( int argc, char* argv[] )
{
    options_description desc( "Allowed options" );
    desc.add_options( )
        ( "help", "Produce help message and exists" )
        ( "atom-url,a", value< string >(), "URL of the AtomPub binding of the server" )
        ( "repository,r", value< string >(), "Name of the repository to use" )
        ( "command", value< string >(), "Command among the following values:\n"
                                        "    list-repos : list the server repositories\n"
                                        "    show-by-id : show the nodes from their ids.\n"
                                        "                 A list of one or more node Ids\n"
                                        "                 is required as arguments." )
        ( "args", value< vector< string > >(), "Arguments for the command" )
    ;

    positional_options_description pd;
    pd.add( "command", 1 );
    pd.add( "args", -1 );

    variables_map vm;
    store( command_line_parser( argc, argv ).options( desc ).positional( pd ).run( ), vm );
    notify( vm );

    if ( vm.count( "atom-url" ) == 0 )
    {
        cout << desc << endl;
        return 1;
    }
    string atomUrl = vm["atom-url"].as<string>();

    if ( vm.count( "command" ) == 1 )
    {
        string command = vm["command"].as<string>();
        if ( "list-repos" == command )
        {
            map< int, string > params;
            params[ATOMPUB_URL] = atomUrl;
            list< string > ids = SessionFactory::getRepositories( params );
        
            cout << "Repositories: ";
            for ( list< string >::iterator it = ids.begin(); it != ids.end(); it++ )
            {
                if ( it != ids.begin() )
                    cout << ", ";
                cout << *it;
            }
            cout << endl;
        }
        else if ( "show-by-id" == command )
        {
            map< int, string > params;
            params[ATOMPUB_URL] = atomUrl;
            list< string > ids = SessionFactory::getRepositories( params );

            // The repository ID is needed to initiate a session
            if ( vm.count( "repository" ) != 1 )
            {
                cout << "Missing repository ID" << endl;
                cout << desc << endl;
                return 1;
            }

            // Get the ids of the objects to fetch
            if ( vm.count( "args" ) == 0 )
            {
                cout << "Please provide the node ids to show as command args" << endl;
                cout << desc << endl;
                return 1;
            }

            vector< string > objIds = vm["args"].as< vector< string > >( );

            params[REPOSITORY_ID] = vm["repository"].as< string >();
            Session* session = SessionFactory::createSession( params );

            for ( vector< string >::iterator it = objIds.begin(); it != objIds.end(); it++ )
            {
                CmisObjectPtr cmisObj = session->getObject( *it );
                cout << "-----------------------" << endl;
                cout << cmisObj->toString() << endl;
            }

            delete session;
        }

        // TODO Add some more useful commands here
    }

    return 0;
}
