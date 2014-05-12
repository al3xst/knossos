#-------------------------------------------------
#
# Project created by QtCreator 2012-09-05T18:04:31
#
#-------------------------------------------------

QT       += core gui opengl network help

TARGET = knossos
TEMPLATE = app
CONFIG += qt c++11 createdoc turbojpeg app_bundle

SOURCES += widgets/mainwindow.cpp \
    openjpeg/cio.c \
    openjpeg/bio.c \
    openjpeg/color.c \
    openjpeg/convert.c \
    openjpeg/dwt.c \
    openjpeg/event.c \
    openjpeg/function_list.c \
    openjpeg/image.c \
    openjpeg/index.c \
    openjpeg/invert.c \
    openjpeg/j2k.c \
    openjpeg/jp2.c \
    openjpeg/mct.c \
    openjpeg/mqc.c \
    openjpeg/openjpeg.c \
    openjpeg/opj_clock.c \
    openjpeg/opj_decompress.c \
    openjpeg/opj_getopt.c \
    openjpeg/pi.c \
    openjpeg/raw.c \
    openjpeg/t1.c \
    openjpeg/t2.c \
    openjpeg/tcd.c \
    openjpeg/tgt.c \
    eventmodel.cpp \
    loader.cpp \
    viewer.cpp \
    remote.cpp \
    skeletonizer.cpp \
    renderer.cpp \
    knossos.cpp \
    coordinate.cpp \
    Hashtable.cpp \
    functions.cpp \
    qsort.cpp \
    ftp.cpp \
    color4F.cpp \
    nodeListElement.cpp \ #segmentListElement.cpp \
    treeListElement.cpp \
    treeLUT_fallback.cpp \
    sha256.cpp \
    widgets/viewport.cpp \    
    widgets/console.cpp \
    widgets/tracingtimewidget.cpp \
    widgets/commentswidget.cpp \
    widgets/commentshortcuts/commentshortcutstab.cpp \
    widgets/commentshortcuts/commentshighlightingtab.cpp \
    widgets/zoomandmultireswidget.cpp \
    widgets/datasavingwidget.cpp \
    widgets/navigationwidget.cpp \
    widgets/viewportsettingswidget.cpp \
    widgets/viewportsettings/vpsliceplaneviewportwidget.cpp \
    widgets/viewportsettings/vpskeletonviewportwidget.cpp \
    widgets/viewportsettings/vpgeneraltabwidget.cpp \
    widgets/splashscreenwidget.cpp \
    widgets/datasetpropertywidget.cpp \
    widgets/task/task.cpp \
    widgets/task/taskloginwidget.cpp \
    widgets/task/taskmanagementwidget.cpp \
    widgets/task/taskmanagementmaintab.cpp \
    widgets/task/taskmanagementdetailstab.cpp \
    widgets/widgetcontainer.cpp \
    widgets/commentshortcuts/commentsnodecommentstab.cpp \
    widgets/documentationwidget.cpp \
    widgets/annotationwidget.cpp \
    widgets/tools/toolscommandstab.cpp \
    widgets/tools/toolstreeviewtab.cpp \
    widgets/tools/nodetable.cpp \
    widgets/tools/treetable.cpp \
    mesh.cpp \
    widgets/gui.cpp \
    widgets/pythonpropertywidget.cpp

PRECOMPILED_HEADERS += openjpeg/tgt.h \
    openjpeg/tcd.h \
    openjpeg/t2.h \
    openjpeg/t1_luts.h \
    openjpeg/t1.h \
    openjpeg/raw.h \
    openjpeg/pi.h \
    openjpeg/opj_stdint.h \
    openjpeg/opj_malloc.h \
    openjpeg/opj_inttypes.h \
    openjpeg/opj_intmath.h \
    openjpeg/opj_includes.h \
    openjpeg/opj_getopt.h \
    openjpeg/opj_config_private.h \
    openjpeg/opj_config.h \
    openjpeg/opj_clock.h \
    openjpeg/opj_apps_config.h \
    openjpeg/openjpeg.h \
    openjpeg/mqc.h \
    openjpeg/mct.h \
    openjpeg/jp2.h \
    openjpeg/j2k.h \
    openjpeg/invert.h \
    openjpeg/indexbox_manager.h \
    openjpeg/index.h \
    openjpeg/image.h \
    openjpeg/function_list.h \
    openjpeg/format_defs.h \
    openjpeg/event.h \
    openjpeg/dwt.h \
    openjpeg/convert.h \
    openjpeg/color.h \
    openjpeg/cio.h \
    openjpeg/cidx_manager.h \
    openjpeg/bio.h

