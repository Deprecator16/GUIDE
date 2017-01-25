QT       += core gui sql xml designer widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BusinessInventoryManager
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    element.cpp \
    createnewbusiness.cpp \
    refine.cpp \
    item.cpp \
    selectiondialog.cpp \
    account.cpp \
    manufacturer.cpp \
    supplier.cpp \
    technician.cpp \
    installation.cpp \
    detailedinventory.cpp \
    site.cpp \
    inventorytransfer.cpp \
    adjustment.cpp \
    PromotedClass/itemtable.cpp \
    PromotedClass/itemwidget.cpp

HEADERS  += mainwindow.h \
    element.h \
    createnewbusiness.h \
    refine.h \
    item.h \
    selectiondialog.h \
    account.h \
    manufacturer.h \
    supplier.h \
    technician.h \
    installation.h \
    detailedinventory.h \
    site.h \
    inventorytransfer.h \
    adjustment.h \
    PromotedClass/itemtable.h \
    PromotedClass/itemwidget.h

FORMS += createnewbusiness.ui \
    item.ui \
    stockrefine.ui \
    selectiondialog.ui \
    manufacturer.ui \
    manufacturerrefine.ui \
    supplier.ui \
    technician.ui \
    supplierrefine.ui \
    technicianrefine.ui \
    installation.ui \
    mainwindow.ui \
    detailedinventory.ui \
    inventorytransfer.ui \
    adjustment.ui \
    PromotedClass/itemwidget.ui

DISTFILES +=

