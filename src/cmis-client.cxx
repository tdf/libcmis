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
#include <libcmis/document.hxx>

using namespace std;
using namespace ::boost::program_options;

class CommandException : public exception
{
    private:
        const char* m_msg;

    public:
        CommandException( const char* msg ) : m_msg( msg ) { }
        CommandException( const CommandException& copy ) : m_msg( copy.m_msg ) { }

        CommandException& operator=( const CommandException& copy )
        {
            m_msg = copy.m_msg;
            return *this;
        }

        virtual const char* what() const throw() { return m_msg; }
};

class CmisClient
{
    private:
        variables_map& m_vm;
    public:
        CmisClient( variables_map& vm ) : m_vm( vm ) { }

        libcmis::Session* getSession( ) throw ( CommandException );

        void execute( ) throw ( exception );

        void printHelp( );

        static options_description getOptionsDescription( );

    private:
        map< int, string > getSessionParams();
};

map< int, string > CmisClient::getSessionParams()
{
    map< int, string > params;

    if ( m_vm.count( "url" ) == 0 )
        throw CommandException( "Missing Atom binding URL" );
    
    string url = m_vm["url"].as<string>();
    params[ATOMPUB_URL] = url;

    // Look for the credentials
    if ( m_vm.count( "username" ) > 0 )
    {
        string username = m_vm["username"].as< string >();

        string password;
        if ( m_vm.count( "password" ) > 0 )
            password = m_vm["password"].as< string >();

        if ( !username.empty() && !password.empty() )
        {
            params[USERNAME] = username;
            params[PASSWORD] = password;
        }
    }

    return params;
}

libcmis::Session* CmisClient::getSession( ) throw ( CommandException )
{
    map< int, string > params = getSessionParams();

    // The repository ID is needed to initiate a session
    if ( m_vm.count( "repository" ) != 1 )
        throw CommandException( "Missing repository ID" );

    params[REPOSITORY_ID] = m_vm["repository"].as< string >();

    return libcmis::SessionFactory::createSession( params );
}

void CmisClient::execute( ) throw ( exception )
{
    if ( ( m_vm.count( "help" ) > 0 ) || m_vm.count( "command" ) != 1 )
    {
        printHelp();
        return; 
    }

    if ( m_vm.count( "command" ) == 1 )
    {
        string command = m_vm["command"].as<string>();
        if ( "list-repos" == command )
        {
            map< int, string > params = getSessionParams( );
            list< string > ids = libcmis::SessionFactory::getRepositories( params );
        
            cout << "Repositories: ";
            for ( list< string >::iterator it = ids.begin(); it != ids.end(); it++ )
            {
                if ( it != ids.begin() )
                    cout << ", ";
                cout << *it;
            }
            cout << endl;
        }
        else if ( "show-root" == command )
        {
            libcmis::Session* session = getSession( );

            libcmis::FolderPtr root = session->getRootFolder();
            if ( root.get() )
            {
                cout << "-----------------------" << endl;
                cout << root->toString() << endl;
            }

            delete session;
        }
        else if ( "show-by-id" == command )
        {
            libcmis::Session* session = getSession( );

            // Get the ids of the objects to fetch
            if ( m_vm.count( "args" ) == 0 )
                throw CommandException( "Please provide the node ids to show as command args" );

            vector< string > objIds = m_vm["args"].as< vector< string > >( );


            for ( vector< string >::iterator it = objIds.begin(); it != objIds.end(); it++ )
            {
                libcmis::ObjectPtr cmisObj = session->getObject( *it );
                cout << "-----------------------" << endl;
                if ( cmisObj.get() )
                    cout << cmisObj->toString() << endl;
                else
                    cout << "No such node: " << *it << endl;
            }

            delete session;
        }
        else if ( "get-content" == command )
        {
            libcmis::Session* session = getSession( );

            vector< string > objIds = m_vm["args"].as< vector< string > >( );
            if ( objIds.size() == 0 )
                throw CommandException( "Please provide a content object Id" );

            libcmis::ObjectPtr cmisObj = session->getObject( objIds.front() );
            libcmis::Document* document = dynamic_cast< libcmis::Document* >( cmisObj.get() );
            if ( NULL != document )
            {
                // TODO Handle name clashes
                FILE* fd = document->getContent( document->getContentFilename().c_str() );

                if ( fd )
                    fclose( fd );
            }

            delete session;
        }
        else if ( "help" == command )
        {
            printHelp();
        }

        // TODO Add some more useful commands here
    }
}

options_description CmisClient::getOptionsDescription( )
{
    options_description desc( "Allowed options" );
    desc.add_options( )
        ( "help", "Produce help message and exists" )
        ( "url", value< string >(), "URL of the binding of the server" )
        ( "repository,r", value< string >(), "Name of the repository to use" )
        ( "username,u", value< string >(), "Username used to authenticate to the repository" )
        ( "password,p", value< string >(), "Password used to authenticate to the repository" )
    ;

    return desc;
}

void CmisClient::printHelp( )
{
    cerr << "CmisClient [options] [command] arguments" << endl;

    cerr << endl << "Commands" << endl;
    cerr << "   list-repos\n"
            "           Lists the repositories available on the server" << endl;
    cerr << "   show-root\n"
            "           Dump the root node of the repository." << endl;
    cerr << "   show-by-id <Node Id 1> [... <Node Id N>]\n"
            "           Dumps the nodes informations for all the ids." << endl;
    cerr << "   get-content <Node Id>\n"
            "           Saves the stream of the content node in the\n"
            "           current folder. Any existing file is overwritten." << endl;
    cerr << "   help\n"
            "           Prints this help message and exits (like --help option)." << endl;

    cerr << endl << getOptionsDescription() << endl;
}

int main ( int argc, char* argv[] )
{
    options_description hidden( "Hidden options" );
    hidden.add_options( )
        ( "command", value< string >(), "Command" )
        ( "args", value< vector< string > >(), "Arguments for the command" )
    ;

    options_description allOptions = CmisClient::getOptionsDescription( );
    allOptions.add( hidden );

    positional_options_description pd;
    pd.add( "command", 1 );
    pd.add( "args", -1 );

    variables_map vm;
    store( command_line_parser( argc, argv ).options( allOptions ).positional( pd ).run( ), vm );
    notify( vm );

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
        client.printHelp();
    }

    return 0;
}
