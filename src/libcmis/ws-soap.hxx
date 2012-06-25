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

#include "ws-relatedmultipart.hxx"
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
typedef SoapResponsePtr ( *SoapResponseCreator ) ( xmlNodePtr, RelatedMultipart& );

/** Base clas for SoapFault details parsed data.
  */
class SoapFaultDetail
{
    public:
        virtual ~SoapFaultDetail() {};

        virtual const std::string toString( ) const { return std::string( ); }
};
typedef boost::shared_ptr< SoapFaultDetail > SoapFaultDetailPtr;
typedef SoapFaultDetailPtr ( *SoapFaultDetailCreator ) ( xmlNodePtr );

class SoapResponseFactory;
/** Class representing a SOAP Fault element, to be used as an exception.
  */
class SoapFault : public std::exception
{
    private:
        std::string m_faultcode;
        std::string m_faultstring;
        std::vector< SoapFaultDetailPtr > m_detail;

    public:
        SoapFault( xmlNodePtr faultNode, SoapResponseFactory* factory );
        virtual ~SoapFault( ) throw ( ) { };

        const std::string& getFaultcode ( ) const { return m_faultcode; }
        const std::string& getFaultstring ( ) const { return m_faultstring; }
        std::vector< SoapFaultDetailPtr > getDetail( ) const { return m_detail; }

        virtual const char* what() const throw();
};


/** Class parsing the SOAP response message and extracting the SoapResponse objects.
  */
class SoapResponseFactory
{
    private:
        std::map< std::string, SoapResponseCreator > m_mapping;
        std::map< std::string, std::string > m_namespaces;
        std::map< std::string, SoapFaultDetailCreator > m_detailMapping;

    public:

        SoapResponseFactory( );

        void setMapping( std::map< std::string, SoapResponseCreator > mapping ) { m_mapping = mapping; }

        /** Set the additional namespaces to parse the responses. There is no need to
            add the soap / wsdl namespaces... they are automatically added.
          */
        void setNamespaces( std::map< std::string, std::string > namespaces ) { m_namespaces = namespaces; }

        void setDetailMapping( std::map< std::string, SoapFaultDetailCreator > mapping ) { m_detailMapping = mapping; }

        /** Get the Soap envelope from the multipart and extract the response objects from it. This
            method will also read the possible related parts to construct the response.
          */
        std::vector< SoapResponsePtr > parseResponse( RelatedMultipart& multipart ) throw ( SoapFault );

        /** Create a SoapResponse object depending on the node we have. This shouldn't be used
            directly: only from parseResponse or unit tests.
          */
        SoapResponsePtr createResponse( xmlNodePtr node, RelatedMultipart& multipart );

        std::vector< SoapFaultDetailPtr > parseFaultDetail( xmlNodePtr detailNode );
};


/** Base class for all SOAP request objects.

    The implementer's toXml() method needs to take care of two things:
    \li generate the XML to put in the Soap envelope body
    \li add the potential attachement to the multipart.
    
    There is no need to add the envelope to the multipart: it will
    automatically be added as the start part of it by getMultipart().
    This also means that adding parts to the multipart will have to
    be done directly on the m_multipart protected member.

    The RelatedMultipart object is the final result to be used.
  */
class SoapRequest : public libcmis::XmlSerializable
{
    protected:
        RelatedMultipart m_multipart;

    public:
        SoapRequest( ) : m_multipart( ) { };
        virtual ~SoapRequest( ) { };

        RelatedMultipart& getMultipart( std::string& username, std::string& password );

    protected:

        std::string createEnvelope( std::string& username, std::string& password );
};

#endif
