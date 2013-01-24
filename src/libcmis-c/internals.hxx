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

#include <vector>

#include <libcmis/allowable-actions.hxx>
#include <libcmis/document.hxx>
#include <libcmis/exception.hxx>
#include <libcmis/folder.hxx>
#include <libcmis/object.hxx>
#include <libcmis/property.hxx>
#include <libcmis/repository.hxx>
#include <libcmis/session.hxx>
#include <libcmis/session-factory.hxx>

struct libcmis_error
{
    std::exception* handle;
    char* cached_type;

    libcmis_error( ) : handle( NULL ), cached_type( NULL ) { }
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
    virtual ~libcmis_object( ) { }
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

struct libcmis_property
{
    libcmis::PropertyPtr handle;

    libcmis_property( ) : handle( ) { }
};

struct libcmis_folder : public libcmis_object
{
    libcmis::FolderPtr handle;

    libcmis_folder( ) : libcmis_object( ), handle( ) { }
    void setHandle( libcmis::FolderPtr folder )
    {
        handle = folder;
        libcmis_object::handle = folder;
    }
};

struct libcmis_document : public libcmis_object
{
    libcmis::DocumentPtr handle;

    libcmis_document( ) : libcmis_object( ), handle( ) { }
    void setHandle( libcmis::DocumentPtr document )
    {
        handle = document;
        libcmis_object::handle = document;
    }
};

struct libcmis_vector_bool
{
    std::vector< bool > handle;

    libcmis_vector_bool( ) : handle( ) { }
};

struct libcmis_vector_string
{
    std::vector< std::string > handle;

    libcmis_vector_string( ) : handle( ) { }
};

struct libcmis_vector_long
{
    std::vector< long > handle;

    libcmis_vector_long( ) : handle( ) { }
};

struct libcmis_vector_double
{
    std::vector< double > handle;

    libcmis_vector_double( ) : handle( ) { }
};

struct libcmis_vector_time
{
    std::vector< boost::posix_time::ptime > handle;

    libcmis_vector_time( ) : handle( ) { }
};

struct libcmis_vector_object_type
{
    std::vector< libcmis::ObjectTypePtr > handle;

    libcmis_vector_object_type( ) : handle( ) { }
};

struct libcmis_vector_property_type
{
    std::vector< libcmis::PropertyTypePtr > handle;

    libcmis_vector_property_type( ) : handle( ) { }
};

struct libcmis_vector_property
{
    std::vector< libcmis::PropertyPtr > handle;

    libcmis_vector_property( ) : handle( ) { }
};

struct libcmis_vector_object
{
    std::vector< libcmis::ObjectPtr > handle;

    libcmis_vector_object( ) : handle( ) { }
};

struct libcmis_vector_folder
{
    std::vector< libcmis::FolderPtr > handle;

    libcmis_vector_folder( ) : handle( ) { }
};

struct libcmis_vector_document
{
    std::vector< libcmis::DocumentPtr > handle;

    libcmis_vector_document( ) : handle( ) { }
};

#endif
