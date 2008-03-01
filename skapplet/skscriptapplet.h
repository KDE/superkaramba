#ifndef SKSCRIPTAPPLET_H
#define SKSCRIPTAPPLET_H

#include <plasma/scripting/appletscript.h>
#include <plasma/dataengine.h>

class SkScriptApplet : public Plasma::AppletScript
{
    Q_OBJECT
public:
    SkScriptApplet(QObject *parent, const QVariantList &args);
    virtual ~SkScriptApplet();

    /**
     * Called when it is safe to initialize the internal state.
     */
    virtual bool init();

    /**
     * Called when the applet should be painted.
     *
     * @param painter the QPainter to use
     * @param option the style option containing such flags as selection, level of detail, etc
     * @param contentsRect the rect to paint within; automatically adjusted for
     *                     the background, if any
     **/
    virtual void paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, const QRect &contentsRect);

/*
public slots:
    void dataUpdated( const QString &name, const Plasma::DataEngine::Data &data );
    void showConfigurationInterface();
    void configAccepted();
private:
*/
};

#endif
