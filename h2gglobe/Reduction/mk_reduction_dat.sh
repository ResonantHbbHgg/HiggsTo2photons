#!/bin/bash

. setup.sh

rm data2012_HCP/*.dat
./AnalysisScripts/mk_reduction_dat.py /store/group/phys_higgs/cmshgg/processed/V14_00_02/data ${storedir}/data data2012_HCP.txt

rm mc_dy_summer12_s10/*.dat
./AnalysisScripts/mk_reduction_dat.py /store/group/phys_higgs/cmshgg/processed/V14_00_02/mc/Summer12_S10_8TeV ${storedir}/mc/Summer12_S10_8TeV mc_dy_summer12_s10.txt

rm mc_bkg_summer12_s7/*.dat
./AnalysisScripts/mk_reduction_dat.py /store/group/phys_higgs/cmshgg/processed/V13_04_00/mc/Summer12_S7_8TeV ${storedir}/mc/Summer12_S7_8TeV mc_bkg_summer12_s7.txt

rm mc_bkg_summer12_s8/*.dat
./AnalysisScripts/mk_reduction_dat.py /store/group/phys_higgs/cmshgg/processed/V13_04_00/mc/Summer12_S8_8TeV ${storedir}/mc/Summer12_S8_8TeV mc_bkg_summer12_s8.txt

rm mc_bkg_summer12_s10/*.dat
./AnalysisScripts/mk_reduction_dat.py /store/group/phys_higgs/cmshgg/processed/V14_00_03/mc/Summer12_S10_8TeV ${storedir}/mc/Summer12_S10_8TeV mc_bkg_summer12_s10.txt

rm mc_sig_summer12_s10/*.dat
./AnalysisScripts/mk_reduction_dat.py /store/group/phys_higgs/cmshgg/processed/V14_00_03/mc/Summer12_S10_8TeV ${storedir}/mc/Summer12_S10_8TeV mc_sig_summer12_s10.txt

tar zcf ${version}.tgz  AnalysisScripts/{common,reduction,aux,*.py}
