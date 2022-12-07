FORMS += \
    $$PWD/carvideoplayer.ui

HEADERS += \
    $$PWD/carvideoclient.h \
    $$PWD/carvideoplayer.h

SOURCES += \
    $$PWD/carvideoclient.cpp \
    $$PWD/carvideoplayer.cpp

INCLUDEPATH += $$PWD/jt1078
include($$PWD/jt1078/jt1078.pri)

INCLUDEPATH += $$PWD/ffmpeg
include($$PWD/ffmpeg/ffmpeg.pri)

INCLUDEPATH += $$PWD/video
include($$PWD/video/video.pri)

INCLUDEPATH += $$PWD/audio
include($$PWD/audio/audio.pri)

INCLUDEPATH += $$PWD/audio_player
include($$PWD/audio_player/audio_player.pri)

INCLUDEPATH += $$PWD/ipc_packet
include($$PWD/ipc_packet/ipc_packet.pri)
