#ifndef _ATOM_CONTENT_HXX_
#define _ATOM_CONTENT_HXX_

#include <string>

#include "content.hxx"
#include "atom-resource.hxx"

class AtomContent : public Content, public AtomResource
{
    private:
        std::string m_contentUrl;
        std::string m_contentType;

    public:
        AtomContent( std::string url );
        AtomContent( xmlNodePtr entryNd );
        ~AtomContent( );

        // Override content methods
        void getContent( size_t (*pCallback)( void *, size_t, size_t, void* ) );
        std::string getContentType( ) { return m_contentType; }
    
    protected:
        virtual void extractInfos( xmlDocPtr doc );
};

#endif
