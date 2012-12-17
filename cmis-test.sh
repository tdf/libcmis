#!/bin/sh
# libcmis
# Version: MPL 1.1 / GPLv2+ / LGPLv2+
#
# The contents of this file are subject to the Mozilla Public License Version
# 1.1 (the "License"); you may not use this file except in compliance with
# the License or as specified alternatively below. You may obtain a copy of
# the License at http://www.mozilla.org/MPL/
#
# Software distributed under the License is distributed on an "AS IS" basis,
# WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
# for the specific language governing rights and limitations under the
# License.
#
# Major Contributor(s):
# Copyright (C) 2012 SUSE <cbosdonnat@suse.com>
#
#
# All Rights Reserved.
#
# For minor contributions see the git repository.
#
# Alternatively, the contents of this file may be used under the terms of
# either the GNU General Public License Version 2 or later (the "GPLv2+"), or
# the GNU Lesser General Public License Version 2 or later (the "LGPLv2+"),
# in which case the provisions of the GPLv2+ or the LGPLv2+ are applicable
# instead of those above.

BINDING_URL=$1
USER=$2
PASS=$3
REPO=$4


function cmis_client ( )
{
    repo_opt=
    if test "z$REPO" != "z"; then
        repo_opt=" -r \"$REPO\""
    fi
    src/cmis-client --url "$BINDING_URL" -u "$USER" -p "$PASS"$repo_opt "$@"
}

function get_versionable_type ( )
{
    versionable_type=
    test_type=$1

    # Is test_type versionable?
    versionable=`cmis_client type-by-id $test_type | grep ^Versionable: | cut -d ' ' -f 2`
    if test "z$versionable" == "z1"; then
        versionable_type=$test_type
    else
        # Otherwise, loop over its children
        children=`cmis_client type-by-id $test_type | sed -n -e '/Children type/,/^[^ ]/ p' | grep -e '^\ ' | tr '()' '\t' | cut -f 2`
        for type_id in $children; do
            versionable_type=`get_versionable_type $type_id`
            if test "z$versionable_type" != "z"; then
                break
            fi
        done
    fi

    echo -n $versionable_type
}

#First get the Root Id
ROOT_ID=`cmis_client show-root | grep '^Id:' | cut -d ' ' -f 2`

# Create a test folder
test_folder_name="test-$$"
test_folder_id=`cmis_client create-folder $ROOT_ID $test_folder_name | grep '^Id:' | cut -d ' ' -f 2`

# Get a Versionable document type, not alway cmis:document for all servers
versionable_type=`get_versionable_type "cmis:document"`

# Create a versionable document
file_path=NEWS
file_mime=`file --mime-type $file_path | cut -d ' ' -f 2`
doc1_id=`cmis_client --object-type $versionable_type --input-file $file_path --input-type $file_mime create-document $test_folder_id doc_1 | grep '^Id:' | cut -d ' ' -f 2`

# Checkout the document
doc1_pwc=`cmis_client checkout $doc1_id | grep '^Id:' | cut -d ' ' -f 2`

# TODO Checkin the document


# Cleanup the test folder to remove all traces of the tests
cmis_client delete $test_folder_id
