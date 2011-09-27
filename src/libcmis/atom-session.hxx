#ifndef _ATOM_SESSION_HXX_
#define _ATOM_SESSION_HXX_

#include <list>
#include <map>
#include <string>

#include <libxml/xmlstring.h>
#include <libxml/xpath.h>

#include "session.hxx"

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

        virtual FolderPtr getRootFolder();

    private:
        void readCollections( xmlNodeSetPtr pNodeSet );
        void readUriTemplates( xmlNodeSetPtr pNodeSet );

        FolderPtr getFolder( std::string id );
};

#endif
