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

#include <string>

#include <boost/date_time.hpp>

#include "exception.hxx"

#define NS_CMIS_PREFIX BAD_CAST( "cmis" )
#define NS_CMIS_URL BAD_CAST( "http://docs.oasis-open.org/ns/cmis/core/200908/" )
#define NS_CMISRA_PREFIX BAD_CAST( "cmisra" )
#define NS_CMISRA_URL BAD_CAST( "http://docs.oasis-open.org/ns/cmis/restatom/200908/" )

namespace libcmis
{
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
