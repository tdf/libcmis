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

#include "folder.hxx"
#include "session.hxx"

using namespace std;

namespace libcmis
{
    vector< string > Folder::getPaths( )
    {
        vector< string > paths;
        paths.push_back( getPath( ) );
        return paths;
    }
    
    libcmis::FolderPtr Folder::getFolderParent( ) throw ( libcmis::Exception )
    {
        if ( getAllowableActions( ).get() && !getAllowableActions()->isAllowed( libcmis::ObjectAction::GetFolderParent ) )
            throw libcmis::Exception( string( "GetFolderParent not allowed on node " ) + getId() );

        if ( m_session == NULL )
            throw libcmis::Exception( string( "Session not defined on the object... weird!" ) );

        return m_session->getFolder( getParentId( ) ); 
    }

    string Folder::getParentId( )
    {
        return getStringProperty( "cmis:parentId" );
    }

    string Folder::getPath( )
    {
        return getStringProperty( "cmis:path" );
    }

    bool Folder::isRootFolder( )
    {
        return getParentId( ).empty( );
    }

    string Folder::toString( )
    {
        stringstream buf;

        buf << "Folder Object:" << endl << endl;
        buf << Object::toString();
        buf << "Path: " << getPath() << endl;
        buf << "Folder Parent Id: " << getParentId( ) << endl;
        buf << "Children [Name (Id)]:" << endl;

        vector< libcmis::ObjectPtr > children = getChildren( );
        for ( vector< libcmis::ObjectPtr >::iterator it = children.begin( );
                it != children.end(); ++it )
        {
            libcmis::ObjectPtr child = *it;
            buf << "    " << child->getName() << " (" << child->getId() << ")" << endl;
        }

        return buf.str();
    }
}
