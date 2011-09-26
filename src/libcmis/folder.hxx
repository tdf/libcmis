#include <string>
#include <vector>

#include "resource.hxx"

/** Class representing a CMIS folder.
  */
class Folder : public Resource
{
    private:
        std::vector< Resource > m_aChildren;

    public:
        Folder( );
        ~Folder( );

        std::vector< Resource > getChildren( ) { return m_aChildren; }
};
