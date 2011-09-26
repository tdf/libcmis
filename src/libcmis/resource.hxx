#include <string>

/** Class representing a CMIS resource like files and folders.
  */
class Resource
{
    private:
        std::string m_sPath;
        std::string m_sName;

    public:
        Resource( );
        ~Resource( );

        std::string getPath( ) { return m_sPath; }
        std::string getName( ) { return m_sName; }

        void setPath( std::string path ) { m_sPath = path; }
        void setName( std::string name ) { m_sName = name; }
};
