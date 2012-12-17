#!/bin/bash

function usage ( )
{
    echo "$0 --url http://binding/url [-u|--username user -p|--password pass] [-r|--repository repo] path/to/local/folder /path/to/remote/folder"
}


# Utility function to call cmis-client
function cmis_client ( )
{
    repo_opt=
    if test "z$REPO" != "z"; then
        repo_opt=" -r \"$REPO\""
    fi
    $CMIS_CLIENT --url "$BINDING_URL" -u "$USER" -p "$PASS"$repo_opt "$@"
}


BASE_FOLDER=
SERVER_FOLDER=
BINDING_URL=
USER=
PASS=
REPO=

# Parse the arguments into variables
while test -n "$1"; do
    case $1 in
        --url)
            shift
            BINDING_URL="$1"
            shift
            ;;
        --username|-u)
            shift
            USER="$1"
            shift
            ;;
        --password|-p)
            shift
            PASS="$1"
            shift
            ;;
        --repository|-r)
            shift
            REPO="$1"
            shift
            ;;
        --help|-h)
            usage
            exit 1
            ;;
        *)
            if test -z "$BASE_FOLDER"; then
                BASE_FOLDER="$1"
                shift
            elif test -z "$SERVER_FOLDER"; then
                SERVER_FOLDER="$1"
                shift
            fi
            ;;
    esac
done

# Check that we had the input we need
if test ! -d "$BASE_FOLDER"; then
    usage
    echo ""
    echo "Missing or invalid local folder path"
    exit 1
fi

if test -z "$BINDING_URL"; then
    usage
    echo ""
    echo "Missing CMIS binding URL"
    exit 1
fi

# Look for cmis-client in PATH
CMIS_CLIENT=`which cmis-client 2>/dev/null`
if test -z $CMIS_CLIENT; then
    echo "cmis-client executable isn't in the PATH"
    exit 1
fi

# Make sure the SERVER_FOLDER is existing
SERVER_FOLDER_ID=`cmis_client show-by-path "$SERVER_FOLDER" 2>&1 | grep '^Id:' | sed -e 's/^[^:]*: \(.*\)$/\1/'`
if test -z $SERVER_FOLDER_ID; then
    echo "Server folder '$SERVER_FOLDER' doesn't exist, please indicate an existing folder"
    exit 1
fi

SERVER_FOLDER_BASE_TYPE=`cmis_client show-by-path "$SERVER_FOLDER" 2>&1 | grep '^Base type:' | sed -e 's/^[^:]*: \(.*\)$/\1/'`
if test "$SERVER_FOLDER_BASE_TYPE" != "cmis:folder"; then
    echo "'$SERVER_FOLDER' isn't a folder, please indicate an existing folder"
    exit 1
fi

# Make sure that SERVER_FOLDER has no trailing slash
SERVER_FOLDER=${SERVER_FOLDER%/}

cd $BASE_FOLDER/..
BASE_FOLDER_NAME=`basename "$BASE_FOLDER"`
find $BASE_FOLDER_NAME -print0 | while read -d $'\0' FILE_PATH
do
    FILE_NAME=`basename "$FILE_PATH"`
    DIRNAME=`dirname "$FILE_PATH"`
    DIRNAME=${DIRNAME#.}
    SERVER_PARENT=$SERVER_FOLDER/$DIRNAME
    if test -z "$DIRNAME"; then
        SERVER_PARENT=$SERVER_FOLDER
    fi

    PARENT_ID=`cmis_client show-by-path "$SERVER_PARENT" 2>&1 | grep '^Id:' | sed -e 's/^[^:]*: \(.*\)$/\1/'`

    # We couldn't find the parent id, then we need to create the folder on the server
    if test -z "$PARENT_ID"; then
        PARENT_NAME=`basename "$SERVER_PARENT"`
        PARENT_PARENT_PATH=`dirname "$SERVER_PARENT"`
        PARENT_PARENT_ID=`cmis_client show-by-path "$PARENT_PARENT_PATH" 2>&1 | grep '^Id:' | sed -e 's/^[^:]*: \(.*\)$/\1/'`
        PARENT_ID=`cmis_client create-folder $PARENT_PARENT_ID $PARENT_NAME 2>&1 | grep '^Id:' | sed -e 's/^[^:]*: \(.*\)$/\1/'`
    fi

    if test -d "$FILE_PATH"; then
        cmis_client create-folder "$PARENT_ID" "$FILE_NAME" >/dev/null
    else
        FILE_MIME=`file --mime-type "$FILE_PATH" | cut -d ' ' -f 2`

        cmis_client --input-file "$FILE_PATH" --input-type $FILE_MIME \
                    create-document "$PARENT_ID" "$FILE_NAME" >/dev/null
    fi

    if test "x$?" == "x0";
    then
        echo -ne "OK\t"
    else
        echo -ne "Failed\t"
    fi
    echo $SERVER_PARENT/$FILE_NAME
done
