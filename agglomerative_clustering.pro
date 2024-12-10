QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        AgglomerativeClustering.cc \
        Cluster.cc \
        TimeMeasurer.cc \
        main.cpp

HEADERS += \
        AgglomerativeClustering.h \
        Cluster.h \
        TimeMeasurer.h
