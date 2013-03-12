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

#include "oauth2-data.hxx"

using namespace std;

namespace libcmis
{
    OAuth2Data::OAuth2Data( ) :
        m_authUrl( ),
        m_tokenUrl( ),
        m_clientId( ),
        m_clientSecret( ),
        m_scope( ),
        m_redirectUri( )
    {
    }

    OAuth2Data::OAuth2Data( string authUrl, string tokenUrl,
                           string scope, string redirectUri,
                           string clientId, string clientSecret ):
        m_authUrl( authUrl ),
        m_tokenUrl( tokenUrl ),
        m_clientId( clientId ),
        m_clientSecret( clientSecret ),
        m_scope( scope ),
        m_redirectUri( redirectUri )
    {
    }

    OAuth2Data::OAuth2Data( const OAuth2Data& copy ) :
        m_authUrl( copy.m_authUrl ),
        m_tokenUrl( copy.m_tokenUrl ),
        m_clientId( copy.m_clientId ),
        m_clientSecret( copy.m_clientSecret ),
        m_scope( copy.m_scope ),
        m_redirectUri( copy.m_redirectUri )
    {
    }

    OAuth2Data::~OAuth2Data( )
    {
    }

    OAuth2Data& OAuth2Data::operator=( const OAuth2Data& copy )
    {
        if ( this != &copy )
        {
            m_authUrl = copy.m_authUrl;
            m_tokenUrl = copy.m_tokenUrl;
            m_clientId = copy.m_clientId;
            m_clientSecret = copy.m_clientSecret;
            m_scope = copy.m_scope;
            m_redirectUri = copy.m_redirectUri;
        }

        return *this;
    }

    bool OAuth2Data::isComplete()
    {
        return !m_authUrl.empty() &&
               !m_tokenUrl.empty() &&
               !m_clientId.empty() &&
               !m_clientSecret.empty() &&
               !m_scope.empty() &&
               !m_redirectUri.empty();
    }
}
