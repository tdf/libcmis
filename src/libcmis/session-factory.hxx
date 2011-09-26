#include <map>
#include <string>

#include "session.hxx"

#define ATOMPUB_URL 0
#define REPOSITORY_ID 1

class SessionFactory
{
    public:

        /** Create a session from the given parameters.

            The resulting pointer should be deleted by the caller.
          */
        Session* createSession( std::map< int, std::string > params );

        std::list< std::string > getRepositories( std::map< int, std::string > params );
}
