TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    datastructure.cpp

HEADERS += \
    datastructure.hpp

# Uncomment the line below to enable debug STL (with more checks on iterator invalidation etc.)
# NOTE 1: Enabling debug STL mode will make the performance WORSE. So don't enable it when running performance tests!
# NOTE 2: If you uncomment or recomment the line, remember to recompile EVERYTHING by selecting
# "Rebuild all" from the Build menu
# QMAKE_CXXFLAGS += -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
