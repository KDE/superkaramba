/*****************************************************************

Copyright (c) 2005 Pedro Suarez Casal, Vinay Khaitan

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
    m_meter(0),
    rootMet(false)
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
    QString name=qName.toUpper();
    if(name!="SKTHEME" && !rootMet)
    {
        error= "Root Element is not <SKTheme>";
        return false;
    }
    if(name=="SKTHEME")
    {
        rootMet=true;
    }
    else if(name=="KARAMBA")
    {
        return parseKarambaAttributes(attributes); 
    }
    else if(name=="METER")
    {    
        return parseMeterAttributes(attributes);
    }
    else if(name=="DEFAULTFONT")
    {
        return parseDefaultFontAttributes(attributes);
    }
    else if(m_meter)
    {
        if(name=="SENSOR")
        {
            return  parseSensorAttributes(attributes);
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
    return error;
}

bool SKThemeHandler::parseMeterAttributes(const QXmlAttributes& attributes)
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

    m_meter= meter;
    return true;
}

bool SKThemeHandler::parseSensorAttributes(const QXmlAttributes& attributes)
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
    if(sensor)
    {
        m_meter->attachToSensor(sensor);
    }
    return true;
}

bool SKThemeHandler::parseKarambaAttributes(const QXmlAttributes& attr)
{
    int x=0,y=0,w=0,h=0;
    bool locked=true,bottom=false,top=false,left=false,right=false,
                    ontop=false,managed=false,onalldesktops=true,
                    topbar=false,bottombar=false,rightbar=false,leftbar=false;
    QString mask;
    
    for(uint i=0 ;i< attr.count() ; ++i)
    {
        QString qName=attr.qName(i).toUpper();
        QString value=attr.value(i);
        if(qName=="X")
        {
            bool ok;
            x=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert X attribute to Int for karamba";
                return false;
            }
        }
        else if(qName=="Y")
        {
            bool ok;
            y=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert Y attribute to Int for karamba";
                return false;
            }
        }
        else if(qName=="W")
        {
            bool ok;
            w=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert W attribute to Int for karamba" ;
            }
        }
        else if(qName=="H")
        {
            bool ok;
            x=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert H attribute to Int for karamba";
            }
        }
        else if(qName=="LOCKED")
        {
            if(value.toUpper()=="TRUE")
            {
                locked=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                locked=false;
            }
            else
            {
                error = "Locked attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="BOTTOM")
        {
            if(value.toUpper()=="TRUE")
            {
                bottom=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                bottom=false;
            }
            else
            {
                error = "Bottom attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="TOP")
        {
            if(value.toUpper()=="TRUE")
            {
                top=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                top=false;
            }
            else
            {
                error = "Top attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="LEFT")
        {
            if(value.toUpper()=="TRUE")
            {
                left=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                left=false;
            }
            else
            {
                error = "Left attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="RIGHT")
        {
            if(value.toUpper()=="TRUE")
            {
                right=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                right=false;
            }
            else
            {
                error = "Right attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="ONTOP")
        {
            if(value.toUpper()=="TRUE")
            {
                ontop=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                ontop=false;
            }
            else
            {
                error = "Ontop attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="MANAGED")
        {
            if(value.toUpper()=="TRUE")
            {
                managed=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                managed=false;
            }
            else
            {
                error = "Managed attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="ONALLDESKTOPS")
        {
            if(value.toUpper()=="TRUE")
            {
                onalldesktops=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                onalldesktops=false;
            }
            else
            {
                error = "Onalldesktops attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="TOPBAR")
        {
            if(value.toUpper()=="TRUE")
            {
                topbar=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                topbar=false;
            }
            else
            {
                error = "Topbar attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="BOTTOMBAR")
        {
            if(value.toUpper()=="TRUE")
            {
                bottombar=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                bottombar=false;
            }
            else
            {
                error = "Bottombar attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="LEFTBAR")
        {
            if(value.toUpper()=="TRUE")
            {
                leftbar=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                leftbar=false;
            }
            else
            {
                error = "Leftbar attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="RIGHTBAR")
        {
            if(value.toUpper()=="TRUE")
            {
                rightbar=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                rightbar=false;
            }
            else
            {
                error = "Rightbar attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="MASK")
        {
            mask=value; 
        }
    }
    return true;
}

bool SKThemeHandler::parseDefaultFontAttributes(const QXmlAttributes& attr)
{
    QString font,fontSize,color,bgcolor,fixedPitch,shadow,align;
    for(uint i=0 ;i< attr.count() ; ++i)
    {
        QString qName=attr.qName(i).toUpper();
        QString value=attr.value(i);
        if(qName=="FONT") font=value;
        else if(qName=="FONTSIZE") fontSize=value;
        else if(qName=="COLOR") color=value;
        else if(qName=="BGCOLOR") bgcolor=value;
        else if(qName=="FIXEDPITCH") fixedPitch=value;
        else if(qName=="SHADOW") shadow=value;
        else if(qName=="ALIGN") 
        {
            align=value;
            if(align.toLower()!="left" || align.toLower()!="center" ||align.toLower()!="right")
            {
                error="Align attribute should have value only left,center or right";
                return false;
            }
        }
    }
    return true;
}
