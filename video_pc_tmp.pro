QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
SOURCES += \
    main.cpp \
    showmanager.cpp

HEADERS += \
    showmanager.h


FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    README.md


INCLUDEPATH += $$PWD/car_video_view
include($$PWD/car_video_view/car_video_view.pri)

INCLUDEPATH += $$PWD/Login
include($$PWD/Login/Login.pri)

INCLUDEPATH += $$PWD/Common
include($$PWD/Common/Common.pri)
