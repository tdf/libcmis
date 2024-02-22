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
#ifndef _FOLDER_HXX_
#define _FOLDER_HXX_

#include <map>
#include <string>
#include <vector>

#include "libcmis/exception.hxx"
#include "libcmis/libcmis-api.h"
#include "libcmis/object.hxx"

namespace libcmis
{
    class Document;
    class Session;
    
    struct LIBCMIS_API UnfileObjects {
        enum Type
        {
            Unfile,
            DeleteSingleFiled,
            Delete
        };
    };

    /** Class representing a CMIS folder.
      */
    class LIBCMIS_API Folder : public virtual Object
    {
        public:
            virtual ~Folder() { }

            virtual std::vector< std::string > getPaths( );

            virtual boost::shared_ptr< Folder > getFolderParent( );
            virtual std::vector< ObjectPtr > getChildren( ) = 0;
            virtual std::string getParentId( );
            virtual std::string getPath( );

            virtual bool isRootFolder( );

            virtual boost::shared_ptr< Folder > createFolder( const std::map< std::string, PropertyPtr >& properties )
                = 0;
            virtual boost::shared_ptr< Document > createDocument( const std::map< std::string, PropertyPtr >& properties,
                                    boost::shared_ptr< std::ostream > os, std::string contentType, std::string fileName ) = 0;

            virtual std::vector< std::string > removeTree( bool allVersion = true, UnfileObjects::Type unfile = UnfileObjects::Delete,
                                    bool continueOnError = false ) = 0;
        
            virtual std::string toString( );
    };
    typedef boost::shared_ptr< Folder > FolderPtr;

}

#endif
