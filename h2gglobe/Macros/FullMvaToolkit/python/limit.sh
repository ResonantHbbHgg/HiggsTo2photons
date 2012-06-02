#!/bin/bash
# run with $1= directory $2=boost type (grad / ada) $3=$PWD
cd $3
source /vols/cms/grid/setup.sh
eval `scramv1 runtime -sh`
cd $1

for m in {110..150}; do 
  echo -------------------------------
  echo Running $2 $1 limit for mass $m.0
  echo -------------------------------
  combine mva-datacard_$2_$m.0.txt -M AsymptoticNew -D data_$2 -m $m --nPoints=20 --scanMax=4.
  #combine mva-datacard_$2_$m.0.txt -M MaxLikelihoodFit -D data_$2 -m $m --minimizerStrategy=2
  #combine mva-datacard_$2_$m.0.txt -M ProfileLikelihood -D data_$2 -m $m -S 1 --rMin=0. --rMax=25 --signif --pvalue
done

for m in {110..149}; do 
  echo -------------------------------
  echo Running $2 $1 limit for mass $m.5 
  echo -------------------------------
  combine mva-datacard_$2_$m.5.txt -M AsymptoticNew -D data_$2 -m $m.5 --nPoints=20 --scanMax=4.
  #combine mva-datacard_$2_$m.5.txt -M MaxLikelihoodFit -D data_$2 -m $m.5 --minimizerStrategy=2
  #combine mva-datacard_$2_$m.5.txt -M ProfileLikelihood -D data_$2 -m $m.5 -S 1 --rMin=0. --rMax=25 --signif --pvalue
done

mkdir AsymptoticNew MaxLikelihoodFit ProfileLikelihood
for m in {110..150}; do
	mv higgsCombineTest.AsymptoticNew.mH$m.root AsymptoticNew/higgsCombineTest.AsymptoticNew.mH$m.0.root
	#mv higgsCombineTest.MaxLikelihoodFit.mH$m.root MaxLikelihoodFit/higgsCombineTest.MaxLikelihoodFit.mH$m.0.root
	#mv higgsCombineTest.ProfileLikelihood.mH$m.root ProfileLikelihood/higgsCombineTest.ProfileLikelihood.mH$m.0.root
done

for m in {110..149}; do
	mv higgsCombineTest.AsymptoticNew.mH$m.5.root AsymptoticNew/higgsCombineTest.AsymptoticNew.mH$m.5.root
	#mv higgsCombineTest.MaxLikelihoodFit.mH$m.5.root MaxLikelihoodFit/higgsCombineTest.MaxLikelihoodFit.mH$m.5.root
	#mv higgsCombineTest.ProfileLikelihood.mH$m.5.root ProfileLikelihood/higgsCombineTest.ProfileLikelihood.mH$m.5.root
done
cd $3

