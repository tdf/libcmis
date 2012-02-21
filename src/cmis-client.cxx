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
#include <fstream>
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
        string m_msg;

    public:
        CommandException( string msg ) : m_msg( msg ) { }
        ~CommandException( ) throw( ) { }
        CommandException( const CommandException& copy ) : m_msg( copy.m_msg ) { }

        CommandException& operator=( const CommandException& copy )
        {
            m_msg = copy.m_msg;
            return *this;
        }

        virtual const char* what() const throw() { return m_msg.c_str(); }
};

class CmisClient
{
    private:
        variables_map& m_vm;
    public:
        CmisClient( variables_map& vm ) : m_vm( vm ) { }

        libcmis::Session* getSession( ) throw ( CommandException, libcmis::Exception );

        void execute( ) throw ( exception );

        void printHelp( );

        static options_description getOptionsDescription( );

    private:
        map< int, string > getSessionParams();
        map< string, string > getObjectProperties( );
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

    if ( m_vm.count( "verbose" ) > 0 )
        params[VERBOSE] = "yes";

    return params;
}

map< string, string > CmisClient::getObjectProperties( )
{
    map< string, string > result;
    if ( m_vm.count( "object-property" ) > 0 )
    {
        vector< string > params = m_vm["object-property"].as< vector< string > >( );
        for ( vector< string >::iterator it = params.begin( ); it != params.end( ); ++it )
        {
            size_t pos = it->find( "=" );
            if ( pos != string::npos )
            {
                string name = it->substr( 0, pos );
                string value = it->substr( pos + 1 );
                result.insert( pair< string, string >( name, value ) );
            }
        }
    }

    return result;
}

libcmis::Session* CmisClient::getSession( ) throw ( CommandException, libcmis::Exception )
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
        try
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
                    cout << "------------------------------------------------" << endl;
                    cout << root->toString() << endl;
                }

                delete session;
            }
            else if ( "type-by-id" == command )
            {
                libcmis::Session* session = getSession( );

                // Get the ids of the types to fetch
                if ( m_vm.count( "args" ) == 0 )
                    throw CommandException( "Please provide the node ids to show as command args" );

                vector< string > ids = m_vm["args"].as< vector< string > >( );


                for ( vector< string >::iterator it = ids.begin(); it != ids.end(); it++ )
                {
                    cout << "------------------------------------------------" << endl;
                    try
                    {
                        libcmis::ObjectTypePtr type = session->getType( *it );
                        cout << type->toString() << endl;
                    }
                    catch ( const libcmis::Exception& e )
                    {
                        cout << e.what() << endl;
                    }
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
                    cout << "------------------------------------------------" << endl;
                    if ( cmisObj.get() )
                        cout << cmisObj->toString() << endl;
                    else
                        cout << "No such node: " << *it << endl;
                }

                delete session;
            }
            else if ( "show-by-path" == command )
            {
                libcmis::Session* session = getSession( );

                // Get the paths of the objects to fetch
                if ( m_vm.count( "args" ) == 0 )
                    throw CommandException( "Please provide the node paths to show as command args" );

                vector< string > objPaths = m_vm["args"].as< vector< string > >( );


                for ( vector< string >::iterator it = objPaths.begin(); it != objPaths.end(); it++ )
                {
                    libcmis::ObjectPtr cmisObj = session->getObjectByPath( *it );
                    cout << "------------------------------------------------" << endl;
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
                    boost::shared_ptr< istream > in = document->getContentStream( );
                    ofstream out( document->getContentFilename().c_str() );
                    out << in->rdbuf();
                    out.close();
                }
                else
                    throw CommandException( string( "Not a document object id: " ) + objIds.front() );

                delete session;
            }
            else if ( "set-content" == command )
            {
                libcmis::Session* session = getSession( );

                vector< string > objIds = m_vm["args"].as< vector< string > >( );
                if ( objIds.size() ==0 )
                    throw CommandException( "Please provide a content object Id" );

                libcmis::ObjectPtr cmisObj = session->getObject( objIds.front() );
                libcmis::Document* document = dynamic_cast< libcmis::Document* >( cmisObj.get() );
                if ( NULL != document )
                {
                    if ( m_vm.count( "input-file" ) == 0 )
                        throw CommandException( "Missing --input-file" );
                    if ( m_vm.count( "input-type" ) == 0 )
                        throw CommandException( "Missing --input-type" );

                    string type = m_vm["input-type"].as<string>();
                    string filename = m_vm["input-file"].as<string>();
                    ifstream is( filename.c_str(), ifstream::in );
                    if ( is.fail( ) )
                        throw CommandException( string( "Unable to open file " ) + filename );

                    document->setContentStream( is, type );

                    is.close( );
                }
                else
                    throw CommandException( string( "Not a document object id: " ) + objIds.front() );

                delete session;
            }
            else if ( "create-folder" == command )
            {
                libcmis::Session* session = getSession( );

                vector< string > args = m_vm["args"].as< vector< string > >( );
                if ( args.size() < 2 )
                    throw CommandException( "Please provide a parent Id and folder name" );

                libcmis::FolderPtr parent = session->getFolder( args[0] );

                // Get the folder type to create
                string folderType( "cmis:folder" );
                if ( m_vm.count( "object-type" ) != 0 )
                    folderType = m_vm["object-type"].as<string>( );

                libcmis::ObjectTypePtr type = session->getType( folderType );
                if ( "cmis:folder" != type->getBaseType( )->getId( ) )
                    throw CommandException( string( "Not a folder type: " ) + folderType );

                map< string, libcmis::PropertyPtr > properties;
                map< string, libcmis::PropertyTypePtr >& propertiesTypes = type->getPropertiesTypes( );

                // Set the name
                map< string, libcmis::PropertyTypePtr >::iterator typeIt = propertiesTypes.find( string( "cmis:name" ) );
                if ( typeIt == propertiesTypes.end( ) )
                    throw CommandException( string( "No cmis:name on the object type... weird" ) );
                vector< string > nameValues;
                nameValues.push_back( args[1] );
                libcmis::PropertyPtr nameProperty( new libcmis::Property( typeIt->second, nameValues ) );
                properties.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:name" ), nameProperty ) );
                
                // Set the objectTypeId
                typeIt = propertiesTypes.find( string( "cmis:objectTypeId" ) );
                if ( typeIt == propertiesTypes.end( ) )
                    throw CommandException( string( "No cmis:objectTypeId on the object type... weird" ) );
                vector< string > typeIdValues;
                typeIdValues.push_back( folderType );
                libcmis::PropertyPtr typeIdProperty( new libcmis::Property( typeIt->second, typeIdValues ) );
                properties.insert( pair< string, libcmis::PropertyPtr >( string( "cmis:objectTypeId" ), typeIdProperty ) );
                
                // Checks for the properties to set if any
                map< string, string > propsToSet = getObjectProperties( );
                for ( map< string, string >::iterator it = propsToSet.begin(); it != propsToSet.end(); ++it )
                {
                    // Create the CMIS property if it exists
                    typeIt = propertiesTypes.find( it->first );
                    if ( typeIt != propertiesTypes.end( ) )
                    {
                        vector< string > values;
                        values.push_back( it->second );
                        libcmis::PropertyPtr cmisProperty( new libcmis::Property( typeIt->second, values ) );
                        properties.insert( pair< string, libcmis::PropertyPtr >( it->first, cmisProperty ) );
                    }
                }

                libcmis::FolderPtr created = parent->createFolder( properties );

                cout << "------------------------------------------------" << endl;
                cout << created->toString() << endl;

                delete session;
            }
            else if ( "help" == command )
            {
                printHelp();
            }
            else
            {
                cerr << "------------------------------------------------" << endl;
                cerr << "ERROR: Unknown command: " << command << endl;
                cerr << "------------------------------------------------" << endl;
                printHelp( );
            }

            // TODO Add some more useful commands here
        }
        catch ( const libcmis::Exception& e )
        {
            cerr << "------------------------------------------------" << endl;
            cerr << "ERROR: " << e.what() << endl;
            cerr << "------------------------------------------------" << endl;
        }
    }
}

