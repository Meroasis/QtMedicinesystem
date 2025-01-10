QT       += core gui sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    StockQuantityDelegate.cpp \
    appointmentsview.cpp \
    doctorview.cpp \
    highlightdelegate.cpp \
    homeview.cpp \
    idatabase.cpp \
    loginview.cpp \
    main.cpp \
    masterview.cpp \
    medicineeditview.cpp \
    medicineview.cpp \
    patienteditview.cpp \
    patientview.cpp \
    registered.cpp \
    roomview.cpp

HEADERS += \
    appointmentsview.h \
    doctorinfo.h \
    doctorview.h \
    highlightdelegate.h \
    homeview.h \
    idatabase.h \
    loginview.h \
    masterview.h \
    medicineeditview.h \
    medicineview.h \
    patienteditview.h \
    patientview.h \
    registered.h \
    roomview.h

FORMS += \
    appointmentsview.ui \
    doctorview.ui \
    homeview.ui \
    loginview.ui \
    masterview.ui \
    medicineeditview.ui \
    medicineview.ui \
    patienteditview.ui \
    patientview.ui \
    registered.ui \
    roomview.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    img.qrc

DISTFILES += \
    img/backgound.jpg
