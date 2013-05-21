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
using libcmis::PropertyPtrMap;

bool isOutOfMemory = false;

/// Ignore all tests results depending on this when running in valgrind
void * operator new ( size_t requestedSize ) throw ( bad_alloc )
{
    if ( isOutOfMemory )
    {
        throw bad_alloc( );
    }

    return malloc( requestedSize );
}

void operator delete ( void* ptr ) throw ( )
{
    free( ptr );
}


namespace dummies
{
    Session::Session( )
    {
    }

    Session::~Session( )
    {
    }

    libcmis::RepositoryPtr Session::getRepository( ) throw ( libcmis::Exception )
    {
        libcmis::RepositoryPtr repo( new Repository( ) );
        return repo;
    }

    
    bool Session::setRepository( std::string )
    {
        return true;
    }

    vector< libcmis::RepositoryPtr > Session::getRepositories( )
    {
        vector< libcmis::RepositoryPtr > repos;
        libcmis::RepositoryPtr repo1( new Repository( ) );
        libcmis::RepositoryPtr repo2( new Repository( ) );
        repos.push_back( repo1 );
        repos.push_back( repo2 );
        return repos;
    }

    libcmis::FolderPtr Session::getRootFolder() throw ( libcmis::Exception )
    {
        libcmis::FolderPtr root( new Folder( true, false ) );
        return root;
    }

    libcmis::ObjectPtr Session::getObject( string id ) throw ( libcmis::Exception )
    {
        return getFolder( id );
    }

    libcmis::ObjectPtr Session::getObjectByPath( string path ) throw ( libcmis::Exception )
    {
        return getFolder( path );
    }

    libcmis::FolderPtr Session::getFolder( string ) throw ( libcmis::Exception )
    {
        libcmis::FolderPtr result( new Folder( false, false ) );
        return result;
    }

    libcmis::ObjectTypePtr Session::getType( string ) throw ( libcmis::Exception )
    {
        libcmis::ObjectTypePtr type( new ObjectType( true, false ) );
        return type;
    }

    std::string Session::getRefreshToken( ) throw ( libcmis::Exception )
    {
        return string( );
    }

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
        m_typeId( ),
        m_childrenIds( ),
        m_triggersFaults( false )
    {
    }

    ObjectType::ObjectType( bool rootType, bool triggersFaults ) :
        libcmis::ObjectType( ),
        m_typeId( ),
        m_childrenIds( ),
        m_triggersFaults( triggersFaults )
    {
        if ( rootType )
            m_typeId = "RootType";
        else
        {
            m_typeId = "ObjectType";
            m_parentTypeId = "ParentType";
            m_childrenIds.push_back( "ChildType1" );
            m_childrenIds.push_back( "ChildType2" );
        }

        m_baseTypeId = "RootType";
        libcmis::PropertyTypePtr propType1( new PropertyType( "Property1", "string" ) );
        m_propertiesTypes.insert( pair< string, libcmis::PropertyTypePtr >( propType1->getId( ), propType1 ) );
        libcmis::PropertyTypePtr propType2( new PropertyType( "Property2", "string" ) );
        m_propertiesTypes.insert( pair< string, libcmis::PropertyTypePtr >( propType2->getId( ), propType2 ) );
        libcmis::PropertyTypePtr propType3( new PropertyType( "Property3", "string" ) );
        m_propertiesTypes.insert( pair< string, libcmis::PropertyTypePtr >( propType3->getId( ), propType3 ) );

        initMembers( );
    }

    void ObjectType::initMembers( )
    {
        
        m_id = m_typeId + "::Id";
        m_localName = m_typeId + "::LocalName";
        m_localNamespace = m_typeId + "::LocalNamespace";
        m_displayName = m_typeId + "::DisplayName";
        m_queryName = m_typeId + "::QueryName";
        m_description = m_typeId + "::Description";
            
        m_creatable = true;
        m_fileable = true;
        m_queryable = true;
        m_fulltextIndexed = true;
        m_includedInSupertypeQuery = true;
        m_controllablePolicy = true;
        m_controllableAcl = true;
        m_versionable = true;
        m_contentStreamAllowed = libcmis::ObjectType::Allowed;
    }

    ObjectType::~ObjectType( )
    {
    }
    
    libcmis::ObjectTypePtr  ObjectType::getParentType( )
        throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        ObjectType* parent = NULL;
        if ( !m_parentTypeId.empty( ) )
        {
            parent = new ObjectType( );
            parent->m_typeId = m_parentTypeId;
            parent->m_parentTypeId = m_baseTypeId;
            parent->m_baseTypeId = m_baseTypeId;
            parent->m_childrenIds.push_back( m_id );
            parent->m_triggersFaults = m_triggersFaults;
            parent->m_propertiesTypes = m_propertiesTypes;

            parent->initMembers( );
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
        if ( m_typeId != m_baseTypeId )
        {
            base = new ObjectType( );
            base->m_typeId = m_baseTypeId;
            base->m_baseTypeId = m_baseTypeId;
            base->m_childrenIds.push_back( m_id );
            base->m_triggersFaults = m_triggersFaults;
            base->m_propertiesTypes = m_propertiesTypes;

            base->initMembers( );
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
            child->m_typeId = *it;
            child->m_parentTypeId = m_typeId;
            child->m_baseTypeId = m_baseTypeId;
            child->m_triggersFaults = m_triggersFaults;
            child->m_propertiesTypes = m_propertiesTypes;

            child->initMembers( );

            libcmis::ObjectTypePtr result( child );
            children.push_back( result );
        } 

        return children; 
    }
    
