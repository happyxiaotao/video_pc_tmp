FORMS += \
    $$PWD/mainform.ui

HEADERS += \
    $$PWD/mainform.h

SOURCES += \
    $$PWD/mainform.cpp

INCLUDEPATH += $$PWD/RealVideo
include($$PWD/RealVideo/RealVideo.pri)
