#! /bin/bash
lab_name=lab-$1
git clone git://g.csail.mit.edu/xv6-labs-2022 $lab_name
cd $lab_name
git checkout $1
rm -rf .git
