#!/bin/bash
#This script grabs the current *_python.h headers,
# converts them to robodoc format, and then generates 
# the api documentation
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
        TEMPFILE="temp.h"
        cat $1 | sed -e 's/^\/\*\*/\/\/\*\*\*\*p\*/' -e 's/^\*\//\/\/\*\*\*/' -e 's/^\*/\/\//' > $TEMPFILE
        if [[ -s $TEMPFILE ]] ; then
                mv $TEMPFILE $1
        fi
}


FOO=`find $SRCDIR -type f -name "*_python.h"`

for FILE in $FOO
do
        cp $FILE $TEMPDIR/
done

BAR=`ls $TEMPDIR`
for FILE in $BAR
do
        replaceComments $TEMPDIR/$FILE
done

echo "Creating api documentation in output file: python_api.html"
robodoc --rc api_html.rc
