#-------------------------------------------------
#
# Project created by QtCreator 2020-12-07T15:26:04
#
#-------------------------------------------------
QT       += core gui  sql widgets
QT += network

TARGET = detect
TEMPLATE = app
LIBS += -ljsoncpp -lcurl -lssl -lcrypto -lMVSDK  -lmysqlclient
# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
INCLUDEPATH += /usr/local/include \
               /usr/local/include/opencv4

LIBS += /usr/local/lib/libopencv_*.so\


SOURCES += \
    DBpool.cpp \
    comwidget.cpp \
    detectweight.cpp \
    initparams.cpp \
    logindialog.cpp \
    loginweight.cpp \
    main.cpp \
    procomimg.cpp \
    registor.cpp \
    socktool.cpp \
    userweight.cpp

HEADERS += \
    DBpool.h \
    comwidget.h \
    detectweight.h \
    initparams.h \
    logindialog.h \
    loginweight.h \
    registor.h \
    timeStamp.h \
    procomimg.h \
    socktool.h \
    userweight.h


FORMS += \
    comwidget.ui \
    detectweight.ui \
    logindialog.ui \
    loginweight.ui \
    registor.ui \
    userweight.ui



DISTFILES += \
    faceDetect/haarcascade_frontalface_default.xml \
    icon/Back.png \
    icon/Me.png \
    icon/More Circle.png \
    icon/More Circle.png \
    icon/close2.png \
    icon/go.png \
    icon/image.png \
    icon/login_Account.png \
    icon/login_密码.png \
    icon/minimum2.png \
    icon/toRight.png \
    icon/victoricon2.png \
    icon/头像.png \
    icon/摄像头.png \
    icon/正确.png \
    icon/错误.png \
    params.json \
    touxiang/Simpson.jpg \
    touxiang/beizi.jpg \
    touxiang/blownbear.jpg \
    touxiang/dog.jpg \
    touxiang/duolaameng.jpg \
    touxiang/fenhongbao.jpg \
    touxiang/girl.jpg \
    touxiang/girl2.jpg \
    touxiang/girl3.jpg \
    touxiang/girl4.jpg \
    touxiang/haimianbob.jpg \
    touxiang/icebear.jpg \
    touxiang/paidaxing.jpg \
    touxiang/panda.jpg \
    touxiang/pig.jpg \
    touxiang/shuaya.jpg \
    touxiang/tom.jpg \
    touxiang/xiaowanzi.jpg \
    touxiang/xiaoxin.jpg \
    touxiang/yazi.jpg

RESOURCES += \
    resource.qrc
