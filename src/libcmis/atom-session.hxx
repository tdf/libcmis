#include <list>
#include <map>
#include <string>

#include <libxml/xmlstring.h>
#include <libxml/xpath.h>

#include "session.hxx"


enum CollectionType {
    Root,
    Types,
    Query,
    Checkedout,
    Unfiled,
    Unknown
};

class AtomPubSession : public Session
{
    private:
        std::string m_sAtomPubUrl;
        std::string m_sRepository;

        // Collections URLs
        std::map< CollectionType, std::string > m_aCollections;

    public:
        AtomPubSession( std::string sAtomPubUrl, std::string repository );
        ~AtomPubSession( );

        static std::list< std::string > getRepositories( std::string url );

        std::string getCollectionUrl( CollectionType );

        // Override session methods

        virtual Folder getRootFolder();

    private:
        void readCollections( xmlNodeSetPtr pNodeSet );

        Folder getFolder( std::string urlGet );
        
        static void http_request( std::string Url,
                size_t (*pCallback)( void *, size_t, size_t, void* ),
                void* pData );

        
        static size_t parseServiceDocument( void* pBuffer, size_t size,
                size_t nmemb, void* pUserData );

        static size_t parseFolder( void* pBuffer, size_t size,
                size_t nmemb, void* pUserData );
};
