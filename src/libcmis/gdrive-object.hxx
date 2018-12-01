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
#ifndef _GDRIVE_OBJECT_HXX_
#define _GDRIVE_OBJECT_HXX_

#include "gdrive-session.hxx"
#include "json-utils.hxx"
#include "object.hxx"

/** Class representing an object for Google Drive protocol.

    This class overrides quite a number of its parent class methods to
    git the Google Drive API into the libcmis API.

    In order to reuse more of the base Object class, this class needs
    to map the main properties (like id, name, etc) to CMIS property
    ids (cmis:id, cmis:name, etc).
  */
class GDriveObject : public virtual libcmis::Object
{
    public:
        GDriveObject( GDriveSession* session );

        // Create a GDrive document from Json properties.
        // In case it's a revision, keep the ID and the name of the original file.
        GDriveObject( GDriveSession* session, Json json, 
               std::string id = std::string( ),
               std::string name = std::string( ) );
        GDriveObject( const GDriveObject& copy );
        virtual ~GDriveObject( ) { }

        GDriveObject& operator=( const GDriveObject& copy );

        void initializeFromJson( Json json, std::string id = std::string( ),
                                            std::string name = std::string( ) );
        void refreshImpl( Json json );
        Json createJsonFromParentId( const std::string& parentId );
    
        std::string getUrl( );
        std::string getUploadUrl( );
        std::vector< std::string > getMultiStringProperty(
                const std::string& propertyName );

        virtual std::vector< libcmis::RenditionPtr> getRenditions( std::string filter = std::string( ) );

        virtual boost::shared_ptr< Object > updateProperties(
            const libcmis::PropertyPtrMap& properties );

        virtual void refresh( );

        virtual void remove( bool allVersions = true );

        virtual void move( boost::shared_ptr< libcmis::Folder > source,
                           boost::shared_ptr< libcmis::Folder > destination );
    
    protected:
        GDriveSession* getSession( );    

};

#endif
