QT       += core gui
QT       += network
QT       += svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

QMAKE_LFLAGS_RELEASE += -static -static-libgcc

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    bglabelclass.cpp \
    closelabelclass.cpp \
    main.cpp \
    mainwindow.cpp \
    settingsframeclass.cpp \
    settingslabelclass.cpp

HEADERS += \
    bglabelclass.hpp \
    closelabelclass.hpp \
    mainwindow.hpp \
    settings.hpp \
    settingsframeclass.hpp \
    settingslabelclass.hpp

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    res.qrc

DISTFILES += \
    bg.svg \
    pictures/svg/dynamic/close/main.svg \
    pictures/svg/dynamic/close/on_click.svg \
    pictures/svg/dynamic/close/on_enter.svg \
    pictures/svg/dynamic/loading/main.gif \
    pictures/svg/dynamic/settings/main.svg \
    pictures/svg/dynamic/settings/on_click.svg \
    pictures/svg/dynamic/settings/on_enter.svg \
    pictures/svg/main/bg.svg \
    pictures/svg/main/favicon.ico \
    pictures/svg/main/logo.svg

RC_ICONS = pictures/svg/main/favicon.ico

DEFINES += QAPPLICATION_CLASS=QApplication

