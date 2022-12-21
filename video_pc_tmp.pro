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
    3part/ini/INIReader.h \
    3part/spdlog/async.h \
    3part/spdlog/async_logger-inl.h \
    3part/spdlog/async_logger.h \
    3part/spdlog/cfg/argv.h \
    3part/spdlog/cfg/env.h \
    3part/spdlog/cfg/helpers-inl.h \
    3part/spdlog/cfg/helpers.h \
    3part/spdlog/common-inl.h \
    3part/spdlog/common.h \
    3part/spdlog/details/backtracer-inl.h \
    3part/spdlog/details/backtracer.h \
    3part/spdlog/details/circular_q.h \
    3part/spdlog/details/console_globals.h \
    3part/spdlog/details/file_helper-inl.h \
    3part/spdlog/details/file_helper.h \
    3part/spdlog/details/fmt_helper.h \
    3part/spdlog/details/log_msg-inl.h \
    3part/spdlog/details/log_msg.h \
    3part/spdlog/details/log_msg_buffer-inl.h \
    3part/spdlog/details/log_msg_buffer.h \
    3part/spdlog/details/mpmc_blocking_q.h \
    3part/spdlog/details/null_mutex.h \
    3part/spdlog/details/os-inl.h \
    3part/spdlog/details/os.h \
    3part/spdlog/details/periodic_worker-inl.h \
    3part/spdlog/details/periodic_worker.h \
    3part/spdlog/details/registry-inl.h \
    3part/spdlog/details/registry.h \
    3part/spdlog/details/synchronous_factory.h \
    3part/spdlog/details/tcp_client-windows.h \
    3part/spdlog/details/tcp_client.h \
    3part/spdlog/details/thread_pool-inl.h \
    3part/spdlog/details/thread_pool.h \
    3part/spdlog/details/udp_client-windows.h \
    3part/spdlog/details/udp_client.h \
    3part/spdlog/details/windows_include.h \
    3part/spdlog/fmt/bin_to_hex.h \
    3part/spdlog/fmt/bundled/args.h \
    3part/spdlog/fmt/bundled/chrono.h \
    3part/spdlog/fmt/bundled/color.h \
    3part/spdlog/fmt/bundled/compile.h \
    3part/spdlog/fmt/bundled/core.h \
    3part/spdlog/fmt/bundled/format-inl.h \
    3part/spdlog/fmt/bundled/format.h \
    3part/spdlog/fmt/bundled/locale.h \
    3part/spdlog/fmt/bundled/os.h \
    3part/spdlog/fmt/bundled/ostream.h \
    3part/spdlog/fmt/bundled/printf.h \
    3part/spdlog/fmt/bundled/ranges.h \
    3part/spdlog/fmt/bundled/xchar.h \
    3part/spdlog/fmt/chrono.h \
    3part/spdlog/fmt/compile.h \
    3part/spdlog/fmt/fmt.h \
    3part/spdlog/fmt/ostr.h \
    3part/spdlog/fmt/ranges.h \
    3part/spdlog/fmt/xchar.h \
    3part/spdlog/formatter.h \
    3part/spdlog/fwd.h \
    3part/spdlog/logger-inl.h \
    3part/spdlog/logger.h \
    3part/spdlog/pattern_formatter-inl.h \
    3part/spdlog/pattern_formatter.h \
    3part/spdlog/sinks/android_sink.h \
    3part/spdlog/sinks/ansicolor_sink-inl.h \
    3part/spdlog/sinks/ansicolor_sink.h \
    3part/spdlog/sinks/base_sink-inl.h \
    3part/spdlog/sinks/base_sink.h \
    3part/spdlog/sinks/basic_file_sink-inl.h \
    3part/spdlog/sinks/basic_file_sink.h \
    3part/spdlog/sinks/daily_file_sink.h \
    3part/spdlog/sinks/dist_sink.h \
    3part/spdlog/sinks/dup_filter_sink.h \
    3part/spdlog/sinks/hourly_file_sink.h \
    3part/spdlog/sinks/mongo_sink.h \
    3part/spdlog/sinks/msvc_sink.h \
    3part/spdlog/sinks/null_sink.h \
    3part/spdlog/sinks/ostream_sink.h \
    3part/spdlog/sinks/qt_sinks.h \
    3part/spdlog/sinks/ringbuffer_sink.h \
    3part/spdlog/sinks/rotating_file_sink-inl.h \
    3part/spdlog/sinks/rotating_file_sink.h \
    3part/spdlog/sinks/sink-inl.h \
    3part/spdlog/sinks/sink.h \
    3part/spdlog/sinks/stdout_color_sinks-inl.h \
    3part/spdlog/sinks/stdout_color_sinks.h \
    3part/spdlog/sinks/stdout_sinks-inl.h \
    3part/spdlog/sinks/stdout_sinks.h \
    3part/spdlog/sinks/syslog_sink.h \
    3part/spdlog/sinks/systemd_sink.h \
    3part/spdlog/sinks/tcp_sink.h \
    3part/spdlog/sinks/udp_sink.h \
    3part/spdlog/sinks/win_eventlog_sink.h \
    3part/spdlog/sinks/wincolor_sink-inl.h \
    3part/spdlog/sinks/wincolor_sink.h \
    3part/spdlog/spdlog-inl.h \
    3part/spdlog/spdlog.h \
    3part/spdlog/stopwatch.h \
    3part/spdlog/tweakme.h \
    3part/spdlog/version.h \
    showmanager.h


FORMS +=

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    3part/spdlog/fmt/bundled/fmt.license.rst \
    README.md \
    conf/client.ini \
    doc/车辆状态.png

INCLUDEPATH += $$PWD/3part
INCLUDEPATH += $$PWD/3part/spdlog
INCLUDEPATH += $$PWD/3part/ini

INCLUDEPATH += $$PWD/car_video_view
include($$PWD/car_video_view/car_video_view.pri)

INCLUDEPATH += $$PWD/Login
include($$PWD/Login/Login.pri)

INCLUDEPATH += $$PWD/Common
include($$PWD/Common/Common.pri)

INCLUDEPATH += $$PWD/ShowMap
include($$PWD/ShowMap/ShowMap.pri)

INCLUDEPATH += $$PWD/MainForm
include($$PWD/MainForm/MainForm.pri)


INCLUDEPATH += $$PWD/CarInfo
include($$PWD/CarInfo/CarInfo.pri)
