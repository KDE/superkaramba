/*
 * Copyright (c) 2007 Alexander Wiedenbruch <mail@wiedenbruch.de>
 *
 * This file is part of SuperKaramba.
 *
 *  SuperKaramba is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  SuperKaramba is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with SuperKaramba; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 ****************************************************************************/

#include "lineparser.h"

#include "karambainterface.h"

KarambaInterface::KarambaInterface(Karamba *k)
  : QObject(),
  m_karamba(k)
{
  setObjectName("karamba");

  Kross::Manager::self().addObject(this, "karamba");

  QString scriptFile = k->theme().path() + "/" + k->theme().pythonModule() + ".py";
  m_action = new Kross::Action(this, KUrl(scriptFile));
  m_action->trigger();
}

KarambaInterface::~KarambaInterface()
{
  delete m_action;
}

// Testing functions -----------------------

bool KarambaInterface::checkKaramba(Karamba *k)
{
  if(!k)
  {
    kWarning() << "Widget pointer was 0" << endl;
    return false;
  }

  if(!karambaApp->hasKaramba(k))
  {
    kWarning() << "Widget " << (long)k << " not found" << endl;
    return false;
  }

  return true;
}

bool KarambaInterface::checkMeter(Karamba *k, Meter *m, QString type)
{
  if(!m)
  {
    kWarning() << "Meter pointer was 0" << endl;
    return false;
  }

  if(!k->hasMeter(m))
  {
    kWarning() << "Widget does not have meter " << (long)m << endl;
    return false;
  }

  if(!m->inherits(type.toAscii().data()))
  {
    kWarning() << "Meter is not of type " << type << endl;
    return false;
  }

  return true;
}

bool KarambaInterface::checkKarambaAndMeter(Karamba *k, Meter *m, QString type)
{
  return checkKaramba(k) && checkMeter(k, m, type);
}

QVariantList KarambaInterface::getMeterMinMax(Karamba *k, Meter *m, QString type)
{
  if(!checkKarambaAndMeter(k, m, type))
    return QVariantList();

  QVariantList ret;
  ret << m->getMin();
  ret << m->getMax();
  return ret;
}

QVariantList KarambaInterface::getMeterSize(Karamba *k, Meter *m, QString type)
{
  if(!checkKarambaAndMeter(k, m, type))
    return QVariantList();

  QVariantList list;
  list << QVariant::fromValue(m->getWidth());
  list << QVariant::fromValue(m->getHeight());
  return list;
}

QVariantList KarambaInterface::getMeterPos(Karamba *k, Meter *m, QString type)
{
  if(!checkKarambaAndMeter(k, m, type))
    return QVariantList();

  QVariantList ret;
  ret << QVariant::fromValue(m->getX());
  ret << QVariant::fromValue(m->getY());
  return ret;
}

QString KarambaInterface::getMeterSensor(Karamba *k, Meter *m, QString type)
{
  if(!checkKarambaAndMeter(k, m, type))
    return QString::null;

  return k->getSensor(m);

}

int KarambaInterface::getMeterValue(Karamba *k, Meter *m, QString type)
{
  if(!checkKarambaAndMeter(k, m, type))
    return 0;

  return m->getValue();
}

QObject* KarambaInterface::getThemeMeter(Karamba *k, QString meter, QString type)
{
  if(!checkKaramba(k))
    return 0;

  QGraphicsItem *item;
  QList<QGraphicsItem*> list = ((QGraphicsItemGroup*)k)->children();
  foreach(item, list)
  {
    Meter *m = (Meter*)item;
    if(m->objectName() == meter)
    {
      if(checkMeter(k, m, type));
        return m;
    }
  }
  return 0;
}

bool KarambaInterface::hideMeter(Karamba *k, Meter *m, QString type)
{
  if(!checkKarambaAndMeter(k, m, type))
    return false;

  m->hide();
  return true;
}

bool KarambaInterface::moveMeter(Karamba *k, Meter *m, QString type, int x, int y)
{
  if(!checkKarambaAndMeter(k, m, type))
    return false;

  k->moveMeter(m, x, y);
  return true;
}

