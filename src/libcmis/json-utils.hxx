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

#ifndef _JSON_UTILS_HXX_
#define _JSON_UTILS_HXX_

#include <string>
#include <map>
#include <vector>

class Json
{
    public :
        typedef std::map< std::string, Json > JsonObject ;
        typedef std::vector< Json > JsonVector ;
        template <typename T> explicit Json( const T& val ) ;

        Json() ;
        Json( const Json& copy ) ;
        Json( const char *str ) ;
        ~Json( ) ;

        Json operator[]( std::string key ) const ;
        Json operator[]( const std::size_t& index ) const ;
        Json& operator=( const Json& rhs ) ;
        friend std::ostream& operator<<( std::ostream& os, const Json& json ) ;

        void swap( Json& other ) ;

        void add( const Json& json);
        
        void add( const std::string& key, const Json& json);

        static Json parse( std::string str ) ;

        std::string toString( );

        enum Type { json_null, json_bool, json_double, json_int, json_object, json_array, json_string, json_datetime } ;

        Type getDataType( ) const ;

        int getLength( ) const;

        JsonObject getObjects();
        JsonVector getList();

    private :
        Json( struct json_object *json ) ;
        struct json_object  *m_json ;
} ;

#endif /* _JSON_UTILS_HXX_ */

