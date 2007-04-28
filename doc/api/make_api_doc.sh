#!/bin/bash
# This script converts the comments in karambainterface.cpp
# to the robodoc format and then generates
# the api documentation.
##########################################################
SRCDIR=../../src/
TEMPDIR=".apiHeaders"

if [[ -d $TEMPDIR ]]; then
        echo "Cleaning out existing $TEMPDIR directory"
        rm -rf $TEMPDIR/*
else
        echo "Creating $TEMPDIR directory"
        mkdir -p $TEMPDIR
fi

function replaceComments()
{
        echo "Converting file $1"
        TEMPFILE="temp.cpp"
        cat $1 | sed -e 's/^\/\*\*/\/\/\*\*\*\*p\*/' -e 's/^\*\//\/\/\*\*\*/' -e 's/^\*/\/\//' > $TEMPFILE
        if [[ -s $TEMPFILE ]] ; then
                mv $TEMPFILE $1
        fi
}


FOO=$SRCDIR"karambainterface.cpp"
cp $FOO $TEMPDIR/

BAR=`ls $TEMPDIR`
for FILE in $BAR
do
        replaceComments $TEMPDIR/$FILE
done

echo "Creating api documentation in output file: superkaramba_api."$1

case "$1" in
        html) robodoc --rc api_html.rc;;
        rtf) robodoc --rc api_rtf.rc;;
        *) echo "Usage: make_api_doc {html | rtf}"
        exit 1

esac
