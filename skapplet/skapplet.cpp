#include "skapplet.h"

//#include <math.h>

//#include <QApplication>
#include <QFile>
//#include <QBitmap>
#include <QGraphicsScene>
//#include <QMatrix>
#include <QPaintEvent>
#include <QPainter>
#include <QPixmap>
#include <QStyleOptionGraphicsItem>

#include <KDebug>
#include <KLocale>
//#include <KIcon>
//#include <KSharedConfig>
#include <KDialog>
#include <KFileDialog>

#include <plasma/svg.h>

extern "C" {
    /// The SuperKaramba library exports this function.
    typedef QGraphicsItemGroup* (*startKaramba)(const KUrl &theme, QGraphicsView *view);
}

SuperKarambaApplet::SuperKarambaApplet(QObject *parent, const QStringList &args)
    : Plasma::Applet(parent, args)
    , m_themeItem(0)
    , m_timer(new QTimer(this))
{
    kDebug() << "========================> SuperKarambaApplet Ctor" << endl;
    setHasConfigurationInterface(true);

    KConfigGroup cg = config("SuperKaramba");
    m_themePath = cg.readEntry("theme", KUrl());

    if( ! m_themePath.isValid() ) {
        //m_themePath = KUrl("file:///home/kde4/svn/_src/KDE/kdeutils/superkaramba/examples/rubyclock/clock.theme");
        //m_themePath = KUrl("/home/kde4/svn/_src/KDE/kdeutils/superkaramba/examples/richtext/richtext.theme");
        //m_themePath = KUrl("/home/kde4/svn/_src/KDE/kdeutils/superkaramba/examples/pythonclock/pythonclock.theme");
        //m_themePath = KUrl("/home/kde4/svn/_src/KDE/kdeutils/superkaramba/examples/aeroai/aero_aio.skz");


        KFileDialog* filedialog = new KFileDialog(
            KUrl("kfiledialog:///SuperKarambaPlasmaApplet"), // startdir
            i18n("*.skz *.theme|Theme Files\n*|All Files"), // filter
            0, // custom widget
            0 // parent
        );
        filedialog->setCaption( i18n("SuperKaramba Theme") );
        filedialog->setOperationMode( KFileDialog::Opening );
        filedialog->setMode( KFile::File | KFile::ExistingOnly | KFile::LocalOnly );
        if( filedialog->exec() )
            m_themePath = filedialog->selectedUrl();

    }

    /*
    m_showTimeString = cg.readEntry("showTimeString", false);
    m_showSecondHand = cg.readEntry("showSecondHand", false);
    m_pixelSize = cg.readEntry("size", 250);
    m_timezone = cg.readEntry("timezone", "Local");
    m_theme = new Plasma::Svg("widgets/clock", this);
    m_theme->setContentType(Plasma::Svg::SingleImage);
    m_theme->resize(m_pixelSize, m_pixelSize);

    Plasma::DataEngine* timeEngine = dataEngine("time");
    timeEngine->connectSource(m_timezone, this);
    timeEngine->setProperty("reportSeconds", m_showSecondHand);
    */

    constraintsUpdated();
    m_timer->setInterval(1000);
    m_timer->setSingleShot(false);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(slotTimeout()));
    //m_timer->start();
}

SuperKarambaApplet::~SuperKarambaApplet()
{
    kDebug() << "========================> SuperKarambaApplet Dtor" << endl;
}

void SuperKarambaApplet::slotTimeout()
{
    kDebug() << "SuperKarambaApplet::slotTimeout()" << endl;
    //update();
    //constraintsUpdated();
}

void SuperKarambaApplet::loadKaramba()
{
    kDebug() << "SuperKarambaApplet::loadKaramba() Theme: " << m_themePath << endl;

    QGraphicsScene *gfxScene = scene();
    Q_ASSERT( gfxScene );

    QString karambaLib = QFile::encodeName(KLibLoader::self()->findLibrary(QLatin1String("libsuperkaramba")));
    KLibrary *lib = KLibLoader::self()->library(karambaLib);
    if (lib) {
        startKaramba createKaramba = 0;
        createKaramba = (startKaramba)lib->resolveFunction("startKaramba");
        if (createKaramba)
            m_themeItem = createKaramba(m_themePath, gfxScene->views()[0]);
    } else {
        kWarning() << "Could not load " << karambaLib << endl;
    }
}

