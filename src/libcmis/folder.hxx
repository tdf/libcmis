#ifndef _FOLDER_HXX_
#define _FOLDER_HXX_

#include <string>
#include <vector>

#include "resource.hxx"

/** Class representing a CMIS folder.
  */
class Folder : public Resource
{
    public:
        virtual std::vector< Resource > getChildren( ) = 0;
};

#endif
