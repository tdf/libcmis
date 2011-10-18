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
#include <sstream>

#include <curl/curl.h>

#include "atom-utils.hxx"

using namespace std;

namespace
{
    size_t lcl_bufferData( void* buffer, size_t size, size_t nmemb, void* data )
    {
        stringstream& out = *( static_cast< stringstream* >( data ) );
        out.write( ( const char* ) buffer, size * nmemb );
        return nmemb;
    }
}

namespace atom
{
    void registerNamespaces( xmlXPathContextPtr pXPathCtx )
    {
        xmlXPathRegisterNs( pXPathCtx, BAD_CAST( "app" ),  NS_APP_URL );
        xmlXPathRegisterNs( pXPathCtx, BAD_CAST( "atom" ),  NS_ATOM_URL );
        xmlXPathRegisterNs( pXPathCtx, BAD_CAST( "cmis" ),  NS_CMIS_URL );
        xmlXPathRegisterNs( pXPathCtx, BAD_CAST( "cmisra" ),  NS_CMISRA_URL );
    }
    
    string getXPathValue( xmlXPathContextPtr pXPathCtx, string req )
    {
        string value;
        xmlXPathObjectPtr pXPathObj = xmlXPathEvalExpression( BAD_CAST( req.c_str() ), pXPathCtx );
        if ( pXPathObj && pXPathObj->nodesetval && pXPathObj->nodesetval->nodeNr > 0 )
        {
            xmlChar* pContent = xmlNodeGetContent( pXPathObj->nodesetval->nodeTab[0] );
            value = string( ( char* )pContent );
            xmlFree( pContent );
        }
        xmlXPathFreeObject( pXPathObj );

        return value;
    }

    xmlDocPtr wrapInDoc( xmlNodePtr entryNd )
    {
        xmlDocPtr doc = xmlNewDoc(BAD_CAST "1.0");
        xmlNodePtr entryCopy = xmlCopyNode( entryNd, 1 );

        xmlDocSetRootElement( doc, entryCopy );
        return doc;
    }

    string httpGetRequest( string url, const string& username, const string& password )
    {
        stringstream stream;

        curl_global_init( CURL_GLOBAL_ALL );
        CURL* pHandle = curl_easy_init( );

        // Grab something from the web
        curl_easy_setopt( pHandle, CURLOPT_URL, url.c_str() );
        curl_easy_setopt( pHandle, CURLOPT_WRITEFUNCTION, lcl_bufferData );
        curl_easy_setopt( pHandle, CURLOPT_WRITEDATA, &stream );

        // Set the credentials
        if ( !username.empty() && !password.empty() )
        {
            curl_easy_setopt( pHandle, CURLOPT_HTTPAUTH, CURLAUTH_ANY );
            curl_easy_setopt( pHandle, CURLOPT_USERNAME, username.c_str() );
            curl_easy_setopt( pHandle, CURLOPT_PASSWORD, password.c_str() );
        }

        // Perform the query
        curl_easy_perform( pHandle );

        curl_easy_cleanup( pHandle );

        return stream.str();
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
}
