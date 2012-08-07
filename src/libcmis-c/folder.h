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
#ifndef _LIBCMIS_FOLDER_H_
#define _LIBCMIS_FOLDER_H_

#include "error.h"
#include "object.h"

typedef struct libcmis_folder* libcmis_FolderPtr;
    
enum libcmis_folder_UnfileObjects
{
    Unfile,
    DeleteSingleFiled,
    Delete
};

void libcmis_folder_free( libcmis_FolderPtr folder );

libcmis_FolderPtr libcmis_folder_getParent( libcmis_FolderPtr folder, libcmis_ErrorPtr error );
libcmis_ObjectPtr* libcmis_folder_getChildren( libcmis_FolderPtr folder, libcmis_ErrorPtr error );
char* libcmis_folder_getPath( libcmis_FolderPtr folder );

bool libcmis_folder_isRootFolder( libcmis_FolderPtr folder );

libcmis_FolderPtr libcmis_folder_createFolder(
        libcmis_FolderPtr folder,
        libcmis_PropertyPtr* properties,
        libcmis_ErrorPtr error );


/* TODO libcmis_DocumentPtr libcmis_folder_createDocument(*/
/*         libcmis_FolderPtr folder,*/
/*         libcmis_PropertyPtr* properties,*/
/*         readFn,*/
/*         char* contentType,*/
/*         libcmis_ErrorPtr );*/

void libcmis_folder_removeTree( libcmis_FolderPtr folder,
        bool allVersion,
        libcmis_folder_UnfileObjects unfile,
        bool continueOnError,
        libcmis_ErrorPtr error );

#endif
