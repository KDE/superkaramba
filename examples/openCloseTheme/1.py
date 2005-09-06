import dcop
import dcopext
import karamba
from qt import QString, QCString

def closeTheme(theme):
    dc = dcop.DCOPClient()
    dc.attach()
    dc.registerAs('sk_script')
    apps = dc.registeredApplications()
    for app in apps:
        s = str(app)
        if s[:12] == 'superkaramba':
            d = dcopext.DCOPApp(s, dc)
            d.KarambaIface.closeTheme(theme)

def initWidget(widget):
    karamba.openTheme('2.theme')

def widgetClicked(widget, x, y, button):
    closeTheme('1')