    string ObjectType::toString( )
    {
        return m_typeId + "::toString";
    }

    Object::Object( bool triggersFaults, string type ):
        libcmis::Object( NULL ),
        m_type( type ),
        m_triggersFaults( triggersFaults )
    {
        libcmis::PropertyTypePtr propertyType( new PropertyType( "Property1", "string" ) );
        vector< string > values;
        values.push_back( "Value1" );
        libcmis::PropertyPtr property( new libcmis::Property( propertyType, values ) );
        m_properties.insert( pair< string, libcmis::PropertyPtr >( propertyType->getId( ), property ) );
    }

    string Object::getId( )
    {
        return m_type + "::Id";
    }

    string Object::getName( )
    {
        return m_type + "::Name";
    }

    vector< string > Object::getPaths( )
    {
        vector< string > paths;
        paths.push_back( string( "/Path1/" ) );
        paths.push_back( string( "/Path2/" ) );

        return paths;
    }

    string Object::getBaseType( )
    {
        return m_type + "::BaseType";
    }

    string Object::getType( )
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

    libcmis::ObjectPtr Object::updateProperties(
           const PropertyPtrMap& ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        time( &m_refreshTimestamp );
        libcmis::ObjectPtr result( new Object( false ) );
        return result;
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

        time( &m_refreshTimestamp );
    }

    void Object::remove( bool ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        time( &m_refreshTimestamp );
    }

    void Object::move( libcmis::FolderPtr, libcmis::FolderPtr ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        time( &m_refreshTimestamp );
    }

    void Object::toXml( xmlTextWriterPtr )
    {
    }

    Folder::Folder( bool isRoot, bool triggersFaults ) :
        libcmis::Object( NULL ),
        libcmis::Folder( NULL ),
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

    libcmis::FolderPtr Folder::createFolder( const PropertyPtrMap& ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        libcmis::FolderPtr created( new Folder( true, m_triggersFaults ) );
        return created;
    }

    libcmis::DocumentPtr Folder::createDocument( const PropertyPtrMap& properties,
                            boost::shared_ptr< ostream > os, string contentType, string filename ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        dummies::Document* document = new dummies::Document( true, false );

        PropertyPtrMap propertiesCopy( properties );
        document->getProperties( ).swap( propertiesCopy );
        document->setContentStream( os, contentType, filename );

        libcmis::DocumentPtr created( document );
        return created;
    }

    vector< string > Folder::removeTree( bool, libcmis::UnfileObjects::Type,
                            bool ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        time( &m_refreshTimestamp );

        vector< string > failed;
        failed.push_back( "failed 1" );
        return failed;
    }
            
    Document::Document( bool isFiled, bool triggersFaults ) :
        libcmis::Object( NULL ),
        libcmis::Document( NULL ),
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
    
    boost::shared_ptr< istream > Document::getContentStream( string /*streamId*/ ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        bool oldOutOfMem = isOutOfMemory;
        isOutOfMemory = false;
        boost::shared_ptr< istream > stream( new stringstream( m_contentString ) );
        isOutOfMemory = oldOutOfMem;
        return stream;
    }

    void Document::setContentStream( boost::shared_ptr< ostream > os, string, string, bool ) throw ( libcmis::Exception )
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

        time( &m_refreshTimestamp );
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

        time( &m_refreshTimestamp );

        libcmis::DocumentPtr result( new Document( true, m_triggersFaults ) );
        return result;
    }

    void Document::cancelCheckout( ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        time( &m_refreshTimestamp );
    }

    libcmis::DocumentPtr Document::checkIn( bool, string, const PropertyPtrMap& properties,
                  boost::shared_ptr< ostream > os, string contentType, string filename ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        m_properties = properties;
        setContentStream( os, contentType, filename );
        time( &m_refreshTimestamp );

        return libcmis::DocumentPtr( new Document( true, false ) );
    }

    vector< libcmis::DocumentPtr > Document::getAllVersions( ) throw ( libcmis::Exception )
    {
        if ( m_triggersFaults )
            throw libcmis::Exception( "Fault triggered" );

        vector< libcmis::DocumentPtr > versions;

        libcmis::DocumentPtr version1( new Document( true, false ) );
        versions.push_back( version1 );
        libcmis::DocumentPtr version2( new Document( true, false ) );
        versions.push_back( version2 );

        return versions;
    }
}
