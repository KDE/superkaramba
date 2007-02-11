#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QPaintEvent>

class MainWidget : public QGraphicsView
{
  public:
    MainWidget(QGraphicsScene *scene, QWidget *parent = 0);

    ~MainWidget();

  protected:
    void paintEvent(QPaintEvent *e);

  private:
    bool m_hasCompManager;
};

#endif
