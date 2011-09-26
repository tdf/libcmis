#include <list>
#include <map>
#include <string>

#include <libxml/xmlstring.h>
#include <libxml/xpath.h>

#include "session.hxx"

#define URI_TEMPLATE_VAR_ID = std::string( "id" )

struct Collection {
    enum Type
    {
        Root,
        Types,
        Query,
        Checkedout,
        Unfiled
    };
};

struct UriTemplate {
    enum Type
    {
        ObjectById,
        ObjectByPath,
        TypeById,
        Query
    };

    static std::string createUrl( const std::string& pattern, std::map< std::string, std::string > variables );
};

class AtomPubSession : public Session
{
    private:
        std::string m_sAtomPubUrl;
        std::string m_sRepository;
        std::string m_sRootId;

        // Collections URLs
        std::map< Collection::Type, std::string > m_aCollections;

        // URI templates
        std::map< UriTemplate::Type, std::string > m_aUriTemplates;

    public:
        AtomPubSession( std::string sAtomPubUrl, std::string repository );
        ~AtomPubSession( );

        static std::list< std::string > getRepositories( std::string url );

        std::string getCollectionUrl( Collection::Type );

        std::string getUriTemplate( UriTemplate::Type );

        // Override session methods

        virtual Folder getRootFolder();

    private:
        void readCollections( xmlNodeSetPtr pNodeSet );
        void readUriTemplates( xmlNodeSetPtr pNodeSet );

        Folder getFolder( std::string urlGet );
        
        static void http_request( std::string Url,
                size_t (*pCallback)( void *, size_t, size_t, void* ),
                void* pData );

        
        static size_t parseServiceDocument( void* pBuffer, size_t size,
                size_t nmemb, void* pUserData );

        static size_t parseFolder( void* pBuffer, size_t size,
                size_t nmemb, void* pUserData );
};
