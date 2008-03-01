#include "skscriptapplet.h"

//#include <QScriptEngine>
//#include <QFile>
//#include <QUiLoader>

//#include <KDebug>
//#include <KLocale>
//#include <KStandardDirs>

//#include <plasma/applet.h>
//#include <plasma/svg.h>
//#include <plasma/uiloader.h>
//#include <plasma/layouts/layout.h>
//#include <plasma/widgets/widget.h>

using namespace Plasma;

K_EXPORT_PLASMA_APPLETSCRIPTENGINE(superkaramba, SkScriptApplet)

SkScriptApplet::SkScriptApplet(QObject *parent, const QVariantList &args)
    : Plasma::AppletScript(parent)
{
    kDebug()<<"############################ ctor";
}

SkScriptApplet::~SkScriptApplet()
{
    kDebug()<<"############################ dtor";
}

bool SkScriptApplet::init()
{
    kDebug()<<"############################ init";
    return true;
}

void SkScriptApplet::paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, const QRect &contentsRect)
{
    kDebug()<<"############################ paintInterface";
}

#include "skscriptapplet.moc"
