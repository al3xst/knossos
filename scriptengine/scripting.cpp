#include <QSettings>
#include <QDir>
#include <QFileInfoList>
#include <QToolBar>
#include <QMenu>

#include "widgets/GuiConstants.h"
#include "scripting.h"
#include "decorators/floatcoordinatedecorator.h"
#include "decorators/coordinatedecorator.h"
#include "decorators/colordecorator.h"
#include "decorators/treelistdecorator.h"
#include "decorators/nodelistdecorator.h"
#include "decorators/segmentlistdecorator.h"
#include "decorators/meshdecorator.h"
#include "decorators/transformdecorator.h"
#include "decorators/pointdecorator.h"
#include "proxies/skeletonproxy.h"

#include "geometry/render.h"
#include "geometry/point.h"
#include "geometry/transform.h"
#include "geometry/shape.h"

#include "highlighter.h"
#include "knossos-global.h"
#include "widgets/mainwindow.h"

extern stateInfo *state;

Scripting::Scripting(QObject *parent) :
    QThread(parent)
{

    floatCoordinateDecorator = new FloatCoordinateDecorator();
    coordinateDecorator = new CoordinateDecorator();
    colorDecorator = new ColorDecorator();
    treeListDecorator = new TreeListDecorator();
    nodeListDecorator = new NodeListDecorator();
    segmentListDecorator = new SegmentListDecorator();
    meshDecorator = new MeshDecorator();
    transformDecorator = new TransformDecorator();
    pointDecorator = new PointDecorator();
    skeletonProxy = new SkeletonProxy();

}

void Scripting::run() {
    PythonQt::init(PythonQt::RedirectStdOut);
    PythonQt_QtAll::init();
    PythonQtObjectPtr ctx = PythonQt::self()->createModuleFromScript("knossos_python_api");

    ctx.evalScript("import sys");
    ctx.evalScript("sys.argv = ['']");  // <- this is needed to import the ipython module from the site-package
#ifdef Q_OS_OSX
    // as ipython does not export it's sys paths after the installation we refer to that site-package
    ctx.evalScript("sys.path.append('/Library/Python/2.7/site-packages')");
#endif

    ctx.addObject("knossos", skeletonProxy);
    ctx.addVariable("GL_POINTS", GL_POINTS);
    ctx.addVariable("GL_LINES", GL_LINES);
    ctx.addVariable("GL_LINE_STRIP", GL_LINE_STRIP);
    ctx.addVariable("GL_LINE_LOOP", GL_LINE_LOOP);
    ctx.addVariable("GL_TRIANGLES", GL_TRIANGLES);
    ctx.addVariable("GL_TRIANGLES_STRIP", GL_TRIANGLE_STRIP);
    ctx.addVariable("GL_TRIANGLE_FAN", GL_TRIANGLE_FAN);
    ctx.addVariable("GL_QUADS", GL_QUADS);
    ctx.addVariable("GL_QUAD_STRIP", GL_QUAD_STRIP);
    ctx.addVariable("GL_POLYGON", GL_POLYGON);

    //ctx.addObject("tool_bar", signalDelegate->toolBarSignal());
    //ctx.addObject("menu_bar", signalDelegate->menuBarSignal());

    QString module("internal");
    PythonQt::init(PythonQt::RedirectStdOut, module.toLocal8Bit());

    PythonQt::self()->addDecorators(floatCoordinateDecorator);
    PythonQt::self()->registerCPPClass("FCoordinate", "", module.toLocal8Bit().data());

    PythonQt::self()->addDecorators(coordinateDecorator);
    PythonQt::self()->registerCPPClass("Coordinate", "", module.toLocal8Bit().data());

    PythonQt::self()->addDecorators(colorDecorator);
    PythonQt::self()->registerCPPClass("Color", "", module.toLocal8Bit().data());

    PythonQt::self()->addDecorators(segmentListDecorator);
    PythonQt::self()->registerCPPClass("Segment", "", module.toLocal8Bit().data());

    PythonQt::self()->addDecorators(treeListDecorator);
    PythonQt::self()->registerCPPClass("Tree", "", module.toLocal8Bit().data());

    PythonQt::self()->addDecorators(nodeListDecorator);
    PythonQt::self()->registerCPPClass("Node", "", module.toLocal8Bit().data());

    PythonQt::self()->addDecorators(meshDecorator);
    PythonQt::self()->registerCPPClass("Mesh", "", module.toLocal8Bit().data());

    executeFromUserDirectory();
    ctx.evalFile("/Users/amos/Desktop/test.py");

    ctx.evalFile(QString("sys.path.append('%1')").arg("./python"));
    ctx.evalScript("import IPython");
    ctx.evalScript("IPython.embed_kernel()");

}

void Scripting::addScriptingObject(const QString &name, QObject *obj) {
    PythonQtObjectPtr ctx = PythonQt::self()->getMainModule();
    ctx.addObject(name, obj);
}

void Scripting::saveSettings(const QString &key, const QVariant &value) {
    settings->setValue(key, value);
}

void Scripting::executeFromUserDirectory() {
    QSettings settings;
    settings.beginGroup(PYTHON_PROPERTY_WIDGET);
    QString path = settings.value(PYTHON_AUTOSTART_FOLDER).toString();
    settings.endGroup();

    QDir scriptDir(path);
    QStringList endings;
    endings << "*.py";
    scriptDir.setNameFilters(endings);
    QFileInfoList entries = scriptDir.entryInfoList();

    PythonQtObjectPtr ctx = PythonQt::self()->getMainModule();
    foreach(const QFileInfo &script, entries) {        
        QFile file(script.canonicalFilePath());

        if(!file.open(QIODevice::Text | QIODevice::ReadOnly)) {
            continue;
        }

        QTextStream stream(&file);
        QString content =  stream.readAll();

        ctx.evalFile(script.canonicalFilePath());

    }
}
