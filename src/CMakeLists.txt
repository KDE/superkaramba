INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}  )

########### next target ###############

SET(superkaramba_SRCS
main.cpp
karamba.cpp
mainwidget.cpp
karambaapp.cpp
lineparser.cpp
themefile.cpp
themelocale.cpp
themesdlg.cpp
themewidget.cpp
kwidgetlistbox.cpp
taskmanager.cpp
#systemtray.cpp
showdesktop.cpp
sknewstuff.cpp
karambasessionmanaged.cpp
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

KDE4_ADD_UI_FILES(superkaramba_SRCS
themewidget_layout.ui
themes_layout.ui
)

KDE4_ADD_KCFG_FILES(superkaramba_SRCS
superkarambasettings.kcfgc
)

QT4_ADD_DBUS_ADAPTOR(superkaramba_SRCS org.kde.superkaramba.xml karambaapp.h KarambaApplication)

set(kdesktop_xml ${DBUS_INTERFACES_DIR}/org.kde.kdesktop.Background.xml)
QT4_ADD_DBUS_INTERFACE(superkaramba_SRCS ${kdesktop_xml} kdesktop_background_interface)

KDE4_AUTOMOC(${superkaramba_SRCS})

KDE4_ADD_EXECUTABLE(superkaramba ${superkaramba_SRCS})

TARGET_LINK_LIBRARIES(superkaramba 
${QT_AND_KDECORE_LIBS} 
${KDE4_KDEUI_LIBRARY}
${KDE4_KNEWSTUFF_LIBS}
${KDE4_KIO_LIBS}
${KDE4_KROSSCORE_LIBS}
${PYTHON_LIBRARIES}
#${QT_QTOPENGL_LIBRARY}
)

install(TARGETS superkaramba DESTINATION ${BIN_INSTALL_DIR})


########### install files ###############

install(FILES superkarambaui.rc DESTINATION ${DATA_INSTALL_DIR}/superkaramba)
install(FILES superkaramba.desktop  DESTINATION ${XDG_APPS_DIR})
install(FILES org.kde.superkaramba.xml DESTINATION ${DBUS_INTERFACES_DIR})
