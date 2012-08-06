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
#ifndef _LIBCMIS_INTERNALS_H_
#define _LIBCMIS_INTERNALS_H_

#include <libcmis/allowable-actions.hxx>
#include <libcmis/exception.hxx>
#include <libcmis/repository.hxx>
#include <libcmis/session.hxx>

struct libcmis_error
{
    libcmis::Exception* handle;

    libcmis_error( ) : handle( NULL ) { }
};

struct libcmis_session
{
    libcmis::Session* handle;
    libcmis::AuthProviderPtr provider;

    // Constructors

    libcmis_session( ) :
        handle( NULL ),
        provider( )
    {
    }

    libcmis_session( const libcmis_session& copy ) :
        handle( copy.handle ),
        provider( copy.provider )
    {
    }

    libcmis_session& operator=( const libcmis_session& copy )
    {
        if ( this != &copy )
        {
            handle = copy.handle;
            provider = copy.provider;
        }
        return *this;
    }
};

struct libcmis_repository
{
    libcmis::RepositoryPtr handle;

    libcmis_repository( ) : handle( ) { }
};

struct libcmis_object
{
    libcmis::ObjectPtr handle;

    libcmis_object( ) : handle( ) { }
};

struct libcmis_object_type
{
    libcmis::ObjectTypePtr handle;

    libcmis_object_type( ) : handle( ) { }
};

struct libcmis_allowable_actions
{
    libcmis::AllowableActionsPtr handle;

    libcmis_allowable_actions( ) : handle ( ) { }
};

struct libcmis_property_type
{
    libcmis::PropertyTypePtr handle;

    libcmis_property_type( ) : handle( ) { }
};

#endif
