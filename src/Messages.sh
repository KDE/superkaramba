#! /usr/bin/env bash
### TEMPORARY for KDE4, might extract too much
$EXTRACTRC `find . -name \*.ui -o -name \*.kcfg -o -name \*.rc` >> rc.cpp || exit 11
$XGETTEXT -kaliasLocal *.cpp -o $podir/superkaramba.pot