bool KarambaInterface::resizeMeter(Karamba *k, Meter *m, QString type, int width, int height)
{
  if(!checkKarambaAndMeter(k, m, type))
    return false;

  m->setSize(m->getX(), m->getY(), width, height);
  return true;
}

bool KarambaInterface::setMeterMinMax(Karamba *k, Meter *m, QString type, int min, int max)
{
  if(!checkKarambaAndMeter(k, m, type))
    return false;

  m->setMin(min);
  m->setMax(max);
  return true;
}

bool KarambaInterface::setMeterSensor(Karamba *k, Meter *m, QString type, QString sensor)
{
  if(!checkKarambaAndMeter(k, m, type))
    return false;

  k->setSensor(LineParser(sensor), m);
  return true;
}

QObject* KarambaInterface::setMeterValue(Karamba *k, Meter *m, QString type, int value)
{
  if(!checkKarambaAndMeter(k, m, type))
    return 0;

  m->setValue(value);
  return m;
}

bool KarambaInterface::showMeter(Karamba *k, Meter *m, QString type)
{
  if(!checkKarambaAndMeter(k, m, type))
    return false;

  m->show();
  return true;
}

bool KarambaInterface::setMeterColor(Karamba *k, Meter *m, QString type, int red, int green, int blue)
{
  if(!checkKarambaAndMeter(k, m, type))
    return false;

  m->setColor(QColor(red, green, blue));
  return true;
}

QVariantList KarambaInterface::getMeterColor(Karamba *k, Meter *m, QString type)
{
  if(!checkKarambaAndMeter(k, m, type))
    return QVariantList();

  QColor color = m->getColor();

  QVariantList ret;
  ret << color.red();
  ret << color.green();
  ret << color.blue();

  return ret;
}

QString KarambaInterface::getMeterStringValue(Karamba *k, Meter *m, QString type)
{
  if(!checkKarambaAndMeter(k, m, type))
    return QString::null;

  return m->getStringValue();
}

QObject* KarambaInterface::setMeterStringValue(Karamba *k, Meter *m, QString type, QString value)
{
  if(!checkKarambaAndMeter(k, m, type))
    return 0;

  m->setValue(value);
  
  return m;
}


// Calls to scripts ----------------------
void KarambaInterface::callInitWidget(Karamba *k)
{
//  QVariantList params;
//  params << QVariant::fromValue((QObject*)k);

  //m_action->callFunction("initWidget", params);
  emit initWidget(k);
}

void KarambaInterface::callWidgetUpdated(Karamba *k)
{
//  QVariantList params;
//  params << QVariant::fromValue((QObject*)k);

  //m_action->callFunction("widgetUpdated", params);
  emit widgetUpdated(k);
}

void KarambaInterface::callMeterClicked(Karamba *k, Meter *m, int button)
{/*
  QVariantList params;
  params << QVariant::fromValue((QObject*)k);
  params << QVariant::fromValue((QObject*)m);
  params << QVariant::fromValue(button);

  m_action->callFunction("meterClicked", params);*/
}

void KarambaInterface::callMeterClicked(Karamba *k, QString str, int button)
{/*
  QVariantList params;
  params << QVariant::fromValue((QObject*)k);
  params << QVariant::fromValue(str);
  params << QVariant::fromValue(button);

  m_action->callFunction("meterClicked", params);*/
}

// Calls from scripts --------------------

QObject* KarambaInterface::createBar(Karamba* k, int x, int y, int w, int h, QString path)
{
  if(!checkKaramba(k))
    return NULL;

  Bar *tmp = new Bar(k, x, y, w, h);
  tmp->setImage(path);
  tmp->setValue(50);
  k->addToGroup(tmp);

  return tmp;
}

bool KarambaInterface::deleteBar(Karamba *k, Bar *bar)
{
  if(!checkKarambaAndMeter(k, bar, "Bar"))
    return false;

  return k->removeMeter(bar);
}

bool KarambaInterface::setBarMinMax(Karamba *k, Bar *bar, int min, int max)
{
  return setMeterMinMax(k, bar, "Bar", min, max);
}

