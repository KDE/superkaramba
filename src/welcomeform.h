/****************************************************************************
** Form interface generated from reading ui file 'welcomeform.ui'
**
** Created: Sat Jun 26 00:41:37 2004
**      by: The User Interface Compiler ($Id: welcomeform.h,v 1.2 2004/06/26 21:30:28 ageitgey Exp $)
**
** WARNING! All changes made in this file will be lost!
****************************************************************************/

#ifndef WELCOMEFORM_H
#define WELCOMEFORM_H

#include <qvariant.h>
#include <qpixmap.h>
#include <qdialog.h>
#include <kurllabel.h>
#include <qtextbrowser.h>
#include <kio/job.h> 

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QPushButton;

class WelcomeForm : public QDialog
{
    Q_OBJECT

public:
    WelcomeForm( QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0 );
    ~WelcomeForm();

    QLabel* newLabel;
    QLabel* openTextLabel;
    QLabel* helpPixmapLabel;
    QLabel* helpTextLabel;
    QLabel* downloadPixmapLabel;
    QLabel* openPixmapLabel;
    QLabel* downloadTextLabel;
    QTextBrowser* textLabel1;
    QPushButton* exitButton;
    QLabel* titlePixmapLabel;
    KURLLabel* openLabel;
    KURLLabel* downloadLabel;
    KURLLabel* helpLabel;


public slots:
    virtual void exitButton_clicked();
    virtual void helpButton_clicked();
    virtual void downloadButton_clicked();
    virtual void openButton_clicked();
    void downloadResult( KIO::Job * job ) ;
    void linkClicked ( const QString & link );

protected:

protected slots:
    virtual void languageChange();

private:
    QPixmap image0;
    QPixmap image1;
    QPixmap image2;
    QPixmap image3;

};

#endif // WELCOMEFORM_H
