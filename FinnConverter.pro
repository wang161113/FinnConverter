QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    convertertool.cpp \
    main.cpp \
    mainwindow.cpp \
    optiondialog.cpp \
    permissiondialog.cpp \
    permissionmanager.cpp

HEADERS += \
    common.h \
    convertertool.h \
    mainwindow.h \
    optiondialog.h \
    permissiondialog.h \
    permissionmanager.h

FORMS += \
    mainwindow.ui \
    optiondialog.ui \
    permissiondialog.ui

#TRANSLATIONS +=
#CONFIG += lrelease
#CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc


win32 {
    # Windows特定的设置
    RC_ICONS = app.ico
    QMAKE_PROJECT_DEPTH = 0
    INCLUDEPATH += "$$PWD/libraw/inc"
    LIBS += -L"$$PWD/libraw/bin" -llibraw

}

unix {
    # 对于所有的Unix和Linux系统，包括macOS

    macx {
        # macOS特定的设置
        ICON = app.icns
        INCLUDEPATH += "/opt/homebrew/Cellar/libraw/0.21.2/include/libraw/"
        LIBS += -L "/opt/homebrew/Cellar/libraw/0.21.2/lib/" -lraw
    }

    !macx {
        # 非macOS的Unix/Linux系统
    }
}
