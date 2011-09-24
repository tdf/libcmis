#include "resource.hxx"

using namespace std;

Resource::Resource( string path, string name ) :
    m_sPath( path ),
    m_sName( name )
{
}

Resource::~Resource( )
{
}
