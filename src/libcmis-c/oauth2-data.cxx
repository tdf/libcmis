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

#include "oauth2-data.h"
#include "internals.hxx"

using namespace std;


libcmis_OAuth2DataPtr libcmis_oauth2data_create(
        char* authUrl, char* tokenUrl, char* scope, char* redirectUri,
        char* clientId, char* clientSecret,
        libcmis_OAuth2AuthCodeProvider authCodeProvider = NULL )
{
    libcmis_OAuth2DataPtr data = NULL;
    try
    {
        data = libcmis_oauth2data( );

        data->handle.reset( new libcmis::OAuth2Data(
                   authUrl, tokenUrl, scope, redirectUri,
                   clientId, clientSecret, authCodeProvider ) );
    }
    catch ( const bad_alloc& e )
    {
        if ( error != NULL )
        {
            error->message = strdup( e.what() );
            error->badAlloc = true;
        }
    }
    return data;
}


void libcmis_oauth2data_free( libcmis_OAuth2DataPtr oauth2 )
{
    delete oauth2;
}


bool libcmis_oauth2data_isComplete( libcmis_OAuth2DataPtr oauth2 )
{
    bool result = false;
    if ( oauth2 != NULL && oauth2->handle != NULL )
        result = oauth2->handle->isComplete();
    return result;
}


const char* libcmis_oauth2data_getAuthUrl( libcmis_OAuth2DataPtr oauth2 )
{
    if ( oauth2 != NULL && oauth2->handle != NULL )
        return oauth2->handle->getAuthUrl().c_str();
    return NULL;
}


const char* libcmis_oauth2data_getTokenUrl( libcmis_OAuth2DataPtr oauth2 )
{
    if ( oauth2 != NULL && oauth2->handle != NULL )
        return oauth2->handle->getTokenUrl().c_str();
    return NULL;
}


const char* libcmis_oauth2data_getClientId( libcmis_OAuth2DataPtr oauth2 )
{
    if ( oauth2 != NULL && oauth2->handle != NULL )
        return oauth2->handle->getClientId().c_str();
    return NULL;
}


const char* libcmis_oauth2data_getClientSecret( libcmis_OAuth2DataPtr oauth2 )
{
    if ( oauth2 != NULL && oauth2->handle != NULL )
        return oauth2->handle->getClientSecret().c_str();
    return NULL;
}


const char* libcmis_oauth2data_getScope( libcmis_OAuth2DataPtr oauth2 )
{
    if ( oauth2 != NULL && oauth2->handle != NULL )
        return oauth2->handle->getScope().c_str();
    return NULL;
}


const char* libcmis_oauth2data_getRedirectUri( libcmis_OAuth2DataPtr oauth2 )
{
    if ( oauth2 != NULL && oauth2->handle != NULL )
        return oauth2->handle->getRedirectUri().c_str();
    return NULL;
}


libcmis_OAuth2AuthCodeProvider libcmis_oauth2data_getAuthCodeProvider( libcmis_OAuth2DataPtr oauth2 )
{
    if ( oauth2 != NULL && oauth2->handle != NULL )
        return oauth2->handle->getAuthCodeProvider( );
    return NULL;
}
