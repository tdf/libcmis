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

#ifndef _ONEDRIVE_DOCUMENT_HXX_
#define _ONEDRIVE_DOCUMENT_HXX_

#include <libcmis/document.hxx>
#include <libcmis/folder.hxx>
#include <libcmis/rendition.hxx>

#include "onedrive-object.hxx"
#include "json-utils.hxx"

class OneDriveDocument : public libcmis::Document, public OneDriveObject
{
    public:
        OneDriveDocument( OneDriveSession* session );

        OneDriveDocument( OneDriveSession* session, Json json, 
                          std::string id = std::string( ),
                          std::string name = std::string( ) );
        ~OneDriveDocument( );
        
        std::string getType( ) { return std::string( "cmis:document" );}
        std::string getBaseType( ) { return std::string( "cmis:document" );} 

        virtual std::vector< libcmis::FolderPtr > getParents( );

        virtual boost::shared_ptr< std::istream > getContentStream( std::string streamId = std::string( ) );

        virtual void setContentStream( boost::shared_ptr< std::ostream > os, 
                                       std::string contentType,
                                       std::string fileName, 
                                       bool overwrite = true );

        virtual libcmis::DocumentPtr checkOut( );
        virtual void cancelCheckout( );

        virtual libcmis::DocumentPtr checkIn( bool isMajor, 
                                              std::string comment,
                                              const std::map< std::string,libcmis::PropertyPtr >& 
                                                  properties,
                                              boost::shared_ptr< std::ostream > stream,
                                              std::string contentType, 
                                              std::string fileName );
        
        virtual std::vector< libcmis::DocumentPtr > getAllVersions( );
};

#endif
