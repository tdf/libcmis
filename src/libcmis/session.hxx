#include <string>
#include <map>

#include <libxml/xmlstring.h>
#include <libxml/xpath.h>


enum CollectionType {
    Root,
    Types,
    Query,
    Checkedout,
    Unfiled,
    Unknown
};

class AtomPubSession
{
    private:
        std::string m_sAtomPubUrl;
        std::string m_sWorkspace;

        // Collections URLs
        std::map< CollectionType, std::string > m_aCollections;

    public:
        AtomPubSession( std::string sAtomPubUrl );
        ~AtomPubSession( );

        std::string getCollectionUrl( CollectionType );

    private:
        void readCollections( xmlNodeSetPtr pNodeSet );

        void http_request( std::string Url,
                size_t (*pCallback)( void *, size_t, size_t, void* ) );
        
        static size_t parseServiceDocument( void* pBuffer, size_t size,
                size_t nmemb, void* pUser_data );
};
