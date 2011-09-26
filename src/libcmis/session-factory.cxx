#include "atom-session.hxx"
#include "session-factory.hxx"

using namespace std;

Session* SessionFactory::createSession( map< int, string > params )
{
    Session* session = NULL;

    // TODO implement me
    string repository;
    map< int, string >::iterator pIt = params.find( ATOMPUB_URL );
    if ( pIt != params.end( ) )
        repository = pIt->second;
    
    pIt = params.find( ATOMPUB_URL );
    if ( pIt != params.end( ) )
        session = new AtomPubSession( pIt->second, repository );

    return session;
}

list< string > SessionFactory::getRepositories( map< int, string > params )
{
    list< string > repos;

    map< int, string >::iterator pIt = params.find( ATOMPUB_URL );
    if ( pIt != params.end( ) )
    {
        repos = AtomPubSession::getRepositories( pIt->second );
    }

    return repos;
}
