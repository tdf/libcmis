#include <libcmis/session.hxx>

#include <string>
#include <cstdio>

using namespace std;

int main ( int argc, char* argv[] )
{
    string sUrl( "http://localhost:8080/inmemory/atom" );
    AtomPubSession session( sUrl );

    printf( "root collection URL: %s\n", session.getCollectionUrl( Root ).c_str()  );

    return 0;
}
