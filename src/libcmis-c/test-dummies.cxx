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

#include "test-dummies.hxx"

using namespace std;

namespace dummies
{
    Repository::Repository( ) :
        libcmis::Repository( )
    {
        m_id = string( "Repository::Id" );
        m_name = string( "Repository::Name" );
        m_description = string( "Repository::Description" );
        m_vendorName = string( "Repository::VendorName" );
        m_productName = string( "Repository::ProductName" );
        m_productVersion = string( "Repository::ProductVersion" );
        m_rootId = string( "Repository::RootId" );
        m_cmisVersionSupported = string( "Repository::CmisVersionSupported" );
        m_thinClientUri.reset( new string( "Repository::ThinClientUri" ) );
        m_principalAnonymous.reset( new string( "Repository::PrincipalAnonymous" ) );
        m_principalAnyone.reset( new string( "Repository::PrincipalAnyone" ) );
    }

    Repository::~Repository( )
    {
    }

    PropertyType::PropertyType( string id, string xmlType ) :
        libcmis::PropertyType( )
    {
        setId( id );
        setLocalName( string( "PropertyType::LocalName" ) );
        setLocalNamespace( string( "PropertyType::LocalNamespace" ) );
        setDisplayName( string( "PropertyType::DisplayName" ) );
        setQueryName( string( "PropertyType::QueryName" ) );
        setTypeFromXml( xmlType );

        // Setting true for the tests to see a difference with 
        // the default false result of the tested functions
        setMultiValued( true );
        setUpdatable( true );
        setInherited( true );
        setRequired( true );
        setQueryable( true );
        setOrderable( true );
        setOpenChoice( true );
    }

    PropertyType::~PropertyType( )
    {
    }
    
    AllowableActions::AllowableActions( ) :
        libcmis::AllowableActions( )
    {
        m_states.insert( pair< libcmis::ObjectAction::Type, bool >( libcmis::ObjectAction::GetProperties, true ) );
        m_states.insert( pair< libcmis::ObjectAction::Type, bool >( libcmis::ObjectAction::GetFolderParent, false ) );
    }

    AllowableActions::~AllowableActions( )
    {
    }

    ObjectType::ObjectType( ) :
        libcmis::ObjectType( ),
        m_id( ),
        m_parentId( ),
        m_baseId( ),
        m_childrenIds( ),
        m_triggersFaults( false ),
        m_propertyTypes( )
    {
    }

    ObjectType::ObjectType( bool rootType, bool triggersFaults ) :
        libcmis::ObjectType( ),
        m_id( ),
        m_parentId( ),
        m_baseId( ),
        m_childrenIds( ),
        m_triggersFaults( triggersFaults ),
        m_propertyTypes( )
    {
        if ( rootType )
            m_id = "RootType";
        else
        {
            m_id = "ObjectType";
            m_parentId = "ParentType";
            m_childrenIds.push_back( "ChildType1" );
            m_childrenIds.push_back( "ChildType2" );
        }

        m_baseId = "RootType";
        libcmis::PropertyTypePtr propType1( new PropertyType( "Property1", "string" ) );
        m_propertyTypes.insert( pair< string, libcmis::PropertyTypePtr >( propType1->getId( ), propType1 ) );
        libcmis::PropertyTypePtr propType2( new PropertyType( "Property2", "string" ) );
        m_propertyTypes.insert( pair< string, libcmis::PropertyTypePtr >( propType2->getId( ), propType2 ) );
        libcmis::PropertyTypePtr propType3( new PropertyType( "Property3", "string" ) );
        m_propertyTypes.insert( pair< string, libcmis::PropertyTypePtr >( propType3->getId( ), propType3 ) );
    }

    ObjectType::~ObjectType( )
    {
    }
    
    string ObjectType::getId( )
    {
        return m_id + "::Id";
    }

    string ObjectType::getLocalName( )
    {
        return m_id + "::LocalName";
    }

    string ObjectType::getLocalNamespace( )
    {
        return m_id + "::LocalNamespace";
    }

    string ObjectType::getDisplayName( )
    {
        return m_id + "::DisplayName";
    }

