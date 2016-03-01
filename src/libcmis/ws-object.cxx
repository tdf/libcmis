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

#include "ws-object.hxx"
#include "ws-document.hxx"
#include "ws-folder.hxx"

using namespace std;
using libcmis::PropertyPtrMap;

WSObject::WSObject( WSSession* session ) :
    libcmis::Object( session )
{
}


WSObject::WSObject( WSSession* session, xmlNodePtr node ) :
    libcmis::Object( session, node )
{
}

WSObject::WSObject( const WSObject& copy ) :
    libcmis::Object( copy )
{
}

WSObject::~WSObject( )
{
}

WSObject& WSObject::operator=( const WSObject& copy )
{
    if ( this != &copy )
    {
        libcmis::Object::operator=( copy );
    }

    return *this;
}

vector< libcmis::RenditionPtr > WSObject::getRenditions( string filter ) throw ( libcmis::Exception )
{
    // Check that the server supports that optional feature. There is no need to check it
    // when getting the object as we may get them by shear luck
    libcmis::RepositoryPtr repo = getSession( )->getRepository( );
    bool isCapable = repo && repo->getCapability( libcmis::Repository::Renditions ) == "read";

    if ( m_renditions.empty() && isCapable )
    {
        string repoId = getSession( )->getRepositoryId( );
        m_renditions = getSession( )->getObjectService( ).getRenditions( repoId, this->getId( ), filter );
    }
    return m_renditions;
}

libcmis::ObjectPtr WSObject::updateProperties(
        const PropertyPtrMap& properties ) throw ( libcmis::Exception )
{
    // No need to send HTTP request if there is nothing to update
    if ( properties.empty( ) )
    {
        libcmis::ObjectPtr object;
        if ( getBaseType( ) == "cmis:document" )
        {
            const WSDocument& thisDoc = dynamic_cast< const WSDocument& >( *this );
            object.reset( new WSDocument( thisDoc ) );
        }
        else if ( getBaseType( ) == "cmis:folder" )
        {
            const WSFolder& thisFolder = dynamic_cast< const WSFolder& >( *this );
            object.reset( new WSFolder( thisFolder ) );
        }
        return object;
    }
    string repoId = getSession( )->getRepositoryId( );
    return getSession( )->getObjectService( ).updateProperties( repoId, this->getId( ), properties, this->getChangeToken( ) );
}

void WSObject::refresh( ) throw ( libcmis::Exception )
{
    libcmis::ObjectPtr object = m_session->getObject( getId( ) );
    WSObject* const other = dynamic_cast< WSObject* >( object.get( ) );
    if ( other != NULL )
        *this = *other;
}

void WSObject::remove( bool allVersions ) throw ( libcmis::Exception )
{
    string repoId = getSession( )->getRepositoryId( );
    getSession( )->getObjectService( ).deleteObject( repoId, this->getId( ), allVersions );
}

void WSObject::move( libcmis::FolderPtr source, libcmis::FolderPtr destination ) throw ( libcmis::Exception )
{
    string repoId = getSession( )->getRepositoryId( );
    getSession( )->getObjectService( ).move( repoId, getId( ), destination->getId( ), source->getId( ) );

    refresh( );
}

WSSession* WSObject::getSession( )
{
    return dynamic_cast< WSSession* >( m_session );
}
