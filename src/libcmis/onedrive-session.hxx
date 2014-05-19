#ifndef _ONEDRIVE_SESSION_HXX_
#define _ONEDRIVE_SESSION_HXX_

#include "base-session.hxx"
#include "repository.hxx"

class OneDriveSession : public BaseSession
{
    public:
        OneDriveSession( std::string baseUrl,
                       std::string username, 
                       std::string password,
                       libcmis::OAuth2DataPtr oauth2,
                       bool verbose = false )
                   throw ( libcmis::Exception );

        OneDriveSession( const OneDriveSession& copy );

        ~OneDriveSession ( );

        virtual libcmis::RepositoryPtr getRepository( ) 
            throw ( libcmis::Exception );

        virtual bool setRepository( std::string ) { return true; }

        virtual libcmis::ObjectPtr getObject( std::string id ) 
            throw ( libcmis::Exception );

        virtual libcmis::ObjectPtr getObjectByPath( std::string path ) 
            throw ( libcmis::Exception );

        virtual libcmis::ObjectTypePtr getType( std::string id )             
            throw ( libcmis::Exception );
        
        virtual std::vector< libcmis::ObjectTypePtr > getBaseTypes( )
            throw ( libcmis::Exception );

    private:
        OneDriveSession( );
};

#endif /* _ONEDRIVE_SESSION_HXX_ */
