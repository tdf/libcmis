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
 * Copyright (C) 2013 SUSE <cbosdonnat@suse.com>
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

#include "gdrive-object.hxx"
#include "gdrive-property.hxx"

using std::string;

GDriveObject::GDriveObject( GDriveSession* session ) :
    libcmis::Object( session )
{
}

GDriveObject::GDriveObject( GDriveSession* session, Json json ) :
    libcmis::Object( session )
{
   initializeFromJson( json ); 
}

GDriveObject::GDriveObject( const GDriveObject& copy ) :
    libcmis::Object( copy )
{
}

GDriveObject& GDriveObject::operator=( const GDriveObject& copy )
{
    if ( this != &copy )
    {
        libcmis::Object::operator=( copy );
    }
    return *this;
}

void GDriveObject::initializeFromJson ( Json json )
{
    Json::JsonObject objs = json.getObjects( );
    Json::JsonObject::iterator it;
    for ( it = objs.begin( ); it != objs.end( ); it++)
    {
        PropertyPtr property(new GDriveProperty( it->first,it->second) );
        if ( property != NULL )
            m_properties[ property->getPropertyType( )->getId()] = property;
    }
}


string GDriveObject::getExportLinks ( )
{
    return getStringProperty( "exportLinks");
}

boost::shared_ptr< libcmis::Object > GDriveObject::updateProperties(
        const map< string, libcmis::PropertyPtr >& /*properties*/ ) throw ( libcmis::Exception )
{
    boost::shared_ptr< libcmis::Object > updatedObject;

    // TODO Implement me

    return updatedObject;
}

void GDriveObject::refresh( ) throw ( libcmis::Exception )
{
    // TODO Implement me
}

void GDriveObject::remove( bool /*allVersions*/ ) throw ( libcmis::Exception )
{
    // TODO Implement me
}

void GDriveObject::move( boost::shared_ptr< libcmis::Folder > /*source*/,
                         boost::shared_ptr< libcmis::Folder > /*destination*/ ) throw ( libcmis::Exception )
{
    // TODO Implement me
}
