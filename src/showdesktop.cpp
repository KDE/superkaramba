/*****************************************************************

Copyright (c) 1996-2001,2002 the kicker authors. See file AUTHORS.

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

#include <kwinmodule.h>
#include <netwm.h>
#include <kwin.h>

#include "karambaapp.h"
#include "showdesktop.h"
#include "showdesktop.moc"

ShowDesktop* ShowDesktop::the()
{
    static ShowDesktop showDesktop;
    return &showDesktop;
}

ShowDesktop::ShowDesktop()
  : QObject()
  , showingDesktop( false )
  , kWinModule( 0 )
{
    kWinModule = new KWinModule( this );

    // on desktop changes or when a window is deiconified, we abort the show desktop mode
    connect( kWinModule, SIGNAL(currentDesktopChanged(int)),
             SLOT(slotCurrentDesktopChanged(int)));
    connect( kWinModule, SIGNAL(windowChanged(WId,unsigned int)),
             SLOT(slotWindowChanged(WId,unsigned int)));
}

void ShowDesktop::slotCurrentDesktopChanged(int)
{
    showDesktop( false );
}

#ifdef KDE_3_3
#define NET_ALL_TYPES_MASK (NET::AllTypesMask)
#else
#define NET_ALL_TYPES_MASK (-1LU)
#endif

void ShowDesktop::slotWindowChanged(WId w, unsigned int dirty)
{
  if (!showingDesktop)
    return;

  // SELI this needs checking for kwin_iii (_NET_SHOWING_DESKTOP)
  if ( dirty & NET::XAWMState )
  {
    NETWinInfo inf(qt_xdisplay(), w, qt_xrootwin(),
                   NET::XAWMState | NET::WMWindowType);
#ifdef KDE_3_2
    NET::WindowType windowType = inf.windowType(NET_ALL_TYPES_MASK);
#else
    NET::WindowType windowType = inf.windowType();
#endif
    if ((windowType == NET::Normal || windowType == NET::Unknown)
        && inf.mappingState() == NET::Visible )
    {
      // a window was deiconified, abort the show desktop mode.
      iconifiedList.clear();
      showingDesktop = false;
      emit desktopShown( false );
    }
  }
}

void ShowDesktop::showDesktop( bool b )
{
    if( b == showingDesktop ) return;
    showingDesktop = b;

    if ( b ) {
        // this code should move to KWin after supporting NETWM1.2
  iconifiedList.clear();
  const QValueList<WId> windows = kWinModule->windows();
  QValueList<WId>::ConstIterator it;

  for ( it=windows.begin(); it!=windows.end(); ++it ) {
      WId w = *it;
      NETWinInfo info( qt_xdisplay(), w, qt_xrootwin(),
          NET::XAWMState | NET::WMDesktop );
      if ( info.mappingState() == NET::Visible &&
     ( info.desktop() == NETWinInfo::OnAllDesktops
       || info.desktop() == (int) kWinModule->currentDesktop() )
     ) {
    iconifiedList.append( w );
      }
  }
        // find first, hide later, otherwise transients may get minimized
        // with the window they're transient for
        for ( it=iconifiedList.begin(); it!=iconifiedList.end(); ++it ) {
            KWin::iconifyWindow( *it, false );
        }
    } else {
  QValueList<WId>::ConstIterator it;
  for ( it=iconifiedList.begin(); it!=iconifiedList.end(); ++it ) {
      KWin::deIconifyWindow( *it, false  );
  }
    }

    emit desktopShown( showingDesktop );
}
