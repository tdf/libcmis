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
#ifndef _ONEDRIVE_UTILS_HXX_
#define _ONEDRIVE_UTILS_HXX_

#include <string>

#include "property.hxx"
#include "json-utils.hxx"

class OneDriveUtils
{
    public :
               
        // Convert a OneDrive Property key to a CMIS key
        static std::string toCmisKey( const std::string& key);

        // Convert a CMIS key to OneDrive key
        static std::string toOneDriveKey( const std::string& key );

        // Check if a property has multiple values
        static bool checkMultiValued( const std::string& key);

        // Parse a Gdrive property value to CMIS values
        static std::vector< std::string > parseOneDriveProperty( std::string key, Json jsonValue );
};

#endif
