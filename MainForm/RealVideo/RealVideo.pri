FORMS += \
    $$PWD/realvideo.ui

HEADERS += \
    $$PWD/realvideo.h

SOURCES += \
    $$PWD/realvideo.cpp

INCLUDEPATH += $$PWD/CarSidebar
include($$PWD/CarSidebar/CarSidebar.pri)

INCLUDEPATH += $$PWD/RealVideoBody
include($$PWD/RealVideoBody/RealVideoBody.pri)
