/****************************************************************************
** Form interface generated from reading ui file 'themelistwindow.ui'
**
** Created: Wed Jun 30 00:15:39 2004
**      by: The User Interface Compiler ($Id: themelistwindow.h,v 1.2 2004/07/15 20:38:09 kodaaja Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef THEMELISTWINDOW_H
#define THEMELISTWINDOW_H

#include <qvariant.h>
#include <qdialog.h>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class KListBox;
class QListBoxItem;
class QPushButton;
class karamba;

class ThemeListWindow : public QDialog
{
    Q_OBJECT

public:
    ThemeListWindow( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~ThemeListWindow();

    QLabel* textLabel1;
    KListBox* lbxThemes;
    QPushButton* btnOpen;
    QPushButton* btnCloseTheme;
    QPushButton* btnQuit;

    void addTheme(QString appId, QString name);
    void removeTheme(QString appId, QString name);

public slots:
    virtual void closeTheme();
    virtual void themeSelected();
    virtual void quitSuperKaramba();
    virtual void openTheme();

protected:
    QGridLayout* ThemeListWindowLayout;
    QVBoxLayout* layout5;
    QHBoxLayout* layout4;
    QHBoxLayout* layout3;

protected slots:
    virtual void languageChange();

};

#endif // THEMELISTWINDOW_H
