QT       += core gui
QT       += charts
QT       += serialport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    fftgraph.cpp \
    fftworker.cpp \
    graphwindow.cpp \
    kissfft-master/kiss_fft.c \
    main.cpp \
    mainwindow.cpp \
    serialhandler.cpp

HEADERS += \
    fftgraph.h \
    fftworker.h \
    graphwindow.h \
    kissfft-master/kiss_fft.h \
    mainwindow.h \
    serialhandler.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    kissfft-master/.github/workflows/build.yaml \
    kissfft-master/.gitignore \
    kissfft-master/.travis.yml \
    kissfft-master/CHANGELOG \
    kissfft-master/CMakeLists.txt \
    kissfft-master/COPYING \
    kissfft-master/LICENSES/BSD-3-Clause \
    kissfft-master/LICENSES/Unlicense \
    kissfft-master/README.md \
    kissfft-master/README.simd \
    kissfft-master/TIPS \
    kissfft-master/cmake/JoinPaths.cmake \
    kissfft-master/kissfft-config.cmake.in \
    kissfft-master/kissfft.pc.in \
    kissfft-master/test/CMakeLists.txt \
    kissfft-master/test/kissfft-testsuite.sh \
    kissfft-master/test/testkiss.py \
    kissfft-master/tools/CMakeLists.txt
