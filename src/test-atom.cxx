#include <libcmis/session.hxx>

#include <string>
#include <cstdio>

using namespace std;

int main ( int argc, char* argv[] )
{
    string sAtomUrl( argv[1] );
    AtomPubSession session( sAtomUrl );

    if ( session.getCollectionUrl( Root ).empty() )
    {
        fprintf( stderr, "Missing root collection URL\n" );
        return 1;
    }

    return 0;
}
