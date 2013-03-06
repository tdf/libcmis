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

#ifndef GDRIVE_COMMON_HXX_
#define GDRIVE_COMMON_HXX_

//SCOPES
/** View and manage the files and documents in your Google Drive. */
#define DRIVE_SCOPE_FULL std::string("https://www.googleapis.com/auth/drive")

/** View your Google Drive apps. */
#define DRIVE_SCOPE_APPS_READONLY std::string("https://www.googleapis.com/auth/drive.apps.readonly")

/** View and manage Google Drive files that you have opened or created with this app. */
#define  DRIVE_SCOPE_FILE std::string("https://www.googleapis.com/auth/drive.file")

/** View metadata for files and documents in your Google Drive. */
#define DRIVE_SCOPE_METADATA_READONLY std::string("https://www.googleapis.com/auth/drive.metadata.readonly")

/** View the files and documents in your Google Drive. */
#define DRIVE_SCOPE_READONLY std::string("https://www.googleapis.com/auth/drive.readonly")

//URLs
/** url to retrieve token **/
#define DRIVE_TOKEN_URL std::string("https://accounts.google.com/o/oauth2/token")

/** url to retrieve authorization code **/
#define DRIVE_AUTH_URL std::string("https://accounts.google.com/o/oauth2/auth")

/** redirect URI **/
#define DRIVE_REDIRECT_URI std::string("urn:ietf:wg:oauth:2.0:oob")

#endif /* GDRIVE_COMMON_HXX_ */
