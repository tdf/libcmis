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
#ifndef _OBJECT_HXX_
#define _OBJECT_HXX_

#include <ctime>
#include <map>
#include <string>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>

#include "allowable-actions.hxx"
#include "exception.hxx"
#include "object-type.hxx"
#include "property.hxx"
#include "xmlserializable.hxx"

namespace libcmis
{
    /** Class representing any CMIS object.
      */
    class Object : public XmlSerializable
    {
        public:
            virtual ~Object( ) { }

            virtual std::string getId( ) = 0;
            virtual std::string getName( ) = 0;

            /** Computes the paths for the objects.

                Note that folders will have only path, documents may have
                several ones and there may be cases where there is no path
                at all (unfiled objects);
              */
            virtual std::vector< std::string > getPaths( ) = 0;

            virtual std::string getBaseType( ) = 0;
            virtual std::string getType( ) = 0;

            virtual std::string getCreatedBy( ) = 0;
            virtual boost::posix_time::ptime getCreationDate( ) = 0;
            virtual std::string getLastModifiedBy( ) = 0;
            virtual boost::posix_time::ptime getLastModificationDate( ) = 0;

            virtual std::string getChangeToken( ) = 0;
            virtual bool isImmutable( ) = 0;

            virtual std::map< std::string, PropertyPtr >& getProperties( ) = 0;
            virtual void updateProperties( ) throw ( Exception ) = 0;

            virtual ObjectTypePtr getTypeDescription( ) = 0;
            virtual AllowableActionsPtr getAllowableActions( ) = 0;

            /** Reload the data from the server.
              */
            virtual void refresh( ) throw ( Exception ) = 0;
            virtual time_t getRefreshTimestamp( ) = 0;

            virtual void remove( bool allVersions = true ) throw ( Exception ) = 0;

            /** Dump the object as a string for debugging or display purpose.
              */
            virtual std::string toString( ) = 0;
    };

    typedef ::boost::shared_ptr< Object > ObjectPtr;
}

#endif
