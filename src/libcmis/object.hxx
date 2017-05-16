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
#include <vector>

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
#include "rendition.hxx"

namespace libcmis
{
    class Folder;
    class Session;

    /** Class representing any CMIS object.
      */
    class Object : public XmlSerializable
    {
        protected:
            Session* m_session;

            ObjectTypePtr m_typeDescription;
            time_t m_refreshTimestamp;

            /** Type id used as cache before we get it as a property
              */
            std::string m_typeId;

            std::map< std::string, PropertyPtr > m_properties;
            boost::shared_ptr< AllowableActions > m_allowableActions;
            std::vector< RenditionPtr > m_renditions;
            void initializeFromNode( xmlNodePtr node );

        public:

            Object( Session* session );
            Object( Session* session, xmlNodePtr node );
            Object( const Object& copy );
            virtual ~Object( ) { }

            Object& operator=( const Object& copy );

            virtual std::string getId( );
            virtual std::string getName( );
            virtual std::string getStringProperty( const std::string& propertyName );

            /** Computes the paths for the objects.

                Note that folders will have only path, documents may have
                several ones and there may be cases where there is no path
                at all (unfilled objects);
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

            virtual std::vector< std::string > getSecondaryTypes();

            /** Convenience function adding a secondary type to the object.

                Behind the scene this function is basically computing the
                properties and sets them for you to avoid reading the CMIS
                1.1 specification, section 2.1.9.

                \param id
                    the identifier of the secondary type to add
                \param properties
                    the properties coming with the secondary type

                \return
                    the updated object. Note that it may represent the same
                    object on the server but it still is a different object
                    instance (see updateProperties method).

                \throw Exception
                    if anything wrong happens. Note that the server is likely
                    to throw a constraint exception if it doesn't allow the
                    operation.
              */
            virtual boost::shared_ptr< Object > addSecondaryType(
                                                        std::string id,
                                                        PropertyPtrMap properties )
                throw ( Exception );

            /** Convenience function removing a secondary type from the object.

                Behind the scene this function is basically computing the
                correct property and sets it for you to avoid reading the
                CMIS 1.1 specification, section 2.1.9.

                The server should remove the related properties, there is
                normally no need to worry about them.

                \param id
                    the identifier of the secondary type to remove

                \return
                    the updated object. Note that it may represent the same
                    object on the server but it still is a different object
                    instance (see updateProperties method).

                \throw Exception
                    if anything wrong happens. Note that the server is likely
                    to throw a constraint exception if it doesn't allow the
                    operation.
              */
            virtual boost::shared_ptr< Object > removeSecondaryType( std::string id )
                throw ( Exception );

            /** Gives access to the properties of the object.

                \attention
                    API users should consider this method as read-only as the
                    changed properties won't be updated to the server. Updating
                    the returned map may lead to changes loss when calling
                    updateProperties.

                \sa updateProperties to change properties on the server
              */
            virtual libcmis::PropertyPtrMap& getProperties( );


            /** Get the renditions of the object.

                \param filter is defined by the CMIS spec section 2.2.1.2.4.1.
                              By default, this value is just ignored, but some bindings and servers
                              may use it.

                \attention
                    The streamId of the rendition is used in getContentStream( )
              */
            virtual std::vector< RenditionPtr> getRenditions( std::string filter = std::string( ) )
                throw ( Exception );
            virtual AllowableActionsPtr getAllowableActions( ) { return m_allowableActions; }

            /** Update the object properties and return the updated object.

                \attention
                    even if the returned object may have the same Id than 'this'
                    and thus representing the same object on the server, those
                    are still two different instances to ease memory handling.
              */
            virtual boost::shared_ptr< Object > updateProperties(
                        const PropertyPtrMap& properties ) throw ( Exception ) = 0;

            virtual ObjectTypePtr getTypeDescription( );

            /** Reload the data from the server.
              */
            virtual void refresh( ) throw ( Exception ) = 0;
            virtual time_t getRefreshTimestamp( ) { return m_refreshTimestamp; }

            virtual void remove( bool allVersions = true ) throw ( Exception ) = 0;

            virtual void move( boost::shared_ptr< Folder > source, boost::shared_ptr< Folder > destination ) throw ( Exception ) = 0;


            virtual std::string getThumbnailUrl( ) throw ( Exception );

            /** Dump the object as a string for debugging or display purpose.
              */
            virtual std::string toString( );

            void toXml( xmlTextWriterPtr writer );
    };

    typedef boost::shared_ptr< Object > ObjectPtr;
}

#endif
