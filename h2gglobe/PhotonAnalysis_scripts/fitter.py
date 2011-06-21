#!/usr/bin/env python

import ROOT
from sys import argv
from python.configProducer import *
import sys

ROOT.gSystem.Load("libRooFit.so")
ROOT.gSystem.Load("libPhysics.so");
ROOT.gSystem.Load("libCore.so");
ROOT.gSystem.Load("../libLoopAll.so");

ROOT.gBenchmark.Start("Analysis");

config_file="datafiles.dat"
if len(sys.argv) > 1:
    config_file = sys.argv[1]

ut = ROOT.LoopAll();
cfg = configProducer(ut,config_file,0)
  
ut.LoopAndFillHistos();
ROOT.gBenchmark.Show("Analysis");

ut.WriteFits();  

if "/" in str(ut.histFileName):
    path,name = str(ut.histFileName).rsplit("/",1)
    ut.histFileName=path+"/histograms_"+name
else:
    ut.histFileName="histograms_"+ut.histFileName
ut.WriteHist();  
ut.WriteCounters();  


ROOT.gROOT.Reset()
