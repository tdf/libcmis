#include "oauth2-handler.hxx"
#include "onedrive-session.hxx"
#include "gdrive-repository.hxx"

using namespace std;

OneDriveSession::OneDriveSession ( string baseUrl,
                               string username,
                               string password,
                               libcmis::OAuth2DataPtr oauth2,
                               bool verbose )
                                    throw ( libcmis::Exception ) :
    BaseSession( baseUrl, string(), username, password, false,
                 libcmis::OAuth2DataPtr(), verbose )

{
    // Add the dummy repository
    m_repositories.push_back( getRepository( ) );

    if ( oauth2 && oauth2->isComplete( ) ){
        setOAuth2Data( oauth2 );
    }
}

OneDriveSession::OneDriveSession( const OneDriveSession& copy ) :
    BaseSession( copy )
{
}

OneDriveSession::OneDriveSession() :
    BaseSession()
{
}

OneDriveSession::~OneDriveSession()
{
}

libcmis::RepositoryPtr OneDriveSession::getRepository( )
    throw ( libcmis::Exception )
{
    // Return a dummy repository since OneDrive doesn't have that notion
    // I think a Gdrive repo will do for the moment
    libcmis::RepositoryPtr repo( new GdriveRepository( ) );
    return repo;
}

libcmis::ObjectPtr OneDriveSession::getObject( string objectId )
    throw ( libcmis::Exception )
{
    objectId += "";
    libcmis::ObjectPtr object;
    return object;
}

libcmis::ObjectPtr OneDriveSession::getObjectByPath( string path )
    throw ( libcmis::Exception )
{
    return getObject( path );
}

libcmis::ObjectTypePtr OneDriveSession::getType( string id )
    throw ( libcmis::Exception )
{
    id += "";
    libcmis::ObjectTypePtr type;
    return type;
}

vector< libcmis::ObjectTypePtr > OneDriveSession::getBaseTypes( )
    throw ( libcmis::Exception )
{
    vector< libcmis::ObjectTypePtr > types;
    return types;
}
