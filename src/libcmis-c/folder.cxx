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

#include "folder.h"
#include "internals.hxx"

using namespace std;


void libcmis_folder_free( libcmis_FolderPtr folder )
{
    delete folder;
}


libcmis_FolderPtr libcmis_folder_getParent( libcmis_FolderPtr folder, libcmis_ErrorPtr error )
{
    libcmis_FolderPtr parent = NULL;
    if ( folder != NULL && folder->handle.get( ) != NULL )
    {
        try
        {
            libcmis::FolderPtr handle = folder->handle->getFolderParent( );
            parent = new libcmis_folder( );
            parent->setHandle( handle );
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }
    return parent;
}


libcmis_ObjectPtr* libcmis_folder_getChildren( libcmis_FolderPtr folder, libcmis_ErrorPtr error )
{
    libcmis_ObjectPtr* result = NULL;
    if ( folder != NULL && folder->handle.get( ) != NULL )
    {
        try
        {
            std::vector< libcmis::ObjectPtr > handles = folder->handle->getChildren( );
            result = new libcmis_ObjectPtr[ handles.size( ) ];
            int i = 0;
            for ( std::vector< libcmis::ObjectPtr >::iterator it = handles.begin( );
                    it != handles.end( ); ++it, ++i )
            {
                libcmis_ObjectPtr child = new libcmis_object( );
                child->handle = *it;
                result[i] = child;
            }
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }
    return result;
}


char* libcmis_folder_getPath( libcmis_FolderPtr folder )
{
    char* path = NULL;
    if ( folder != NULL && folder->handle.get( ) != NULL )
        path = strdup( folder->handle->getPath( ).c_str( ) );
    return path;
}


bool libcmis_folder_isRootFolder( libcmis_FolderPtr folder )
{
    bool isRoot = false;
    if ( folder != NULL && folder->handle.get( ) != NULL )
        isRoot = folder->handle->isRootFolder( );
    return isRoot;
}

libcmis_FolderPtr libcmis_folder_createFolder(
        libcmis_FolderPtr folder,
        libcmis_PropertyPtr* properties,
        libcmis_ErrorPtr error )
{
    libcmis_FolderPtr result = NULL;
    if ( folder != NULL && folder->handle.get( ) != NULL )
    {
        try
        {
            map< string, libcmis::PropertyPtr > mappedProperties;
            int size = sizeof( properties ) / sizeof( *properties );
            for ( int i = 0; i < size; ++i )
            {
                libcmis_PropertyPtr property = properties[i];
                if ( property != NULL && property->handle.get( ) != NULL )
                {
                    string id = property->handle->getPropertyType( )->getId( );
                    mappedProperties.insert( pair< string, libcmis::PropertyPtr >( id, property->handle ) );
                }
            }

            libcmis::FolderPtr handle = folder->handle->createFolder( mappedProperties );
            result = new libcmis_folder( );
            result->setHandle( handle );
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }
    return result;
}


void libcmis_folder_removeTree( libcmis_FolderPtr folder,
        bool allVersion,
        libcmis_folder_UnfileObjects unfile,
        bool continueOnError,
        libcmis_ErrorPtr error )
{
    if ( folder != NULL && folder->handle.get( ) != NULL )
    {
        try
        {
            folder->handle->removeTree( allVersion,
                    libcmis::UnfileObjects::Type( unfile ), continueOnError );
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }
}
