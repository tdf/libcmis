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
#include "property-type.h"

void libcmis_property_type_free( libcmis_PropertyTypePtr type )
{
    delete type;
}


const char* libcmis_property_type_getId( libcmis_PropertyTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->getId( ).c_str( );
    else
        return NULL;
}


const char* libcmis_property_type_getLocalName( libcmis_PropertyTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->getLocalName( ).c_str( );
    else
        return NULL;
}


const char* libcmis_property_type_getLocalNamespace( libcmis_PropertyTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->getLocalNamespace( ).c_str( );
    else
        return NULL;
}


const char* libcmis_property_type_getDisplayName( libcmis_PropertyTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->getDisplayName( ).c_str( );
    else
        return NULL;
}


const char* libcmis_property_type_getQueryName( libcmis_PropertyTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->getQueryName( ).c_str( );
    else
        return NULL;
}


libcmis_property_type_Type libcmis_property_type_getType( libcmis_PropertyTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return libcmis_property_type_Type( type->handle->getType( ) );
    else
        return libcmis_String;
}


const char* libcmis_property_type_getXmlType( libcmis_PropertyTypePtr type )
{
    if ( type != NULL && type->handle.get( ) != NULL )
        return type->handle->getXmlType( ).c_str( );
    else
        return NULL;
}


bool libcmis_property_type_isMultiValued( libcmis_PropertyTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isMultiValued( );
    return value;
}


bool libcmis_property_type_isUpdatable( libcmis_PropertyTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isUpdatable( );
    return value;
}


bool libcmis_property_type_isInherited( libcmis_PropertyTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isInherited( );
    return value;
}


bool libcmis_property_type_isRequired( libcmis_PropertyTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isRequired( );
    return value;
}


bool libcmis_property_type_isQueryable( libcmis_PropertyTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isQueryable( );
    return value;
}


bool libcmis_property_type_isOrderable( libcmis_PropertyTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isOrderable( );
    return value;
}


bool libcmis_property_type_isOpenChoice( libcmis_PropertyTypePtr type )
{
    bool value = false;
    if ( type != NULL && type->handle.get( ) != NULL )
        value = type->handle->isOpenChoice( );
    return value;
}