options_description CmisClient::getOptionsDescription( )
{
    options_description desc( "Allowed options" );
    desc.add_options( )
        ( "help", "Produce help message and exists" )
        ( "verbose,v", "Show loads of useful messages for debugging" )
        ( "url", value< string >(), "URL of the binding of the server" )
        ( "repository,r", value< string >(), "Name of the repository to use" )
        ( "username,u", value< string >(), "Username used to authenticate to the repository" )
        ( "password,p", value< string >(), "Password used to authenticate to the repository" )
    ;

    options_description setcontentOpts( "modification operations options" );
    setcontentOpts.add_options( )
        ( "input-file", value< string >(), "File to push to the repository" )
        ( "input-type", value< string >(), "Mime type of the file to push to the repository" )
        ( "object-type", value< string >(), "CMIS type of the object to create" )
        ( "object-property", value< string >(), "under the form prop-id=prop-value, defines a property"
                                                "to be set on the object" )
    ;

    desc.add( setcontentOpts );
    return desc;
}

void CmisClient::printHelp( )
{
    cerr << "cmis-client [options] [command] arguments" << endl;

    cerr << endl << "Commands" << endl;
    cerr << "   list-repos\n"
            "           Lists the repositories available on the server" << endl;
    cerr << "   show-root\n"
            "           Dump the root node of the repository." << endl;
    cerr << "   type-by-id <Type Id 1> [... <Type Id N>]\n"
            "           Dumps the type informations for all the ids." << endl;
    cerr << "   show-by-id <Object Id 1> [... <Object Id N>]\n"
            "           Dumps the objects informations for all the ids." << endl;
    cerr << "   show-by-path <Object Path 1> [... <Object Path N>]\n"
            "           Dumps the objects informations for all the paths." << endl;
    cerr << "   get-content <Object Id>\n"
            "           Saves the stream of the content object in the\n"
            "           current folder. Any existing file is overwritten." << endl;
    cerr << "   set-content <Object Id>\n"
            "           Replaces the stream of the content object by the\n"
            "           file selected with --input-file." << endl;
    cerr << "   create-folder <Parent Id> <Folder Name>\n"
            "           Creates a new folder inside the folder <Parent Id> named <Folder Name>." << endl;
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
        cerr << "------------------------------------------------" << endl;
        cerr << "ERROR: " << e.what() << endl;
        cerr << "------------------------------------------------" << endl;
        client.printHelp();
    }

    return 0;
}
