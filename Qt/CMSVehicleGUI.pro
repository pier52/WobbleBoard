#-------------------------------------------------
#
# Project created by QtCreator 2016-10-24T10:17:05
#
#-------------------------------------------------

QT       += core gui network multimedia multimediawidgets
CONFIG += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport serialport

TARGET = CMSVehicleGUI
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    tcpclient.cpp \
    videostreamreader.cpp \
    mplayerwrapper.cpp \
    rpicamcomm.cpp \
    MotorComm.cpp \
    logger.cpp \
    qcustomplot.cpp \
    plotwindow.cpp \
    robotimagerenderer.cpp \
    gradientslider.cpp \
    plotlegend.cpp \
    motorboardsettingswindow.cpp \
    cameramodulesettingswindow.cpp \
    mouseclickdetector.cpp \
    wifiicon.cpp \
    networkpinger.cpp \
    loggingoptionswindow.cpp \
    popupmenu.cpp \
    logviewer.cpp \
    logchooserdialog.cpp \
    plotlinemenu.cpp \
    plotlegendlinedisplay.cpp \
    logfile.cpp \
    ikeyboardhandler.cpp \
    mainwindowkeyhandler.cpp \
    plotwindowkeyhandler.cpp \
    motorcommwrapper.cpp \
    mapwindow.cpp \
    positionwidget.cpp

HEADERS  += mainwindow.h \
    tcpclient.h \
    videostreamreader.h \
    mplayerwrapper.h \
    rpicamcomm.h \
    MotorComm.h \
    logger.h \
    qcustomplot.h \
    plotwindow.h \
    robotimagerenderer.h \
    gradientslider.h \
    plotlegend.h \
    motorboardsettingswindow.h \
    cameramodulesettingswindow.h \
    mouseclickdetector.h \
    wifiicon.h \
    networkpinger.h \
    loggingoptionswindow.h \
    popupmenu.h \
    logviewer.h \
    logchooserdialog.h \
    plotlinemenu.h \
    plotlegendlinedisplay.h \
    logfile.h \
    logenum.h \
    ikeyboardhandler.h \
    mainwindowkeyhandler.h \
    plotwindowkeyhandler.h \
    motorcommwrapper.h \
    mapwindow.h \
    positionwidget.h

FORMS    += mainwindow.ui \
    plotwindow.ui \
    motorboardsettingswindow.ui \
    cameramodulesettingswindow.ui \
    loggingoptionswindow.ui \
    logchooserdialog.ui \
    mapwindow.ui \
    positionwidget.ui
