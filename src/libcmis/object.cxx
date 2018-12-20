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

#include <libcmis/object.hxx>

#include <algorithm>

#include <libcmis/session.hxx>
#include <libcmis/xml-utils.hxx>

using namespace std;

namespace libcmis
{
    Object::Object( Session* session ) :
        m_session( session ),
        m_typeDescription( ),
        m_refreshTimestamp( 0 ),
        m_typeId( ),
        m_properties( ),
        m_allowableActions( ),
        m_renditions( )
    {
    }

    Object::Object( Session* session, xmlNodePtr node ) :
        m_session( session ),
        m_typeDescription( ),
        m_refreshTimestamp( 0 ),
        m_typeId( ),
        m_properties( ),
        m_allowableActions( ),
        m_renditions( )
    {
        initializeFromNode( node );
    }

    Object::Object( const Object& copy ) :
        m_session( copy.m_session ),
        m_typeDescription( copy.m_typeDescription ),
        m_refreshTimestamp( copy.m_refreshTimestamp ),
        m_typeId( copy.m_typeId ),
        m_properties( copy.m_properties ),
        m_allowableActions( copy.m_allowableActions ),
        m_renditions( copy.m_renditions )
    {
    }

    Object& Object::operator=( const Object& copy )
    {
        if ( this != &copy )
        {
            m_session = copy.m_session;
            m_typeDescription = copy.m_typeDescription;
            m_refreshTimestamp = copy.m_refreshTimestamp;
            m_typeId = copy.m_typeId;
            m_properties = copy.m_properties;
            m_allowableActions = copy.m_allowableActions;
            m_renditions = copy.m_renditions;
        }

        return *this;
    }

    void Object::initializeFromNode( xmlNodePtr node )
    {
        // Even if node is NULL we'll have an empty doc, so no need
        // to worry about it.
        xmlDocPtr doc = wrapInDoc( node );
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext( doc );

        libcmis::registerNamespaces( xpathCtx );

        if ( NULL != xpathCtx )
        {
            // Get the allowableActions
            xmlXPathObjectPtr xpathObj = xmlXPathEvalExpression( BAD_CAST( "//cmis:allowableActions" ), xpathCtx );
            if ( xpathObj && xpathObj->nodesetval && xpathObj->nodesetval->nodeNr > 0 )
            {
                xmlNodePtr actionsNode = xpathObj->nodesetval->nodeTab[0];
                m_allowableActions.reset( new libcmis::AllowableActions( actionsNode ) );
            }
            xmlXPathFreeObject( xpathObj );

            // TODO Get rid of this request:
            //   * Too time consuming
            //   * Makes secondary aspect properties annoying to create
            //   * Prevents from getting Alfresco additional properties
            // First get the type id as it will give us the property definitions
            string typeIdReq( "/*/cmis:properties/cmis:propertyId[@propertyDefinitionId='cmis:objectTypeId']/cmis:value/text()" );
            m_typeId = libcmis::getXPathValue( xpathCtx, typeIdReq );

            string propertiesReq( "/*/cmis:properties/*" );
            xpathObj = xmlXPathEvalExpression( BAD_CAST( propertiesReq.c_str() ), xpathCtx );
            if ( NULL != xpathObj && NULL != xpathObj->nodesetval )
            {
                int size = xpathObj->nodesetval->nodeNr;
                for ( int i = 0; i < size; i++ )
                {
                    xmlNodePtr propertyNode = xpathObj->nodesetval->nodeTab[i];
                    libcmis::PropertyPtr property = libcmis::parseProperty( propertyNode, getTypeDescription( ) );
                    if ( property != NULL )
                        m_properties[ property->getPropertyType( )->getId() ] = property;
                }
            }
            xmlXPathFreeObject( xpathObj );
        }

        xmlXPathFreeContext( xpathCtx );
        xmlFreeDoc( doc );

        m_refreshTimestamp = time( NULL );
    }

    string Object::getStringProperty( const string& propertyName )
    {
       string name;
       PropertyPtrMap::const_iterator it = getProperties( ).find( string( propertyName ) );
       if ( it != getProperties( ).end( ) && it->second != NULL && !it->second->getStrings( ).empty( ) )
           name = it->second->getStrings( ).front( );
       return name;
    }

