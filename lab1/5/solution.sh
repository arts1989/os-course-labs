#!/usr/bin/env bash
set -e
mkdir out
cd out
touch me.txt
whoami >> me.txt
cp me.txt metoo.txt
touch wchelp.txt
man wc >> wchelp.txt
cat wchelp.txt
wc -l wchelp.txt | cut -b 1,2 >> wchelp-lines.txt
tac wchelp.txt >> wchelp-reversed.txt
cat me.txt metoo.txt wchelp-lines.txt wchelp-reversed.txt wchelp.txt >> all.txt
tar -cf result.tar $( find -name "*.txt" )
gzip result.tar
cd ..
mv ./out/result.tar.gz ./
rm -r out
