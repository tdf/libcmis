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

#include "gdrive-document.hxx"
#include "gdrive-session.hxx"

using namespace std;

GDriveDocument::GDriveDocument( GDriveSession* session ) :
    libcmis::Object( session),
    libcmis::Document( session ),
    GDriveObject( session )
{
}

GDriveDocument::GDriveDocument( GDriveSession* session, Json json ) :
    libcmis::Object( session),
    libcmis::Document( session ),
    GDriveObject( session, json )
{
}

GDriveDocument::~GDriveDocument( )
{
}


vector< libcmis::FolderPtr > GDriveDocument::getParents( ) throw ( libcmis::Exception )
{
    //TODO implementation
    vector< libcmis::FolderPtr > result;
    return result;
}

boost::shared_ptr< istream > GDriveDocument::getContentStream( ) throw ( libcmis::Exception )
{
    //TODO implementation
    boost::shared_ptr< istream > result;
    return result;
}

void GDriveDocument::setContentStream( boost::shared_ptr< ostream > /*os*/, 
                                       string /*contentType*/, 
                                       string /*fileName*/, 
                                       bool /*overwrite*/ ) 
                                            throw ( libcmis::Exception )
{
    //TODO implemenetation
}

libcmis::DocumentPtr GDriveDocument::checkOut( ) throw ( libcmis::Exception )
{
    libcmis::DocumentPtr ptr;
    // TODO implementation
    return ptr;
}

void GDriveDocument::cancelCheckout( ) throw ( libcmis::Exception )
{
    
}

libcmis::DocumentPtr GDriveDocument::checkIn( bool /*isMajor*/, 
                                              std::string /*comment*/,
                                              const std::map< std::string, 
                                                libcmis::PropertyPtr >& /*properties*/,
                                              boost::shared_ptr< std::ostream > /*stream*/,
                                              std::string /*contentType*/, 
                                              std::string /*fileName*/ ) 
                                                throw ( libcmis::Exception )
{
    libcmis::DocumentPtr ptr;
    // TODO implementation
    return ptr;
}
        

vector< libcmis::DocumentPtr > GDriveDocument::getAllVersions( ) throw ( libcmis::Exception )
{
    //TODO implementation
    vector< libcmis::DocumentPtr > result;
    return result;
}
