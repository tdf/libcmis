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

#include "onedrive-object-type.hxx"

OneDriveObjectType::OneDriveObjectType( const std::string& id ): ObjectType( )
{
    m_id = id;
    m_localName = "OneDrive Object Type";
    m_localNamespace = "OneDrive Object Type";
    m_displayName = "OneDrive Object Type";
    m_queryName = "OneDrive Object Type";
    m_description = "OneDrive Object Type";
    m_parentTypeId = id;
    m_baseTypeId = id;
    m_creatable = true;
    m_versionable = false;
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
    nameType->setId( "cmis:contentStreamFileName" );
    nameType->setType( libcmis::PropertyType::String );
    nameType->setUpdatable( true );
    m_propertiesTypes[ nameType->getId( ) ] = streamFileNameType;

    // description
    libcmis::PropertyTypePtr descriptionType( new libcmis::PropertyType( ) );
    descriptionType->setId( "cmis:description" );
    descriptionType->setType( libcmis::PropertyType::String );
    descriptionType->setUpdatable( true );
    m_propertiesTypes[ descriptionType->getId( ) ] = descriptionType;

    // modifiedDate
    libcmis::PropertyTypePtr modifiedDateType( new libcmis::PropertyType( ) );
    modifiedDateType->setId( "cmis:lastModificationDate" );
    modifiedDateType->setType( libcmis::PropertyType::DateTime );
    modifiedDateType->setUpdatable( false );
    m_propertiesTypes[ modifiedDateType->getId( ) ] = modifiedDateType;

    // creationDate 
    libcmis::PropertyTypePtr creationDateType( new libcmis::PropertyType( ) );
    modifiedDateType->setId( "cmis:creationDate" );
    modifiedDateType->setType( libcmis::PropertyType::DateTime );
    modifiedDateType->setUpdatable( false );
    m_propertiesTypes[ modifiedDateType->getId( ) ] = creationDateType;

    // size 
    libcmis::PropertyTypePtr contentStreamLength( new libcmis::PropertyType( ) );
    modifiedDateType->setId( "cmis:contentStreamLength" );
    modifiedDateType->setType( libcmis::PropertyType::Integer );
    modifiedDateType->setUpdatable( false );
    m_propertiesTypes[ modifiedDateType->getId( ) ] = contentStreamLength;
}

libcmis::ObjectTypePtr OneDriveObjectType::getParentType( )
                                            throw( libcmis::Exception )
{
    libcmis::ObjectTypePtr parentTypePtr( new OneDriveObjectType( m_parentTypeId ) );
    return parentTypePtr;
}

libcmis::ObjectTypePtr OneDriveObjectType::getBaseType( )
                                            throw( libcmis::Exception )
{
    libcmis::ObjectTypePtr baseTypePtr( new OneDriveObjectType( m_baseTypeId ) );
    return baseTypePtr;
}
