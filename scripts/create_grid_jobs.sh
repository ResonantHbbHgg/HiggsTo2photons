#!/bin/bash

#dbsql "find dataset,sum(block.numevents) where dataset like *Radion*2G2B*nm-madgraph*AODSIM"
#dbsql "find dataset,sum(block.numevents) where dataset like *Graviton*2Gamma*madgraph*AODSIM"

for dataset in `echo "\
/RadionToHH_2Gamma_2b_M-270_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-300_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-350_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-400_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-450_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-500_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-550_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-600_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-650_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-700_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-800_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v3/AODSIM \
/RadionToHH_2Gamma_2b_M-900_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-1100_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-1200_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-1300_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHH_2Gamma_2b_M-1400_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHHTo2G2B_M-300_TuneZ2star_8TeV-nm-madgraph/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHHTo2G2B_M-500_TuneZ2star_8TeV-nm-madgraph/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHHTo2G2B_M-700_TuneZ2star_8TeV-nm-madgraph/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHHTo2G2B_M-1000_TuneZ2star_8TeV-nm-madgraph/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/RadionToHHTo2G2B_M-1500_TuneZ2star_8TeV-nm-madgraph/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/GravitonToHH_2Gamma_2b_M-300_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/GravitonToHH_2Gamma_2b_M-500_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/GravitonToHH_2Gamma_2b_M-700_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/GravitonToHH_2Gamma_2b_M-1000_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/GravitonToHH_2Gamma_2b_M-1500_TuneZ2star_8TeV-Madgraph_pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/GluGluToHTohhTo2Gam2B_mH-260_mh-125_8TeV-pythia6-tauola/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/GluGluToHTohhTo2Gam2B_mH-300_mh-125_8TeV-pythia6-tauola/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/GluGluToHTohhTo2Gam2B_mH-350_mh-125_8TeV-pythia6-tauola/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM \
/GluGluToHHTo2B2G_M-125_8TeV-madgraph-pythia6/Summer12_DR53X-PU_RD1_START53_V7N-v1/AODSIM
"`
do
#dbsql "find dataset,sum(block.numevents) where dataset like ${dataset}"
  name=`echo ${dataset} | cut -d / -f 2`
#PU=`echo ${dataset} | cut -d / -f 3 | cut -d - -f 2 | cut -d _ -f 1`
  datasetpath="${dataset}"
#name=`echo "${name}_${PU}"`
#outdir="group/phys_higgs/cmshgg/processed/V16_00_02/mc/Summer12_POSTLS1_14TeV/${name}"
#outdir="cmst3/user/obondu/H2GGLOBE/Radion/processed/V15_00_05/${name}"
outdir="caf/user/hebda/h2gglobe"
  ev_per_job=2500
  echo -e "dataset= ${datasetpath}\tname= ${name}\toutdir= ${outdir}\tev_per_job=${ev_per_job}"
  echo "./job_maker --MC --AOD --noskim --storage=eos --name=${name} --datasetpath=${datasetpath} --events=-1 --events_per_job=${ev_per_job} --outputdir=${outdir} --globaltag=START53_V7N::All --hlttag=HLT"
  ./job_maker --MC --AOD --noskim --storage=eos --name=${name} --datasetpath=${datasetpath} --events=-1 --events_per_job=${ev_per_job} --outputdir=${outdir} --globaltag=START53_V7N::All --hlttag=HLT --scheduler=remoteGlidein
#break
done



#for data, use storage /store/group/phys_higgs/cmshgg/processed/V15_00_08/mc/ or /store/group/phys_higgs/Resonant_HH/
#/Photon/Run2012A-22Jan2013-v1/AOD
#/DoublePhoton/Run2012B-22Jan2013-v1/AOD
#/DoublePhoton/Run2012C-22Jan2013-v2/AOD
#/DoublePhoton/Run2012D-22Jan2013-v1/AOD
#./job_maker --data --AOD --noskim --storage=eos --name=Photon2012A --datasetpath=/Photon/Run2012A-22Jan2013-v1/AOD --events=-1 --lumis_per_job=5 --outputdir=caf/user/hebda/h2gglobe --globaltag=FT_53_V21_AN6::All --hlttag=HLT --scheduler=remoteGlidein --json=/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions12/8TeV/Reprocessing/Cert_190456-208686_8TeV_22Jan2013ReReco_Collisions12_JSON.txt

