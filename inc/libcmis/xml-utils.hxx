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
#ifndef _XML_UTILS_HXX_
#define _XML_UTILS_HXX_

#include <map>
#include <ostream>
#include <sstream>
#include <string>

#include <boost/date_time.hpp>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/xpathInternals.h>
#include <libxml/xmlwriter.h>

#include "libcmis/exception.hxx"
#include "libcmis/libcmis-api.h"

#define NS_CMIS_PREFIX      "cmis"
#define NS_CMISRA_PREFIX    "cmisra"
#define NS_SOAP_ENV_PREFIX  "soap-env"
#define NS_CMIS_URL         "http://docs.oasis-open.org/ns/cmis/core/200908/"
#define NS_CMISRA_URL       "http://docs.oasis-open.org/ns/cmis/restatom/200908/"
#define NS_CMISM_URL        "http://docs.oasis-open.org/ns/cmis/messaging/200908/"
#define NS_CMISW_URL        "http://docs.oasis-open.org/ns/cmis/ws/200908/"
#define NS_APP_URL          "http://www.w3.org/2007/app"
#define NS_ATOM_URL         "http://www.w3.org/2005/Atom"
#define NS_SOAP_URL         "http://schemas.xmlsoap.org/wsdl/soap/"
#define NS_SOAP_ENV_URL     "http://schemas.xmlsoap.org/soap/envelope/"

#define LIBCURL_VERSION_VALUE ( \
        ( LIBCURL_VERSION_MAJOR << 16 ) | ( LIBCURL_VERSION_MINOR << 8 ) | ( LIBCURL_VERSION_PATCH ) \
)

namespace libcmis
{
    /** Class used to decode a stream.

        An instance of this class can hold remaining un-decoded data to use
        for a future decode call.
      */
    class LIBCMIS_API EncodedData
    {
        private:
            xmlTextWriterPtr m_writer;
            FILE* m_stream;
            std::ostream* m_outStream;

            std::string m_encoding;
            bool m_decode;
            unsigned long m_pendingValue;
            int m_pendingRank;
            size_t m_missingBytes;

        public:
            EncodedData( FILE* stream );
            EncodedData( std::ostream* stream );
            EncodedData( const EncodedData& rCopy );
            EncodedData( xmlTextWriterPtr writer );

            EncodedData& operator=( const EncodedData& rCopy );

            void setEncoding( std::string encoding ) { m_encoding = encoding; }
            void decode( void* buf, size_t size, size_t nmemb );
            void encode( void* buf, size_t size, size_t nmemb );
            void finish( );

        private:
            void write( void* buf, size_t size, size_t nmemb );
            void decodeBase64( const char* buf, size_t len );
            void encodeBase64( const char* buf, size_t len );
    };

    class LIBCMIS_API HttpResponse
    {
        private:
            std::map< std::string, std::string > m_headers;
            boost::shared_ptr< std::stringstream > m_stream;
            boost::shared_ptr< EncodedData > m_data;

        public:
            HttpResponse( );
            ~HttpResponse( ) { };

            std::map< std::string, std::string >& getHeaders( ) { return m_headers; }
            boost::shared_ptr< EncodedData > getData( ) { return m_data; }
            boost::shared_ptr< std::stringstream > getStream( ) { return m_stream; }
    };
    typedef boost::shared_ptr< HttpResponse > HttpResponsePtr;

    LIBCMIS_API void registerNamespaces( xmlXPathContextPtr xpathCtx );

    /** Register the CMIS and WSDL / SOAP namespaces
      */
    LIBCMIS_API void registerCmisWSNamespaces( xmlXPathContextPtr xpathCtx );

    /** Register only the WSD / SOAP namespaces.
      */
    LIBCMIS_API void registerSoapNamespaces( xmlXPathContextPtr xpathCtx );

    LIBCMIS_API std::string getXPathValue( xmlXPathContextPtr xpathCtx, std::string req );

    LIBCMIS_API xmlDocPtr wrapInDoc( xmlNodePtr entryNode );

    /** Utility extracting an attribute value from an Xml Node,
        based on the attribute name. If the defaultValue is NULL and
        the attribute can't be found then throw an exception.
      */
    LIBCMIS_API std::string getXmlNodeAttributeValue( xmlNodePtr node,
                                          const char* attributeName,
                                          const char* defaultValue = NULL );

    /** Parse a xsd:dateTime string and return the corresponding UTC posix time.
     */
    LIBCMIS_API boost::posix_time::ptime parseDateTime( std::string dateTimeStr );

    /// Write a UTC time object to an xsd:dateTime string
    LIBCMIS_API std::string writeDateTime( boost::posix_time::ptime time );

    LIBCMIS_API bool parseBool( std::string str );

    LIBCMIS_API long parseInteger( std::string str );

    LIBCMIS_API double parseDouble( std::string str );

    /** Trim spaces on the left and right of a string.
     */
    LIBCMIS_API std::string trim( const std::string& str );

    LIBCMIS_API std::string base64encode( const std::string& str );

    LIBCMIS_API std::string sha1( const std::string& str );

    LIBCMIS_API int stringstream_write_callback(void * context, const char * s, int len);

    LIBCMIS_API std::string escape( std::string str );

    LIBCMIS_API std::string unescape( std::string str );
}

#endif
