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
#include <libxml/tree.h>

#include "allowable-actions.hxx"
#include "exception.hxx"
#include "object-type.hxx"
#include "property.hxx"
#include "xmlserializable.hxx"

namespace libcmis
{
    class Folder;

    /** Class representing any CMIS object.
      */
    class Object : public XmlSerializable
    {
        protected:
            time_t m_refreshTimestamp;

            /** Type id used as cache before we get it as a property
              */
            std::string m_typeId;

            std::map< std::string, libcmis::PropertyPtr > m_properties;
            boost::shared_ptr< libcmis::AllowableActions > m_allowableActions;

            Object( );
            void initializeFromNode( xmlNodePtr node );

        public:
            Object( xmlNodePtr node );
            Object( const Object& copy );
            virtual ~Object( ) { }

            Object& operator=( const Object& copy );

            virtual std::string getId( );
            virtual std::string getName( );

            /** Computes the paths for the objects.

                Note that folders will have only path, documents may have
                several ones and there may be cases where there is no path
                at all (unfiled objects);
              */
            virtual std::vector< std::string > getPaths( );

            virtual std::string getBaseType( );
            virtual std::string getType( );

            virtual std::string getCreatedBy( );
            virtual boost::posix_time::ptime getCreationDate( );
            virtual std::string getLastModifiedBy( );
            virtual boost::posix_time::ptime getLastModificationDate( );

            virtual std::string getChangeToken( );
            virtual bool isImmutable( );

            virtual std::map< std::string, PropertyPtr >& getProperties( );
            virtual AllowableActionsPtr getAllowableActions( ) { return m_allowableActions; }

            virtual void updateProperties( ) throw ( Exception ) = 0;

            virtual ObjectTypePtr getTypeDescription( ) = 0;

            /** Reload the data from the server.
              */
            virtual void refresh( ) throw ( Exception ) = 0;
            virtual time_t getRefreshTimestamp( ) { return m_refreshTimestamp; }

            virtual void remove( bool allVersions = true ) throw ( Exception ) = 0;

            virtual void move( boost::shared_ptr< Folder > source, boost::shared_ptr< Folder > destination ) throw ( Exception ) = 0;

            /** Dump the object as a string for debugging or display purpose.
              */
            virtual std::string toString( );

            void toXml( xmlTextWriterPtr writer );
    };

    typedef ::boost::shared_ptr< Object > ObjectPtr;
}

#endif
