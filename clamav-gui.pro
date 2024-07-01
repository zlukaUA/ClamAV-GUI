#-------------------------------------------------
#
# Project created by QtCreator 2015-07-01T11:42:00
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = clamav-gui
TEMPLATE = app


SOURCES += src/main.cpp\
    src/clamdmanager.cpp \
    src/setuptab.cpp \
    src/clamav_gui.cpp \
    src/cfilesystemmodel.cpp \
    src/optionsdialog.cpp \
    src/scantab.cpp \
    src/setupfilehandler.cpp \
    src/clamav_ctrl.cpp \
    src/scanlimitstab.cpp \
    src/includeexcludeoptions.cpp \
    src/freshclamsetter.cpp \
    src/dragablepushbutton.cpp \
    src/scheduler.cpp \
    src/highlighter.cpp \
    src/infodialog.cpp \
    src/schedulescanobject.cpp \
    src/logviewobject.cpp \
    src/partiallogobject.cpp \
    src/logviewerobject.cpp \
    src/qroundprogressbar.cpp \
    src/progressdialog.cpp \
    src/profilewizarddialog.cpp \
    src/profilemanager.cpp

HEADERS  += src/clamav_gui.h \
    src/clamdmanager.h \
    src/setuptab.h \
    src/cfilesystemmodel.h \
    src/optionsdialog.h \
    src/scantab.h \
    src/setupfilehandler.h \
    src/clamav_ctrl.h \
    src/scanlimitstab.h \
    src/includeexcludeoptions.h \
    src/freshclamsetter.h \
    src/dragablepushbutton.h \
    src/scheduler.h \
    src/highlighter.h \
    src/infodialog.h \
    src/schedulescanobject.h \
    src/logviewobject.h \
    src/partiallogobject.h \
    src/logviewerobject.h \
    src/qroundprogressbar.h \
    src/progressdialog.h \
    src/profilewizarddialog.h \
    src/profilemanager.h

FORMS    += ui/clamav_gui.ui \
    ui/clamdmanager.ui \
    ui/setuptab.ui \
    ui/optionsdialog.ui \
    ui/scantab.ui \
    ui/clamav_ctrl.ui \
    ui/scanlimitstab.ui \
    ui/includeexcludeoptions.ui \
    ui/freshclamsetter.ui \
    ui/dragablepushbutton.ui \
    ui/scheduler.ui \
    ui/infodialog.ui \
    ui/schedulescanobject.ui \
    ui/logviewobject.ui \
    ui/partiallogobject.ui \
    ui/logviewerobject.ui \
    ui/progressdialog.ui \
    ui/profilewizarddialog.ui \
    ui/profilemanager.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS += translations/clamav-gui-de_DE.ts \
    translations/clamav-gui-en_GB.ts \
    translations/clamav-gui-da_DK.ts \
    translations/clamav-gui-es_ES.ts \
    translations/clamav-gui-fr_FR.ts \
    translations/clamav-gui-pt_PT.ts \
    translations/clamav-gui-it_IT.ts \
    translations/clamav-gui-uk_UA.ts

INSTALLS += target \
    clamdicon \
    freshclamicon \
    onaccessicon \
    icon16 \
    icon22 \
    icon32 \
    icon48 \
    icon64 \
    icon128 \
    icon256 \
    translation \
    desktopfile \
    servicemenufile \
    manpages_de \
    manpages_en \
    manpages_dk \
    manpages_uk \
    manpages_es \
    manpages_fr \
    manpages_it \
    manpages_pt \
    documents

target.path = /$(DESTDIR)/usr/bin
target.files = clamav-gui

clamdicon.path = /$(DESTDIR)/usr/share/icons/hicolor/32x32/apps
clamdicon.files = icons/crosshair.png

freshclamicon.path = /$(DESTDIR)/usr/share/icons/hicolor/32x32/apps
freshclamicon.files = icons/freshclam.png

onaccessicon.path = /$(DESTDIR)/usr/share/icons/hicolor/32x32/apps
onaccessicon.files = icons/onaccess.png

icon16.path = /$(DESTDIR)/usr/share/icons/hicolor/16x16/apps
icon16.files = extra/icon16/clamav-gui.png

icon22.path = /$(DESTDIR)/usr/share/icons/hicolor/22x22/apps
icon22.files = extra/icon22/clamav-gui.png

icon32.path = /$(DESTDIR)/usr/share/icons/hicolor/32x32/apps
icon32.files = extra/icon32/clamav-gui.png

icon48.path = /$(DESTDIR)/usr/share/icons/hicolor/48x48/apps
icon48.files = extra/icon48/clamav-gui.png

icon64.path = /$(DESTDIR)/usr/share/icons/hicolor/64x64/apps
icon64.files = extra/icon64/clamav-gui.png

icon128.path = /$(DESTDIR)/usr/share/icons/hicolor/128x128/apps
icon128.files = extra/icon128/clamav-gui.png

icon256.path = /$(DESTDIR)/usr/share/icons/hicolor/256x256/apps
icon256.files = extra/icon256/clamav-gui.png

desktopfile.path = /$(DESTDIR)/usr/share/applications
desktopfile.files = extra/ClamAV-GUI.desktop

servicemenufile.path = /$(DESTDIR)/usr/share/kservices5/ServiceMenus
servicemenufile.files = extra/ClamAV-GUI-ServiceMenu.desktop

translation.path = /$(DESTDIR)/usr/share/clamav-gui
translation.files = translations/clamav-gui-de_DE.qm translations/clamav-gui-en_GB.qm translations/clamav-gui-da_DK.qm translations/clamav-gui-es_ES.qm translations/clamav-gui-fr_FR.qm translations/clamav-gui-pt_PT.qm translations/clamav-gui-it_IT.qm translations/clamav-gui-uk_UA.qm

manpages_de.path = /$(DESTDIR)/usr/share/man/de/man1
manpages_de.files = man/de/clamav-gui.1.gz

manpages_en.path = /$(DESTDIR)/usr/share/man/en/man1
manpages_en.files = man/en/clamav-gui.1.gz

manpages_dk.path = /$(DESTDIR)/usr/share/man/da_DK/man1
manpages_dk.files = man/da_DK/clamav-gui.1.gz

manpages_uk.path = /$(DESTDIR)/usr/share/man/uk/man1
manpages_uk.files = man/uk/clamav-gui.1.gz

manpages_es.path = /$(DESTDIR)/usr/share/man/es/man1
manpages_es.files = man/es/clamav-gui.1.gz

manpages_fr.path = /$(DESTDIR)/usr/share/man/fr/man1
manpages_fr.files = man/fr/clamav-gui.1.gz

manpages_it.path = /$(DESTDIR)/usr/share/man/it/man1
manpages_it.files = man/it/clamav-gui.1.gz

manpages_pt.path = /$(DESTDIR)/usr/share/man/pt/man1
manpages_pt.files = man/pt/clamav-gui.1.gz

documents.path = /$(DESTDIR)/usr/share/doc/ClamAV-GUI
documents.files = README CHANGES


