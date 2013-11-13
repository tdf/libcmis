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
#include "object.h"

using namespace std;
using libcmis::PropertyPtrMap;

namespace
{
    string lcl_stdString( const char* str )
    {
        string result;
        if ( str )
            result = string( str );
        return result;
    }

    PropertyPtrMap lcl_createPropertiesMap( libcmis_vector_property_Ptr properties )
    {
        PropertyPtrMap propertiesMap;
        if ( properties )
        {
            for ( vector< libcmis::PropertyPtr >::iterator it = properties->handle.begin( );
                    it != properties->handle.end( ); ++it )
            {
                libcmis::PropertyPtr propHandle = *it;
                propertiesMap[ propHandle->getPropertyType()->getId( ) ] = propHandle;
            }
        }
        return propertiesMap;
    }
}

void libcmis_vector_object_free( libcmis_vector_object_Ptr vector )
{
    delete vector;
}


size_t libcmis_vector_object_size( libcmis_vector_object_Ptr vector )
{
    size_t size = 0;
    if ( vector != NULL )
        size = vector->handle.size( );
    return size;
}


libcmis_ObjectPtr libcmis_vector_object_get( libcmis_vector_object_Ptr vector, size_t i )
{
    libcmis_ObjectPtr item = NULL;
    if ( vector != NULL && i < vector->handle.size( ) )
    {
        libcmis::ObjectPtr type = vector->handle[i];
        item = new ( nothrow ) libcmis_object( );
        if ( item )
            item->handle = type;
    }
    return item;
}


void libcmis_object_free( libcmis_ObjectPtr object )
{
    delete object;
}


char* libcmis_object_getId( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return strdup( object->handle->getId( ).c_str( ) );
    else
        return NULL;
}


char* libcmis_object_getName( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return strdup( object->handle->getName( ).c_str( ) );
    else
        return NULL;
}

libcmis_vector_string_Ptr libcmis_object_getPaths( libcmis_ObjectPtr object )
{
    libcmis_vector_string_Ptr c_paths = NULL;
    if ( object != NULL && object->handle != NULL )
    {
        std::vector< std::string > paths = object->handle->getPaths( );
        c_paths = new ( nothrow ) libcmis_vector_string( );
        if ( c_paths )
            c_paths->handle = paths;
    }
    return c_paths;
}

char* libcmis_object_getBaseType( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return strdup( object->handle->getBaseType( ).c_str( ) );
    else
        return NULL;
}


char* libcmis_object_getType( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return strdup( object->handle->getType( ).c_str( ) );
    else
        return NULL;
}


char* libcmis_object_getCreatedBy( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return strdup( object->handle->getCreatedBy( ).c_str( ) );
    else
        return NULL;
}


time_t libcmis_object_getCreationDate( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
    {
        tm time = boost::posix_time::to_tm( object->handle->getCreationDate( ) );
        return mktime( &time );
    }
    else
        return 0;
}


char* libcmis_object_getLastModifiedBy( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return strdup( object->handle->getLastModifiedBy( ).c_str( ) );
    else
        return NULL;
}


time_t libcmis_object_getLastModificationDate( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
    {
        tm time = boost::posix_time::to_tm( object->handle->getLastModificationDate( ) );
        return mktime( &time );
    }
    else
        return 0;
}


char* libcmis_object_getChangeToken( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return strdup( object->handle->getChangeToken( ).c_str( ) );
    else
        return NULL;
}

char* libcmis_object_getThumbnailUrl( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return strdup( object->handle->getThumbnailUrl( ).c_str( ) );
    else
        return NULL;
}

