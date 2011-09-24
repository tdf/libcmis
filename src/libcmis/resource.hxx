#include <string>

/** Class representing a CMIS resource like files and folders.
  */
class Resource
{
    private:
        std::string m_sPath;
        std::string m_sName;

    public:
        Resource( std::string path, std::string name );
        ~Resource( );

        std::string getPath( ) { return m_sPath; }
        std::string getName( ) { return m_sName; }
};
