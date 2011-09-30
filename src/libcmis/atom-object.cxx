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

using namespace std;

/** Constructor for the object, the url provided url should point to the object
    CMIS properties. The content of the URL isn't extracted and parsed by the constructor:
    this task is left to the class children.
  */
AtomCmisObject::AtomCmisObject( AtomPubSession* session, string url ) :
    m_session( session ),
    m_infosUrl( url ),
    m_name( ),
    m_baseType( ),
    m_type( )
{
}

AtomCmisObject::~AtomCmisObject( )
{
}

string AtomCmisObject::getId( )
{
    return m_id;
}

string AtomCmisObject::getName( )
{
    return m_name;
}

string AtomCmisObject::getBaseType( )
{
    return m_baseType;
}

string AtomCmisObject::getType( )
{
    return m_type;
}

string AtomCmisObject::toString( )
{
    stringstream buf;

    buf << "Id: " << getId() << endl;
    buf << "Name: " << getName() << endl;
    buf << "Type: " << getType() << endl;
    buf << "Base type: " << getBaseType() << endl;

    return buf.str();
}

void AtomCmisObject::extractInfos( xmlDocPtr doc )
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
    }
    xmlXPathFreeContext( pXPathCtx );
}
