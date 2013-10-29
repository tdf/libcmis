/* libcmis
 * Version: MPL 1.1 / GPLv2+ / LGPLv2+
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License or as specified alternatively below. You may obtain a copy of
 * the License at http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * Major Contributor(s):
 * Copyright (C) 2011 SUSE <cbosdonnat@suse.com>
 *
 *
 * All Rights Reserved.
 *
 * For minor contributions see the git repository.
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPLv2+"), or
 * the GNU Lesser General Public License Version 2 or later (the "LGPLv2+"),
 * in which case the provisions of the GPLv2+ or the LGPLv2+ are applicable
 * instead of those above.
 */
#include <cctype>
#include <string>

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpath.h>

#include "base-session.hxx"
#include "oauth2-handler.hxx"
#include "session-factory.hxx"
#include "xml-utils.hxx"

using namespace std;

BaseSession::BaseSession( string bindingUrl, string repositoryId, string username,
        string password, bool noSslCheck, libcmis::OAuth2DataPtr oauth2, bool verbose ) throw ( libcmis::Exception ) :
    Session( ),
    HttpSession( username, password, noSslCheck, oauth2, verbose ),
    m_bindingUrl( bindingUrl ),
    m_repositoryId( repositoryId ),
    m_repositories( )
{
}

BaseSession::BaseSession( string sBindingUrl, string repository,
             const HttpSession& httpSession ) throw ( libcmis::Exception ) :
    Session( ),
    HttpSession( httpSession ),
    m_bindingUrl( sBindingUrl ),
    m_repositoryId( repository ),
    m_repositories( )
{
}

BaseSession::BaseSession( const BaseSession& copy ) :
    Session( ),
    HttpSession( copy ),
    m_bindingUrl( copy.m_bindingUrl ),
    m_repositoryId( copy.m_repositoryId ),
    m_repositories( copy.m_repositories )
{
}

BaseSession::BaseSession( ) :
    Session( ),
    HttpSession( ),
    m_bindingUrl( ),
    m_repositoryId( ),
    m_repositories( )
{
}

BaseSession& BaseSession::operator=( const BaseSession& copy )
{
    if ( this != &copy )
    {
        HttpSession::operator=( copy );
        m_bindingUrl = copy.m_bindingUrl;
        m_repositoryId = copy.m_repositoryId;
        m_repositories = copy.m_repositories;
    }

    return *this;
}

BaseSession::~BaseSession( )
{
}

string BaseSession::createUrl( const string& pattern, map< string, string > variables )
{
    string url( pattern );

    // Decompose the pattern and replace the variables by their values
    map< string, string >::iterator it = variables.begin( );
    while ( it != variables.end( ) )
    {
        string name = "{";
        name += it->first;
        name += "}";
        string value = it->second;

        // Search and replace the variable
        size_t pos = url.find( name );
        if ( pos != string::npos )
        {
            // Escape the URL by chunks
            url = url.replace( pos, name.size(), libcmis::escape( value ) );
        }

        ++it;
    }

    // Cleanup the remaining unset variables
    size_t pos1 = url.find( '{' );
    while ( pos1 != string::npos )
    {
        // look for the closing bracket
        size_t pos2 = url.find( '}', pos1 );
        if ( pos2 != string::npos )
            url.erase( pos1, pos2 - pos1 + 1 );

        pos1 = url.find( '{', pos1 - 1 );
    }

    return url;
}


void BaseSession::setNoSSLCertificateCheck( bool noCheck )
{
    HttpSession::setNoSSLCertificateCheck( noCheck );
}

 void BaseSession::setOAuth2Data( libcmis::OAuth2DataPtr oauth2 )
            throw ( libcmis::Exception )
{
    m_oauth2Handler = new OAuth2Handler( this, oauth2 );
    m_oauth2Handler->setOAuth2Parser( OAuth2Providers::getOAuth2Parser( getBindingUrl( ) ) );

    oauth2Authenticate( );
}

vector< libcmis::RepositoryPtr > BaseSession::getRepositories( )
{
    return m_repositories;
}

libcmis::FolderPtr BaseSession::getRootFolder() throw ( libcmis::Exception )
{
    return getFolder( getRootId() );
}

libcmis::FolderPtr BaseSession::getFolder( string id ) throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object = getObject( id );
    libcmis::FolderPtr folder = boost::dynamic_pointer_cast< libcmis::Folder >( object );
    return folder;
}
