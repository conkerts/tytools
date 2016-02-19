# ty, a collection of GUI and command-line tools to manage Teensy devices
#
# Distributed under the MIT license (see LICENSE.txt or http://opensource.org/licenses/MIT)
# Copyright (c) 2015 Niels Martignène <niels.martignene@gmail.com>

TEMPLATE = app
QT += widgets network

TARGET = tyqt

LIBS = -lty

FORMS += about_dialog.ui \
    arduino_dialog.ui \
    board_widget.ui \
    log_window.ui \
    main_window.ui \
    selector_dialog.ui

HEADERS += about_dialog.hh \
    arduino_dialog.hh \
    arduino_install.hh \
    board.hh \
    board_widget.hh \
    commands.hh \
    database.hh \
    descriptor_notifier.hh \
    enhanced_plain_text.hh \
    firmware.hh \
    log_window.hh \
    main_window.hh \
    monitor.hh \
    selector_dialog.hh \
    session_channel.hh \
    task.hh \
    tyqt.hh

SOURCES += about_dialog.cc \
    arduino_dialog.cc \
    arduino_install.cc \
    board.cc \
    board_widget.cc \
    commands.cc \
    database.cc \
    descriptor_notifier.cc \
    enhanced_plain_text.cc \
    firmware.cc \
    log_window.cc \
    main.cc \
    main_window.cc \
    monitor.cc \
    selector_dialog.cc \
    session_channel.cc \
    task.cc \
    tyqt.cc

RESOURCES += tyqt.qrc
