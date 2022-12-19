FORMS += \
    $$PWD/clientheader.ui \
    $$PWD/mainform.ui

HEADERS += \
    $$PWD/clientheader.h \
    $$PWD/mainform.h

SOURCES += \
    $$PWD/clientheader.cpp \
    $$PWD/mainform.cpp

INCLUDEPATH += $$PWD/ClientHeader
include($$PWD/ClientHeader/ClientHeader.pri)

INCLUDEPATH += $$PWD/RealVideo
include($$PWD/RealVideo/RealVideo.pri)
