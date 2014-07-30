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
 * Copyright (C) 2014 Mihai Varga <mihai.mv13@gmail.com>
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

#include "sharepoint-object-type.hxx"

SharePointObjectType::SharePointObjectType( const std::string& id ): ObjectType( )
{
    m_id = id;
    m_localName = "SharePoint Object Type";
    m_localNamespace = "SharePoint Object Type";
    m_displayName = "SharePoint Object Type";
    m_queryName = "SharePoint Object Type";
    m_description = "SharePoint Object Type";
    m_parentTypeId = id;
    m_baseTypeId = id;
    m_creatable = true;
    m_versionable = true;
    m_fulltextIndexed = true;

    libcmis::PropertyTypePtr idType(new libcmis::PropertyType( ) );
    idType->setId( "cmis:objectTypeId" );
    idType->setType( libcmis::PropertyType::String );
    m_propertiesTypes[ idType->getId( ) ] = idType;

    // create PropertyTypes which are updatable.

    // name
    libcmis::PropertyTypePtr nameType( new libcmis::PropertyType( ) );
    nameType->setId( "cmis:name" );
    nameType->setType( libcmis::PropertyType::String );
    nameType->setUpdatable( true );
    m_propertiesTypes[ nameType->getId( ) ] = nameType;

    // streamFileName
    libcmis::PropertyTypePtr streamFileNameType( new libcmis::PropertyType( ) );
    streamFileNameType->setId( "cmis:contentStreamFileName" );
    streamFileNameType->setType( libcmis::PropertyType::String );
    streamFileNameType->setUpdatable( true );
    m_propertiesTypes[ streamFileNameType->getId( ) ] = streamFileNameType;

    // modifiedDate
    libcmis::PropertyTypePtr modifiedDateType( new libcmis::PropertyType( ) );
    modifiedDateType->setId( "cmis:lastModificationDate" );
    modifiedDateType->setType( libcmis::PropertyType::DateTime );
    modifiedDateType->setUpdatable( false );
    m_propertiesTypes[ modifiedDateType->getId( ) ] = modifiedDateType;

    // creationDate 
    libcmis::PropertyTypePtr creationDateType( new libcmis::PropertyType( ) );
    creationDateType->setId( "cmis:creationDate" );
    creationDateType->setType( libcmis::PropertyType::DateTime );
    creationDateType->setUpdatable( false );
    m_propertiesTypes[ creationDateType->getId( ) ] = creationDateType;

    // size 
    libcmis::PropertyTypePtr contentStreamLength( new libcmis::PropertyType( ) );
    contentStreamLength->setId( "cmis:contentStreamLength" );
    contentStreamLength->setType( libcmis::PropertyType::Integer );
    contentStreamLength->setUpdatable( false );
    m_propertiesTypes[ contentStreamLength->getId( ) ] = contentStreamLength;

    // checkinComment
    libcmis::PropertyTypePtr checkinComment( new libcmis::PropertyType( ) );
    checkinComment->setId( "cmis:checkinComment" );
    checkinComment->setType( libcmis::PropertyType::String );
    checkinComment->setUpdatable( false );
    m_propertiesTypes[ checkinComment->getId( ) ] = checkinComment;
}

libcmis::ObjectTypePtr SharePointObjectType::getParentType( )
                                            throw( libcmis::Exception )
{
    libcmis::ObjectTypePtr parentTypePtr( new SharePointObjectType( m_parentTypeId ) );
    return parentTypePtr;
}

libcmis::ObjectTypePtr SharePointObjectType::getBaseType( )
                                            throw( libcmis::Exception )
{
    libcmis::ObjectTypePtr baseTypePtr( new SharePointObjectType( m_baseTypeId ) );
    return baseTypePtr;
}