libcmis_vector_rendition_Ptr libcmis_object_getRenditions( libcmis_ObjectPtr object,
                                                           libcmis_ErrorPtr error )
{
    libcmis_vector_rendition_Ptr result = NULL;
    if ( object != NULL && object->handle.get( ) != NULL )
    {
        try
        {
            std::vector< libcmis::RenditionPtr > handles = object->handle->getRenditions( );
            result = new libcmis_vector_rendition( );
            result->handle = handles;
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

bool libcmis_object_isImmutable( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return object->handle->isImmutable( );
    else
        return true;
}

libcmis_vector_string_Ptr libcmis_object_getSecondaryTypes( libcmis_ObjectPtr object )
{
    libcmis_vector_string_Ptr c_types = NULL;
    if ( object != NULL && object->handle != NULL )
    {
        vector< string > types = object->handle->getSecondaryTypes( );
        c_types = new ( nothrow ) libcmis_vector_string( );
        if ( c_types )
            c_types->handle = types;
    }
    return c_types;
}

libcmis_ObjectPtr
libcmis_object_addSecondaryType( libcmis_ObjectPtr object,
                                 const char* id,
                                 libcmis_vector_property_Ptr properties,
                                 libcmis_ErrorPtr error )
{
    libcmis_ObjectPtr updated = NULL;
    if ( object != NULL && object->handle != NULL && properties != NULL )
    {
        try
        {
            PropertyPtrMap propertiesMap = lcl_createPropertiesMap( properties );
            libcmis::ObjectPtr result = object->handle->addSecondaryType(
                                                    lcl_stdString( id ),
                                                    propertiesMap );
            updated = new libcmis_object( );
            updated->handle = result;
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
    return updated;
}

libcmis_ObjectPtr
libcmis_object_removeSecondaryType( libcmis_ObjectPtr object,
                                    const char* id,
                                    libcmis_ErrorPtr error )
{
    libcmis_ObjectPtr updated = NULL;
    if ( object != NULL && object->handle != NULL )
    {
        try
        {
            libcmis::ObjectPtr result = object->handle->removeSecondaryType(
                                                lcl_stdString( id ) );
            updated = new libcmis_object( );
            updated->handle = result;
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
    return updated;
}

libcmis_vector_property_Ptr libcmis_object_getProperties( libcmis_ObjectPtr object )
{
    libcmis_vector_property_Ptr properties = NULL;
    if ( object != NULL && object->handle.get( ) != NULL )
    {
        PropertyPtrMap& handles = object->handle->getProperties( );
        properties = new ( nothrow ) libcmis_vector_property( );
        if ( properties )
        {
            int i = 0;
            for ( PropertyPtrMap::iterator it = handles.begin( );
                    it != handles.end( ); ++it, ++i )
            {
                properties->handle.push_back( it->second );
            }
        }
    }
    return properties;
}


libcmis_PropertyPtr libcmis_object_getProperty( libcmis_ObjectPtr object, const char* name )
{
    libcmis_PropertyPtr property = NULL;
    if ( object != NULL && object->handle.get( ) != NULL )
    {
        PropertyPtrMap& handles = object->handle->getProperties( );
        PropertyPtrMap::iterator it = handles.find( lcl_stdString( name ) );
        if ( it != handles.end( ) )
        {
            property = new ( nothrow ) libcmis_property( );
            if ( property )
                property->handle = it->second;
        }
    }
    return property;
}


libcmis_ObjectPtr libcmis_object_updateProperties(
        libcmis_ObjectPtr object,
        libcmis_vector_property_Ptr properties,
        libcmis_ErrorPtr error )
{
    libcmis_ObjectPtr result = NULL;
    if ( object != NULL && object->handle != NULL && properties != NULL )
    {
        try
        {
            // Build the map of changed properties
            PropertyPtrMap propertiesMap = lcl_createPropertiesMap( properties );
            libcmis::ObjectPtr handle = object->handle->updateProperties( propertiesMap );
            result = new libcmis_object( );
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
    return result;
}


libcmis_ObjectTypePtr libcmis_object_getTypeDescription( libcmis_ObjectPtr object )
{
    libcmis_ObjectTypePtr result = NULL;
    if ( object != NULL && object->handle.get( ) != NULL )
    {
        result = new ( nothrow ) libcmis_object_type( );
        if ( result )
            result->handle = object->handle->getTypeDescription( );
    }
    return result;
}


libcmis_AllowableActionsPtr libcmis_object_getAllowableActions( libcmis_ObjectPtr object )
{
    libcmis_AllowableActionsPtr result = NULL;
    if ( object != NULL && object->handle.get( ) != NULL )
    {
        result = new ( nothrow ) libcmis_allowable_actions( );
        if ( result )
            result->handle = object->handle->getAllowableActions( );
    }
    return result;
}


void libcmis_object_refresh( libcmis_ObjectPtr object, libcmis_ErrorPtr error )
{
    if ( object != NULL && object->handle != NULL )
    {
        try
        {
            object->handle->refresh( );
        }
        catch ( const libcmis::Exception& e )
        {
            if ( error != NULL )
            {
                error->message = strdup( e.what() );
                error->type = strdup( e.getType().c_str() );
            }
        }
    }
}


time_t libcmis_object_getRefreshTimestamp( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return object->handle->getRefreshTimestamp( );
    else
        return 0;
}


void libcmis_object_remove( libcmis_ObjectPtr object, bool allVersions, libcmis_ErrorPtr error )
{
    if ( object != NULL && object->handle != NULL )
    {
        try
        {
            object->handle->remove( allVersions );
        }
        catch ( const libcmis::Exception& e )
        {
            if ( error != NULL )
            {
                error->message = strdup( e.what() );
                error->type = strdup( e.getType().c_str() );
            }
        }
    }
}


void libcmis_object_move( libcmis_ObjectPtr object,
        libcmis_FolderPtr source,
        libcmis_FolderPtr dest,
        libcmis_ErrorPtr error )
{
    if ( object != NULL && object->handle != NULL )
    {
        try
        {
            libcmis::FolderPtr sourceHandle;
            if ( source != NULL )
                sourceHandle = source->handle;
            libcmis::FolderPtr destHandle;
            if ( dest != NULL )
                destHandle = dest->handle;

            object->handle->move( sourceHandle, destHandle );
        }
        catch ( const libcmis::Exception& e )
        {
            if ( error != NULL )
            {
                error->message = strdup( e.what() );
                error->type = strdup( e.getType().c_str() );
            }
        }
    }
}


char* libcmis_object_toString( libcmis_ObjectPtr object )
{
    if ( object != NULL && object->handle != NULL )
        return strdup( object->handle->toString( ).c_str( ) );
    else
        return NULL;
}
