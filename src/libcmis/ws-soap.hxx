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
#ifndef _WS_SOAP_HXX_
#define _WS_SOAP_HXX_

#include <exception>
#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <libxml/tree.h>

#include "xmlserializable.hxx"


/** Base class for all SOAP response objects.

    The factory will need to create the response objects using a static
    creator method in each class.
  */
class SoapResponse
{
    public:
        virtual ~SoapResponse( ) { };
};
typedef boost::shared_ptr< SoapResponse > SoapResponsePtr;
typedef SoapResponsePtr (*SoapResponseCreator) ( xmlNodePtr );


/** Class representing a SOAP Fault element, to be used as an exception.
  */
class SoapFault : public std::exception
{
    private:
        std::string m_faultcode;
        std::string m_faultstring;

    public:
        SoapFault( xmlNodePtr faultNode );
        virtual ~SoapFault( ) throw ( ) { };

        const std::string& getFaultcode ( ) const { return m_faultcode; }
        const std::string& getFaultstring ( ) const { return m_faultstring; }

        virtual const char* what() const throw();
};


/** Class parsing the SOAP response message and extracting the SoapResponse objects.
  */
class SoapResponseFactory
{
    private:
        std::map< std::string, SoapResponseCreator > m_mapping;
        std::map< std::string, std::string > m_namespaces;

    public:

        SoapResponseFactory( );

        void setMapping( std::map< std::string, SoapResponseCreator > mapping ) { m_mapping = mapping; }

        /** Set the additional namespaces to parse the responses. There is no need to
            add the soap / wsdl namespaces... they are automatically added.
          */
        void setNamespaces( std::map< std::string, std::string > namespaces ) { m_namespaces = namespaces; }

        /** Parse the whole SOAP enveloppe to extract the response objects.
          */
        std::vector< SoapResponsePtr > parseResponse( std::string xml ) throw ( SoapFault );

        /** Create a SoapResponse object depending on the node we have. This shouldn't be used
            directly: only from parseResponse or unit tests.
          */
        SoapResponsePtr createResponse( xmlNodePtr node );
};


/** Base class for all SOAP request objects.
  */
class SoapRequest : public libcmis::XmlSerializable
{
    public:
        virtual ~SoapRequest( ) { };
};

#endif
