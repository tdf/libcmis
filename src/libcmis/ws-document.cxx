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

#include "ws-document.hxx"

using namespace std;
using libcmis::PropertyPtrMap;

WSDocument::WSDocument( const WSObject& object ) :
    libcmis::Object( object ),
    WSObject( object )
{
}

WSDocument::~WSDocument( )
{
}

vector< libcmis::FolderPtr > WSDocument::getParents( )
{
    string repoId = getSession( )->getRepositoryId( );
    return getSession( )->getNavigationService( ).getObjectParents( repoId, getId( ) );
}

boost::shared_ptr< istream > WSDocument::getContentStream( std::string /* streamId */ ) 
{
    string repoId = getSession( )->getRepositoryId( );
    return getSession( )->getObjectService( ).getContentStream( repoId, getId( ) );
}

void WSDocument::setContentStream( boost::shared_ptr< ostream > os, string contentType,
                               string fileName, bool overwrite )
{
    string repoId = getSession( )->getRepositoryId( );
    getSession( )->getObjectService( ).setContentStream( repoId, getId( ),
            overwrite, getChangeToken( ), os, contentType, fileName );

    refresh( );
}

libcmis::DocumentPtr WSDocument::checkOut( )
{
    string repoId = getSession( )->getRepositoryId( );
    return getSession( )->getVersioningService( ).checkOut( repoId, getId( ) );
}

void WSDocument::cancelCheckout( )
{
    string repoId = getSession( )->getRepositoryId( );
    getSession( )->getVersioningService( ).cancelCheckOut( repoId, getId( ) );
}

libcmis::DocumentPtr WSDocument::checkIn( bool isMajor, string comment,
                          const PropertyPtrMap& properties,
                          boost::shared_ptr< ostream > stream,
                          string contentType, string fileName )
{
    string repoId = getSession( )->getRepositoryId( );
    libcmis::DocumentPtr newVersion;

    // Try the normal request first, but if we have a server error, we may want to resend it
    // without the stream as SharePoint wants no stream in the request, but gets the one from
    // the PWC see the following discussion:
    // http://social.technet.microsoft.com/Forums/eu/sharepoint2010programming/thread/b30e4d82-5b7e-4ceb-b9ad-c6f0d4c59d11
    bool tryNoStream = false;
    try
    {
        newVersion = getSession( )->getVersioningService( ).checkIn( repoId, getId( ),
                isMajor, properties, stream, contentType, fileName, comment );
    }
    catch ( const libcmis::Exception& e )
    {
        string spError( "Object reference not set to an instance of an object" ); 
        if ( string( e.what( ) ).find( spError ) != string::npos )
            tryNoStream = true;
        else
            throw;
    }

    if ( tryNoStream )
    {
        // Set the content stream first
        setContentStream( stream, contentType, fileName );

        // Then check-in
        boost::shared_ptr< ostream > nostream;
        newVersion = getSession( )->getVersioningService( ).checkIn( repoId, getId( ),
                isMajor, properties, nostream, string( ), string( ), comment );
    }

    if ( newVersion->getId( ) == getId( ) )
        refresh( );

    return newVersion;
}

vector< libcmis::DocumentPtr > WSDocument::getAllVersions( )
{
    vector< libcmis::DocumentPtr > versions;
    string repoId = getSession( )->getRepositoryId( );
    PropertyPtrMap::const_iterator it = getProperties( ).find( string( "cmis:versionSeriesId" ) );
    if ( it != getProperties( ).end( ) && !it->second->getStrings( ).empty( ) )
    {
        string versionSeries = it->second->getStrings( ).front( );
        versions = getSession( )->getVersioningService( ).getAllVersions( repoId, versionSeries );
    }
    return versions;
}
