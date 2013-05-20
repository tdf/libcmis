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
 * Copyright (C) 2013 Cao Cuong Ngo <cao.cuong.ngo@gmail.com>
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

#include <string>

#include "property.hxx"

#ifndef _GDRIVE_UTILS_HXX_
#define _GDRIVE_UTILS_HXX_

static const std::string GDRIVE_FOLDER_MIME_TYPE = "application/vnd.google-apps.folder" ;
static const std::string GDRIVE_UPLOAD_LINKS = "https://www.googleapis.com/upload/drive/v2/files/";

class GdriveUtils
{
    public :
               
        // Convert a GDrive Property key to a CMIS key
        static std::string toCmisKey( const std::string& key);

        // Convert a CMIS key to GDrive key
        static std::string toGdriveKey( const std::string& key );

        // Convert CMIS properties to GDrive properties
        static Json toGdriveJson( const libcmis::PropertyPtrMap& properties );

        // Check if a property is updatable
        static bool checkUpdatable( const std::string& key);
        
        /*
        * Parse the authorization code from the response page
        * in the input tag, with id = code
        */
        static std::string parseCode ( const char* response );

        /*
         * Parse input values and redirect link from the response page
         */
        static int parseResponse ( const char* response, 
                                   std::string& post, 
                                   std::string& link );

        // Create a Json array from a ParentId
        static Json createJsonFromParentId( const std::string& parentId );
};

#endif
