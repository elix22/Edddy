TARGET = edddy

LIBS += ../Edddy/Urho3D/lib/libUrho3D.a \
    -lpthread \
    -ldl \
    -lGL

DEFINES += URHO3D_COMPILE_QT

QMAKE_CXXFLAGS += -std=c++11 -O2

INCLUDEPATH += \
    ../Edddy/Urho3D/include \
    ../Edddy/Urho3D/include/Urho3D/ThirdParty \

TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    luckey.cpp \
    mastercontrol.cpp \
    inputmaster.cpp \
    edddycam.cpp \
    castmaster.cpp \
    resourcemaster.cpp \
    effectmaster.cpp \
    gridblock.cpp \
    blockmap.cpp \
    blockinstance.cpp \
    freeblock.cpp \
    block.cpp \
    edddycursor.cpp \
    editmaster.cpp \
    intvector3.cpp \
    guimaster.cpp \
    blockset.cpp \
    gui3d.cpp \
    history.cpp

HEADERS += \
    luckey.h \
    mastercontrol.h \
    inputmaster.h \
    edddycam.h \
    castmaster.h \
    resourcemaster.h \
    effectmaster.h \
    edddyevents.h \
    gridblock.h \
    blockmap.h \
    blockinstance.h \
    freeblock.h \
    block.h \
    edddycursor.h \
    intvector3.h \
    editmaster.h \
    guimaster.h \
    blockset.h \
    gui3d.h \
    history.h
