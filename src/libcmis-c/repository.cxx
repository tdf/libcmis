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
#include "repository.h"

libcmis_RepositoryPtr libcmis_repository_create( xmlNodePtr node )
{
    libcmis_RepositoryPtr repository = new libcmis_repository( );

    libcmis::RepositoryPtr handle( new libcmis::Repository( node ) );
    repository->handle = handle;

    return repository;
}


void libcmis_repository_free( libcmis_RepositoryPtr repository )
{
    delete repository;
}


const char* libcmis_repository_getId( libcmis_RepositoryPtr repository )
{
    if ( repository != NULL && repository->handle != NULL )
        return repository->handle->getId( ).c_str( );
    else
        return NULL;
}


const char* libcmis_repository_getName( libcmis_RepositoryPtr repository )
{
    if ( repository != NULL && repository->handle != NULL )
        return repository->handle->getName( ).c_str( );
    else
        return NULL;
}


const char* libcmis_repository_getDescription( libcmis_RepositoryPtr repository )
{
    if ( repository != NULL && repository->handle != NULL )
        return repository->handle->getDescription( ).c_str( );
    else
        return NULL;
}


const char* libcmis_repository_getVendorName( libcmis_RepositoryPtr repository )
{
    if ( repository != NULL && repository->handle != NULL )
        return repository->handle->getVendorName( ).c_str( );
    else
        return NULL;
}


const char* libcmis_repository_getProductVersion( libcmis_RepositoryPtr repository )
{
    if ( repository != NULL && repository->handle != NULL )
        return repository->handle->getProductVersion( ).c_str( );
    else
        return NULL;
}


const char* libcmis_repository_getRootId( libcmis_RepositoryPtr repository )
{
    if ( repository != NULL && repository->handle != NULL )
        return repository->handle->getRootId( ).c_str( );
    else
        return NULL;
}


const char* libcmis_repository_getCmisVersionSupported( libcmis_RepositoryPtr repository )
{
    if ( repository != NULL && repository->handle != NULL )
        return repository->handle->getCmisVersionSupported( ).c_str( );
    else
        return NULL;
}


const char* libcmis_repository_getThinClientUri( libcmis_RepositoryPtr repository )
{
    if ( repository != NULL && repository->handle != NULL &&
            repository->handle->getThinClientUri( ).get( ) != NULL )
        return repository->handle->getThinClientUri( )->c_str( );
    else
        return NULL;
}


const char* libcmis_repository_getPrincipalAnonymous( libcmis_RepositoryPtr repository )
{
    if ( repository != NULL && repository->handle != NULL &&
            repository->handle->getPrincipalAnonymous( ).get( ) != NULL )
        return repository->handle->getPrincipalAnonymous( )->c_str( );
    else
        return NULL;
}


const char* libcmis_repository_getPrincipalAnyone( libcmis_RepositoryPtr repository )
{
    if ( repository != NULL && repository->handle != NULL &&
            repository->handle->getPrincipalAnyone( ).get( ) != NULL )
        return repository->handle->getPrincipalAnyone( )->c_str( );
    else
        return NULL;
}

