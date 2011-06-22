#!/bin/sh

mkdir outputToy
echo "max events from CRAB: $MaxEvents"
n="$MaxEvents"
./combine 130GeVmodel.root -M MarkovChainMC -D data_mass -m 130 -t $n -s -1 --generateBinnedWorkaround -S 1 -H ProfileLikelihood --rMin=0 --rMax=40 --iteration 50000 --tries 100 --hintStatOnly
rm CMS-HGG.root
rm 130GeVmodel.root
mv *.root outputToy/
rm *.root
tar cvfz outputToy.tgz outputToy/
