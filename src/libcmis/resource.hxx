#ifndef _RESOURCE_HXX_
#define _RESOURCE_HXX_

#include <string>

#include <boost/shared_ptr.hpp>

/** Class representing a CMIS resource like files and folders.
  */
class Resource
{
    public:
        virtual std::string getName( ) = 0;
};

typedef ::boost::shared_ptr< Resource > ResourcePtr;

#endif
