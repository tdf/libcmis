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

namespace
{
    char* lcl_queryAuthCode( const char* url, const char* /*username*/, const char* /*password*/ )
    {
        string code;
        cout << "Copy the following link to your browser and take the code: " << endl << endl << url << endl << endl;
        cout << "Enter the code:" << endl;
        cin >> code;

        return strdup( code.c_str( ) );
    }

    class CinAuthProvider : public libcmis::AuthProvider
    {
        public:
            CinAuthProvider( ) { }
            ~CinAuthProvider( ) { }

            virtual bool authenticationQuery( string& username, string& password );
    };

    bool CinAuthProvider::authenticationQuery( string& username, string& password )
    {
        bool cancelled = true;
        bool askUsername = username.empty();
        if ( askUsername )
        {
            cout << "Username (empty to cancel): ";
            getline( cin, username );
            cancelled = username.empty();
        }

        if ( !cancelled && ( askUsername || password.empty( ) ) )
        {
            cout << "Password (empty to cancel): ";
            getline( cin, password );
            cancelled = password.empty();
        }
        return !cancelled;
    }
}

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
            if ( this != &copy )
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

        libcmis::Session* getSession( bool inGetRepositories = false ) throw ( CommandException, libcmis::Exception );

        void execute( ) throw ( exception );

        void printHelp( );

        static options_description getOptionsDescription( );

    private:
        map< int, string > getSessionParams();
        map< string, string > getObjectProperties( );
};

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