HEADERS  += widgets/mainwindow.h \
    knossos-global.h \
    eventmodel.h \
    loader.h \
    viewer.h \
    remote.h \
    skeletonizer.h \
    renderer.h \
    knossos.h\
    functions.h \
    ftp.h \
    sha256.h \
    widgets/GuiConstants.h \
    widgets/viewport.h \
    widgets/console.h \
    widgets/tracingtimewidget.h \
    widgets/commentswidget.h \
    widgets/commentshortcuts/commentshortcutstab.h \
    widgets/commentshortcuts/commentshighlightingtab.h \
    widgets/zoomandmultireswidget.h \
    widgets/datasavingwidget.h \
    widgets/navigationwidget.h \
    widgets/viewportsettingswidget.h \
    widgets/viewportsettings/vpsliceplaneviewportwidget.h \
    widgets/viewportsettings/vpskeletonviewportwidget.h \
    widgets/viewportsettings/vpgeneraltabwidget.h \
    widgets/splashscreenwidget.h \
    widgets/datasetpropertywidget.h \
    widgets/task/taskloginwidget.h \
    widgets/task/taskmanagementwidget.h \
    widgets/task/taskmanagementmaintab.h \
    widgets/task/taskmanagementdetailstab.h \
    widgets/commentshortcuts/commentsnodecommentstab.h \
    widgets/widgetcontainer.h \
    widgets/documentationwidget.h \
    widgets/annotationwidget.h \
    widgets/tools/toolscommandstab.h \
    widgets/tools/toolstreeviewtab.h \
    widgets/tools/nodetable.h \
    widgets/tools/treetable.h \
    widgets/gui.h \
    widgets/pythonpropertywidget.h

OTHER_FILES += \
    python/* \
    installer/osx/setup.sh \
    installer/linux/setup.sh \
    installer/* \
    doc/* \
    doc/images/* \
    LICENSE \
    Makefile \
    splash.png \
    default.lut \
    knossos.res \
    knossos_private.res \
    knossos.rc \
    knossos_private.rc \
    logo.ico \
    ChangeLog.txt \
    ChangeLog_v4.txt \
    style.qss

exists(.svn) {
    #stringify (svnversion ouput maybe not integer)
    SVNREV = \\\"$$system(svnversion)\\\"
    DEFINES += REVISION=$$SVNREV
    message(svn revision: $$SVNREV)
}
exists(.git) {
    COMMIT = $$system(git log -1 --pretty=format:%H)
    GITREV = $$system(git svn find-rev $$COMMIT)
    DEFINES += REVISION=$$GITREV
    message(git svn revision: $$GITREV)
}

include(scriptengine/scriptengine.pri)

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.8
macx {
    INCLUDEPATH += $(QTDIR)/include
    LIBS += -framework GLUT \
            -lcurl \
            $(QTDIR)/lib/libPythonQt.dylib \
            $(QTDIR)/lib/libPythonQt_QtAll.dylib \



    # copy the content of the doc folder to the build-dir
    script.path = $$OUT_PWD/knossos.app/Contents/MacOS/python \
    script.files = scriptengine/python/* \
    INSTALLS += script \
    ICON += knossos.icns \
}



linux {
    LIBS += -lcurl
    LIBS += -lGLU -lglut

}

win32 {
    LIBS += -lcurl.dll -lwsock32
    LIBS += -lglut32

    RC_FILE = knossos.rc
}

createdoc {
    message(building createdoc)
    createdoctarget.target = $$PWD/doc/collection.qhc
    createdoctarget.depends = $$PWD/doc/collection.qhcp $$PWD/doc/doc.qhp
    createdoctarget.commands = qcollectiongenerator $$PWD/doc/collection.qhcp -o $$PWD/doc/collection.qhc
    QMAKE_EXTRA_TARGETS += createdoctarget
    PRE_TARGETDEPS += $$PWD/doc/collection.qhc
}

turbojpeg {    
    DEFINES += KNOSSOS_USE_TURBOJPEG
    mac {
        INCLUDEPATH += /opt/libjpeg-turbo/include
        LIBS += -L/opt/libjpeg-turbo/lib
    }
    win32 {
        INCLUDEPATH += C:/libjpeg-turbo-gcc/include
        LIBS += -LC:/libjpeg-turbo-gcc/lib
    }
    LIBS += -lturbojpeg
}


    RESOURCES += Resources.qrc


QMAKE_CXXFLAGS_RELEASE += -O3