    string Object::getId( )
    {
        return getStringProperty( "cmis:objectId" );
    }

    string Object::getName( )
    {
        return getStringProperty( "cmis:name" );
    }

    string Object::getBaseType( )
    {
        return getStringProperty( "cmis:baseTypeId" );
    }

    string Object::getType( )
    {
        string value = getStringProperty( "cmis:objectTypeId" );
        if ( value.empty( ) )
            value = m_typeId;
        return value;
    }

    string Object::getCreatedBy( )
    {
        return getStringProperty( "cmis:createdBy" );
    }

    string Object::getLastModifiedBy( )
    {
        return getStringProperty( "cmis:lastModifiedBy" );
    }

    string Object::getChangeToken( )
    {
        return getStringProperty( "cmis:changeToken" );
    }

    vector< string > Object::getPaths( )
    {
        return vector< string > ( );
    }

    boost::posix_time::ptime Object::getCreationDate( )
    {
        boost::posix_time::ptime value;
        PropertyPtrMap::const_iterator it = getProperties( ).find( string( "cmis:creationDate" ) );
        if ( it != getProperties( ).end( ) && it->second != NULL && !it->second->getDateTimes( ).empty( ) )
            value = it->second->getDateTimes( ).front( );
        return value;
    }

    boost::posix_time::ptime Object::getLastModificationDate( )
    {
        boost::posix_time::ptime value;
        PropertyPtrMap::const_iterator it = getProperties( ).find( string( "cmis:lastModificationDate" ) );
        if ( it != getProperties( ).end( ) && it->second != NULL && !it->second->getDateTimes( ).empty( ) )
            value = it->second->getDateTimes( ).front( );
        return value;
    }

    bool Object::isImmutable( )
    {
        bool value = false;
        PropertyPtrMap::const_iterator it = getProperties( ).find( string( "cmis:isImmutable" ) );
        if ( it != getProperties( ).end( ) && it->second != NULL && !it->second->getBools( ).empty( ) )
            value = it->second->getBools( ).front( );
        return value;
    }

    vector< string > Object::getSecondaryTypes( )
    {
        vector< string > types;
        PropertyPtrMap::const_iterator it = getProperties( ).find( string( "cmis:secondaryObjectTypeIds" ) );
        if ( it != getProperties( ).end( ) && it->second != NULL )
            types = it->second->getStrings( );

        return types;
    }

    ObjectPtr Object::addSecondaryType( string id, PropertyPtrMap properties )
    {
        // First make sure the cmis:secondaryObjectTypeIds property can be defined
        map< string, PropertyTypePtr >& propertyTypes = getTypeDescription( )->
            getPropertiesTypes();

        map< string, PropertyTypePtr >::iterator it = propertyTypes.find( "cmis:secondaryObjectTypeIds" );
        if ( it == propertyTypes.end() )
            throw ( Exception( "Secondary Types not supported", "constraint" ) );

        // Copy all the new properties without checking they are
        // defined in the secondary type definition: that would
        // require one more HTTP request and the server will complain
        // anyway if it's not good.
        PropertyPtrMap newProperties( properties );

        // Prepare the new cmis:secondaryObjectTypeIds property
        vector< string > secTypes = getSecondaryTypes( );
        if ( find( secTypes.begin(), secTypes.end(), id ) == secTypes.end( ) )
        {
            secTypes.push_back( id );
            PropertyPtr newSecTypes( new Property( it->second, secTypes ) );
            newProperties["cmis:secondaryObjectTypeIds"] = newSecTypes;
        }
        return updateProperties( newProperties );
    }

    ObjectPtr Object::removeSecondaryType( string id )
    {
        // First make sure the cmis:secondaryObjectTypeIds property can be defined
        map< string, PropertyTypePtr >& propertyTypes = getTypeDescription( )->
            getPropertiesTypes();

        map< string, PropertyTypePtr >::iterator it = propertyTypes.find( "cmis:secondaryObjectTypeIds" );
        if ( it == propertyTypes.end() )
            throw ( Exception( "Secondary Types not supported", "constraint" ) );

        // Prepare the new cmis:secondaryObjectTypeIds property
        PropertyPtrMap newProperties;
        vector< string > secTypes = getSecondaryTypes( );
        vector< string > newSecTypes;
        for ( vector< string >::iterator idIt = secTypes.begin( );
                idIt != secTypes.end( ); ++idIt )
        {
            if ( *idIt != id )
                newSecTypes.push_back( *idIt );
        }

        // No need to update the property if it didn't change
        if ( newSecTypes.size( ) != secTypes.size( ) )
        {
            PropertyPtr property ( new Property( it->second, newSecTypes ) );
            newProperties["cmis:secondaryObjectTypeIds"] = property;
        }

        return updateProperties( newProperties );
    }

