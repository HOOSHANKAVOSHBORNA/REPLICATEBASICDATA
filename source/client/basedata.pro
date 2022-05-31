QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += sql

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    createrequestdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    requestinfodialog.cpp \
    reviewdialog.cpp \
    senddialog.cpp

HEADERS += \
    createrequestdialog.h \
    datastrucs.h \
    mainwindow.h \
    requestinfodialog.h \
    reviewdialog.h \
    senddialog.h

FORMS += \
    createrequestdialog.ui \
    mainwindow.ui \
    requestinfodialog.ui \
    reviewdialog.ui \
    senddialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    dbconfig.ini

RESOURCES +=

copydata.commands = $(COPY_DIR) $$PWD/dbconfig.ini $$OUT_PWD
first.depends = $(first) copydata
export(first.depends)
export(copydata.commands)
QMAKE_EXTRA_TARGETS += first copydata


unix:!macx: LIBS += -L$$OUT_PWD/../common/ -lcommon

INCLUDEPATH += $$PWD/../common
DEPENDPATH += $$PWD/../common
