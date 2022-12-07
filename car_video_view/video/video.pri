HEADERS += \
    $$PWD/h264decoder.h

SOURCES += \
    $$PWD/h264decoder.cpp


win32: LIBS += -L$$PWD/../ffmpeg/lib/ -lavcodec -lavutil -lswscale

INCLUDEPATH += $$PWD/../ffmpeg/include
DEPENDPATH += $$PWD/../ffmpeg/include

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/avcodec.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/libavcodec.a

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/avutil.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/libavutil.a

win32:!win32-g++: PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/swscale.lib
else:win32-g++: PRE_TARGETDEPS += $$PWD/../ffmpeg/lib/libswscale.a
