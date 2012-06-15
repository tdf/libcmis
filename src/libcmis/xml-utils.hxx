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

#include <ostream>
#include <string>

#include <boost/date_time.hpp>
#include <libxml/tree.h>
#include <libxml/xpathInternals.h>

#include "exception.hxx"

#define NS_CMIS_PREFIX      BAD_CAST( "cmis" )
#define NS_CMISRA_PREFIX    BAD_CAST( "cmisra" )
#define NS_CMIS_URL         BAD_CAST( "http://docs.oasis-open.org/ns/cmis/core/200908/" )
#define NS_CMISRA_URL       BAD_CAST( "http://docs.oasis-open.org/ns/cmis/restatom/200908/" )
#define NS_CMISM_URL        BAD_CAST( "http://docs.oasis-open.org/ns/cmis/messaging/200908/" )
#define NS_CMISW_URL        BAD_CAST( "http://docs.oasis-open.org/ns/cmis/ws/200908/" )
#define NS_APP_URL          BAD_CAST( "http://www.w3.org/2007/app" )
#define NS_ATOM_URL         BAD_CAST( "http://www.w3.org/2005/Atom" )
#define NS_SOAP_URL         BAD_CAST( "http://schemas.xmlsoap.org/wsdl/soap/" )
#define NS_SOAP_ENV_URL     BAD_CAST( "http://schemas.xmlsoap.org/soap/envelope/" )

#define LIBCURL_VERSION_VALUE ( \
        ( LIBCURL_VERSION_MAJOR << 16 ) | ( LIBCURL_VERSION_MINOR << 8 ) | ( LIBCURL_VERSION_PATCH ) \
)

namespace libcmis
{
    /** Class used to decode a stream.

        An instance of this class can hold remaining un-decoded data to use
        for a future decode call.
      */
    class EncodedData
    {
        private:
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

            const EncodedData& operator=( const EncodedData& rCopy );

            void setEncoding( std::string encoding ) { m_encoding = encoding; }
            void decode( void* buf, size_t size, size_t nmemb );
            void encode( void* buf, size_t size, size_t nmemb );
            void finish( );

        private:
            void write( void* buf, size_t size, size_t nmemb );
            void decodeBase64( const char* buf, size_t len );
            void encodeBase64( const char* buf, size_t len );
    };
    
    void registerNamespaces( xmlXPathContextPtr xpathCtx );
    
    /** Register the CMIS and WSDL / SOAP namespaces
      */
    void registerCmisWSNamespaces( xmlXPathContextPtr xpathCtx );
    
    /** Register only the WSD / SOAP namespaces.
      */
    void registerSoapNamespaces( xmlXPathContextPtr xpathCtx );

    std::string getXPathValue( xmlXPathContextPtr xpathCtx, std::string req ); 

    xmlDocPtr wrapInDoc( xmlNodePtr entryNode );
    /** Utility extracting an attribute value from an Xml Node,
        based on the attribute name.
      */
    std::string getXmlNodeAttributeValue( xmlNodePtr node, const char* attributeName ) throw ( Exception );

    /** Parse a xsd:dateTime string and return the corresponding UTC posix time.
     */ 
    boost::posix_time::ptime parseDateTime( std::string dateTimeStr );

    /// Write a UTC time object to an xsd:dateTime string
    std::string writeDateTime( boost::posix_time::ptime time );

    bool parseBool( std::string str ) throw ( Exception );

    long parseInteger( std::string str ) throw ( Exception );

    double parseDouble( std::string str ) throw ( Exception );
}

#endif
