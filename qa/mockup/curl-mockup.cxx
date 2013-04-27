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

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sstream>

#include "curl/curl.h"
#include "internals.hxx"

using namespace std;

namespace mockup
{
    extern Configuration* config;
}

CurlHandle::CurlHandle( ) :
    m_url( ),
    m_writeFn( NULL ),
    m_writeData( NULL ),
    m_readFn( NULL ),
    m_readData( NULL ),
    m_readSize( 0 ),
    m_headersFn( NULL ),
    m_headersData( NULL ),
    m_username( ),
    m_password( ),
    m_proxy( ),
    m_noProxy( ),
    m_proxyUser( ),
    m_proxyPass( ),
    m_httpError( 0 ),
    m_method( "GET" )
{
}

CurlHandle::CurlHandle( const CurlHandle& copy ) :
    m_url( copy.m_url ),
    m_writeFn( copy.m_writeFn ),
    m_writeData( copy.m_writeData ),
    m_readFn( copy.m_readFn ),
    m_readData( copy.m_readData ),
    m_readSize( copy.m_readSize ),
    m_headersFn( copy.m_headersFn ),
    m_headersData( copy.m_headersData ),
    m_username( copy.m_username ),
    m_password( copy.m_password ),
    m_proxy( copy.m_proxy ),
    m_noProxy( copy.m_noProxy ),
    m_proxyUser( copy.m_proxyUser ),
    m_proxyPass( copy.m_proxyPass ),
    m_httpError( copy.m_httpError ),
    m_method( copy.m_method )
{
}

CurlHandle& CurlHandle::operator=( const CurlHandle& copy )
{
    if ( this != &copy )
    {
        m_url = copy.m_url;
        m_writeFn = copy.m_writeFn;
        m_writeData = copy.m_writeData;
        m_readFn = copy.m_readFn;
        m_readData = copy.m_readData;
        m_readSize = copy.m_readSize;
        m_headersFn = copy.m_headersFn;
        m_headersData = copy.m_headersData;
        m_username = copy.m_username;
        m_password = copy.m_password;
        m_httpError = copy.m_httpError;
        m_method = copy.m_method;
    }
    return *this;
}

void CurlHandle::reset( )
{
    m_url = string( );
    m_writeFn = NULL;
    m_writeData = NULL;
    m_readFn = NULL;
    m_readData = NULL;
    m_readSize = 0;
    m_username = string( );
    m_password = string( );
    m_method = "GET";
}
