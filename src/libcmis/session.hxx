#include "folder.hxx"

class Session
{
    public:

        /** Get the Root folder of the repository
          */
        virtual Folder getRootFolder() = 0;
};
