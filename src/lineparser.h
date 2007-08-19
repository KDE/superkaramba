#ifndef LINEPARSER_H
#define LINEPARSER_H

#include <QColor>

class LineParser
{
public:
    LineParser(const QString& line = QString());
    ~LineParser();

    void set(const QString& line);

    int getInt(const QString &w, int def = 0) const;
    QColor getColor(const QString &w, const QColor &def = QColor()) const;
    QString getString(const QString &w, const QString &def = QString()) const;
    bool getBoolean(const QString &w, bool def = false) const;

    const QString& meter() const
    {
        return m_meter;
    }

private:
    QString m_line;
    QString m_meter;
};

#endif
