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
 * Copyright (C) 2014 Mihai Varga <mihai.mv13@gmail.com>
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
#ifndef _SHAREPOINT_OBJECT_HXX_
#define _SHAREPOINT_OBJECT_HXX_

#include <libcmis/object.hxx>

#include "sharepoint-session.hxx"
#include "json-utils.hxx"

// Class representing an object for SharePoint protocol.
class SharePointObject : public virtual libcmis::Object
{
    public:
        SharePointObject( SharePointSession* session );

        // Create a SharePoint document from Json properties.
        SharePointObject( SharePointSession* session, Json json, 
               std::string parentId = std::string( ),
               std::string name = std::string( ) );
        SharePointObject( const SharePointObject& copy );
        virtual ~SharePointObject( ) { }

        SharePointObject& operator=( const SharePointObject& copy );

        void initializeFromJson( Json json, std::string parentId = std::string( ),
                                            std::string name = std::string( ) );

        void refreshImpl( Json json );
        virtual void refresh( );
        virtual void remove( bool allVersions = true );

        std::vector< std::string > getMultiStringProperty(
                const std::string& propertyName );

       virtual boost::shared_ptr< Object > updateProperties(
            const libcmis::PropertyPtrMap& properties );

        virtual std::vector< libcmis::RenditionPtr> getRenditions( std::string /*filter = std::string( )*/ )
            {return std::vector< libcmis::RenditionPtr>( );}

        virtual void move( boost::shared_ptr< libcmis::Folder > source,
                           boost::shared_ptr< libcmis::Folder > destination );

    protected:
        SharePointSession* getSession( );    

};
#endif
