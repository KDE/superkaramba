#include "mainwidget.h"

#include <kmanagerselection.h>
#include <kdebug.h>

#include <QTime>

MainWidget::MainWidget(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent),
      m_hasCompManager(false)
{
  setWindowFlags(Qt::FramelessWindowHint);
  setFrameStyle(QFrame::NoFrame);

  //viewport()->setAttribute(Qt::WA_NoBackground);
  //viewport()->setAttribute(Qt::WA_OpaquePaintEvent);

  Display *dpy = XOpenDisplay(0);
  m_hasCompManager = !XGetSelectionOwner(dpy, 
                        XInternAtom(dpy, "_NET_WM_CM_S0", false));
}

MainWidget::~MainWidget()
{
}

void MainWidget::paintEvent(QPaintEvent *e)
{
  if(m_hasCompManager)
  {
    QPainter p(viewport());
    p.setRenderHint(QPainter::Antialiasing);
    p.setClipRect(e->rect());

    //p.save();
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(rect(), Qt::transparent);
    //p.restore();
  }
  QGraphicsView::paintEvent(e);

/*
  QTime time;
  time.start();
  QGraphicsView::paintEvent(e);
  kDebug() << time.elapsed() << endl;
  */
}

