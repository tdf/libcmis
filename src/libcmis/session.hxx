#include <string>
#include <libxml/xmlstring.h>

class AtomPubSession
{
    private:
        std::string m_sAtomPubUrl;
        std::string m_sWorkspace;

        // Collections URLs
        std::string m_sRootCollectionUrl;
        std::string m_sTypesCollectionUrl;
        std::string m_sQueryCollectionUrl;
        std::string m_sCheckedoutCollectionUrl;
        std::string m_sUnfiledCollectionUrl;


    public:
        AtomPubSession( std::string sAtomPubUrl );
        ~AtomPubSession( );

    private:
        void readCollections( xmlNodeSetPtr pNodeSet );

        void http_request( std::string Url,
                size_t (*pCallback)( void *, size_t, size_t, void* ) );
        
        static size_t parseServiceDocument( void* pBuffer, size_t size,
                size_t nmemb, void* pUser_data );
};
