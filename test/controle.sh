#!/usr/bin/bash

set -e -x
g++ ../ljsonp.cpp -c --std=c++11
g++ main.cpp -c --std=c++11
g++ main.o ljsonp.o --std=c++11 -o ljsoncppp


./ljsoncppp test1.json sortie1.json
diff -y -W 200 -t test1.json sortie1.json || :


./ljsoncppp test2.json sortie2.json
diff -y -W 200 -t test2.json sortie2.json || :

./ljsoncppp sortie1.json sortie12.json
diff -y -W 200 -t sortie1.json sortie12.json || :

./ljsoncppp sortie2.json sortie22.json
diff -y -W 200 -t sortie2.json sortie22.json || :

      