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

#ifndef _SHAREPOINT_PROPERTY_HXX_
#define _SHAREPOINT_PROPERTY_HXX_

#include <libcmis/property.hxx>

#include "json-utils.hxx"

// reference: http://msdn.microsoft.com/en-us/library/hh243648.aspx
class SharePointProperty : public libcmis::Property
{
    public :
        // Create a SharePoint Property from a Json property with its key
        SharePointProperty( const std::string& key, Json json);
        ~SharePointProperty( );    
        SharePointProperty( const SharePointProperty& copy);
        SharePointProperty& operator=( const SharePointProperty& copy );

    private :
        // Avoid calling default constructor
        SharePointProperty( );
};
#endif
