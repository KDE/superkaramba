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
#include "textfield.h"
#include "themelocale.h"
#include <QDesktopWidget>

SKThemeHandler::SKThemeHandler(KarambaWidget* w) :
        m_widget(w),
        m_meter(0),
        rootMet(false),
        defaultTextField(0)
{}

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
    kDebug() << qName << " in startElement" << endl;
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

bool SKThemeHandler::fatalError (const QXmlParseException & exception)
{
  kDebug() << "Fatal error on line: " << exception.lineNumber()
            << ", column: " << exception.columnNumber() << " : "
            << exception.message() << endl;

  return false;
}

bool SKThemeHandler::parseMeterAttributes(const QXmlAttributes& attr)
{
    if(m_meter)
    {
        error="Meters cant be nested";
        return false;
    }
    Meter* returnMeter = 0;
    QString type,path,color,bgcolor,font,align,onclick,format,name,tooltip,rdf,preview,parameter,string;
    int x=0,y=0,w=0,h=0,fontSize=0,min=0,max=100,points=0,
                                       interval=0,pvalue=0,shadow=0;
    bool background=false,fixedPitch=false,vertical=false;
    for(int i=0 ;i< attr.count() ; ++i)
    {
        QString qName=attr.qName(i).toUpper();
        QString value=attr.value(i);
        kDebug() << "qName =" << qName << " value= " << value << endl;
        if(qName=="TYPE")
        {
            type=value.toLower();
        }
        else if(qName=="PATH")
        {
            path=value;
        }
        else if(qName=="COLOR")
        {
            color=value;
        }
        else if(qName=="BGCOLOR")
        {
            bgcolor=value;
        }
        else if(qName=="STRING")
        {
            string=value;
        }
        else if(qName=="FONT")
        {
            font=value.toLower();
        }
        else if(qName=="ALIGN")
        {
            align=value.toLower();
        }
        else if(qName=="ONCLICK")
        {
            onclick=value;
        }
        else if(qName=="FORMAT")
        {
            format=value;
        }
        else if(qName=="NAME")
        {
            name=value;
        }
        else if(qName=="TOOLTIP")
        {
            tooltip=value;
        }
        else if(qName=="RDF")
        {
            rdf=value;
        }
        else if(qName=="PREVIEW")
        {
            preview=value;
        }
        else if(qName=="PARAMETER")
        {
            parameter=value;
        }
        else if(qName=="VALUE")
        {
            bool ok;
            pvalue=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert VALUE attribute to Int for Meter";
                return false;
            }
        }
        else if(qName=="FIXEDPITCH")
        {
            if(value.toUpper()=="TRUE")
            {
                fixedPitch=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                fixedPitch=false;
            }
            else
            {
                error = "FIXEDPITCH attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="BACKGROUND")
        {
            if(value.toUpper()=="TRUE")
            {
                background=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                background=false;
            }
            else
            {
                error = "BACKGROUND attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="VERTICAL")
        {
            if(value.toUpper()=="TRUE")
            {
                vertical=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                vertical=false;
            }
            else
            {
                error = "VERTICAL attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="SHADOW")
        {
            bool ok;
            shadow=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert SHADOW attribute to Int for Meter";
                return false;
            }
        }
        else if(qName=="X")
        {
            bool ok;
            x=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert X attribute to Int for Meter";
                return false;
            }
        }
        else if(qName=="Y")
        {
            bool ok;
            y=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert Y attribute to Int for Meter";
                return false;
            }
        }
        else if(qName=="W")
        {
            bool ok;
            w=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert W attribute to Int for Meter";
                return false;
            }
        }
        else if(qName=="H")
        {
            bool ok;
            h=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert H attribute to Int for Meter";
                return false;
            }
        }
        else if(qName=="FONTSIZE")
        {
            bool ok;
            fontSize=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert FONTSIZE attribute to Int for Meter";
                return false;
            }
        }
        else if(qName=="POINTS")
        {
            bool ok;
            points=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert POINTS attribute to Int for Meter";
                return false;
            }
        }
        else if(qName=="MIN")
        {
            bool ok;
            min=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert MIN attribute to Int for Meter";
                return false;
            }
        }
        else if(qName=="MAX")
        {
            bool ok;
            max=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert MAX attribute to Int for Meter";
                return false;
            }
        }
        else if(qName=="INTERVAL")
        {
            bool ok;
            interval=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert INTERVAL attribute to Int for Meter";
                return false;
            }
        }

    }
    if(type=="bar")
    {
        Bar* meter = new Bar(m_widget, x, y, w, h );
        meter->setImage(path.toAscii());
        meter->setVertical(vertical);
        meter->setMax(max);
        meter->setMin(min);
        meter->setValue(pvalue);
        if (!name.isEmpty())
            meter->setObjectName(name.toAscii());
        meter->setFormat(format.toAscii());
        returnMeter=meter;
    }
    else if(type=="graph")
    {
        kDebug() << "number of points " << points << endl;
        Graph* meter = new Graph(m_widget, x, y, w, h, points);
        meter->setMax(max);
        meter->setMin(min);
        if (!name.isEmpty())
            meter->setObjectName(name.toAscii());
        meter->setColor(color);
        meter->setFormat(format.toAscii());
        returnMeter=meter;
    }
    else if(type=="text" || type =="input")
    {
        TextField defTxt;

        if(defaultTextField)
            defTxt = *defaultTextField;

        TextField* tmpText = new TextField();

        tmpText->setColor(color.isEmpty()? defTxt.getColor():color);
        tmpText->setBGColor(bgcolor.isEmpty()? defTxt.getBGColor():bgcolor);
        tmpText->setFont(font.isEmpty()? defTxt.getFont() : font );
        tmpText->setFontSize(fontSize? fontSize: defTxt.getFontSize());
        tmpText->setAlignment(align.isEmpty()? defTxt.getAlignmentAsString():align);
        tmpText->setFixedPitch(fixedPitch? fixedPitch:defTxt.getFixedPitch());
        tmpText->setShadow(shadow?shadow:defTxt.getShadow());


        if(type == "text")
        {
            RichTextLabel* meter = new RichTextLabel(m_widget, x, y, w, h);

            meter->setText(m_widget-> m_theme.locale()->translate(string.toAscii()));
            meter->setTextProps( tmpText );
            meter->resize(w,h);
            if (!name.isEmpty())
                meter->setObjectName(name.toAscii());
            meter->setFormat(format.toAscii());
            returnMeter=meter;
        }

        else if(type == "INPUT")
        {
            Input* meter = new Input(m_widget, x, y, w, h);
            if (!name.isEmpty())
                meter->setObjectName(name.toAscii());

            meter->setTextProps(tmpText);
            meter->setValue(m_widget->m_theme.locale()->translate(string.toAscii()));
            meter->setFormat(format.toAscii());
            //             m_widget->passive = false;
            returnMeter=meter;
        }
    }
    else if(type=="image")
    {
        ImageLabel *meter = new ImageLabel(m_widget, x, y, 0, 0);
        meter->setValue(path);
        meter->setBackground(background);
        if (!name.isEmpty())
            meter->setObjectName(name.toAscii());
        meter->setFormat(format.toAscii());
        if (!tooltip.isEmpty())
            meter->setTooltip(tooltip);
        returnMeter=meter;

    }
    m_meter= returnMeter;
    return true;
}

