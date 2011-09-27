#ifndef _FOLDER_HXX_
#define _FOLDER_HXX_

#include <string>
#include <vector>

#include "resource.hxx"

/** Class representing a CMIS folder.
  */
class Folder : public virtual Resource
{
    public:
        virtual std::vector< ResourcePtr > getChildren( ) = 0;
        virtual std::string getPath( ) = 0;
};
typedef ::boost::shared_ptr< Folder > FolderPtr;

#endif
