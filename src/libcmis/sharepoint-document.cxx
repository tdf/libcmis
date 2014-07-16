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

#include "sharepoint-document.hxx"
#include "sharepoint-session.hxx"
#include "sharepoint-utils.hxx"
#include "json-utils.hxx"

using namespace std;
using namespace libcmis;

SharePointDocument::SharePointDocument( SharePointSession* session ) :
    libcmis::Object( session),
    libcmis::Document( session ),
    SharePointObject( session )
{
}

SharePointDocument::SharePointDocument( SharePointSession* session, Json json, string parentId, string name ) :
    libcmis::Object( session),
    libcmis::Document( session ),
    SharePointObject( session, json, parentId, name )
{
}

SharePointDocument::~SharePointDocument( )
{
}

vector< libcmis::FolderPtr > SharePointDocument::getParents( ) 
    throw ( libcmis::Exception )
{
    vector< libcmis::FolderPtr > parents;

    string parentId = getStringProperty( "cmis:parentId" );
   
    libcmis::ObjectPtr obj = getSession( )->getObject( parentId );
    libcmis::FolderPtr parent = boost::dynamic_pointer_cast< libcmis::Folder >( obj );
    parents.push_back( parent );
    return parents;
}

boost::shared_ptr< istream > SharePointDocument::getContentStream( string /*streamId*/ )
    throw ( libcmis::Exception )
{
    boost::shared_ptr< istream > stream;
    // file uri + /$value
    string streamUrl = getId( ) + "/%24value";
    try
    {
        stream = getSession( )->httpGetRequest( streamUrl )->getStream( );
    }
    catch ( const CurlException& e )
    {
        throw e.getCmisException( );
    }
    return stream;
}

void SharePointDocument::uploadStream( boost::shared_ptr< ostream > /*os*/, 
                                   string /*contentType*/ )
                                throw ( libcmis::Exception )
{
}

void SharePointDocument::setContentStream( boost::shared_ptr< ostream > /*os*/, 
                                       string /*contentType*/, 
                                       string /*fileName*/, 
                                       bool /*overwrite*/ ) 
                                            throw ( libcmis::Exception )
{
}

libcmis::DocumentPtr SharePointDocument::checkOut( ) throw ( libcmis::Exception )
{
    libcmis::DocumentPtr checkout ;
    return checkout;
}

void SharePointDocument::cancelCheckout( ) throw ( libcmis::Exception )
{
}

libcmis::DocumentPtr SharePointDocument::checkIn( 
    bool /*isMajor*/, 
    std::string /*comment*/,
    const PropertyPtrMap& /*properties*/,
    boost::shared_ptr< std::ostream > /*stream*/,
    std::string /*contentType*/, 
    std::string /*fileName*/ ) 
        throw ( libcmis::Exception )
{     
    libcmis::DocumentPtr checkin;
    return checkin;
}

vector< libcmis::DocumentPtr > SharePointDocument::getAllVersions( ) 
    throw ( libcmis::Exception )
{   
    return vector< libcmis::DocumentPtr > ( );
}
