#include "pyapi_templates.h"

long getMeter(long widget, char* name) {
  if (!widget)
    return 0;
  if (!((QObject*)widget)->isA("karamba"))
    return 0;

 karamba* theme = (karamba*)widget;
 QObjectListIt it( *theme->meterList ); // iterate over meters

 while ( it != 0 )
 {
   if (strcmp(((Meter*) *it)->name(), name) == 0)
     return (long)*it;
   ++it;
 }
 return 0;
}

PyObject* toPyObject(QPoint& p)
{
  return Py_BuildValue("(i,i)", p.x(), p.y());
}

PyObject* toPyObject(int i)
{
  return Py_BuildValue("i", i);
}

PyObject* toPyObject(QString s)
{
  return Py_BuildValue("s", s.ascii());
}

PY_API_BASIC(Bar, Bar)

PY_API_METER_CREATE(createBar, Bar)
  Bar *tmp = new Bar(x,y,w,h);
  tmp->setThemePath(theme->themePath);
  if (!s.isEmpty())
    tmp->setImage(s);
  theme->meterList->append(tmp);
PY_API_RETURN((long)tmp)

PY_API_METER_0(deleteBar, Bar)
  theme->deleteMeterFromSensors(meter);
  long res = theme->meterList->removeRef(meter);
  //meterList is autoDelete(true) so we don't have to delete the bar here
PY_API_RETURN(res)

PY_API_METER_0(getBarImage, Bar)
  QString path = meter->getImage();
PY_API_RETURN(path)

PY_API_METER_S(setBarImage, Bar)
  long res = (long) meter->setImage(s);
PY_API_RETURN(res)

PY_API_METER_0(getBarVertical, Bar)
  long ori = (long)meter->getVertical();
PY_API_RETURN(ori)

PY_API_METER_L(setBarVertical, Bar)
  meter->setVertical(l);
PY_API_RETURN(1)

PY_API_METER_0(getBarValue, Bar)
  long val = (long)meter->getBarValue();
PY_API_RETURN(val)

PY_API_METER_L(setBarValue, Bar)
  meter->setValue(l);
PY_API_RETURN(1)

PY_API_METER_0(getBarMinMax, Bar)
  QPoint minmax(meter->getMin(), meter->getMax());
PY_API_RETURN(minmax)

PY_API_METER_P(setBarMinMax, Bar)
  meter->setMin(point.x());
  meter->setMax(point.y());
PY_API_RETURN(1)

//PY_API_BASIC(Image, ImageLabel)
//PY_API_BASIC(Text, TextLabel)
