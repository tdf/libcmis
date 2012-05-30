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

#include <errno.h>
#include <stdlib.h>

#include "xml-utils.hxx"

using namespace std;

namespace libcmis
{
    string getXmlNodeAttributeValue( xmlNodePtr node, const char* attributeName ) throw ( Exception )
    {
        xmlChar* xmlStr = xmlGetProp( node, BAD_CAST( attributeName ) );
        if ( xmlStr == NULL )
            throw Exception( "Missing attribute" );
        string value( ( char * ) xmlStr );
        xmlFree( xmlStr );
        return value;
    }

    boost::posix_time::ptime parseDateTime( string dateTimeStr )
    {
        // Get the time zone offset
        boost::posix_time::time_duration tzOffset( boost::posix_time::duration_from_string( "+00:00" ) );

        size_t teePos = dateTimeStr.find( 'T' );
        string noTzStr = dateTimeStr.substr( 0, teePos + 1 );

        string timeStr = dateTimeStr.substr( teePos + 1 );

        // Get the TZ if any
        if ( timeStr[ timeStr.size() - 1] == 'Z' )
        {
            noTzStr += timeStr.substr( 0, timeStr.size() - 1 );
        }
        else
        {
            size_t tzPos = timeStr.find( '+' );
            if ( tzPos == string::npos )
                tzPos = timeStr.find( '-' );

            if ( tzPos != string::npos )
            {
                noTzStr += timeStr.substr( 0, tzPos );
                
                // Check the validity of the TZ value
                string tzStr = timeStr.substr( tzPos );
                tzOffset = boost::posix_time::time_duration( boost::posix_time::duration_from_string( tzStr.c_str() ) );

            }
            else
                noTzStr += timeStr;
        }

        // Remove all the '-' and ':'
        size_t pos = noTzStr.find_first_of( ":-" );
        while ( pos != string::npos )
        {
            noTzStr.erase( pos, 1 );
            pos = noTzStr.find_first_of( ":-" );
        }
        boost::posix_time::ptime t( boost::posix_time::from_iso_string( noTzStr.c_str( ) ) );
        t = t + tzOffset;

        return t;
    }

    string writeDateTime( boost::posix_time::ptime time )
    {
        string str = boost::posix_time::to_iso_extended_string( time );
        str += "Z";
        return str;
    }

    bool parseBool( string boolStr ) throw ( Exception )
    {
        bool value = false;
        if ( boolStr == "true" || boolStr == "1" )
            value = true;
        else if ( boolStr == "false" || boolStr == "0" )
            value = false;
        else
            throw Exception( string( "Invalid xsd:boolean input: " ) + boolStr );
        return value;
    }

    long parseInteger( string intStr ) throw ( Exception )
    {
        char* end;
        errno = 0;
        long value = strtol( intStr.c_str(), &end, 0 );

        if ( ( ERANGE == errno && ( LONG_MAX == value || LONG_MIN == value ) ) ||
             ( errno != 0 && value == 0 ) )
        {
            throw Exception( string( "xsd:integer input can't fit to long: " ) + intStr );
        }
        else if ( !string( end ).empty( ) )
        {
            throw Exception( string( "Invalid xsd:integer input: " ) + intStr );
        }

        return value;
    }
    
    double parseDouble( string doubleStr ) throw ( Exception )
    {
        char* end;
        errno = 0;
        double value = strtod( doubleStr.c_str(), &end );

        if ( ( ERANGE == errno ) || ( errno != 0 && value == 0 ) )
        {
            throw Exception( string( "xsd:decimal input can't fit to double: " ) + doubleStr );
        }
        else if ( !string( end ).empty( ) )
        {
            throw Exception( string( "Invalid xsd:decimal input: " ) + doubleStr );
        }

        return value;
    }
}
