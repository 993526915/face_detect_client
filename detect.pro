#-------------------------------------------------
#
# Project created by QtCreator 2020-12-07T15:26:04
#
#-------------------------------------------------
QT       += core gui  sql widgets


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
INCLUDEPATH += /usr/local/opencv4/include \
               /usr/local/opencv4/include/opencv4

LIBS += /usr/local/opencv4/lib/libopencv_*.so\
        /home/wzx/detect/camera/lib/libGCBase_gcc421_v3_0.so\
        /home/wzx/detect/camera/lib/libGenApi_gcc421_v3_0.so  \
        /home/wzx/detect/camera/lib/libLog_gcc421_v3_0.so  \
        /home/wzx/detect/camera/lib/liblog4cpp_gcc421_v3_0.so\
        /home/wzx/detect/camera/lib/libMathParser_gcc421_v3_0.so  \
        /home/wzx/detect/camera/lib/libMediaProcess.so  \
        /home/wzx/detect/camera/lib/libMvCameraControl.so\
        /home/wzx/detect/camera/lib/libMVGigEVisionSDK.so  \
        /home/wzx/detect/camera/lib/libMVRender.so  \
        /home/wzx/detect/camera/lib/libMvUsb3vTL.so \
        /home/wzx/detect/camera/lib/libNodeMapData_gcc421_v3_0.so  \
        /home/wzx/detect/camera/lib/libXmlParser_gcc421_v3_0.so

SOURCES += \
    DBPool.cpp \
    initparams.cpp \
    main.cpp \
    mainwindow.cpp \
    camera/camera.cpp \
    procomimg.cpp \
    faceDetect/facedetect.cpp \
    addface.cpp

HEADERS += \
    DBpool.h \
    initparams.h \
    mainwindow.h \
    camera/camera.h\
    camera/cam_inc/CameraParams.h \
    camera/cam_inc/MvCameraControl.h \
    camera/cam_inc/MvDeviceBase.h \
    camera/cam_inc/MvErrorDefine.h \
    camera/cam_inc/MvGigEDevice.h \
    camera/cam_inc/MvInclude.h \
    camera/cam_inc/MvUsb3VDevice.h \
    camera/cam_inc/PixelType.h \
    camera/cam_inc/TlFactory.h \
    timeStamp.h \
    procomimg.h \
    faceDetect/base/utils.h \
    faceDetect/facedetect.h \
    faceDetect/face.h \
    faceDetect/base/base.h \
    faceDetect/base/base64.h \
    faceDetect/base/http.h \
    addface.h


FORMS += \
        mainwindow.ui \
    addface.ui



DISTFILES += \
    faceDetect/haarcascade_frontalface_default.xml \
    icon/image.png \
    params.json
