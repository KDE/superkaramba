/*****************************************************************

Copyright (c) 1996-2000,2002 the kicker authors. See file AUTHORS.

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

#ifndef __showdesktop_h__
#define __showdesktop_h__

class KWinModule;

/**
 * Singleton class that handles desktop access (minimizing all windows)
 */
class ShowDesktop : public QObject
{
    Q_OBJECT

public:
    static ShowDesktop* the();
    bool desktopShowing() { return showingDesktop; }

public slots:
    void showDesktop( bool show );
    void toggle() { showDesktop( !desktopShowing() ); }

signals:
    void desktopShown( bool shown );

private slots:
    void slotCurrentDesktopChanged(int);
    void slotWindowChanged(WId w, unsigned int dirty);

private:
    ShowDesktop();

    bool              showingDesktop;
    KWinModule*       kWinModule;
    QValueList<WId>   iconifiedList;

};

#endif
