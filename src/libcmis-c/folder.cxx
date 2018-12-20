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

#include <libcmis-c/folder.h>

#include "internals.hxx"

using namespace std;
using libcmis::PropertyPtrMap;
using libcmis::FolderPtr;
using boost::dynamic_pointer_cast;

void libcmis_vector_folder_free( libcmis_vector_folder_Ptr vector )
{
    delete vector;
}


size_t libcmis_vector_folder_size( libcmis_vector_folder_Ptr vector )
{
    size_t size = 0;
    if ( vector != NULL )
        size = vector->handle.size( );
    return size;
}


libcmis_FolderPtr libcmis_vector_folder_get( libcmis_vector_folder_Ptr vector, size_t i )
{
    libcmis_FolderPtr item = NULL;
    if ( vector != NULL && i < vector->handle.size( ) )
    {
        libcmis::FolderPtr handle = vector->handle[i];
        item = new ( nothrow ) libcmis_folder( );
        if ( item )
            item->handle = handle;
    }
    return item;
}


bool libcmis_is_folder( libcmis_ObjectPtr object )
{
    bool isFolder = false;
    if ( object != NULL && object->handle.get( ) != NULL )
    {
        libcmis::FolderPtr folder = boost::dynamic_pointer_cast< libcmis::Folder >( object->handle );
        isFolder = folder.get( ) != NULL;
    }
    return isFolder;
}


libcmis_FolderPtr libcmis_folder_cast( libcmis_ObjectPtr object )
{
    libcmis_FolderPtr folder = NULL;

    if ( object != NULL && object->handle.get( ) != NULL )
    {
        libcmis::FolderPtr handle = boost::dynamic_pointer_cast< libcmis::Folder >( object->handle );
        if ( handle.get( ) != NULL )
        {
            folder = new ( nothrow ) libcmis_folder( );
            if ( folder )
                folder->handle = handle;
        }
    }

    return folder;
}


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
            FolderPtr folderHandle = dynamic_pointer_cast< libcmis::Folder >( folder->handle );
            if ( folder )
            {
                libcmis::FolderPtr handle = folderHandle->getFolderParent( );
                if ( handle.get( ) != NULL )
                {
                    parent = new libcmis_folder( );
                    parent->handle = handle;
                }
            }
        }
        catch ( const libcmis::Exception& e )
        {
            if ( error != NULL )
            {
                error->message = strdup( e.what() );
                error->type = strdup( e.getType().c_str() );
            }
        }
        catch ( const bad_alloc& e )
        {
            if ( error != NULL )
            {
                error->message = strdup( e.what() );
                error->badAlloc = true;
            }
        }
    }
    return parent;
}


libcmis_vector_object_Ptr libcmis_folder_getChildren( libcmis_FolderPtr folder, libcmis_ErrorPtr error )
{
    libcmis_vector_object_Ptr result = NULL;
    if ( folder != NULL && folder->handle.get( ) != NULL )
    {
        try
        {
            libcmis::FolderPtr folderHandle = dynamic_pointer_cast< libcmis::Folder >( folder->handle );
            if ( folder )
            {
                std::vector< libcmis::ObjectPtr > handles = folderHandle->getChildren( );
                result = new libcmis_vector_object( );
                result->handle = handles;
            }
        }
        catch ( const libcmis::Exception& e )
        {
            if ( error != NULL )
            {
                error->message = strdup( e.what() );
                error->type = strdup( e.getType().c_str() );
            }
        }
        catch ( const bad_alloc& e )
        {
            if ( error != NULL )
            {
                error->message = strdup( e.what() );
                error->badAlloc = true;
            }
        }
    }
    return result;
}


char* libcmis_folder_getPath( libcmis_FolderPtr folder )
{
    char* path = NULL;
    if ( folder != NULL && folder->handle.get( ) != NULL )
    {
        libcmis::FolderPtr folderHandle = dynamic_pointer_cast< libcmis::Folder >( folder->handle );
        if ( folder )
            path = strdup( folderHandle->getPath( ).c_str( ) );
    }
    return path;
}


bool libcmis_folder_isRootFolder( libcmis_FolderPtr folder )
{
    bool isRoot = false;
    if ( folder != NULL && folder->handle.get( ) != NULL )
    {
        libcmis::FolderPtr folderHandle = dynamic_pointer_cast< libcmis::Folder >( folder->handle );
        if ( folder )
            isRoot = folderHandle->isRootFolder( );
    }
    return isRoot;
}