QVariantList KarambaInterface::getBarMinMax(Karamba *k, Bar *bar)
{
  return getMeterMinMax(k, bar, "Bar");
}

bool KarambaInterface::moveBar(Karamba *k, Bar *bar, int x, int y)
{
  return moveMeter(k, bar, "Bar", x, y);
}

QVariantList KarambaInterface::getBarPos(Karamba *k, Bar *bar)
{
  return getMeterPos(k, bar, "Bar");
}

bool KarambaInterface::setBarSensor(Karamba *k, Bar *bar, QString sensor)
{
  return setMeterSensor(k, bar, "Bar", sensor);
}

QString KarambaInterface::getBarSensor(Karamba *k, Bar *bar)
{
  return getMeterSensor(k, bar, "Bar");
}

bool KarambaInterface::resizeBar(Karamba *k, Bar *bar, int width, int height)
{
  return resizeMeter(k, bar, "Bar", width, height);
}

QVariantList KarambaInterface::getBarSize(Karamba *k, Bar *bar)
{
  return getMeterSize(k, bar, "Bar");
}

QObject* KarambaInterface::setBarValue(Karamba *k, Bar *bar, int value)
{
  return setMeterValue(k, bar, "Bar", value);
}

int KarambaInterface::getBarValue(Karamba *k, Bar *bar)
{
  return getMeterValue(k, bar, "Bar");
}

QObject* KarambaInterface::getThemeBar(Karamba *k, QString meter)
{
  return getThemeMeter(k, meter, "Bar");
}

bool KarambaInterface::hideBar(Karamba *k, Bar *bar)
{
  return hideMeter(k, bar, "Bar");
}

bool KarambaInterface::showBar(Karamba *k, Bar *bar)
{
  return showMeter(k, bar, "Bar");
}

bool KarambaInterface::setBarVertical(Karamba *k, Bar *bar, bool vert)
{
  if(!checkKarambaAndMeter(k, bar, "Bar"))
    return false;

  bar->setVertical(vert);
  return true;
}

bool KarambaInterface::getBarVertical(Karamba *k, Bar *bar)
{
  if(!checkKarambaAndMeter(k, bar, "Bar"))
    return false;

  return bar->getVertical();
}

bool KarambaInterface::setBarImage(Karamba *k, Bar *bar, QString image)
{
  if(!checkKarambaAndMeter(k, bar, "Bar"))
    return false;

  return bar->setImage(image);
}

QString KarambaInterface::getBarImage(Karamba *k, Bar *bar)
{
  if(!checkKarambaAndMeter(k, bar, "Bar"))
    return QString::null;

  return bar->getImage();
}




QObject* KarambaInterface::createGraph(Karamba* k, int x, int y, int w, int h, int points)
{
  if(!checkKaramba(k))
    return NULL;

  Graph *tmp = new Graph(k, x, y, w, h, points);
  k->addToGroup(tmp);

  return tmp;
}

bool KarambaInterface::deleteGraph(Karamba *k, Graph *graph)
{
  if(!checkKarambaAndMeter(k, graph, "Graph"))
    return false;

  return k->removeMeter(graph);
}

bool KarambaInterface::setGraphMinMax(Karamba *k, Graph *graph, int min, int max)
{
  return setMeterMinMax(k, graph, "Graph", min, max);
}

QVariantList KarambaInterface::getGraphMinMax(Karamba *k, Graph *graph)
{
  return getMeterMinMax(k, graph, "Graph");
}

bool KarambaInterface::moveGraph(Karamba *k, Graph *graph, int x, int y)
{
  return moveMeter(k, graph, "Graph", x, y);
}

QVariantList KarambaInterface::getGraphPos(Karamba *k, Graph *graph)
{
  return getMeterPos(k, graph, "Graph");
}

bool KarambaInterface::setGraphSensor(Karamba *k, Graph *graph, QString sensor)
{
  return setMeterSensor(k, graph, "Graph", sensor);
}

QString KarambaInterface::getGraphSensor(Karamba *k, Graph *graph)
{
  return getMeterSensor(k, graph, "Graph");
}

