QT -= gui
QT       += dbus sql
CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        systemPermissions.cc \
        main.cc

# Define a custom target to run clang-format
clang_format.target = clang-format
clang_format.commands = clang-format --verbose -i $(SOURCES)
clang_format.depends = $(SOURCES)
QMAKE_EXTRA_TARGETS += clang_format

# Optionally, add a target for running clang-format
QMAKE_POST_LINK += echo "Run 'make clang-format' to format your code"

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    systemPermissions.h