void SuperKarambaApplet::paintInterface(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if( ! m_themeItem ) {
        if( m_themePath.isValid() ) {
            loadKaramba();
        }
        else {
            kDebug() << "No Theme defined" << endl;
            return;
        }
    }

    if( m_themeItem ) {
        //kDebug()<<"SuperKarambaApplet::paintInterface: Has m_themeItem"<<endl;
        m_themeItem->paint(painter, option, widget);
    }
    else {
        kDebug()<<"SuperKarambaApplet::paintInterface: m_themeItem is NULL !!!"<<endl;
    }
}

void SuperKarambaApplet::showConfigurationInterface()
{
    kDebug() << "START -------------> SuperKarambaApplet::showConfigurationInterface" << endl;

    KDialog* dialog = new KDialog();
    dialog->setCaption( "Configure SuperKaramba" );
    dialog->setButtons( KDialog::Ok | KDialog::Cancel | KDialog::Apply );
    /*
    ui.setupUi(m_dialog->mainWidget());
    ui.timeZones->setSelected(m_timezone, true);
    ui.spinSize->setValue((int)m_bounds.width());
    ui.showTimeStringCheckBox->setChecked(m_showTimeString);
    ui.showSecondHandCheckBox->setChecked(m_showSecondHand);
    */
    connect( dialog, SIGNAL(applyClicked()), this, SLOT(configAccepted()) );
    connect( dialog, SIGNAL(okClicked()), this, SLOT(configAccepted()) );
    dialog->show();
    kDebug() << "DONE -------------> SuperKarambaApplet::showConfigurationInterface" << endl;
    delete dialog;
}

void SuperKarambaApplet::configAccepted()
{
    kDebug() << "SuperKarambaApplet::configAccepted" << endl;

    KConfigGroup cg = config("SuperKaramba");
    cg.writeEntry("theme", m_themePath);
    /*
    m_showTimeString = ui.showTimeStringCheckBox->checkState() == Qt::Checked;
    m_showSecondHand = ui.showSecondHandCheckBox->checkState() == Qt::Checked;
    cg.writeEntry("showTimeString", m_showTimeString);
    cg.writeEntry("showSecondHand", m_showSecondHand);
    dataEngine("time")->setProperty("reportSeconds", m_showSecondHand);
    */
    QGraphicsItem::update();
    /*
    cg.writeEntry("size", ui.spinSize->value());
    m_theme->resize(ui.spinSize->value(), ui.spinSize->value());
    QStringList tzs = ui.timeZones->selection();
    dataEngine("time")->connectSource(m_timezone, this);
    */
    constraintsUpdated();
    cg.config()->sync();
}

void SuperKarambaApplet::constraintsUpdated()
{
    kDebug() << "SuperKarambaApplet::constraintsUpdated" << endl;
    Plasma::Applet::constraintsUpdated();
}

#if 0
QRectF Clock::boundingRect() const
{
    return m_bounds;
}

void Clock::constraintsUpdated()
{
    prepareGeometryChange();
    if (formFactor() == Plasma::Planar ||
        formFactor() == Plasma::MediaCenter) {
        QSize s = m_theme->size();
        m_bounds = QRect(0, 0, s.width(), s.height());
        update();
    } else {
        QFontMetrics fm(QApplication::font());
        m_bounds = QRectF(0, 0, fm.width("00:00:00") * 1.2, fm.height() * 1.5);
    }
}

void Clock::updated(const QString& source, const Plasma::DataEngine::Data &data)
{
    Q_UNUSED(source);
    m_time = data["Time"].toTime();

    if (m_time.minute() == m_lastTimeSeen.minute() &&
        m_time.second() == m_lastTimeSeen.second()) {
        // avoid unnecessary repaints
        //kDebug() << "avoided unecessary update!" << endl;
        return;
    }

    m_lastTimeSeen = m_time;
    update();
}