bool SKThemeHandler::parseSensorAttributes(const QXmlAttributes& attr)
{
    Sensor* sensor = 0;
    for(int i=0 ;i< attr.count() ; ++i)
    {
        QString qName=attr.qName(i).toUpper();
        QString value=attr.value(i);
        if(qName=="TYPE")
        {
            value=value.toLower();

            if(value=="cpu")
            {
                sensor = CPUSensor::self();
            }
            else if(value=="memory")
            {
                sensor = MemSensor::self();
            }
            else if(value=="disk")
            {
                sensor = DiskSensor::self();
            }
            else if(value=="network")
            {
                sensor = NetworkSensor::self();
            }
            else if(value=="sensor")
            {
                sensor = SensorSensor::self();
            }
            else if(value=="uptime")
            {
                sensor = UptimeSensor::self();
            }
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
    int x=0,y=0,w=300,h=300;
    bool locked=true,bottom=false,top=false,left=false,right=false,
         ontop=false,managed=false,onalldesktops=true,
         topbar=false,bottombar=false,rightbar=false,leftbar=false;
    QString mask;

    for(int i=0 ;i< attr.count() ; ++i)
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
            h=value.toInt(&ok,0);
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
    m_widget->toggleLocked->setChecked(locked);
    m_widget->slotToggleLocked();
    m_widget->setFixedSize(w,h);
    if(right)
    {
        QDesktopWidget *d = QApplication::desktop();
        x = d->width() - w;
    }
    else if(left)
    {
        x = 0;
    }

    if(bottom)
    {
        QDesktopWidget *d = QApplication::desktop();
        y = d->height() - h;
    }
    else if(top)
    {
        y = 0;
    }
    m_widget->move(x,y);
    if(ontop)
    {
        m_widget->onTop = true;
        KWin::setState( m_widget->winId(), NET::StaysOnTop );
    }

    if(managed)
    {
        m_widget->managed = true;
        m_widget->setParent(0, Qt::Dialog);
        m_widget->setAttribute(Qt::WA_DeleteOnClose);
    }
    else
    {
        m_widget->info->setState( NETWinInfo::SkipTaskbar
                                  | NETWinInfo::SkipPager,NETWinInfo::SkipTaskbar
                                  | NETWinInfo::SkipPager );
        if (ontop)
        {
            KWin::setState(m_widget->winId(), NET::StaysOnTop );
        }
    }
    if (onalldesktops)
    {
        //desktop = 200; // ugly
    }

    if(topbar)
    {
        m_widget->move(x,0);
        KWin::setStrut( m_widget->winId(), 0, 0, h, 0 );
        m_widget->toggleLocked->setChecked( true );
        m_widget->slotToggleLocked();
        m_widget->toggleLocked->setEnabled(false);

    }

    if(bottombar)
    {
        int dh = QApplication::desktop()->height();
        m_widget->move( x, dh - h );
        KWin::setStrut( m_widget->winId(), 0, 0, 0, h );
        m_widget->toggleLocked->setChecked( true );
        m_widget->slotToggleLocked();
        m_widget->toggleLocked->setEnabled(false);
    }

    if(rightbar)
    {
        int dw = QApplication::desktop()->width();
        m_widget->move( dw - w, y );
        KWin::setStrut( m_widget->winId(), 0, w, 0, 0 );
        m_widget->toggleLocked->setChecked( true );
        m_widget->slotToggleLocked();
        m_widget->toggleLocked->setEnabled(false);
    }

    if(leftbar)
    {
        m_widget->move( 0, y );
        KWin::setStrut( m_widget->winId(), w, 0, 0, 0 );
        m_widget->toggleLocked->setChecked( true );
        m_widget->slotToggleLocked();
        m_widget->toggleLocked->setEnabled(false);
    }

    QFileInfo fileInfo(mask);
    QString absPath;
    QBitmap bmMask;
    QByteArray ba;
    if( fileInfo.isRelative() )
    {
        absPath.setAscii(m_widget->m_theme.path().toAscii());
        absPath.append(mask.toAscii());
        ba = m_widget->m_theme.readThemeFile(mask);
    }
    else
    {
        absPath.setAscii(mask.toAscii());
        ba = m_widget->m_theme.readThemeFile(fileInfo.fileName());
    }
    if(m_widget->m_theme.isZipTheme())
    {
        bmMask.loadFromData(ba);
    }
    else
    {
        bmMask.load(absPath);
    }
    m_widget->setMask(bmMask);

    return true;
}

bool SKThemeHandler::parseDefaultFontAttributes(const QXmlAttributes& attr)
{
    QString font,color,bgcolor,align;
    int fontSize=0,shadow=0;
    bool fixedPitch=false;
    for(int i=0 ;i< attr.count() ; ++i)
    {
        QString qName=attr.qName(i).toUpper();
        QString value=attr.value(i);
        if(qName=="FONT")
            font=value;
        else if(qName=="FONTSIZE")
        {
            bool ok;
            fontSize=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert FONTSIZE attribute to Int for Meter";
                return false;
            }
        }
        else if(qName=="COLOR")
            color=value;
        else if(qName=="BGCOLOR")
            bgcolor=value;
        else if(qName=="FIXEDPITCH")
        {
            if(value.toUpper()=="TRUE")
            {
                fixedPitch=true;
            }
            else if(value.toUpper()=="FALSE")
            {
                fixedPitch=false;
            }
            else
            {
                error = "FIXEDPITCH attribute should be either false for true for karamba";
                return false;
            }
        }
        else if(qName=="SHADOW")
        {
            bool ok;
            shadow=value.toInt(&ok,0);
            if(!ok)
            {
                error= "could not convert SHADOW attribute to Int for Meter";
                return false;
            }
        }
        else if(qName=="ALIGN")
        {
            align=value.toLower();
            if(align!="left" || align!="center" ||align!="right")
            {
                error="Align attribute should have value only left,center or right";
                return false;
            }
        }
    }
    delete defaultTextField;
    defaultTextField = new TextField( );

    defaultTextField->setColor(color.isEmpty() ? "black" : color );
    defaultTextField->setBGColor(bgcolor.isEmpty() ? "white" : bgcolor);
    defaultTextField->setFont(font.isEmpty()? "Helvetica" :font);
    defaultTextField->setFontSize(fontSize? fontSize :12 );
    defaultTextField->setAlignment(align.isEmpty()? "left" : align);
    defaultTextField->setFixedPitch(fixedPitch? fixedPitch: false);
    defaultTextField->setShadow(shadow? shadow: 0);
    return true;
}
