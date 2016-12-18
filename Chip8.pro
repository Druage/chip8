TEMPLATE = app

QT += qml quick multimedia
CONFIG += c++14

Debug:DESTDIR = debug #Target file directory
Debug:OBJECTS_DIR = debug/.obj #Intermediate object files directory
 Debug:MOC_DIR = debug/.moc #Intermediate moc files directory

Release:DESTDIR = debug #Target file directory
Release:OBJECTS_DIR = debug/.obj #Intermediate object files directory
Release:MOC_DIR = debug/.moc #Intermediate moc files directory

SOURCES += main.cpp \
    videoutput.cpp \
    chip8.cpp \
    audiooutput.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    videooutput.h \
    chip8.h \
    audiooutput.h

# Copy the sound effect

Debug:sound_effect.commands = $(COPY_FILE) \"$$PWD/beep.wav\" \"$$OUT_PWD/debug/beep.wav\"
Release:sound_effect.commands = $(COPY_FILE) \"$$PWD/beep.wav\" \"$$OUT_PWD/release/beep.wav\"

first.depends = $(first) sound_effect
export( first.depends )
export( sound_effect.commands )
QMAKE_EXTRA_TARGETS += first sound_effect
