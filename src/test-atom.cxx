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

    if ( session.getCollectionUrl( Root ).empty() )
    {
        fprintf( stderr, "Missing root collection URL\n" );
        return 1;
    }

    return 0;
}
