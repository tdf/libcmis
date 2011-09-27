#ifndef _ATOM_RESOURCE_HXX_
#define _ATOM_RESOURCE_HXX_

#include <libxml/tree.h>

#include "resource.hxx"

class AtomResource : public virtual Resource
{
    private:
        std::string m_infosUrl;

        std::string m_name;

    public:
        AtomResource( std::string url );
        ~AtomResource( );
        
        virtual std::string getName( );

    protected:

        std::string& getInfosUrl( ) { return m_infosUrl; }
        virtual void extractInfos( xmlDocPtr doc );
};

#endif