libcmis::Session* CmisClient::getSession( bool inGetRepositories ) throw ( CommandException, libcmis::Exception )
{
    if ( m_vm.count( "url" ) == 0 )
        throw CommandException( "Missing binding URL" );
    
    // Setup the authentication provider in case we have missing credentials
    libcmis::AuthProviderPtr provider( new CinAuthProvider( ) );
    libcmis::SessionFactory::setAuthenticationProvider( provider );
    
    string url = m_vm["url"].as<string>();

    // Look for the credentials
    string username;
    string password;
    if ( m_vm.count( "username" ) > 0 )
    {
        username = m_vm["username"].as< string >();

        if ( m_vm.count( "password" ) > 0 )
            password = m_vm["password"].as< string >();
    }

    // Look for proxy settings
    string proxyUrl;
    string proxyUser;
    string proxyPass;
    string noproxy;
    if ( m_vm.count( "proxy" ) > 0 )
    {
        proxyUrl = m_vm["proxy"].as< string >();

        if ( m_vm.count( "proxy-user" ) > 0 )
            proxyUser = m_vm["proxy-user"].as< string >();
        
        if ( m_vm.count( "proxy-password" ) > 0 )
            proxyPass = m_vm["proxy-password"].as< string >();

        if ( m_vm.count( "noproxy" ) > 0 )
            noproxy = m_vm["noproxy"].as< string >();

        libcmis::SessionFactory::setProxySettings( proxyUrl, noproxy, proxyUser, proxyPass );
    }

    bool verbose = m_vm.count( "verbose" ) > 0;

    string repoId;
    // The repository ID is needed to initiate a session
    if ( m_vm.count( "repository" ) == 0 && !inGetRepositories )
    {
        // Do we have a single repository on the server?
        libcmis::Session* session = getSession( true );
        if ( session != NULL )
        {
            list< libcmis::RepositoryPtr > repos = session->getRepositories();
            if ( repos.size() == 1 )
                repoId = repos.front( )->getId( );
        }
        delete session;

        // We couldn't auto-guess the repository, then throw an error
        if ( repoId.empty( ) )
            throw CommandException( "Missing repository ID" );
    }
    else if ( m_vm.count( "repository" ) > 0 )
    {
        repoId = m_vm["repository"].as< string >();
    }

    // Should we use OAuth2?
    string oauth2ClientId;
    string oauth2ClientSecret;
    string oauth2AuthUrl;
    string oauth2TokenUrl;
    string oauth2RedirectUri;
    string oauth2Scope;
    if ( m_vm.count( "oauth2-client-id" ) > 0 )
        oauth2ClientId = m_vm["oauth2-client-id"].as< string >();
    if ( m_vm.count( "oauth2-client-secret" ) > 0 )
        oauth2ClientSecret = m_vm["oauth2-client-secret"].as< string >();
    if ( m_vm.count( "oauth2-auth-url" ) > 0 )
        oauth2AuthUrl = m_vm["oauth2-auth-url"].as< string >();
    if ( m_vm.count( "oauth2-token-url" ) > 0 )
        oauth2TokenUrl = m_vm["oauth2-token-url"].as< string >();
    if ( m_vm.count( "oauth2-redirect-uri" ) > 0 )
        oauth2RedirectUri = m_vm["oauth2-redirect-uri"].as< string >();
    if ( m_vm.count( "oauth2-scope" ) > 0 )
        oauth2Scope = m_vm["oauth2-scope"].as< string >();

    libcmis::OAuth2DataPtr oauth2Data( new libcmis::OAuth2Data( oauth2AuthUrl, oauth2TokenUrl,
                oauth2Scope, oauth2RedirectUri, oauth2ClientId, oauth2ClientSecret ) );

    if ( oauth2Data->isComplete( ) )
    {
        // Set the fallback AuthCode provider
        libcmis::SessionFactory::setOAuth2AuthCodeProvider( lcl_queryAuthCode );
    }
    else
    {
        oauth2Data.reset( );
    }

    return libcmis::SessionFactory::createSession( url, username, password, repoId, oauth2Data, verbose );
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
            libcmis::Session* session = getSession( true );
            if ( session != NULL )
            {
                list< libcmis::RepositoryPtr > repos = session->getRepositories();
        
                cout << "Repositories: name (id)" << endl;
                for ( list< libcmis::RepositoryPtr >::iterator it = repos.begin(); it != repos.end(); ++it )
                    cout << "\t" << ( *it )->getName( ) << " (" << ( *it )->getId( ) << ")" << endl;
            }
            else
            {
                cerr << "Couldn't create a session for some reason" << endl;
            }
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


            for ( vector< string >::iterator it = ids.begin(); it != ids.end(); ++it )
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


            for ( vector< string >::iterator it = objIds.begin(); it != objIds.end(); ++it )
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


            for ( vector< string >::iterator it = objPaths.begin(); it != objPaths.end(); ++it )
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
            if ( objIds.empty( ) )
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
            if ( objIds.empty( ) )
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
                string filename;
                if ( m_vm.count( "input-name" ) > 0 )
                    filename = m_vm["input-name"].as<string>();
                string file = m_vm["input-file"].as<string>();
                ifstream is( file.c_str(), ifstream::in );
                boost::shared_ptr< ostream > os ( new ostream ( is.rdbuf( ) ) );
                if ( is.fail( ) )
                    throw CommandException( string( "Unable to open file " ) + file );

                document->setContentStream( os, type, filename );

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
        else if ( "create-document" == command )
        {
            libcmis::Session* session = getSession( );

            vector< string > args = m_vm["args"].as< vector< string > >( );
            if ( args.size() < 2 )
                throw CommandException( "Please provide a parent Id and document name" );

            libcmis::FolderPtr parent = session->getFolder( args[0] );

            // Get the document type to create
            string documentType( "cmis:document" );
            if ( m_vm.count( "object-type" ) != 0 )
                documentType = m_vm["object-type"].as<string>( );

            libcmis::ObjectTypePtr type = session->getType( documentType );
            if ( "cmis:document" != type->getBaseType( )->getId( ) )
                throw CommandException( string( "Not a document type: " ) + documentType );

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
            typeIdValues.push_back( documentType );
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

            // Get the content type and stream
            boost::shared_ptr< ostream > contentStream;
            string contentType;
            string filename;
            
            bool hasInputFile = m_vm.count( "input-file" ) != 0;
            bool hasInputType = m_vm.count( "input-type" ) != 0;
            bool hasInputName = m_vm.count( "input-name" ) != 0;

            if ( hasInputType && !hasInputFile )
                throw CommandException( "Missing --input-file" );
            if ( hasInputFile && !hasInputType )
                throw CommandException( "Missing --input-type" );

            if ( hasInputFile && hasInputType )
            {
                contentType = m_vm["input-type"].as<string>();
                string file = m_vm["input-file"].as<string>();
                fstream is( file.c_str() );
                if ( is.fail( ) )
                    throw CommandException( string( "Unable to open file " ) + file );
                contentStream.reset( new ostringstream( ios_base::out | ios_base::in ) );

                *contentStream << is.rdbuf();
            }

            if ( hasInputName )
                filename = m_vm[ "input-name" ].as< string >( );

            // Actually create the document
            libcmis::DocumentPtr created = parent->createDocument( properties, contentStream, contentType, filename );

            cout << "------------------------------------------------" << endl;
            cout << created->toString() << endl;

            delete session;
        }
        else if ( "update-object" == command )
        {
            libcmis::Session* session = getSession( );

            vector< string > args = m_vm["args"].as< vector< string > >( );
            if ( args.size() != 1 )
                throw CommandException( "Please provide an object id" );

            libcmis::ObjectPtr object = session->getObject( args[0] );
            libcmis::ObjectTypePtr type = session->getType( object->getType( ) );
            map< string, libcmis::PropertyTypePtr >& propertiesTypes = type->getPropertiesTypes( );

            map< string, libcmis::PropertyPtr > properties;

            // Checks for the properties to set if any
            map< string, string > propsToSet = getObjectProperties( );
            for ( map< string, string >::iterator it = propsToSet.begin(); it != propsToSet.end(); ++it )
            {
                // Create the CMIS property if it exists
                map< string, libcmis::PropertyTypePtr >::iterator typeIt = propertiesTypes.find( it->first );
                if ( typeIt != propertiesTypes.end( ) && typeIt->second->isUpdatable( ) )
                {
                    vector< string > values;
                    values.push_back( it->second );
                    libcmis::PropertyPtr property( new libcmis::Property( typeIt->second, values ) );
                    properties[ it->first ] = property;
                }
            }

            libcmis::ObjectPtr updated = object->updateProperties( properties );

            cout << "------------------------------------------------" << endl;
            // Output updated instead of object as it may be different depending on the server
            cout << updated->toString() << endl;

            delete session;
        }
        else if ( "move-object" == command )
        {
            libcmis::Session* session = getSession( );

            vector< string > args = m_vm["args"].as< vector< string > > ( );
            if ( args.size() != 3 )
                throw CommandException( "Please provide an object id and source and destination folder ids" );
            string& objId = args[0];
            string& srcId = args[1];
            string& dstId = args[2];

            try
            {
                libcmis::ObjectPtr obj = session->getObject( objId );

                libcmis::ObjectPtr src = session->getObject( srcId );
                libcmis::FolderPtr srcFolder = boost::dynamic_pointer_cast< libcmis::Folder > ( src );
                if ( !srcFolder )
                    throw CommandException( "Source object is not a folder" );

                libcmis::ObjectPtr dst = session->getObject( dstId );
                libcmis::FolderPtr dstFolder = boost::dynamic_pointer_cast< libcmis::Folder > ( dst );
                if ( !dstFolder )
                    throw CommandException( "Destinaton object is not a folder" );

                obj->move( srcFolder, dstFolder );

                cout << "------------------------------------------------" << endl;
                cout << obj->toString( ) << endl;
            }
            catch ( const std::exception& e )
            {
                delete session;
                throw;
            }
            delete session;
        }
        else if ( "delete" == command )
        {
            libcmis::Session* session = getSession( );

            // Get the ids of the objects to fetch
            if ( m_vm.count( "args" ) == 0 )
                throw CommandException( "Please provide the node ids to delete as command args" );

            vector< string > objIds = m_vm["args"].as< vector< string > >( );

            vector< string > errors;
            for ( vector< string >::iterator it = objIds.begin(); it != objIds.end(); ++it )
            {
                libcmis::ObjectPtr cmisObj = session->getObject( *it );
                libcmis::Folder* folder = dynamic_cast< libcmis::Folder* >( cmisObj.get() );
                if ( NULL != folder )
                {
                    try
                    {
                        vector< string > failed = folder->removeTree( );
                        string error;
                        for ( vector< string >::iterator dumpIt = failed.begin( );
                                dumpIt != failed.end( ); ++dumpIt )
                        {
                            if ( dumpIt == failed.begin( ) )
                                error += "Failed to remove children nodes: ";
                            else
                                error += ", ";
                            error = *dumpIt;
                        }
                        if ( !error.empty( ) )
                            errors.push_back( error );
                    }
                    catch ( const libcmis::Exception& e )
                    {
                        string msg = *it + ": " + e.what( );
                        errors.push_back( msg );
                    }
                }
                else if ( cmisObj.get() )
                {
                    try
                    {
                        cmisObj->remove( );
                    }
                    catch ( const libcmis::Exception& e )
                    {
                        string msg = *it + ": " + e.what( );
                        errors.push_back( msg );
                    }
                }
                else
                {
                    string msg = "No such node: " + *it;
                    errors.push_back( msg );
                }
            }

            // Show the errors
            if ( !errors.empty( ) )
            {
                cout << "Errors:" << endl;
                for ( vector< string >::iterator it = errors.begin( ); it != errors.end( ); ++it )
                {
                    cout << "\t" << *it << endl;
                }
            }
            else
            {
                cout << "All nodes have been removed" << endl;
            }

            delete session;
        }
        else if ( "checkout" == command )
        {
            libcmis::Session* session = getSession( );

            // Get the ids of the objects to fetch
            if ( m_vm.count( "args" ) == 0 )
                throw CommandException( "Please provide the node id to checkout as command args" );

            vector< string > objIds = m_vm["args"].as< vector< string > >( );

            libcmis::ObjectPtr cmisObj = session->getObject( objIds.front() );
            if ( cmisObj.get() )
            {
                libcmis::Document* doc = dynamic_cast< libcmis::Document* >( cmisObj.get() );
                if ( NULL != doc )
                {
                    libcmis::DocumentPtr pwc = doc->checkOut( );
                    if ( pwc.get( ) )
                    {
                        cout << "------------------------------------------------" << endl;
                        cout << pwc->toString() << endl;
                    }
                    else
                        cout << "No Private Working Copy returned?" << endl;
                }
                else
                    throw CommandException( string( "Not a document object id: " ) + objIds.front() );
            }
            else
                cout << "No such node: " << objIds.front() << endl;

            delete session;
        }
        else if ( "cancel-checkout" == command )
        {
            libcmis::Session* session = getSession( );

            // Get the ids of the objects to fetch
            if ( m_vm.count( "args" ) == 0 )
                throw CommandException( "Please provide the private working copy object id to cancel as command args" );

            vector< string > objIds = m_vm["args"].as< vector< string > >( );

            libcmis::ObjectPtr cmisObj = session->getObject( objIds.front() );
            cout << "------------------------------------------------" << endl;
            if ( cmisObj.get() )
            {
                libcmis::Document* doc = dynamic_cast< libcmis::Document* >( cmisObj.get() );
                if ( NULL != doc )
                {
                    doc->cancelCheckout( );
                    cout << "Checkout cancelled" << endl;
                }
                else
                    throw CommandException( string( "Not a document object id: " ) + objIds.front() );
            }
            else
                cout << "No such node: " << objIds.front() << endl;

            delete session;
        }
        else if ( "checkin" == command )
        {
            libcmis::Session* session = getSession( );

            // Get the ids of the objects to fetch
            if ( m_vm.count( "args" ) == 0 )
                throw CommandException( "Please provide the node id to checkin as command args" );

            vector< string > objIds = m_vm["args"].as< vector< string > >( );

            libcmis::ObjectPtr object = session->getObject( objIds.front() );
            if ( object.get() )
            {
                // Create the properties map
                map< string, libcmis::PropertyPtr > properties;
                map< string, string > propsToSet = getObjectProperties( );
                libcmis::ObjectTypePtr type = session->getType( object->getType( ) );
                map< string, libcmis::PropertyTypePtr > propertyTypes = type->getPropertiesTypes( );
                for ( map< string, string >::iterator propIt = propsToSet.begin();
                      propIt != propsToSet.end(); ++propIt )
                {
                    string name = propIt->first;
                    map< string, libcmis::PropertyTypePtr >::iterator typeIt = propertyTypes.find( name );
                    if ( typeIt != propertyTypes.end( ) )
                    {
                        vector< string > values;
                        values.push_back( propIt->second );
                        libcmis::PropertyPtr prop( new libcmis::Property( typeIt->second, values ) );
                        properties.insert( pair< string, libcmis::PropertyPtr >( name, prop ) );
                    }
                }

                // Get the content stream if any
                string contentType;
                string filename;
                boost::shared_ptr< ostream > stream;
                if ( m_vm.count( "input-file" ) > 0 )
                {
                    string file = m_vm["input-file"].as<string>();
                    ifstream is( file.c_str(), ios_base::in );
                    stringstream os;
                    os << is.rdbuf( );
                    if ( is.fail( ) )
                        throw CommandException( string( "Unable to open file " ) + file );

                    string content = os.str( );
                    stream.reset( new stringstream( content ) );
                    is.close( );
                    
                    if ( m_vm.count( "input-type" ) > 0 )
                    {
                        contentType = m_vm["input-type"].as<string>();
                    }
                    if ( m_vm.count( "input-name" ) > 0 )
                    {
                        filename = m_vm["input-name"].as<string>();
                    }
                }

                bool major = false;
                if ( m_vm.count( "major" ) > 0 )
                    major = "yes";

                string comment;
                if ( m_vm.count( "message" ) > 0 )
                    comment = m_vm["message"].as< string >( );

                libcmis::Document* doc = dynamic_cast< libcmis::Document* >( object.get() );
                if ( NULL != doc )
                {
                    libcmis::DocumentPtr newDoc = doc->checkIn( major, comment, properties, stream, contentType, filename );

                    cout << "------------------------------------------------" << endl;
                    cout << newDoc->toString() << endl;
                }
                else
                    throw CommandException( string( "Not a document object id: " ) + objIds.front() );
            }
            else
                cout << "No such node: " << objIds.front() << endl;

            delete session;
        }
        else if ( "get-versions" == command )
        {
            libcmis::Session* session = getSession( );

            // Get the ids of the objects to fetch
            if ( m_vm.count( "args" ) == 0 )
                throw CommandException( "Please provide the node id to get versions from as command args" );

            vector< string > objIds = m_vm["args"].as< vector< string > >( );

            libcmis::ObjectPtr object = session->getObject( objIds.front() );
            if ( object.get() )
            {
                libcmis::Document* doc = dynamic_cast< libcmis::Document* >( object.get() );
                if ( NULL != doc )
                {
                    vector< libcmis::DocumentPtr > versions = doc->getAllVersions( );

                    for ( vector< libcmis::DocumentPtr >::iterator it = versions.begin( );
                            it != versions.end( ); ++it )
                    {
                        cout << "------------------------------------------------" << endl;
                        cout << ( *it )->toString() << endl;
                    }
                }
                else
                    throw CommandException( string( "Not a document object id: " ) + objIds.front() );
            }
            else
                cout << "No such node: " << objIds.front() << endl;

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
        ( "proxy", value< string >(), "HTTP proxy url to override the system settings" )
        ( "noproxy", value< string >(), "Coma separated list if host and domain names not going"
                                        "through the proxy" )
        ( "proxy-username", value< string >(), "Username to authenticate on the proxy" )
        ( "proxy-password", value< string >(), "Password to authenticate on the proxy" )
        ( "oauth2-client-id", value< string >(), "OAuth2 application client_id" )
        ( "oauth2-client-secret", value< string >(), "OAuth2 application client_secret" )
        ( "oauth2-auth-url", value< string >(), "URL to authenticate in the OAuth2 flow" )
        ( "oauth2-token-url", value< string >(), "URL to convert code to tokens in the OAuth2 flow" )
        ( "oauth2-redirect-uri", value< string >(), "redirect URI indicating that the authentication is finished in OAuth2 flow" )
        ( "oauth2-scope", value< string >(), "The authentication scope in OAuth2" )
    ;

    options_description setcontentOpts( "modification operations options" );
    setcontentOpts.add_options( )
        ( "input-file", value< string >(), "File to push to the repository" )
        ( "input-type", value< string >(), "Mime type of the file to push to the repository" )
        ( "input-name", value< string >(), "Name of the file to push to the repository"
                                           "(may be different from the local name)" )
        ( "object-type", value< string >(), "CMIS type of the object to create" )
        ( "object-property", value< vector< string > >(), "under the form prop-id=prop-value, defines a property"
                                                "to be set on the object" )
        ( "message,m", value< string >(), "Check in message" )
        ( "major", "The version to create during the check in will be a major version." )
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
    cerr << "   create-document <Parent Id> <Document Name>\n"
            "           Creates a new document inside the folder <Parent Id>\n"
            "           named <Document Name>.\n"
            "           Note that --input-file and --input-type may be requested if\n"
            "           the server requires a content stream." << endl; 
    cerr << "   update-object <Object Id>\n"
            "           Update the object matching id <Object Id> with the properties\n"
            "           defined with --object-property." << endl;
    cerr << "   delete <Object Id 1> [... <Object Id N>]\n"
            "           Delete the objects corresponding to the ids. If the node"
            "           is a folder, its content will be removed as well." << endl;
    cerr << "   checkout <Object Id>\n"
            "           Check out the document corresponding to the id and shows the\n"
            "           Private Working Copy document infos." << endl;
    cerr << "   cancel-checkout <Object Id>\n"
            "           Cancel the Private Working Copy corresponding to the id" << endl;
    cerr << "   checkin <Object Id>\n"
            "           Check in the Private Working copy corresponding to the id.\n"
            "           Use the --message and --major parameters to give more\n"
            "           details about the new version.\n"
            "           The modification options may be needed to set the new\n"
            "           version properties and content stream if the repository\n"
            "           doesn't allow to change the private working copies." << endl;
    cerr << "   get-versions <Object-Id>\n"
            "           Show all the versions of a document." << endl;
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
    catch ( const CommandException& e )
    {
        cerr << "------------------------------------------------" << endl;
        cerr << "ERROR: " << e.what() << endl;
        cerr << "------------------------------------------------" << endl;
        client.printHelp();
        return 1;
    }
    catch ( const exception& e )
    {
        cerr << "------------------------------------------------" << endl;
        cerr << "ERROR: " << e.what() << endl;
        cerr << "------------------------------------------------" << endl;
        return 1;
    }

    return 0;
}
