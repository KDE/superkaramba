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

#include <QtXml/QXmlDefaultHandler>

class KarambaWidget;
class Meter;
class Sensor;

class SKThemeHandler : public QXmlDefaultHandler
{
public:
    SKThemeHandler(KarambaWidget* w);
    
    bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& attributes);
    bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);
    //bool notationDecl(const QString& name, const QString& publicId, const QString& systemId);
    //bool unparsedEntityDecl(const QString& name, const QString& publicId, const QString& systemId, const QString& notationName);
    QString errorString() const;

private:
    Meter* parseMeterAttributes(const QXmlAttributes& attributes);
    Sensor* parseSensorAttributes(const QXmlAttributes& attributes);
    
    KarambaWidget* m_widget;
    Meter* m_meter;
};
