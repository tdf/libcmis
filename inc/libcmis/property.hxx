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
#ifndef _PROPERTY_HXX_
#define _PROPERTY_HXX_

#include <libxml/tree.h>
#include <libxml/xmlwriter.h>

#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

#include "libcmis/libcmis-api.h"
#include "libcmis/property-type.hxx"
#include "libcmis/xmlserializable.hxx"

namespace libcmis
{
    class ObjectType;

    class LIBCMIS_API Property : public XmlSerializable
    {
        private:
            PropertyTypePtr m_propertyType;
            std::vector< std::string > m_strValues;
            std::vector< bool > m_boolValues;
            std::vector< long > m_longValues;
            std::vector< double > m_doubleValues;
            std::vector< boost::posix_time::ptime > m_dateTimeValues;

        protected:
            Property( );

        public:
            /** Property constructor allowing to use different values for the id and names.
              */
            Property( PropertyTypePtr propertyType, std::vector< std::string > strValues );

            ~Property( ){ }

            PropertyTypePtr getPropertyType( ) { return m_propertyType; }

            std::vector< boost::posix_time::ptime > getDateTimes( ) { return m_dateTimeValues; }
            std::vector< bool > getBools( ) { return m_boolValues; }
            std::vector< std::string > getStrings( ) { return m_strValues; }
            std::vector< long > getLongs( ) { return m_longValues; }
            std::vector< double > getDoubles( ) { return m_doubleValues; }

            void setPropertyType( PropertyTypePtr propertyType);
            void setValues( std::vector< std::string > strValues );

            void toXml( xmlTextWriterPtr writer );

            std::string toString( );
    };
    typedef boost::shared_ptr< Property > PropertyPtr;
    typedef std::map< std::string, libcmis::PropertyPtr > PropertyPtrMap;

    PropertyPtr parseProperty( xmlNodePtr node, boost::shared_ptr< ObjectType > objectType );
}

#endif
