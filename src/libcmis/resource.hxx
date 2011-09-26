#ifndef _RESOURCE_HXX_
#define _RESOURCE_HXX_

#include <string>

/** Class representing a CMIS resource like files and folders.
  */
class Resource
{
    public:

        virtual std::string getPath( ) = 0;
        virtual std::string getName( ) = 0;
};

#endif
