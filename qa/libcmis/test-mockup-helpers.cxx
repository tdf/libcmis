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

#include <test-helpers.hxx>
#include <mockup-config.h>

using namespace std;

namespace test
{
    void addWsResponse( const char* url, const char* filename,
                        const char* bodyMatch = 0 )
    {
        string outBuf;
        loadFromFile( filename, outBuf );

        string emptyLine = ( "\n\n" );
        size_t pos = outBuf.find( emptyLine );
        string headers = outBuf.substr( 0, pos );
        string body = outBuf.substr( pos + emptyLine.size() );

        curl_mockup_addResponse( url, "", "POST", body.c_str(), 0, false,
                                 headers.c_str(), bodyMatch );
    }
}
