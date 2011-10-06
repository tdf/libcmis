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
#ifndef _ATOM_OBJECT_HXX_
#define _ATOM_OBJECT_HXX_

#include <libxml/tree.h>

#include "object.hxx"

class AtomPubSession;

class AtomObject : public virtual libcmis::Object
{
    private:
        AtomPubSession* m_session;

        std::string m_infosUrl;

        std::string m_id;
        std::string m_name;
        std::string m_baseType;
        std::string m_type;

        std::string m_createdBy;
        boost::posix_time::ptime m_creationDate;
        std::string m_lastModifiedBy;
        boost::posix_time::ptime m_lastModificationDate;

        std::string m_changeToken;

    public:
        AtomObject( AtomPubSession* session, std::string url );
        AtomObject( const AtomObject& copy );
        ~AtomObject( );

        AtomObject& operator=( const AtomObject& copy );

        // Overridden methods from libcmis::Object
        virtual std::string getId( ); 
        virtual std::string getName( );
        
        virtual std::string getBaseType( );
        virtual std::string getType( );
            
        virtual std::string getCreatedBy( );
        virtual boost::posix_time::ptime getCreationDate( );
        virtual std::string getLastModifiedBy( );
        virtual boost::posix_time::ptime getLastModificationDate( );

        virtual std::string getChangeToken( );

        virtual std::string toString( );

    protected:

        std::string& getInfosUrl( ) { return m_infosUrl; }
        virtual void extractInfos( xmlDocPtr doc );

        AtomPubSession* getSession( ) { return m_session; }
};

#endif
