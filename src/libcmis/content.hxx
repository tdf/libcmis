#ifndef _CONTENT_HXX_
#define _CONTENT_HXX_

#include <string>

#include "resource.hxx"

class Content : public virtual Resource
{
    public:
        virtual void getContent( size_t (*pCallback)( void *, size_t, size_t, void* ) ) = 0;
        virtual std::string getContentType( ) = 0;
};

#endif