libcmis_FolderPtr libcmis_folder_createFolder(
        libcmis_FolderPtr folder,
        libcmis_vector_property_Ptr properties,
        libcmis_ErrorPtr error )
{
    libcmis_FolderPtr result = NULL;
    if ( folder != NULL && folder->handle.get( ) != NULL )
    {
        libcmis::FolderPtr folderHandle = dynamic_pointer_cast< libcmis::Folder >( folder->handle );
        if ( folder )
        {
            try
            {
                PropertyPtrMap mappedProperties;
                if ( properties != NULL )
                {
                    size_t size = properties->handle.size( );
                    for ( size_t i = 0; i < size; ++i )
                    {
                        libcmis::PropertyPtr property = properties->handle[i];
                        if ( property.get( ) != NULL )
                        {
                            string id = property->getPropertyType( )->getId( );
                            mappedProperties.insert( pair< string, libcmis::PropertyPtr >( id, property ) );
                        }
                    }
                }

                libcmis::FolderPtr handle = folderHandle->createFolder( mappedProperties );
                result = new libcmis_folder( );
                result->handle = handle;
            }
            catch ( const libcmis::Exception& e )
            {
                if ( error != NULL )
                {
                    error->message = strdup( e.what() );
                    error->type = strdup( e.getType().c_str() );
                }
            }
            catch ( const bad_alloc& e )
            {
                if ( error != NULL )
                {
                    error->message = strdup( e.what() );
                    error->badAlloc = true;
                }
            }
        }
    }
    return result;
}


libcmis_DocumentPtr libcmis_folder_createDocument(
        libcmis_FolderPtr folder,
        libcmis_vector_property_Ptr properties,
        libcmis_readFn readFn,
        void* userData,
        const char* contentType,
        const char* filename,
        libcmis_ErrorPtr error )
{
    libcmis_DocumentPtr created = NULL;
    if ( folder != NULL && folder->handle.get( ) != NULL )
    {
        libcmis::FolderPtr folderHandle = dynamic_pointer_cast< libcmis::Folder >( folder->handle );
        if ( folder )
        {
            try
            {
                // Create the ostream
                boost::shared_ptr< std::ostream > stream( new stringstream( ) );

                size_t bufSize = 2048;
                char* buf = new char[ bufSize ];
                size_t read = 0;
                do
                {
                    read = readFn( ( void * )buf, size_t( 1 ), bufSize, userData );
                    stream->write( buf, read );
                } while ( read == bufSize );
                delete[] buf;

                // Create the property map
                PropertyPtrMap propertiesMap;
                if ( properties != NULL )
                {
                    for ( vector< libcmis::PropertyPtr >::iterator it = properties->handle.begin( );
                            it != properties->handle.end( ); ++it )
                    {
                        string id = ( *it )->getPropertyType( )->getId( );
                        propertiesMap.insert( pair< string, libcmis::PropertyPtr >( id, *it ) );
                    }
                }

                libcmis::DocumentPtr handle = folderHandle->createDocument( propertiesMap, stream, contentType, filename );
                created = new libcmis_document( );
                created->handle = handle;
            }
            catch ( const libcmis::Exception& e )
            {
                if ( error != NULL )
                {
                    error->message = strdup( e.what() );
                    error->type = strdup( e.getType().c_str() );
                }
            }
            catch ( const bad_alloc& e )
            {
                if ( error != NULL )
                {
                    error->message = strdup( e.what() );
                    error->badAlloc = true;
                }
            }
            catch ( const exception& e )
            {
                if ( error != NULL )
                    error->message = strdup( e.what() );
            }
        }
    }
    return created;
}


libcmis_vector_string_Ptr libcmis_folder_removeTree( libcmis_FolderPtr folder,
        bool allVersion,
        libcmis_folder_UnfileObjects unfile,
        bool continueOnError,
        libcmis_ErrorPtr error )
{
    libcmis_vector_string_Ptr failed = NULL;
    try
    {
        failed = new libcmis_vector_string( );
        if ( folder != NULL && folder->handle.get( ) != NULL )
        {
            libcmis::FolderPtr folderHandle = dynamic_pointer_cast< libcmis::Folder >( folder->handle );
            if ( folder )
            {
                vector< string > handle = folderHandle->removeTree( allVersion,
                        libcmis::UnfileObjects::Type( unfile ), continueOnError );
                failed->handle = handle;
            }
        }
    }
    catch ( const libcmis::Exception& e )
    {
        if ( error != NULL )
        {
            error->message = strdup( e.what() );
            error->type = strdup( e.getType().c_str() );
        }
    }
    catch ( const bad_alloc& e )
    {
        if ( error != NULL )
        {
            error->message = strdup( e.what() );
            error->badAlloc = true;
        }
    }
    return failed;
}
