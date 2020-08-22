#!/bin/sh

while read line
do
    rm -fr ${line}
done < .gitignore

cd m4

while read line
do
    rm -fr ${line}
done < .gitignore

cd ..

rm -f src/Makefile.in src/config.h.in
