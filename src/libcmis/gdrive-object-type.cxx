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
 * Copyright (C) 2013 Cao Cuong Ngo <cao.cuong.ngo@gmail.com>
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

#include "gdrive-object-type.hxx"

GdriveObjectType::GdriveObjectType( const std::string& id ): ObjectType( )
{
    m_id = id;
    m_localName = "GoogleDrive Object Type";
    m_localNamespace = "GoogleDrive Object Type";
    m_displayName = "GoogleDrive Object Type";
    m_queryName = "GoogleDrive Object Type";
    m_description = "GoogleDrive Object Type";
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

    // title
    libcmis::PropertyTypePtr nameType( new libcmis::PropertyType( ) );
    nameType->setId( "cmis:name" );
    nameType->setType( libcmis::PropertyType::String );
    nameType->setUpdatable( true );
    m_propertiesTypes[ nameType->getId( ) ] = nameType;

    // mimeType
    libcmis::PropertyTypePtr mimeType( new libcmis::PropertyType( ) );
    mimeType->setId( "cmis:contentStreamMimeType" );
    mimeType->setType( libcmis::PropertyType::String );
    mimeType->setUpdatable( false );
    m_propertiesTypes[ mimeType->getId( ) ] = mimeType;

    // parents
    libcmis::PropertyTypePtr parentsType( new libcmis::PropertyType( ) );
    parentsType->setId( "cmis:parentId" );
    parentsType->setType( libcmis::PropertyType::String );
    parentsType->setUpdatable( false );
    parentsType->setMultiValued( true );
    m_propertiesTypes[ parentsType->getId( ) ] = parentsType;

    // labels
    libcmis::PropertyTypePtr labelsType( new libcmis::PropertyType( ) );
    labelsType->setId( "labels" );
    labelsType->setType( libcmis::PropertyType::String );
    labelsType->setUpdatable( false );
    labelsType->setMultiValued( true );
    m_propertiesTypes[ labelsType->getId( ) ] = labelsType;

    // ownerNames
    libcmis::PropertyTypePtr ownerNamesType( new libcmis::PropertyType( ) );
    ownerNamesType->setId( "ownerNames" );
    ownerNamesType->setType( libcmis::PropertyType::String );
    ownerNamesType->setUpdatable( false );
    ownerNamesType->setMultiValued( true );
    m_propertiesTypes[ ownerNamesType->getId( ) ] = ownerNamesType;

    // owners
    libcmis::PropertyTypePtr ownersType( new libcmis::PropertyType( ) );
    ownersType->setId( "owners" );
    ownersType->setType( libcmis::PropertyType::String );
    ownersType->setUpdatable( false );
    ownersType->setMultiValued( true );
    m_propertiesTypes[ ownersType->getId( ) ] = ownersType;

    // export links
    libcmis::PropertyTypePtr exportLinksType( new libcmis::PropertyType( ) );
    exportLinksType->setId( "exportLinks" );
    exportLinksType->setType( libcmis::PropertyType::String );
    exportLinksType->setUpdatable( false );
    exportLinksType->setMultiValued( true );
    m_propertiesTypes[ exportLinksType->getId( ) ] = exportLinksType;

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
    modifiedDateType->setUpdatable( true );
    m_propertiesTypes[ modifiedDateType->getId( ) ] = modifiedDateType;

    // lastViewedByMeDate
    libcmis::PropertyTypePtr lastViewedByMeDateType( new libcmis::PropertyType( ) );
    lastViewedByMeDateType->setId( "lastViewedByMeDate" );
    lastViewedByMeDateType->setType( libcmis::PropertyType::DateTime );
    lastViewedByMeDateType->setUpdatable( true );
    m_propertiesTypes[ lastViewedByMeDateType->getId( ) ] = lastViewedByMeDateType;

}


libcmis::ObjectTypePtr GdriveObjectType::getParentType( )
                                            throw( libcmis::Exception )
{
    libcmis::ObjectTypePtr parentTypePtr( new GdriveObjectType( m_parentTypeId ) );
    return parentTypePtr;
}

libcmis::ObjectTypePtr GdriveObjectType::getBaseType( )
                                            throw( libcmis::Exception )
{
    libcmis::ObjectTypePtr baseTypePtr( new GdriveObjectType( m_baseTypeId ) );
    return baseTypePtr;
}

