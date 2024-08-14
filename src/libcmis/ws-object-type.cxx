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

#include "ws-object-type.hxx"

using namespace std;

WSObjectType::WSObjectType( WSSession* session, xmlNodePtr node ) :
    libcmis::ObjectType( node ),
    m_session( session )
{
}

WSObjectType::WSObjectType( ) :
    libcmis::ObjectType( ),
    m_session( NULL )
{
}

WSObjectType::WSObjectType( const WSObjectType& copy ) :
    libcmis::ObjectType( copy ),
    m_session( copy.m_session )
{
}

WSObjectType::~WSObjectType( )
{
}

WSObjectType& WSObjectType::operator=( const WSObjectType& copy )
{
    if ( this != &copy )
    {
        libcmis::ObjectType::operator=( copy );
        m_session = copy.m_session;
    }

    return *this;
}

void WSObjectType::refresh( )
{
    libcmis::ObjectTypePtr type = m_session->getType( m_id );
    const auto other = dynamic_cast< const WSObjectType* >( type.get( ) );
    if ( other != NULL )
        *this = *other;
}

libcmis::ObjectTypePtr WSObjectType::getParentType( )
{
    return m_session->getType( m_parentTypeId );
}

libcmis::ObjectTypePtr WSObjectType::getBaseType( )
{
    return m_session->getType( m_baseTypeId );
}

vector< libcmis::ObjectTypePtr > WSObjectType::getChildren( )
{
    vector< libcmis::ObjectTypePtr > children;
    children = m_session->getRepositoryService( ).getTypeChildren(
            m_session->getRepositoryId( ), m_id );
    return children;
}