bool KarambaInterface::resizeGraph(Karamba *k, Graph *graph, int width, int height)
{
  return resizeMeter(k, graph, "Graph", width, height);
}

QVariantList KarambaInterface::getGraphSize(Karamba *k, Graph *graph)
{
  return getMeterSize(k, graph, "Graph");
}

QObject* KarambaInterface::setGraphValue(Karamba *k, Graph *graph, int value)
{
  return setMeterValue(k, graph, "Graph", value);
}

int KarambaInterface::getGraphValue(Karamba *k, Graph *graph)
{
  return getMeterValue(k, graph, "Graph");
}

QObject* KarambaInterface::getThemeGraph(Karamba *k, QString meter)
{
  return getThemeMeter(k, meter, "Graph");
}

bool KarambaInterface::hideGraph(Karamba *k, Graph *graph)
{
  return hideMeter(k, graph, "Graph");
}

bool KarambaInterface::showGraph(Karamba *k, Graph *graph)
{
  return showMeter(k, graph, "Graph");
}

bool KarambaInterface::setGraphColor(Karamba *k, Graph *graph, int red, int green, int blue)
{
  return setMeterColor(k, graph, "Graph", red, green, blue);
}

QVariantList KarambaInterface::getGraphColor(Karamba *k, Graph *graph)
{
  return getMeterColor(k, graph, "Graph");
}



QObject* KarambaInterface::createImage(Karamba* k, int x, int y, QString image)
{
  if(!checkKaramba(k))
    return NULL;

  ImageLabel *tmp = new ImageLabel(k, x, y, 0, 0);
  tmp->setValue(image);

  k->setSensor(LineParser(image), tmp);

  k->addToGroup(tmp);

  return tmp;
}

bool KarambaInterface::deleteImage(Karamba *k, ImageLabel *image)
{
  if(!checkKarambaAndMeter(k, image, "ImageLabel"))
    return false;

  return k->removeMeter(image);
}

bool KarambaInterface::setImageMinMax(Karamba *k, ImageLabel *image, int min, int max)
{
  return setMeterMinMax(k, image, "ImageLabel", min, max);
}

QVariantList KarambaInterface::getImageMinMax(Karamba *k, ImageLabel *image)
{
  return getMeterMinMax(k, image, "ImageLabel");
}

bool KarambaInterface::moveImage(Karamba *k, ImageLabel *image, int x, int y)
{
  return moveMeter(k, image, "ImageLabel", x, y);
}

QVariantList KarambaInterface::getImagePos(Karamba *k, ImageLabel *image)
{
  return getMeterPos(k, image, "ImageLabel");
}

bool KarambaInterface::setImageSensor(Karamba *k, ImageLabel *image, QString sensor)
{
  return setMeterSensor(k, image, "ImageLabel", sensor);
}

QString KarambaInterface::getImageSensor(Karamba *k, ImageLabel *image)
{
  return getMeterSensor(k, image, "ImageLabel");
}

bool KarambaInterface::resizeImage(Karamba *k, ImageLabel *image, int width, int height)
{
  return resizeMeter(k, image, "ImageLabel", width, height);
}

QVariantList KarambaInterface::getImageSize(Karamba *k, ImageLabel *image)
{
  return getMeterSize(k, image, "ImageLabel");
}

QObject* KarambaInterface::setImagePath(Karamba *k, ImageLabel *image, QString path)
{
  return setMeterStringValue(k, image, "ImageLabel", path);
}

QString KarambaInterface::getImagePath(Karamba *k, ImageLabel *image)
{
  return getMeterStringValue(k, image, "ImageLabel");
}

QObject* KarambaInterface::getThemeImage(Karamba *k, QString meter)
{
  return getThemeMeter(k, meter, "ImageLabel");
}

bool KarambaInterface::hideImage(Karamba *k, ImageLabel *image)
{
  return hideMeter(k, image, "ImageLabel");
}

bool KarambaInterface::showImage(Karamba *k, ImageLabel *image)
{
  return showMeter(k, image, "ImageLabel");
}