void Clock::drawHand(QPainter *p, int rotation, const QString &handName)
{
    Q_UNUSED(p);
    Q_UNUSED(rotation);
    Q_UNUSED(handName);
// TODO: IMPLEMENT ME!
//     p->save();
//     QRectF tempRect(0, 0, 0, 0);
//     QSizeF boundSize = boundingRect().size();
//     QSize elementSize;
// 
//     p->translate(boundSize.width()/2, boundSize.height()/2);
//     p->rotate(rotation);
//     elementSize = m_theme->elementSize(handName);
//     if (scaleFactor != 1) {
//         elementSize = QSize(elementSize.width()*scaleFactor, elementSize.height()*scaleFactor);
//     }
//     p->translate(-elementSize.width()/2, -elementSize.width());
//     m_theme->resize(elementSize);
//     tempRect.setSize(elementSize);
//     m_theme->paint(p, tempRect, handName);
//     p->restore();
}

void Clock::paintInterface(QPainter *p, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    QRectF tempRect(0, 0, 0, 0);
    QRectF boundRect = boundingRect();

    QSizeF boundSize = boundRect.size();
    QSize elementSize;

    p->setRenderHint(QPainter::SmoothPixmapTransform);

    qreal seconds = 6.0 * m_time.second() - 180;
    qreal minutes = 6.0 * m_time.minute() - 180;
    qreal hours = 30.0 * m_time.hour() - 180 + ((m_time.minute() / 59.0) * 30.0);

    if (formFactor() == Plasma::Horizontal ||
        formFactor() == Plasma::Vertical) {
        QString time = m_time.toString();
        QFontMetrics fm(QApplication::font());
        if (m_showSecondHand) {
            p->drawText((int)(boundRect.width() * 0.1), (int)(boundRect.height() * 0.25), m_time.toString());
        } else {
            p->drawText((int)(boundRect.width() * 0.1), (int)(boundRect.height() * 0.25), m_time.toString("hh:mm"));
        }
        return;
    }
    m_theme->paint(p, boundRect, "ClockFace");

    p->save();
    p->translate(boundSize.width()/2, boundSize.height()/2);
    p->rotate(hours);
    elementSize = m_theme->elementSize("HourHand");

    p->translate(-elementSize.width()/2, -elementSize.width());
    tempRect.setSize(elementSize);
    m_theme->paint(p, tempRect, "HourHand");
    p->restore();

//     drawHand(p, hours, "SecondHand", 1);
    p->save();
    p->translate(boundSize.width()/2, boundSize.height()/2);
    p->rotate(minutes);
    elementSize = m_theme->elementSize("MinuteHand");
    elementSize = QSize(elementSize.width(), elementSize.height());
    p->translate(-elementSize.width()/2, -elementSize.width());
    tempRect.setSize(elementSize);
    m_theme->paint(p, tempRect, "MinuteHand");
    p->restore();

    //Make sure we paint the second hand on top of the others
    if (m_showSecondHand) {
        p->save();
        p->translate(boundSize.width()/2, boundSize.height()/2);
        p->rotate(seconds);
        elementSize = m_theme->elementSize("SecondHand");
        elementSize = QSize(elementSize.width(), elementSize.height());
        p->translate(-elementSize.width()/2, -elementSize.width());
        tempRect.setSize(elementSize);
        m_theme->paint(p, tempRect, "SecondHand");
        p->restore();
    }


    p->save();
    m_theme->resize(boundSize);
    elementSize = m_theme->elementSize("HandCenterScrew");
    tempRect.setSize(elementSize);
    p->translate(boundSize.width() / 2 - elementSize.width() / 2, boundSize.height() / 2 - elementSize.height() / 2);
    m_theme->paint(p, tempRect, "HandCenterScrew");
    p->restore();

    if (m_showTimeString) {
        if (m_showSecondHand) {
            //FIXME: temporary time output
            QString time = m_time.toString();
            QFontMetrics fm(QApplication::font());
            p->drawText((int)(boundRect.width()/2 - fm.width(time) / 2),
                        (int)((boundRect.height()/2) - fm.xHeight()*3), m_time.toString());
        } else {
            QString time = m_time.toString("hh:mm");
            QFontMetrics fm(QApplication::font());
            p->drawText((int)(boundRect.width()/2 - fm.width(time) / 2),
                        (int)((boundRect.height()/2) - fm.xHeight()*3), m_time.toString("hh:mm"));
        }
    }

    m_theme->paint(p, boundRect, "Glass");
}
#endif

#include "skapplet.moc"
