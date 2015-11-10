#-------------------------------------------------
#
# Project created by QtCreator 2013-11-13T13:17:10
#
#-------------------------------------------------

QT       += core gui
QT       += xml
QT           += network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QHoldemServer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    src_gfx/graphicsscene.cpp \
    src_gfx/cardpixmap.cpp \
    src_threads/checkerthread.cpp \
    src_network/lobby.cpp \
    src_data/database.cpp \
    src_threads/clientthread.cpp \
    src_game/gametracker.cpp \
    src_game/gametable.cpp \
    src_game/gameplayer.cpp

HEADERS  += mainwindow.h \
    src_headers/toplevel.h \
    src_gfx/graphicsscene.h \
    src_gfx/gfx.h \
    src_gfx/cardpixmap.h \
    src_threads/checkerthread.h \
    src_threads/threads.h \
    src_network/lobby.h \
    src_network/network.h \
    src_data/database.h \
    src_threads/clientthread.h \
    src_game/gametracker.h \
    src_game/game.h \
    src_game/gametable.h \
    src_game/gameplayer.h

RESOURCES += \
    src_resources/cards.qrc
