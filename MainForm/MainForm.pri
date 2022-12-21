FORMS += \
    $$PWD/mainform.ui

HEADERS += \
    $$PWD/mainform.h

SOURCES += \
    $$PWD/mainform.cpp

INCLUDEPATH += $$PWD/ClientHeader
include($$PWD/ClientHeader/ClientHeader.pri)

INCLUDEPATH += $$PWD/RealVideo
include($$PWD/RealVideo/RealVideo.pri)
