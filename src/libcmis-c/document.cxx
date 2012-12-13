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

#include "document.h"
#include "internals.hxx"

using namespace std;


void libcmis_vector_document_free( libcmis_vector_document_Ptr vector )
{
    delete vector;
}


size_t libcmis_vector_document_size( libcmis_vector_document_Ptr vector )
{
    size_t size = 0;
    if ( vector != NULL )
        size = vector->handle.size( );
    return size;
}


libcmis_DocumentPtr libcmis_vector_document_get( libcmis_vector_document_Ptr vector, size_t i )
{
    libcmis_DocumentPtr item = NULL;
    if ( vector != NULL && i < vector->handle.size( ) )
    {
        libcmis::DocumentPtr handle = vector->handle[i];
        item = new libcmis_document( );
        item->setHandle( handle );
    }
    return item;
}

bool libcmis_is_document( libcmis_ObjectPtr object )
{
    bool isDocument = false;
    if ( object != NULL && object->handle.get( ) != NULL )
    {
        libcmis::DocumentPtr document = boost::dynamic_pointer_cast< libcmis::Document >( object->handle );
        isDocument = document.get( ) != NULL;
    }
    return isDocument;
}


libcmis_DocumentPtr libcmis_document_cast( libcmis_ObjectPtr object )
{
    libcmis_DocumentPtr document = NULL;

    if ( object != NULL && object->handle.get( ) != NULL )
    {
        libcmis::DocumentPtr handle = boost::dynamic_pointer_cast< libcmis::Document >( object->handle );
        if ( handle.get( ) != NULL )
        {
            document = new libcmis_document( );
            document->setHandle( handle );
        }
    }

    return document;
}


void libcmis_document_free( libcmis_DocumentPtr document )
{
    delete document;
}


libcmis_vector_folder_Ptr libcmis_document_getParents( libcmis_DocumentPtr document, libcmis_ErrorPtr error )
{
    libcmis_vector_folder_Ptr parents = NULL;
    if ( document != NULL && document->handle.get( ) != NULL )
    {
        try
        {
            vector< libcmis::FolderPtr > handles = document->handle->getParents( );
            parents = new libcmis_vector_folder( );
            parents->handle = handles;
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }
    return parents;
}


void libcmis_document_getContentStream(
        libcmis_DocumentPtr document,
        libcmis_writeFn writeFn,
        void* userData,
        libcmis_ErrorPtr error )
{
    if ( document != NULL && document->handle.get( ) != NULL )
    {
        try
        {
            boost::shared_ptr< istream > stream = document->handle->getContentStream( );

            stream->seekg( 0 );
            int bufSize = 2048;
            char* buf = new char[ bufSize ];
            while ( !stream->eof( ) )
            {
                stream->read( buf, bufSize );
                size_t read = stream->gcount( );
                writeFn( ( const void * )buf, size_t( 1 ), read, userData );
            }
            delete[] buf;
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
        catch ( const ios_base::failure& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new ios_base::failure( e );
        }
    }
}


void libcmis_document_setContentStream(
        libcmis_DocumentPtr document,
        libcmis_readFn readFn,
        void* userData,
        const char* contentType,
        const char* fileName,
        bool overwrite,
        libcmis_ErrorPtr error )
{
    if ( document != NULL && document->handle.get( ) != NULL )
    {
        try
        {
            boost::shared_ptr< std::ostream > stream( new stringstream( ) );

            size_t bufSize = 2048;
            char* buf = new char[ bufSize ];
            size_t read = 0;
            do
            {
                read = readFn( ( void * )buf, size_t( 1 ), bufSize, userData );
                stream->write( buf, read );
            } while ( read == bufSize );
            delete[] buf;

            document->handle->setContentStream( stream, contentType, fileName, overwrite );
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
        catch ( const ios_base::failure& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new ios_base::failure( e );
        }
    }
}


char* libcmis_document_getContentType( libcmis_DocumentPtr document )
{
    char* value = NULL;
    if ( document != NULL && document->handle.get( ) != NULL )
        value = strdup( document->handle->getContentType( ).c_str( ) );
    return value;
}


char* libcmis_document_getContentFilename( libcmis_DocumentPtr document )
{
    char* value = NULL;
    if ( document != NULL && document->handle.get( ) != NULL )
        value = strdup( document->handle->getContentFilename( ).c_str( ) );
    return value;
}


long libcmis_document_getContentLength( libcmis_DocumentPtr document )
{
    long value = 0;
    if ( document != NULL && document->handle.get( ) != NULL )
        value = document->handle->getContentLength( );
    return value;
}


libcmis_DocumentPtr libcmis_document_checkOut( libcmis_DocumentPtr document, libcmis_ErrorPtr error )
{
    libcmis_DocumentPtr pwc = NULL;
    if ( document != NULL && document->handle.get( ) != NULL )
    {
        try
        {
            libcmis::DocumentPtr handle = document->handle->checkOut( );
            pwc= new libcmis_document( );
            pwc->handle = handle;
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }
    return pwc;
}


void libcmis_document_cancelCheckout( libcmis_DocumentPtr document, libcmis_ErrorPtr error )
{
    if ( document != NULL && document->handle.get( ) != NULL )
    {
        try
        {
            document->handle->cancelCheckout( );
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }
}

libcmis_DocumentPtr libcmis_document_checkIn(
        libcmis_DocumentPtr document,
        bool isMajor,
        const char* comment,
        libcmis_vector_property_Ptr properties,
        libcmis_readFn readFn,
        void* userData,
        const char* contentType,
        const char* filename,
        libcmis_ErrorPtr error )
{
    libcmis_DocumentPtr newVersion = NULL;

    if ( document != NULL && document->handle.get( ) != NULL )
    {
        try
        {
            // Create the ostream
            boost::shared_ptr< std::ostream > stream( new stringstream( ) );

            size_t bufSize = 2048;
            char* buf = new char[ bufSize ];
            size_t read = 0;
            do
            {
                read = readFn( ( void * )buf, size_t( 1 ), bufSize, userData );
                stream->write( buf, read );
            } while ( read == bufSize );
            delete[] buf;

            // Create the property map
            map< string, libcmis::PropertyPtr > propertiesMap;
            if ( properties != NULL )
            {
                for ( vector< libcmis::PropertyPtr >::iterator it = properties->handle.begin( );
                        it != properties->handle.end( ); ++it )
                {
                    string id = ( *it )->getPropertyType( )->getId( );
                    propertiesMap.insert( pair< string, libcmis::PropertyPtr >( id, *it ) );
                }
            }

            libcmis::DocumentPtr handle = document->handle->checkIn( isMajor, comment, propertiesMap,
                    stream, contentType, filename );
            newVersion = new libcmis_document( );
            newVersion->handle = handle;
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
        catch ( const ios_base::failure& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new ios_base::failure( e );
        }
    }
    return newVersion;
}

libcmis_vector_document_Ptr libcmis_document_getAllVersions(
        libcmis_DocumentPtr document,
        libcmis_ErrorPtr error )
{
    libcmis_vector_document_Ptr result = NULL;
    if ( document != NULL && document->handle.get( ) != NULL )
    {
        try
        {
            std::vector< libcmis::DocumentPtr > handles = document->handle->getAllVersions( );
            result = new libcmis_vector_document( );
            result->handle = handles;
        }
        catch ( const libcmis::Exception& e )
        {
            // Set the error handle
            if ( error != NULL )
                error->handle = new libcmis::Exception( e );
        }
    }
    return result;
}
