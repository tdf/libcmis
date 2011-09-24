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
        Folder( std::string path, std::string name );
        ~Folder( );

        std::vector< Resource > getChildren( ) { return m_aChildren; }
};
