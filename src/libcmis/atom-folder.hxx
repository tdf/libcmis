#ifndef _ATOM_FOLDER_HXX_
#define _ATOM_FOLDER_HXX_

#include <string>

#include "folder.hxx"
#include "atom-resource.hxx"

class AtomFolder : public Folder, public AtomResource
{
    private:
        std::string m_childrenUrl;

    public:
        AtomFolder( std::string url );
        ~AtomFolder( );

        // virtual pure methods from Folder
        virtual std::vector< Resource > getChildren( );
        
        // virtual pure methods from Resource
        virtual std::string getName( );
        virtual std::string getPath( );

    protected:
        virtual void extractInfos( xmlDocPtr doc );

    private:
        static size_t parseFolderInfos( void* pBuffer, size_t size, size_t nmemb, void* pUserData );
};

#endif