bool KarambaInterface::setImageColor(Karamba *k, ImageLabel *image, int red, int green, int blue)
{
  return setMeterColor(k, image, "ImageLabel", red, green, blue);
}

QVariantList KarambaInterface::getImageColor(Karamba *k, ImageLabel *image)
{
  return getMeterColor(k, image, "ImageLabel");
}

bool KarambaInterface::addImageTooltip(Karamba *k, ImageLabel *image, QString text)
{
  if(!checkKarambaAndMeter(k, image, "ImageLabel"))
    return false;

  image->setTooltip(text);
  return true;
}

bool KarambaInterface::changeImageChannelIntensity(Karamba *k, ImageLabel *image, double ratio, QString channel, int ms)
{
  if(!checkKarambaAndMeter(k, image, "ImageLabel"))
    return false;

  image->channelIntensity(ratio, channel, ms);
  return true;
}

bool KarambaInterface::changeImageIntensity(Karamba *k, ImageLabel *image, double ratio, int ms)
{
  if(!checkKarambaAndMeter(k, image, "ImageLabel"))
    return false;

  image->intensity(ratio, ms);
  return true;
}

bool KarambaInterface::changeImageToGray(Karamba *k, ImageLabel *image, int ms)
{
  if(!checkKarambaAndMeter(k, image, "ImageLabel"))
    return false;

  image->toGray(ms);
  return true;
}

QObject* KarambaInterface::createBackgroundImage(Karamba *k, int x, int y, QString imagePath)
{
  if(!checkKaramba(k))
    return 0;

  ImageLabel *tmp = new ImageLabel(k, x, y, 0, 0);
  tmp->setValue(imagePath);
  tmp->setBackground(true);
  k->setSensor(LineParser(imagePath), tmp);

  k->addToGroup(tmp);

  return tmp;
}

QObject* KarambaInterface::createTaskIcon(Karamba *k, int x, int y, int ctask)
{
  if(!checkKaramba(k))
    return NULL;

  QList<Task::TaskPtr> tasks = TaskManager::self()->tasks().values();
  Task::TaskPtr task;
  Task::TaskPtr currTask;
  foreach(task, tasks)
  {
    if ((long)task.data() == (long)ctask)
    {
      //task found
      currTask = task;
      break;
    }
  }

  if (currTask.isNull())
  {
    return 0;
  }

  //retrieve the QPixmap that represents this image
  QPixmap iconPixmap = KWin::icon(currTask->window());

  ImageLabel *tmp = new ImageLabel(k, x, y, 0, 0);
  tmp->setValue(iconPixmap);

  k->addToGroup(tmp);
  return tmp;
}

int KarambaInterface::getImageHeight(Karamba *k, ImageLabel *image)
{
  if(!checkKarambaAndMeter(k, image, "ImageLabel"))
    return -1;

  return image->getHeight();
}

int KarambaInterface::getImageWidth(Karamba *k, ImageLabel *image)
{
  if(!checkKarambaAndMeter(k, image, "ImageLabel"))
    return -1;

  return image->getWidth();
}

bool KarambaInterface::removeImageEffects(Karamba *k, ImageLabel *image)
{
  if(!checkKarambaAndMeter(k, image, "ImageLabel"))
    return false;

  image->removeEffects();
  return true;
}

bool KarambaInterface::removeImageTransformations(Karamba *k, ImageLabel *image)
{
  if(!checkKarambaAndMeter(k, image, "ImageLabel"))
    return false;

  image->removeImageTransformations();
  return true;
}

bool KarambaInterface::resizeImageSmooth(Karamba *k, ImageLabel *image, int width, int height)
{
  if(!checkKarambaAndMeter(k, image, "ImageLabel"))
    return false;

  k->scaleImageLabel(image, width, height);
  return true;
}

bool KarambaInterface::rotateImage(Karamba *k, ImageLabel *image, int deg)
{
  if(!checkKarambaAndMeter(k, image, "ImageLabel"))
    return false;

  image->rotate(deg);
  return true;
}

#include "karambainterface.moc"
