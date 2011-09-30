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

#include <stdio.h>

#include <exception>
#include <iostream>
#include <map>
#include <string>

#include <boost/program_options.hpp>

#include <libcmis/session-factory.hxx>
#include <libcmis/content.hxx>

using namespace std;
using namespace ::boost::program_options;

class CommandException : public exception
{
    private:
        const char* m_msg;

    public:
        CommandException( const char* msg ) : m_msg( msg ) { }
        virtual const char* what() const throw() { return m_msg; }
};

class CmisClient
{
    private:
        variables_map& m_vm;
    public:
        CmisClient( variables_map& vm ) : m_vm( vm ) { }

        Session* getSession( ) throw ( CommandException );

        void execute( ) throw ( exception );
};

Session* CmisClient::getSession( ) throw ( CommandException )
{
    map< int, string > params;

    if ( m_vm.count( "atom-url" ) == 0 )
        throw CommandException( "Missing URL" );
    
    string atomUrl = m_vm["atom-url"].as<string>();
    params[ATOMPUB_URL] = atomUrl;
    list< string > ids = SessionFactory::getRepositories( params );

    // The repository ID is needed to initiate a session
    if ( m_vm.count( "repository" ) != 1 )
        throw CommandException( "Missing repository ID" );

    // Get the ids of the objects to fetch
    if ( m_vm.count( "args" ) == 0 )
        throw CommandException( "Please provide the node ids to show as command args" );

    params[REPOSITORY_ID] = m_vm["repository"].as< string >();
    return SessionFactory::createSession( params );
}

void CmisClient::execute( ) throw ( exception )
{
    if ( m_vm.count( "command" ) == 1 )
    {
        string command = m_vm["command"].as<string>();
        if ( "list-repos" == command )
        {
            map< int, string > params;
            if ( m_vm.count( "atom-url" ) == 0 )
                throw CommandException( "Missing URL" );
            string atomUrl = m_vm["atom-url"].as<string>();

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
            Session* session = getSession( );

            vector< string > objIds = m_vm["args"].as< vector< string > >( );


            for ( vector< string >::iterator it = objIds.begin(); it != objIds.end(); it++ )
            {
                CmisObjectPtr cmisObj = session->getObject( *it );
                cout << "-----------------------" << endl;
                cout << cmisObj->toString() << endl;
            }

            delete session;
        }
        else if ( "get-content" == command )
        {
            Session* session = getSession( );

            vector< string > objIds = m_vm["args"].as< vector< string > >( );
            if ( objIds.size() == 0 )
                throw CommandException( "Please provide a content object Id" );

            CmisObjectPtr cmisObj = session->getObject( objIds.front() );
            Content* content = dynamic_cast< Content* >( cmisObj.get() );
            if ( NULL != content )
            {
                // TODO Handle name clashes
                FILE* fd = fopen( content->getContentFilename().c_str(), "w" );
                content->getContent( (size_t (*)( void*, size_t, size_t, void* ) )fwrite, fd );
                fclose( fd );
            }

            delete session;
        }

        // TODO Add some more useful commands here
    }
}

int main ( int argc, char* argv[] )
{
    options_description desc( "Allowed options" );
    desc.add_options( )
        ( "help", "Produce help message and exists" )
        ( "atom-url,a", value< string >(), "URL of the AtomPub binding of the server" )
        ( "repository,r", value< string >(), "Name of the repository to use" )
        ( "command", value< string >(), "Command among the following values:\n"
                                        "    list-repos  :\n"
                                        "           list the server repositories\n"
                                        "    show-by-id  :\n"
                                        "           show the nodes from their ids.\n"
                                        "           A list of one or more node Ids\n"
                                        "           is required as arguments.\n"
                                        "    get-content :\n"
                                        "           get the content of the node with\n"
                                        "           the given Id.\n" )
        ( "args", value< vector< string > >(), "Arguments for the command" )
    ;

    positional_options_description pd;
    pd.add( "command", 1 );
    pd.add( "args", -1 );

    variables_map vm;
    store( command_line_parser( argc, argv ).options( desc ).positional( pd ).run( ), vm );
    notify( vm );

    if ( vm.count( "help" ) > 0 )
    {
        cerr << desc << endl;
        return 0; 
    }

    CmisClient client( vm );
    try
    {
        client.execute( );
    }
    catch ( CommandException e )
    {
        cerr << "-------------------------" << endl;
        cerr << "ERROR: " << e.what() << endl;
        cerr << "-------------------------" << endl;
        cerr << desc << endl;
    }

    return 0;
}
