# 方便使用ntohs之类的函数
LIBS += -lws2_32
HEADERS += \
    $$PWD/JT1078Decoder.h \
    $$PWD/JT1078Header.h \
    $$PWD/JT1078Packet.h \
    $$PWD/buffer/Buffer.h \
    $$PWD/socket/Socket.h

SOURCES += \
    $$PWD/JT1078Decoder.cpp \
    $$PWD/buffer/Buffer.cpp
