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

#ifndef _GDRIVE_PROPERTY_HXX_
#define _GDRIVE_PROPERTY_HXX_

#include "json-utils.hxx"
#include "property.hxx"

using namespace libcmis;

class GDriveProperty : public virtual libcmis::Property
{
    public :
        // Create a GDrive Property from a Json property with its key
        GDriveProperty( const std::string& key, Json json);
        ~GDriveProperty( );    
        GDriveProperty( const GDriveProperty& copy);
        GDriveProperty& operator=( const GDriveProperty& copy );

        // Convert a GDrive Property key to a CMIS key
        std::string convertToCmisKey( const std::string& key );

        // Convert a CMIS key to GDrive key
        std::string convertToGDriveKey( const std::string& key );

        virtual Json toJson( );
    private :
        // Avoid calling default constructor
        GDriveProperty( );
};
#endif /* _GDRIVE_PROPERTY_HXX_ */
