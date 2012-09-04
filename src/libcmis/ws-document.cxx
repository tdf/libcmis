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


WSDocument::WSDocument( const WSObject& object ) :
    libcmis::Object( object ),
    libcmis::Document( const_cast< WSObject& >( object ).getSession( ) ),
    WSObject( object )
{
}

WSDocument::~WSDocument( )
{
}

vector< libcmis::FolderPtr > WSDocument::getParents( ) throw ( libcmis::Exception )
{
    string repoId = getSession( )->getRepositoryId( );
    return getSession( )->getNavigationService( ).getObjectParents( repoId, getId( ) );
}

boost::shared_ptr< istream > WSDocument::getContentStream( ) throw ( libcmis::Exception )
{
    string repoId = getSession( )->getRepositoryId( );
    return getSession( )->getObjectService( ).getContentStream( repoId, getId( ) );
}

void WSDocument::setContentStream( boost::shared_ptr< ostream > os, string contentType,
                               bool overwrite ) throw ( libcmis::Exception )
{
    // TODO Implement me
}

libcmis::DocumentPtr WSDocument::checkOut( ) throw ( libcmis::Exception )
{
    libcmis::DocumentPtr empty;

    // TODO Implement me

    return empty;
}

void WSDocument::cancelCheckout( ) throw ( libcmis::Exception )
{
    // TODO Implement me
}

void WSDocument::checkIn( bool isMajor, string comment,
                          const map< string, libcmis::PropertyPtr >& properties,
                          boost::shared_ptr< ostream > stream,
                          string contentType ) throw ( libcmis::Exception )
{
    // TODO Implement me
}
