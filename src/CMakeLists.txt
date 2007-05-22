########### superkaramba lib ###############
SET(superkaramba_LIB_SRCS
karamba.cpp
mainwidget.cpp
lineparser.cpp
themefile.cpp
themelocale.cpp
taskmanager.cpp
showdesktop.cpp
karambainterface.cpp

meters/meter.cpp
meters/bar.cpp
meters/textlabel.cpp
meters/imagelabel.cpp
meters/graph.cpp
meters/textfield.cpp
meters/richtextlabel.cpp
meters/input.cpp
meters/clickarea.cpp
meters/clickmap.cpp

sensors/sensor.cpp
sensors/date.cpp
sensors/mem.cpp
sensors/uptime.cpp
sensors/cpu.cpp
sensors/network.cpp
sensors/program.cpp
sensors/disk.cpp
sensors/sensorparams.cpp
sensors/lmsensor.cpp
sensors/textfile.cpp
sensors/rss.cpp

python/karamba.cpp
python/meter.cpp
python/bar.cpp
python/textlabel.cpp
python/imagelabel.cpp
python/graph.cpp
python/richtextlabel.cpp
python/input.cpp
python/misc.cpp
python/svcgrp.cpp
python/menu.cpp
#python/systray.cpp
python/widget.cpp
python/task.cpp
python/config.cpp
)

KDE4_ADD_KCFG_FILES(superkaramba_LIB_SRCS
superkarambasettings.kcfgc
)

set(kdesktop_xml ${DBUS_INTERFACES_DIR}/org.kde.kdesktop.Background.xml)
QT4_ADD_DBUS_INTERFACE(superkaramba_LIB_SRCS ${kdesktop_xml} kdesktop_background_interface)

KDE4_AUTOMOC(${superkaramba_LIB_SRCS})

KDE4_ADD_LIBRARY(superkaramba SHARED ${superkaramba_LIB_SRCS})
set_target_properties(superkaramba PROPERTIES VERSION 1.0.0 SOVERSION 1 )

TARGET_LINK_LIBRARIES(superkaramba
${KDE4_KIO_LIBS}
${KDE4_KROSSCORE_LIBS}
${PYTHON_LIBRARIES}
)

install(TARGETS superkaramba DESTINATION ${LIB_INSTALL_DIR})

########### superkaramba bin ###############

SET(superkaramba_BIN_SRCS
main.cpp
karambaapp.cpp
themesdlg.cpp
themewidget.cpp
kwidgetlistbox.cpp
#sknewstuff.cpp
karambasessionmanaged.cpp
)

KDE4_ADD_UI_FILES(superkaramba_BIN_SRCS
themewidget_layout.ui
themes_layout.ui
)

KDE4_ADD_KCFG_FILES(superkaramba_BIN_SRCS
superkarambasettings.kcfgc
)

QT4_ADD_DBUS_ADAPTOR(superkaramba_BIN_SRCS org.kde.superkaramba.xml karambaapp.h KarambaApplication)

KDE4_AUTOMOC(${superkaramba_BIN_SRCS})

KDE4_ADD_EXECUTABLE(superkaramba-bin ${superkaramba_BIN_SRCS})

TARGET_LINK_LIBRARIES(superkaramba-bin
${KDE4_KNEWSTUFF2_LIBS}
superkaramba
)

install(TARGETS superkaramba-bin DESTINATION ${BIN_INSTALL_DIR})
install(FILES superkarambaui.rc DESTINATION ${DATA_INSTALL_DIR}/superkaramba)
install(FILES superkaramba.desktop  DESTINATION ${XDG_APPS_DIR})
install(FILES org.kde.superkaramba.xml DESTINATION ${DBUS_INTERFACES_DIR})
install(FILES superkaramba.knsrc DESTINATION ${CONFIG_INSTALL_DIR})

