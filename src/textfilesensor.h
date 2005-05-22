
#ifndef TEXTFILESENSOR_H
#define TEXTFILESENSOR_H


#include <sensor.h>
#include <qfile.h>
#include <qtextstream.h>
#include <qstring.h>
#include <qtextcodec.h>
#include <qvaluevector.h>
/**
 *
 * Hans Karlsson
 **/
class TextFileSensor : public Sensor
{

Q_OBJECT
public:
    TextFileSensor( QString fileName, bool rdf, int interval, QString encoding="" );

    ~TextFileSensor();

    void update();

private:
QTextCodec *codec;
QString fileName;
bool rdf;
};

#endif // TEXTFILESENSOR_H
