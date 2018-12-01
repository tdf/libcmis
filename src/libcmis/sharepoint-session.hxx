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
#ifndef _SHAREPOINT_SESSION_HXX_
#define _SHAREPOINT_SESSION_HXX_

#include "base-session.hxx"
#include "repository.hxx"
#include "json-utils.hxx"

class SharePointSession : public BaseSession
{
    public:
        SharePointSession( std::string baseUrl,
                       std::string username, 
                       std::string password,
                       bool verbose = false );

        SharePointSession( std::string baseUrl,
                        const HttpSession& httpSession,
                        libcmis::HttpResponsePtr response );

        SharePointSession( const SharePointSession& copy );

        ~SharePointSession ( );

        virtual libcmis::RepositoryPtr getRepository( );

        virtual bool setRepository( std::string );

        virtual libcmis::ObjectPtr getObject( std::string id );

        virtual libcmis::ObjectPtr getObjectByPath( std::string path );

        virtual libcmis::ObjectTypePtr getType( std::string id );
        
        virtual std::vector< libcmis::ObjectTypePtr > getBaseTypes( );

       libcmis::ObjectPtr getObjectFromJson( Json& jsonRes,
                                             std::string parentId = std::string( ) );

        Json getJsonFromUrl( std::string url );

        void fetchDigestCode( );

        void httpRunRequest( std::string url,
                             std::vector< std::string > headers,
                             bool redirect );

        libcmis::HttpResponsePtr httpPutRequest( std::string url,
                                                 std::istream& is,
                                                 std::vector< std::string > headers );
        libcmis::HttpResponsePtr httpPostRequest( const std::string& url,
                                                  std::istream& is,
                                                  const std::string& contentType,
                                                  bool redirect = true );
        void httpDeleteRequest( std::string url );


    private:
        SharePointSession( );
        void fetchDigestCodeCurl( );
        std::string m_digestCode;
};

#endif /* _SHAREPONT_SESSION_HXX_ */
