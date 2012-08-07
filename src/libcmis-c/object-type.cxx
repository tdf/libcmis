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

#include "internals.hxx"
#include "object-type.h"

using namespace std;

void libcmis_object_type_free( libcmis_ObjectTypePtr type )
{
    delete type;
}


void libcmis_object_type_list_free( libcmis_ObjectTypePtr* list )
{
    int size = sizeof( list ) / sizeof( *list );
    for ( int i = 0; i < size; ++i )
    {
        delete list[i];
    }
    delete[ ] list;
}


const char* libcmis_object_type_getId( libcmis_ObjectTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->getId( ).c_str( );
    else
        return NULL;
}


const char* libcmis_object_type_getLocalName( libcmis_ObjectTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->getLocalName( ).c_str( );
    else
        return NULL;
}


const char* libcmis_object_type_getLocalNamespace( libcmis_ObjectTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->getLocalNamespace( ).c_str( );
    else
        return NULL;
}


const char* libcmis_object_type_getQueryName( libcmis_ObjectTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->getQueryName( ).c_str( );
    else
        return NULL;
}


const char* libcmis_object_type_getDisplayName( libcmis_ObjectTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->getDisplayName( ).c_str( );
    else
        return NULL;
}


const char* libcmis_object_type_getDescription( libcmis_ObjectTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->getDescription( ).c_str( );
    else
        return NULL;
}


libcmis_ObjectTypePtr libcmis_object_type_getParentType(
        libcmis_ObjectTypePtr type,
        libcmis_ErrorPtr error )
{
    libcmis_ObjectTypePtr result = NULL;
    if ( type != NULL && type->handle.get( ) != NULL )
    {
        try
        {
            libcmis::ObjectTypePtr handle = type->handle->getParentType( );
            result = new libcmis_object_type( );
            result->handle = handle;
        }
        catch( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }

    return result;
}


libcmis_ObjectTypePtr libcmis_object_type_getBaseType(
        libcmis_ObjectTypePtr type,
        libcmis_ErrorPtr error )
{
    libcmis_ObjectTypePtr result = NULL;
    if ( type != NULL && type->handle.get( ) != NULL )
    {
        try
        {
            libcmis::ObjectTypePtr handle = type->handle->getBaseType( );
            result = new libcmis_object_type( );
            result->handle = handle;
        }
        catch( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }

    return result;
}


libcmis_ObjectTypePtr* libcmis_object_type_getChildren(
        libcmis_ObjectTypePtr type, libcmis_ErrorPtr error )
{
    libcmis_ObjectTypePtr* children = NULL;
    if ( type != NULL && type->handle.get( ) != NULL )
    {
        try
        {
            std::vector< libcmis::ObjectTypePtr > types = type->handle->getChildren( );
            children = new libcmis_ObjectTypePtr[ types.size( ) ];
            for ( size_t i = 0; i < types.size( ); ++i )
            {
                libcmis_ObjectTypePtr child = new libcmis_object_type( );
                child->handle = types[i];
                children[i] = child;
            }
        }
        catch( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }

    return children;
}


bool libcmis_object_type_isCreatable( libcmis_ObjectTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isCreatable( );
    return value;
}


bool libcmis_object_type_isFileable( libcmis_ObjectTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isFileable( );
    return value;
}


bool libcmis_object_type_isQueryable( libcmis_ObjectTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isQueryable( );
    return value;
}


bool libcmis_object_type_isFulltextIndexed( libcmis_ObjectTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isFulltextIndexed( );
    return value;
}


bool libcmis_object_type_isIncludedInSupertypeQuery( libcmis_ObjectTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isIncludedInSupertypeQuery( );
    return value;
}


bool libcmis_object_type_isControllablePolicy( libcmis_ObjectTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isControllablePolicy( );
    return value;
}


bool libcmis_object_type_isControllableACL( libcmis_ObjectTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isControllableACL( );
    return value;
}


bool libcmis_object_type_isVersionable( libcmis_ObjectTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isVersionable( );
    return value;
}


libcmis_object_type_ContentStreamAllowed libcmis_object_type_getContentStreamAllowed( libcmis_ObjectTypePtr type )
{
    libcmis_object_type_ContentStreamAllowed result = libcmis_NotAllowed;
    if ( type != NULL && type->handle.get( ) != NULL )
    {
        libcmis::ObjectType::ContentStreamAllowed value = type->handle->getContentStreamAllowed( );
        result = libcmis_object_type_ContentStreamAllowed( value );
    }
    return result;
}


libcmis_PropertyTypePtr* libcmis_object_type_getPropertiesTypes( libcmis_ObjectTypePtr type )
{
    libcmis_PropertyTypePtr* propertyTypes = NULL;
    if ( type != NULL && type->handle != NULL )
    {
        map< string, libcmis::PropertyTypePtr >& handles = type->handle->getPropertiesTypes( );
        propertyTypes = new libcmis_PropertyTypePtr[ handles.size( ) ];
        int i = 0;
        for ( map< string, libcmis::PropertyTypePtr >::iterator it = handles.begin( );
                it != handles.end( ); ++it, ++i )
        {
            libcmis_PropertyTypePtr propertyType = new libcmis_property_type( );
            propertyType->handle = it->second;
            propertyTypes[i] = propertyType;
        }
    }

    return propertyTypes;
}

libcmis_PropertyTypePtr libcmis_object_type_getPropertyType( libcmis_ObjectTypePtr type, const char* id )
{
    libcmis_PropertyTypePtr propertyType = NULL;
    if ( type != NULL && type->handle != NULL )
    {
        map< string, libcmis::PropertyTypePtr >& handles = type->handle->getPropertiesTypes( );
        map< string, libcmis::PropertyTypePtr >::iterator it = handles.find( string( id ) );
        if ( it != handles.end( ) )
        {
            propertyType = new libcmis_property_type( );
            propertyType->handle = it->second;
        }
    }

    return propertyType;
}


const char* libcmis_object_type_toString( libcmis_ObjectTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->toString( ).c_str( );
    else
        return NULL;
}


