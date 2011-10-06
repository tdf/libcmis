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

#include <sstream>

#include "atom-object.hxx"
#include "atom-session.hxx"
#include "atom-utils.hxx"

using namespace boost;
using namespace std;


/** Constructor for the object, the url provided url should point to the object
    CMIS properties. The content of the URL isn't extracted and parsed by the constructor:
    this task is left to the class children.
  */
AtomObject::AtomObject( AtomPubSession* session, string url ) :
    m_session( session ),
    m_infosUrl( url ),
    m_id( ),
    m_name( ),
    m_baseType( ),
    m_type( ),
    m_createdBy( ),
    m_creationDate( ),
    m_lastModifiedBy( ),
    m_lastModificationDate( ),
    m_changeToken( )
{
}

AtomObject::AtomObject( const AtomObject& copy ) :
    m_session( copy.m_session ),
    m_infosUrl( copy.m_infosUrl ),
    m_id( copy.m_id ),
    m_name( copy.m_name ),
    m_baseType( copy.m_baseType ),
    m_type( copy.m_type ),
    m_createdBy( copy.m_createdBy ),
    m_creationDate( copy.m_creationDate ),
    m_lastModifiedBy( copy.m_lastModifiedBy ),
    m_lastModificationDate( copy.m_lastModificationDate ),
    m_changeToken( copy.m_changeToken )
{
}

AtomObject& AtomObject::operator=( const AtomObject& copy )
{
    m_session = copy.m_session;
    m_infosUrl = copy.m_infosUrl;
    m_id = copy.m_id;
    m_name = copy.m_name;
    m_baseType = copy.m_baseType;
    m_type = copy.m_type;
    m_createdBy = copy.m_createdBy;
    m_creationDate = copy.m_creationDate;
    m_lastModifiedBy = copy.m_lastModifiedBy;
    m_lastModificationDate = copy.m_lastModificationDate;
    m_changeToken = copy.m_changeToken;

    return *this;
}

AtomObject::~AtomObject( )
{
}

string AtomObject::getId( )
{
    return m_id;
}

string AtomObject::getName( )
{
    return m_name;
}

string AtomObject::getBaseType( )
{
    return m_baseType;
}

string AtomObject::getType( )
{
    return m_type;
}

string AtomObject::getCreatedBy( )
{
    return m_createdBy;
}

posix_time::ptime AtomObject::getCreationDate( )
{
    return m_creationDate;
}

string AtomObject::getLastModifiedBy( )
{
    return m_lastModifiedBy;
}

posix_time::ptime AtomObject::getLastModificationDate( )
{
    return m_lastModificationDate;
}

string AtomObject::getChangeToken( )
{
    return m_changeToken;
}

string AtomObject::toString( )
{
    stringstream buf;

    buf << "Id: " << getId() << endl;
    buf << "Name: " << getName() << endl;
    buf << "Type: " << getType() << endl;
    buf << "Base type: " << getBaseType() << endl;
    buf << "Created on " << posix_time::to_simple_string( getCreationDate() )
        << " by " << getCreatedBy() << endl;
    buf << "Last modified on " << posix_time::to_simple_string( getLastModificationDate() )
        << " by " << getLastModifiedBy() << endl;
    buf << "Change token: " << getChangeToken() << endl;

    return buf.str();
}

void AtomObject::extractInfos( xmlDocPtr doc )
{
    xmlXPathContextPtr pXPathCtx = xmlXPathNewContext( doc );

    // Register the Service Document namespaces
    atom::registerNamespaces( pXPathCtx );

    if ( NULL != pXPathCtx )
    {
        // Get the name
        string nameReq( "//cmis:propertyString[@propertyDefinitionId='cmis:name']/cmis:value/text()" );
        m_name = atom::getXPathValue( pXPathCtx, nameReq );
        
        // Get the id
        string idReq( "//cmis:propertyId[@propertyDefinitionId='cmis:objectId']/cmis:value/text()" );
        m_id = atom::getXPathValue( pXPathCtx, idReq );

        // Get the base type
        string baseTypeReq( "//cmis:propertyId[@propertyDefinitionId='cmis:baseTypeId']/cmis:value/text()" );
        m_baseType = atom::getXPathValue( pXPathCtx, baseTypeReq );
        
        // Get the type
        string typeReq( "//cmis:propertyId[@propertyDefinitionId='cmis:objectTypeId']/cmis:value/text()" );
        m_type = atom::getXPathValue( pXPathCtx, typeReq );

        // Get the createdBy property
        string createdByReq( "//cmis:propertyString[@propertyDefinitionId='cmis:createdBy']/cmis:value/text()" );
        m_createdBy = atom::getXPathValue( pXPathCtx, createdByReq );

        // Get the creation date
        string creationReq( "//cmis:propertyDateTime[@propertyDefinitionId='cmis:creationDate']/cmis:value/text()" );
        m_creationDate = atom::parseDateTime( atom::getXPathValue( pXPathCtx, creationReq ) );

        // Get the lastModifiedBy property
        string lastModifByReq( "//cmis:propertyString[@propertyDefinitionId='cmis:lastModifiedBy']/cmis:value/text()" );
        m_lastModifiedBy = atom::getXPathValue( pXPathCtx, lastModifByReq );
        
        // Get the kast modification date
        string lastModifReq( "//cmis:propertyDateTime[@propertyDefinitionId='cmis:lastModificationDate']/cmis:value/text()" );
        m_lastModificationDate = atom::parseDateTime( atom::getXPathValue( pXPathCtx, lastModifReq ) );

        // Get the change token
        string changeTokenReq( "//cmis:propertyString[@propertyDefinitionId='cmis:changeToken']/cmis:value/text()" );
        m_changeToken = atom::getXPathValue( pXPathCtx, changeTokenReq );

    }
    xmlXPathFreeContext( pXPathCtx );
}
