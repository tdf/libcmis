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
#ifndef _WS_DOCUMENT_HXX_
#define _WS_DOCUMENT_HXX_

#include "document.hxx"
#include "folder.hxx"
#include "ws-object.hxx"

class WSDocument : public libcmis::Document, public WSObject
{
    public:
        WSDocument( const WSObject& object );
        virtual ~WSDocument( );

        virtual std::vector< libcmis::FolderPtr > getParents( ) throw ( libcmis::Exception );

        virtual boost::shared_ptr< std::istream > getContentStream( ) throw ( libcmis::Exception );

        virtual void setContentStream( boost::shared_ptr< std::ostream > os, std::string contentType,
                                       bool overwrite = true ) throw ( libcmis::Exception );
        
        virtual libcmis::DocumentPtr checkOut( ) throw ( libcmis::Exception );
        virtual void cancelCheckout( ) throw ( libcmis::Exception );
        virtual libcmis::DocumentPtr checkIn( bool isMajor, std::string comment,
                              const std::map< std::string, libcmis::PropertyPtr >& properties,
                              boost::shared_ptr< std::ostream > stream,
                              std::string contentType ) throw ( libcmis::Exception );
};

#endif