    PropertyPtrMap& Object::getProperties( )
    {
        return m_properties;
    }

    libcmis::ObjectTypePtr Object::getTypeDescription( )
    {
        if ( !m_typeDescription.get( ) && m_session != NULL )
            m_typeDescription = m_session->getType( getType( ) );

        return m_typeDescription;
    }

    vector< RenditionPtr> Object::getRenditions( string /*filter*/ )
    {
        return m_renditions;
    }

    string Object::getThumbnailUrl( )
    {
        string url;
        vector< RenditionPtr > renditions = getRenditions( );
        for ( vector< RenditionPtr >::iterator it = renditions.begin( );
            it != renditions.end( ); ++it)

        {
            if ( (*it)->getKind( ) == "cmis:thumbnail" ) return (*it)->getUrl( );
        }

        return url;
    }

    // LCOV_EXCL_START
    string Object::toString( )
    {
        stringstream buf;

        buf << "Id: " << getId() << endl;
        buf << "Name: " << getName() << endl;
        buf << "Type: " << getType() << endl;
        buf << "Base type: " << getBaseType() << endl;
        buf << "Created on " << boost::posix_time::to_simple_string( getCreationDate() )
            << " by " << getCreatedBy() << endl;
        buf << "Last modified on " << boost::posix_time::to_simple_string( getLastModificationDate() )
            << " by " << getLastModifiedBy() << endl;
        buf << "Change token: " << getChangeToken() << endl;

        // Write Allowable Actions
        if ( getAllowableActions( ) )
            buf << endl << getAllowableActions( )->toString( ) << endl;

        // Write remaining properties
        static const char* skippedProps[] = {
            "cmis:name", "cmis:baseTypeId", "cmis:objectTypeId", "cmis:createdBy",
            "cmis:creationDate", "cmis:lastModifiedBy", "cmis:lastModificationDate",
            "cmis::changeToken"
        };
        int skippedCount = sizeof( skippedProps ) / sizeof( char* );

        for ( PropertyPtrMap::iterator it = getProperties( ).begin();
                it != getProperties( ).end( ); ++it )
        {
            string propId = it->first;
            bool toSkip = false;
            for ( int i = 0; i < skippedCount && !toSkip; ++i )
            {
                toSkip = propId == skippedProps[i];
            }

            if ( !toSkip )
            {
                libcmis::PropertyPtr prop = it->second;
                if ( prop != NULL && prop->getPropertyType( ) != NULL )
                {
                    buf << prop->getPropertyType( )->getDisplayName( ) << "( " << prop->getPropertyType()->getId( ) << " ): " << endl;
                    vector< string > strValues = prop->getStrings( );
                    for ( vector< string >::iterator valueIt = strValues.begin( );
                          valueIt != strValues.end( ); ++valueIt )
                    {
                        buf << "\t" << *valueIt << endl;
                    }
                }
            }
        }

        vector< libcmis::RenditionPtr > renditions = getRenditions( );
        if ( !renditions.empty() )
        {
            buf << "Renditions: " << endl;
            for ( vector< libcmis::RenditionPtr >::iterator it = renditions.begin();
                   it != renditions.end(); ++it )
            {
                buf << ( *it )->toString( ) << endl;
            }
        }

        return buf.str();
    }
    // LCOV_EXCL_STOP

    void Object::toXml( xmlTextWriterPtr writer )
    {
        // Output the properties
        xmlTextWriterStartElement( writer, BAD_CAST( "cmis:properties" ) );
        for ( PropertyPtrMap::iterator it = getProperties( ).begin( );
                it != getProperties( ).end( ); ++it )
        {
            it->second->toXml( writer );
        }
        xmlTextWriterEndElement( writer ); // cmis:properties
    }
}
