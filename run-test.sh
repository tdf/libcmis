#!/usr/bin/env sh

LIBCMIS_DIR=$PWD/`dirname $0`

TEST_SERVER=${LIBCMIS_DIR}/test-server
TEST_SERVER_SCRIPT=${TEST_SERVER}/bin/jetty.sh

TEST_SERVER_URL=http://localhost:8080/inmemory
REST_URL=${TEST_SERVER_URL}/atom
SOAP_URL=${TEST_SERVER_URL}/RepositoryService


JETTY_VERSION=8.0.1
JETTY_NAME=jetty-hightide-${JETTY_VERSION}.v20110908

CHEMISTRY_VERSION=0.4.0
CHEMISTRY_WEBAPPS=chemistry-opencmis-dist-${CHEMISTRY_VERSION}-server-webapps.tar.gz
INMEMORY_WAR=chemistry-opencmis-server-inmemory-${CHEMISTRY_VERSION}.war


function setup_server()
{
    # Downloads
    if test ! -e  ${LIBCMIS_DIR}/${JETTY_NAME}.tar.gz; then
        echo "Downloading Jetty"
        curl -o ${LIBCMIS_DIR}/${JETTY_NAME}.tar.gz \
            http://dist.codehaus.org/jetty/jetty-hightide-${JETTY_VERSION}/${JETTY_NAME}.tar.gz
    fi

    if test ! -e  ${LIBCMIS_DIR}/${CHEMISTRY_WEBAPPS}; then
        echo "Downloading apache chemistry webapps"
        curl -o  ${LIBCMIS_DIR}/${CHEMISTRY_WEBAPPS} \
            http://mirror.speednetwork.de/apache/chemistry/opencmis/${CHEMISTRY_VERSION}/${CHEMISTRY_WEBAPPS}
    fi

    # Untar it all
    echo "Uncompressing ${JETTY_NAME}.tar.gz"
    tar xzf ${LIBCMIS_DIR}/${JETTY_NAME}.tar.gz -C ${LIBCMIS_DIR}
    mv ${LIBCMIS_DIR}/${JETTY_NAME} ${TEST_SERVER}

    echo "Uncompressing ${CHEMISTRY_WEBAPPS}"
    tar xzf ${LIBCMIS_DIR}/${CHEMISTRY_WEBAPPS} -C ${LIBCMIS_DIR} ${INMEMORY_WAR}
    mv ${LIBCMIS_DIR}/${INMEMORY_WAR} ${TEST_SERVER}/webapps/inmemory.war
}

function server()
{
    case $1 in
        start)
            ${TEST_SERVER_SCRIPT} start >/dev/null 2>&1
            # Wait for the server to be actually up and running
            i="0"
            started="no"
            echo -n "Waiting for test web server to be started"
            while test $i -lt 120; do
                sleep 1s
                curl --silent ${TEST_SERVER_URL} >/dev/null
                if test $? -eq 0; then
                    break
                fi
                i=`expr $i + 1`
                echo -n "."
            done
            echo "Started"
            ;;
        stop)
            ${TEST_SERVER_SCRIPT} stop >/dev/null
            ;;
    esac
}

if test ! -e ${TEST_SERVER}/webapps/inmemory.war; then
    setup_server
fi

server "start"

#Run the test
$1 $REST_URL $SOAP_URL
result=$?

server "stop"

exit $result
