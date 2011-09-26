#include <libcmis/atom-session.hxx>

#include <string>
#include <cstdio>

using namespace std;

int main ( int argc, char* argv[] )
{
    string sAtomUrl( argv[1] );

    // Test the getRepositories
    list< string > ids = AtomPubSession::getRepositories( sAtomUrl );
    if ( ids.size( ) == 0 )
    {
        fprintf( stderr, "There should be at least one repository ID\n" );
        return 1;
    }
    string firstId = ids.front( );
    
    AtomPubSession session( sAtomUrl, firstId );

    if ( session.getCollectionUrl( Collection::Root ).empty() )
    {
        fprintf( stderr, "Missing root collection URL\n" );
        return 1;
    }

    string objectbyid = session.getUriTemplate( UriTemplate::ObjectById );
    fprintf( stdout, "objectbyid URI Template: %s\n", objectbyid.c_str() );
    if ( objectbyid.empty() )
    {
        fprintf( stderr, "Missing objectbyid uri template\n");
        return 1;
    }

    Folder* folder = session.getRootFolder( );
    fprintf( stdout, "Root name: %s\n", folder->getName( ).c_str() );
    fprintf( stdout, "Root path: %s\n",folder->getPath( ).c_str() );
    if ( folder->getName().empty() || folder->getPath().empty() )
    {
        fprintf( stderr, "Missing root folder name or path\n" );
        return 1;
    }

    delete folder;

    return 0;
}
