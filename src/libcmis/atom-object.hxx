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

#include <map>
#include <ostream>

#include "object.hxx"

class AtomPubSession;

class AtomLink
{
    private:
        std::string m_rel;
        std::string m_type;
        std::string m_id;
        std::string m_href;
        std::map< std::string, std::string > m_others;

    public:
        AtomLink( xmlNodePtr node );

        std::string getRel( ) { return m_rel; }
        std::string getType( ) { return m_type; }
        std::string getId( ) { return m_id; }
        bool hasId( ) { return !m_id.empty( ); }
        std::string getHref( ) { return m_href; }
        std::map< std::string, std::string >& getOthers( ) { return m_others; }
};

class AtomObject : public virtual libcmis::Object
{
    private:

        std::vector< AtomLink > m_links;

    public:
        AtomObject( AtomPubSession* session );
        AtomObject( const AtomObject& copy );
        ~AtomObject( );

        AtomObject& operator=( const AtomObject& copy );

        // Overridden methods from libcmis::Object
        virtual libcmis::ObjectPtr updateProperties(
                    const std::map< std::string, libcmis::PropertyPtr >& properties );

        virtual libcmis::AllowableActionsPtr getAllowableActions( );

        /** Reload the data from the server.
              */
        virtual void refresh( ) { refreshImpl( NULL ); }

        virtual void remove( bool allVersion = true );

        virtual void move( boost::shared_ptr< libcmis::Folder > source, boost::shared_ptr< libcmis::Folder > destination );

        static void writeAtomEntry( xmlTextWriterPtr writer,
                const std::map< std::string, libcmis::PropertyPtr >& properties,
                boost::shared_ptr< std::ostream > os, std::string contentType );

    protected:

        std::string getInfosUrl( );
        virtual void refreshImpl( xmlDocPtr doc );
        virtual void extractInfos( xmlDocPtr doc );

        AtomPubSession* getSession( );

        /** Get the atom link corresponding to the given relation and type or NULL
            if no link matched those criteria.

            \param rel the relation to match
            \param type the type to match or the empty string to match all types.
          */
        AtomLink* getLink( std::string rel, std::string type );
};

#endif
