/*****************************************************************

Copyright (c) 1996-2000 the kicker authors. See file AUTHORS.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

******************************************************************/

#include <kdebug.h>

#include <QtXml/QXmlAttributes>

#include "skthemehandler.h"
#include "karambawidget.h"
#include "cpusensor.h"
#include "disksensor.h"
#include "memsensor.h"
#include "networksensor.h"
#include "sensorsensor.h"
#include "uptimesensor.h"

#include "graph.h"
#include "richtextlabel.h"
#include "bar.h"
#include "meter.h"

SKThemeHandler::SKThemeHandler(KarambaWidget* w) :
    m_widget(w),
    m_meter(0)
{
}

/*
bool SKThemeHandler::notationDecl(const QString& name, const QString& publicId, const QString& systemId)
{
    //cout << name << endl;
}

bool SKThemeHandler::unparsedEntityDecl(const QString& name, const QString& publicId, const QString& systemId, const QString& notationName)
{
}
*/
bool SKThemeHandler::startElement(const QString& /*namespaceURI*/, const QString& /*localName*/, const QString& qName, const QXmlAttributes& attributes)
{				        
    if(qName=="METER")
    {    
        m_meter = parseMeterAttributes(attributes);
    } 
    else if(m_meter)
    {
        if(qName=="SENSOR")
        {
            Sensor* sensor = parseSensorAttributes(attributes);
            if(sensor)
            {
                m_meter->attachToSensor(sensor);
            }
        }
        else if(qName=="ALIGN")
        {
        
        }
	else if(qName=="FORMAT")
	{
            int pos = attributes.index("value");
            if(pos>-1)
            {
                m_meter->setFormat(attributes.value(pos));
            }
	}
	else if(qName=="FONT")
	{
	}
    }

    return true;
}

bool SKThemeHandler::endElement(const QString& /*namespaceURI*/, const QString& /*localName*/, const QString& qName)
{
    if(qName=="METER")
    {
	m_meter = 0;
    }

    return true;
}

QString SKThemeHandler::errorString() const
{
    return "Error!";
}

Meter* SKThemeHandler::parseMeterAttributes(const QXmlAttributes& attributes)
{
    Meter* meter = 0;
    int posValue = attributes.index("value");
    
    if(posValue > -1)
    {
       QString value = attributes.value(posValue);
       if(value=="text")
       {
           meter = new RichTextLabel(m_widget);
           m_widget->addMeter(meter);
       } 
       else if(value=="graph")
       {
           meter = new Graph(m_widget, 0, 0, 0, 0, 20);
           m_widget->addMeter(meter);
       }
       else if(value=="bar")
       {
           meter = new Bar(m_widget, 0, 0, 0, 0);
           m_widget->addMeter(meter);
       }

       int posX = attributes.index("x");
       int posY = attributes.index("y");
       int posW = attributes.index("w");
       int posH = attributes.index("h");
       if(posX  > -1 && posY > -1)
       {
           meter->move(QString(attributes.value(posX)).toInt(), QString(attributes.value(posY)).toInt());
       }

       if(posW > -1 && posH > -1)
       {
           meter->resize(QString(attributes.value(posW)).toInt(), QString(attributes.value(posH)).toInt());
       }
    }

    return meter;
}

Sensor* SKThemeHandler::parseSensorAttributes(const QXmlAttributes& attributes)
{
    Sensor* sensor = 0;

    int posName = attributes.index("name");

    if(posName > -1)
    {
        QString name = attributes.value(posName);
        if(name=="cpu")
        {
            sensor = CPUSensor::self();
        }
        else if(name=="memory")
        {
            sensor = MemSensor::self();
        }
        else if(name=="disk")
        {
            sensor = DiskSensor::self();
        }
        else if(name=="network")
        {
            sensor = NetworkSensor::self();
        }
        else if(name=="sensor")
        {
            sensor = SensorSensor::self();
        }
        else if(name=="uptime")
        {
            sensor = UptimeSensor::self();
        }
    }

    return sensor;
}
