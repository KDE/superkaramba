#ifndef LINEPARSER_H
#define LINEPARSER_H

#include <QString>
#include <QColor>

class LineParser
{
  public:
    LineParser(const QString& line = QString::null);
    ~LineParser();

    void set(const QString& line);

    int getInt(QString w, int def = 0) const;
    QColor getColor(QString w, QColor def = QColor()) const;
    QString getString(QString w, QString def = QString()) const;
    bool getBoolean(QString w, bool def = false) const;

    const QString& meter() const { return m_meter; };

  private:
    QString m_line;
    QString m_meter;
};

#endif
