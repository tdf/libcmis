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

#include <boost/date_time.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <vector>

namespace libcmis
{
    class Property
    {
        public:

            enum Type
            {
                String,
                Integer,
                Decimal,
                Bool,
                DateTime
            };

        private:
            std::string m_id;
            std::string m_localName;
            std::string m_displayName;
            std::string m_queryName;
            Type m_type;
            std::vector< std::string > m_strValues;


        public:

            /** Property constructor allowing to use different values for the id and names.
              */
            Property( std::string id, std::string localName,
                      std::string displayName, std::string queryName,
                      std::vector< std::string > strValues, Type type );

            virtual ~Property( ){ }

            std::string getId( ) { return m_id; }
            std::string getLocalName( ) { return m_localName; }
            std::string getDisplayName( ) { return m_displayName; }
            std::string getQueryName( ) { return m_queryName; }
            
            void setId( std::string id ) { m_id = id; }
            void setLocalName( std::string localName ) { m_localName = localName; }
            void setDisplayName( std::string displayName ) { m_displayName = displayName; }
            void setQueryName( std::string queryName ) { m_queryName = queryName; }

            Type getType( ) { return m_type; }

            virtual std::vector< boost::posix_time::ptime > getDateTimes( ) = 0;
            virtual std::vector< bool > getBools( ) = 0;
            virtual std::vector< std::string > getStrings( ) { return m_strValues; }
            virtual std::vector< long > getLongs( ) = 0;
            virtual std::vector< double > getDoubles( ) = 0;
    };
    typedef ::boost::shared_ptr< Property > PropertyPtr;

    class IntegerProperty : public Property
    {
        private:
            std::vector< long > m_values;

        public:
            IntegerProperty( std::string id, std::string localName,
                    std::string displayName, std::string queryName,
                    std::vector< std::string > values );
            
            virtual std::vector< boost::posix_time::ptime > getDateTimes( );
            virtual std::vector< bool > getBools( );
            virtual std::vector< long > getLongs( ) { return m_values; }
            virtual std::vector< double > getDoubles( );
    };
    
    class DecimalProperty : public Property
    {
        private:
            std::vector< double > m_values;

        public:
            DecimalProperty( std::string id, std::string localName,
                    std::string displayName, std::string queryName,
                    std::vector< std::string > values );
            
            virtual std::vector< boost::posix_time::ptime > getDateTimes( );
            virtual std::vector< bool > getBools( );
            virtual std::vector< long > getLongs( );
            virtual std::vector< double > getDoubles( ) { return m_values; }
    };
    
    class BoolProperty : public Property
    {
        private:
            std::vector< bool > m_values;

        public:
            BoolProperty( std::string id, std::string localName,
                    std::string displayName, std::string queryName,
                    std::vector< std::string > values );
            
            virtual std::vector< boost::posix_time::ptime > getDateTimes( );
            virtual std::vector< bool > getBools( ) { return m_values; }
            virtual std::vector< long > getLongs( );
            virtual std::vector< double > getDoubles( );
    };
    
    class DateTimeProperty : public Property
    {
        private:
            std::vector< boost::posix_time::ptime > m_values;

        public:
            DateTimeProperty( std::string id, std::string localName,
                    std::string displayName, std::string queryName,
                    std::vector< std::string > values );
            
            virtual std::vector< boost::posix_time::ptime > getDateTimes( ) { return m_values; }
            virtual std::vector< bool > getBools( );
            virtual std::vector< long > getLongs( );
            virtual std::vector< double > getDoubles( );
    };
    
    class StringProperty : public Property
    {
        public:
            StringProperty( std::string id, std::string localName,
                    std::string displayName, std::string queryName,
                    std::vector< std::string > values );
            
            virtual std::vector< boost::posix_time::ptime > getDateTimes( );
            virtual std::vector< bool > getBools( );
            virtual std::vector< long > getLongs( );
            virtual std::vector< double > getDoubles( );
    };

    class IdProperty : public StringProperty
    {
        public:
            IdProperty( std::string id, std::string localName,
                    std::string displayName, std::string queryName,
                    std::vector< std::string > values );
    };
    
    class HtmlProperty : public StringProperty
    {
        public:
            HtmlProperty( std::string id, std::string localName,
                    std::string displayName, std::string queryName,
                    std::vector< std::string > values );
    };
    
    class UriProperty : public StringProperty
    {
        public:
            UriProperty( std::string id, std::string localName,
                    std::string displayName, std::string queryName,
                    std::vector< std::string > values );
    };
    
    PropertyPtr parseProperty( xmlNodePtr node );
}

#endif
