#ifndef _ATOM_RESOURCE_HXX_
#define _ATOM_RESOURCE_HXX_

#include <libxml/tree.h>

#include "resource.hxx"

class AtomResource : public Resource
{
    private:
        std::string m_infosUrl;

        std::string m_name;
        std::string m_path;

    public:
        AtomResource( std::string url );
        ~AtomResource( );
        
        virtual std::string getName( );
        virtual std::string getPath( );

    protected:

        std::string& getInfosUrl( ) { return m_infosUrl; }
        virtual void extractInfos( xmlDocPtr doc );
};

#endif
