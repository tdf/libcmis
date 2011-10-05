EXTERNAL_WARNINGS_NOT_ERRORS := TRUE

PRJ=..$/..$/..$/..$/..$/..

PRJNAME=libcmis
TARGET=cmislib
ENABLE_EXCEPTIONS=TRUE
LIBTARGET=NO

.INCLUDE :  settings.mk

.IF "$(GUI)$(COM)"=="WNTMSC"
CFLAGS+=-GR
.ENDIF
.IF "$(COM)"=="GCC"
CFLAGSCXX+=-frtti
.ENDIF

.IF "$(SYSTEM_LIBXML)"=="YES"
CFLAGS+=$(LIBXML_CFLAGS)
.ENDIF
.IF "$(SYSTEM_CURL)"=="YES"
CFLAGS+=$(CURL_CFLAGS)
.ENDIF

SLOFILES= \
	$(SLO)$/session-factory.obj \
	$(SLO)$/atom-document.obj \
	$(SLO)$/atom-folder.obj \
	$(SLO)$/atom-object.obj \
	$(SLO)$/atom-session.obj \
	$(SLO)$/atom-utils.obj

LIB1ARCHIV=$(LB)$/libcmislib.a
LIB1TARGET=$(SLB)$/$(TARGET).lib
LIB1OBJFILES= $(SLOFILES)

.INCLUDE :  target.mk
