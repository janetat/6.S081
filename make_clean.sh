#! /bin/bash

BASE=`pwd`
echo ${BASE}

FILES=$(find . -name "Makefile")

for f in ${FILES}
    do
        cd ${BASE}
        cd `dirname ${f}`
        make clean
    done

echo "success"