    string ObjectType::getQueryName( )
    {
        return m_id + "::QueryName";
    }

    string ObjectType::getDescription( )
    {
        return m_id + "::Description";
    }

    libcmis::ObjectTypePtr  ObjectType::getParentType( )
        throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        ObjectType* parent = NULL;
        if ( !m_parentId.empty( ) )
        {
            parent = new ObjectType( );
            parent->m_id = m_parentId;
            parent->m_parentId = m_baseId;
            parent->m_baseId = m_baseId;
            parent->m_childrenIds.push_back( m_id );
            parent->m_triggersFaults = m_triggersFaults;
            parent->m_propertyTypes = m_propertyTypes;
        }

        libcmis::ObjectTypePtr result( parent );
        return result;
    }

    libcmis::ObjectTypePtr  ObjectType::getBaseType( )
        throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        ObjectType* base = this;
        if ( m_id != m_baseId )
        {
            base = new ObjectType( );
            base->m_id = m_baseId;
            base->m_baseId = m_baseId;
            base->m_childrenIds.push_back( m_id );
            base->m_triggersFaults = m_triggersFaults;
            base->m_propertyTypes = m_propertyTypes;
        }

        libcmis::ObjectTypePtr result( base );
        return result;
    }

    vector< libcmis::ObjectTypePtr > ObjectType::getChildren( )
        throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        vector< libcmis::ObjectTypePtr > children;

        for ( vector< string >::iterator it = m_childrenIds.begin( ); it != m_childrenIds.end( ); ++it )
        {
            ObjectType* child = new ObjectType( );
            child->m_id = *it;
            child->m_parentId = m_id;
            child->m_baseId = m_baseId;
            child->m_triggersFaults = m_triggersFaults;
            child->m_propertyTypes = m_propertyTypes;
            libcmis::ObjectTypePtr result( child );
            children.push_back( result );
        } 

        return children; 
    }
    
    map< string, libcmis::PropertyTypePtr >& ObjectType::getPropertiesTypes( )
    {
        return m_propertyTypes;
    }

    string ObjectType::toString( )
    {
        return m_id + "::toString";
    }

    Object::Object( bool triggersFaults, string type ):
        libcmis::Object( ),
        m_type( type ),
        m_triggersFaults( triggersFaults ),
        m_timestamp( 0 ),
        m_properties( )
    {
        libcmis::PropertyTypePtr propertyType( new PropertyType( "Property1", "string" ) );
        vector< string > values;
        values.push_back( "Value1" );
        libcmis::PropertyPtr property( new libcmis::Property( propertyType, values ) );
        m_properties.insert( pair< string, libcmis::PropertyPtr >( propertyType->getId( ), property ) );
    }

    std::string Object::getId( )
    {
        return m_type + "::Id";
    }

    std::string Object::getName( )
    {
        return m_type + "::Name";
    }

    std::vector< std::string > Object::getPaths( )
    {
        vector< string > paths;
        paths.push_back( string( "/Path1/" ) );
        paths.push_back( string( "/Path2/" ) );

        return paths;
    }

    std::string Object::getBaseType( )
    {
        return m_type + "::BaseType";
    }

    std::string Object::getType( )
    {
        return m_type + "::Type";
    }

    boost::posix_time::ptime Object::getCreationDate( )
    {
        boost::posix_time::ptime now( boost::posix_time::second_clock::local_time( ) );
        return now;
    }

    boost::posix_time::ptime Object::getLastModificationDate( )
    {
        boost::posix_time::ptime now( boost::posix_time::second_clock::local_time( ) );
        return now;
    }

    std::map< std::string, libcmis::PropertyPtr >& Object::getProperties( )
    {
        return m_properties;
    }

    void Object::updateProperties( ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        time( &m_timestamp );
    }

    libcmis::ObjectTypePtr Object::getTypeDescription( )
    {
        libcmis::ObjectTypePtr type( new ObjectType( false, m_triggersFaults ) );
        return type;
    }

    libcmis::AllowableActionsPtr Object::getAllowableActions( )
    {
        libcmis::AllowableActionsPtr allowableActions( new AllowableActions( ) );
        return allowableActions;
    }

    void Object::refresh( ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        time( &m_timestamp );
    }

    void Object::remove( bool ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        time( &m_timestamp );
    }

    void Object::toXml( xmlTextWriterPtr )
    {
    }

    Folder::Folder( bool isRoot, bool triggersFaults ) :
        libcmis::Folder( ),
        dummies::Object( triggersFaults, "Folder" ),
        m_isRoot( isRoot )
    {
    }

    libcmis::FolderPtr Folder::getFolderParent( ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        libcmis::FolderPtr parent;

        if ( !m_isRoot )
            parent.reset( new Folder( true, m_triggersFaults ) );

        return parent;
    }

    vector< libcmis::ObjectPtr > Folder::getChildren( ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        vector< libcmis::ObjectPtr > children;

        libcmis::ObjectPtr child1( new Object( m_triggersFaults ) );
        children.push_back( child1 );
        libcmis::ObjectPtr child2( new Object( m_triggersFaults ) );
        children.push_back( child2 );

        return children;
    }

    string Folder::getPath( )
    {
        return string( "/Path/" );
    }

    bool Folder::isRootFolder( )
    {
        return m_isRoot;
    }

    libcmis::FolderPtr Folder::createFolder( map< string, libcmis::PropertyPtr >& ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        libcmis::FolderPtr created( new Folder( true, m_triggersFaults ) );
        return created;
    }

    libcmis::DocumentPtr Folder::createDocument( map< std::string, libcmis::PropertyPtr >& properties,
                            boost::shared_ptr< ostream > os, std::string contentType ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        libcmis::DocumentPtr created;

        // TODO Implement me

        return created;
    }

    void Folder::removeTree( bool allVersion, libcmis::UnfileObjects::Type unfile,
                            bool continueOnError ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        time( &m_timestamp );
    }
            
    Document::Document( bool isFiled, bool triggersFaults ) :
        libcmis::Document( ),
        dummies::Object( triggersFaults, "Document" ),
        m_isFiled( isFiled ),
        m_contentString( "Document::ContentStream" )
    {
    }
            
    vector< libcmis::FolderPtr > Document::getParents( ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        vector< libcmis::FolderPtr > parents;
        if ( m_isFiled )
        {
            libcmis::FolderPtr parent1( new Folder( true, m_triggersFaults ) );
            parents.push_back( parent1 );
            libcmis::FolderPtr parent2( new Folder( false, m_triggersFaults ) );
            parents.push_back( parent2 );
        }

        return parents;
    }
    
    boost::shared_ptr< istream > Document::getContentStream( ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        boost::shared_ptr< istream > stream( new stringstream( m_contentString ) );
        return stream;
    }

    void Document::setContentStream( boost::shared_ptr< ostream > os, string, bool overwrite ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        istream is( os->rdbuf( ) );
        stringstream out;
        is.seekg( 0 );
        int bufSize = 2048;
        char* buf = new char[ bufSize ];
        while ( !is.eof( ) )
        {
            is.read( buf, bufSize );
            size_t read = is.gcount( );
            out.write( buf, read );
        }
        delete[] buf;

        m_contentString = out.str( );

        time( &m_timestamp );
    }

    string Document::getContentType( )
    {
        return "Document::ContentType";
    }
    
    string Document::getContentFilename( )
    {
        return "Document::ContentFilename";
    }

    long Document::getContentLength( )
    {
        return long( 12345 );
    }

    libcmis::DocumentPtr Document::checkOut( ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        time( &m_timestamp );

        libcmis::DocumentPtr result( new Document( true, m_triggersFaults ) );
        return result;
    }

    void Document::cancelCheckout( ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        time( &m_timestamp );
    }

    void Document::checkIn( bool, string, map< string, libcmis::PropertyPtr >& properties,
                  boost::shared_ptr< ostream > os, string contentType ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        m_properties = properties;
        setContentStream( os, contentType );
        time( &m_timestamp );
    }
}
