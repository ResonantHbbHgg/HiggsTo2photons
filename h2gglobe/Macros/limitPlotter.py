# Original Authors - Nicholas Wardle, Nancy Marinelli, Doug Berry

# Major cleanup from limit-plotter-complete.py

# Standard Imports and calculators
import ROOT
import array,sys,numpy
ROOT.gROOT.ProcessLine(".L medianCalc.C++")
ROOT.gROOT.ProcessLine(".L ../libLoopAll.so")

from ROOT import medianCalc
from ROOT import FrequentistLimits
from optparse import OptionParser

ROOT.gROOT.SetStyle("Plain")
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptFit(1111)

#-------------------------------------------------------------------------
# Configuration for the Plotter
intlumi = "1.5 (2012) + 5.1 (2011)"
#EXPmasses = [110.,115.,120.,125.,130.,135.,140.,150.]     
OBSmasses = numpy.arange(110,150.5,1.)
EXPmasses = numpy.arange(110,150.5,1.)
#OBSmasses = [110.,115.,120.,125.,130.,135.,140.,150.]

# Plotting Styles --------------------------------------------------------
OFFSETLOW=0
OFFSETHIGH=0

FILLSTYLE=1001
SMFILLSTYLE=3244
FILLCOLOR_95=ROOT.kYellow
FILLCOLOR_68=ROOT.kGreen
RANGEYABS=[0.0,0.6]
RANGEYRAT=[0.0,6]
#-------------------------------------------------------------------------
# UserInput
parser=OptionParser()
parser.add_option("-M","--Method",dest="Method")
parser.add_option("-r","--doRatio",action="store_true")
parser.add_option("-s","--doSmooth",action="store_true")
parser.add_option("-b","--bayes",dest="bayes")
parser.add_option("-o","--outputLimits",dest="outputLimits")
parser.add_option("-e","--expectedOnly",action="store_true")
parser.add_option("-p","--path",dest="path")
parser.add_option("","--massfac",action="store_true",default=False,dest="massfac")
parser.add_option("","--addLine",action="store_true",default=False,dest="addLine")
parser.add_option("","--pval",action="store_true")
parser.add_option("","--is2011",action="store_true",default=False,dest="is2011")
(options,args)=parser.parse_args()
# ------------------------------------------------------------------------
# SM Signal Normalizer
signalNormalizer = ROOT.Normalization_8TeV()
extraString = "SM"
# ------------------------------------------------------------------------
if options.pval: EXPmasses=[]

if options.bayes:
  BayesianFile =ROOT.TFile(options.bayes) 
  bayesObs = BayesianFile.Get("observed")
 
Method = options.Method

EXPName = Method+"/higgsCombineEXPECTED."+Method
if Method == "Asymptotic" or Method == "AsymptoticNew":  EXPName = Method+"/higgsCombineTest."+Method  # everyhting contained here
if Method == "ProfileLikelihood" or Method=="Asymptotic" or Method=="AsymptoticNew":
  OBSName = Method+"/higgsCombineTest."+Method
if Method == "Bayesian":
  OBSName = Method+"/higgsCombineOBSERVED.MarkovChainMC"
if Method == "HybridNew":
  OBSName = Method+"/higgsCombineOBSERVED.HybridNew"

if options.path:
  OBSName = options.path+'/'+OBSName
  EXPName = options.path+'/'+EXPName

if Method == "HybridNew" or Method == "Asymptotic" or Method == "AsymptoticNew": EXPmasses = OBSmasses[:]
# Done with setip
# --------------------------------------------------------------------------
ROOT.gROOT.ProcessLine( \
   "struct Entry{	\
    double r;		\
   };"
)
from ROOT import Entry
def getOBSERVED(file,entry=0):
  try:
   tree = file.Get("limit")
  except:
   return -1
  br = tree.GetBranch("limit")
  c = Entry()
  br.SetAddress(ROOT.AddressOf(c,'r'))
  tree.GetEntry(entry)
  return c.r

if Method=="HybridNew":
  EXPfiles=[]
  EXPmasses = OBSmasses[:]
  for m in EXPmasses:
    if int(m)==m:
      EXPfiles.append(ROOT.TFile(EXPName+".mH%d.quant0.500.root"%m))
    else:
      EXPfiles.append(ROOT.TFile(EXPName+".mH%.1f.quant0.500.root"%m))
  
elif Method=="Asymptotic" or Method=="AsymptoticNew":
  EXPfiles=[]
  EXPmasses = OBSmasses[:]
  for m in EXPmasses:
    if int(m)==m:
      EXPfiles.append(ROOT.TFile(EXPName+".mH%.1f.root"%m))
    else:
      EXPfiles.append(ROOT.TFile(EXPName+".mH%.1f.root"%m))

else:
  EXPfiles = [ROOT.TFile(EXPName+".mH%d.root"%m) for m in EXPmasses]

# Get the observed limits - Currently only does up to 1 decimal mass points
OBSfiles = []
if not options.expectedOnly:
  for m in OBSmasses:
    if int(m)==m:
      OBSfiles.append(ROOT.TFile(OBSName+".mH%.1f.root"%m))
    else:
      OBSfiles.append(ROOT.TFile(OBSName+".mH%.1f.root"%m))
  if Method == "Asymptotic" or Method =="AsymptoticNew":  obs = [getOBSERVED(O,5) for O in OBSfiles] # observed is last entry in these files
  else: obs = [getOBSERVED(O) for O in OBSfiles]
else:
  obs = [0 for O in OBSmasses]
  OBSfiles = obs[:]
#-------------------------------------------------------------------------
# Set-up the GRAPHS
leg=ROOT.TLegend(0.16,0.61,0.57,0.89)
leg.SetFillColor(0)
leg.SetBorderSize(0)

graph68  = ROOT.TGraphAsymmErrors()
graph95  = ROOT.TGraphAsymmErrors()
graphMed = ROOT.TGraphAsymmErrors()
graphObs = ROOT.TGraphAsymmErrors()
graphOne = ROOT.TGraphAsymmErrors()
dummyGraph= ROOT.TGraphAsymmErrors()

graph68up = ROOT.TGraphErrors()
graph68dn = ROOT.TGraphErrors()
graph95up = ROOT.TGraphErrors()
graph95dn = ROOT.TGraphErrors()
graphmede = ROOT.TGraphErrors()

if options.massfac:
  lim2011 = ROOT.TFile.Open("FullMvaToolkit/CombineCards/7TeV_5.1fb/MassFacMVA/limit.root")
else:
  lim2011 = ROOT.TFile.Open("FullMvaToolkit/CombineCards/7TeV_5.1fb/SidebandMVA/limit.root")
graph2011 = lim2011.Get("median")
graph2011.SetLineColor(ROOT.kBlue+2)
graph2011.SetLineStyle(7)
graph2011.SetLineWidth(3)
if options.massfac:
  lim2012 = ROOT.TFile.Open("FullMvaToolkit/CombineCards/8TeV_1.5fb/MassFacMVA/limit.root")
else:
  lim2012 = ROOT.TFile.Open("FullMvaToolkit/CombineCards/8TeV_1.5fb/SidebandMVA/limit.root")
graph2012 = lim2012.Get("median")
graph2012.SetLineColor(ROOT.kMagenta+1)
graph2012.SetLineStyle(7)
graph2012.SetLineWidth(3)

# Stuff for bkg smooth test
regFile = ROOT.TFile.Open("BkgSmoothTest/ZeeReg/limit.root")
noRegFile = ROOT.TFile.Open("BkgSmoothTest/ZeeNoReg/limit.root")

graphRegExp = regFile.Get("median")
graphRegObs = regFile.Get("observed")
graphNoRegExp = noRegFile.Get("median")
graphNoRegObs = noRegFile.Get("observed")

graphRegExp.SetLineColor(ROOT.kRed)
graphRegExp.SetLineStyle(7)
graphRegExp.SetLineWidth(3)
graphRegObs.SetLineColor(ROOT.kRed)
graphRegObs.SetLineWidth(3)

graphNoRegExp.SetLineColor(ROOT.kBlue)
graphNoRegExp.SetLineStyle(7)
graphNoRegExp.SetLineWidth(3)
graphNoRegObs.SetLineColor(ROOT.kBlue)
graphNoRegObs.SetLineWidth(3)

#-------------------------------------------------------------------------
# Different entries for the different methods
LegendEntry = ""
if Method == "ProfileLikelihood": LegendEntry = "PL"
if Method == "Bayesian": LegendEntry = "Bayesian"
if Method == "HybridNew": LegendEntry = "CLs"
if Method == "Asymptotic": LegendEntry = "CLs (Asymptotic)"
if Method == "AsymptoticNew": LegendEntry = "CLs (AsymptoticNew)"

leg.SetTextSize(0.025);
leg.SetHeader("%s"%LegendEntry)
#leg.AddEntry(graphObs,"Baseline","L")
#leg.AddEntry(graphRegObs,"Zee regression","L")
#leg.AddEntry(graphNoRegObs,"Zee no regression","L")
if not options.expectedOnly: leg.AddEntry(graphObs,"Observed","L")
if options.bayes and not options.expectedOnly: leg.AddEntry(bayesObs,"Observed Bayesian Limit","L")
if options.addLine: leg.AddEntry(graphMed,"Expected (combined)","L")
else: leg.AddEntry(graphMed,"Expected","L")
leg.AddEntry(graph68,"#pm 1#sigma","F")
leg.AddEntry(graph95,"#pm 2#sigma","F")
if options.addLine:
  leg.AddEntry(graph2011,"Expected (\sqrt{s}=7TeV L=5.1fb^{-1})","L")
  leg.AddEntry(graph2012,"Expected (\sqrt{s}=8TeV L=1.5fb^{-1})","L")

MG = ROOT.TMultiGraph()

#-------------------------------------------------------------------------
#EXPECTED + Bands
for i,mass,f in zip(range(len(EXPfiles)),EXPmasses,EXPfiles):
  if options.pval: continue
  sm = 1.
  median = array.array('d',[0])
  up68   = array.array('d',[0])
  dn68   = array.array('d',[0])
  up95   = array.array('d',[0])
  dn95   = array.array('d',[0])

  if not options.doRatio: sm = signalNormalizer.GetBR(mass)*signalNormaliser.GetXsection(mass) 
  if Method == "Asymptotic" or Method=="AsymptoticNew":   
      median[0] = getOBSERVED(f,2)
      up95[0]   = getOBSERVED(f,4)
      dn95[0]   = getOBSERVED(f,0)
      up68[0]   = getOBSERVED(f,3)
      dn68[0]   = getOBSERVED(f,1)

  else:
    tree = f.Get("limit")
    medianCalc("r_mH"+str(mass),tree,median,up68,dn68,up95,dn95)

  graph68.SetPoint(i,float(mass),median[0]*sm)
  graph95.SetPoint(i,float(mass),median[0]*sm)
  graphMed.SetPoint(i,float(mass),median[0]*sm)
  graphOne.SetPoint(i,float(mass),1.*sm)
  
  if Method == "HybridNew":

      up95[0]   = FrequentistLimits(f.GetName().replace("0.500.root","0.975.root"))
      dn95[0]   = FrequentistLimits(f.GetName().replace("0.500.root","0.025.root"))
      up68[0]   = FrequentistLimits(f.GetName().replace("0.500.root","0.840.root"))
      dn68[0]   = FrequentistLimits(f.GetName().replace("0.500.root","0.160.root"))

  
  diff95_up = abs(median[0] - up95[0])*sm
  diff95_dn = abs(median[0] - dn95[0])*sm
  diff68_up = abs(median[0] - up68[0])*sm
  diff68_dn = abs(median[0] - dn68[0])*sm
  
  graph68.SetPointError(i,0,0,diff68_dn,diff68_up)
  graph95.SetPointError(i,0,0,diff95_dn,diff95_up)
  graphMed.SetPointError(i,0,0,0,0)
  graphOne.SetPointError(i,0,0,0,0)

  if options.doSmooth:  # Always fit the absolute not the ratio
    sm=1.   

    graphmede.SetPoint(i,float(mass),median[0]*sm)
    graph68up.SetPoint(i,float(mass),up68[0]*sm)
    graph68dn.SetPoint(i,float(mass),dn68[0]*sm)
    graph95up.SetPoint(i,float(mass),up95[0]*sm)
    graph95dn.SetPoint(i,float(mass),dn95[0]*sm)

# Smooth the Bands set -doSmooth
# Since i always fitted to the Absolute, need to see if i want the Ratio instead
if options.doSmooth:
 fitstring = "[0] + [1]*x*x + [2]*x*x*x +[3]*x*x*x*x + [4]*x"
 medfunc = ROOT.TF1("medfunc",fitstring,109.,150.);
 up68func = ROOT.TF1("up68func",fitstring,109.,150.);
 dn68func = ROOT.TF1("dn68func",fitstring,109.,150.);
 up95func = ROOT.TF1("up95func",fitstring,109.,150.);
 dn95func = ROOT.TF1("dn95func",fitstring,109.,150.);

 graphmede.Fit(medfunc,"R,M,EX0","Q")
 graph68up.Fit(up68func,"R,M,EX0","Q")
 graph68dn.Fit(dn68func,"R,M,EX0","Q")
 graph95up.Fit(up95func,"R,M,EX0","Q")
 graph95dn.Fit(dn95func,"R,M,EX0","Q")
 
 newCanvas = ROOT.TCanvas()
 graphmede.SetMarkerSize(0.8)
 graphmede.SetMarkerStyle(21)
 graphmede.Draw("A")
 newCanvas.SaveAs("smoothTest.pdf")

 for i,mass in zip(range(len(EXPmasses)),EXPmasses):

  sm=1.0
  if not options.doRatio:
   sm = signalNormalizer.GetBR(mass)*signalNormalizer.GetXsection(mass)

  mediansmooth = medfunc.Eval(mass)

  graphMed.SetPoint(i,mass,mediansmooth*sm)
  graph68.SetPoint(i,mass,mediansmooth*sm)
  graph95.SetPoint(i,mass,mediansmooth*sm)

  diff95_up = abs(mediansmooth - up95func.Eval(mass))*sm
  diff95_dn = abs(mediansmooth - dn95func.Eval(mass))*sm
  diff68_up = abs(mediansmooth - up68func.Eval(mass))*sm
  diff68_dn = abs(mediansmooth - dn68func.Eval(mass))*sm

  graph68.SetPointError(i,0,0,diff68_dn,diff68_up)
  graph95.SetPointError(i,0,0,diff95_dn,diff95_up)

# OBSERVED -------- easy as that !
for i,mass in zip(range(len(OBSfiles)),OBSmasses):

    sm = 1.;
    if obs[i] ==-1: continue
    if not options.doRatio: sm = signalNormalizer.GetBR(M)*signalNormaliser.GetXsection(M)
    graphObs.SetPoint(i,float(mass),obs[i]*sm)
    graphObs.SetPointError(i,0,0,0,0)

# Finally setup the graphs and plot
graph95.SetFillColor(FILLCOLOR_95)
graph95.SetFillStyle(FILLSTYLE)
graph68.SetFillColor(FILLCOLOR_68)
graph68.SetFillStyle(FILLSTYLE)
graphMed.SetLineStyle(2)
graphMed.SetLineColor(2)
graphMed.SetMarkerColor(2)
graphMed.SetLineWidth(3)
graphObs.SetLineWidth(3)

if options.bayes:
 bayesObs.SetLineWidth(3)
 bayesObs.SetLineColor(4)
 bayesObs.SetMarkerColor(4)
 bayesObs.SetLineStyle(7)

graphOne.SetLineWidth(3)
graphOne.SetLineColor(ROOT.kAzure+7)
graphObs.SetMarkerStyle(20)
graphObs.SetMarkerSize(2.0)
graphObs.SetLineColor(1)

graphMed.SetLineStyle(7)
graphMed.SetLineColor(ROOT.kBlack)
MG.Add(graph95)
MG.Add(graph68)
MG.Add(graphMed)
if options.addLine:
  MG.Add(graph2011)
  MG.Add(graph2012)
#MG.Add(graphBase)
#MG.Add(graphRegExp)
#MG.Add(graphRegObs)
#MG.Add(graphNoRegExp)
#MG.Add(graphNoRegObs)

if not options.expectedOnly:
  MG.Add(graphObs)
  if options.bayes:
   MG.Add(bayesObs)

MG.Add(graphOne)
# Plot -------------------------------------
C = ROOT.TCanvas("#int L = %s"%intlumi,"#int L = %s"%intlumi,1260,1100)
C.SetGrid(True)

dummyHist = ROOT.TH1D("dummy","",1,min(OBSmasses)-OFFSETLOW,max(OBSmasses)+OFFSETHIGH)
dummyHist.Draw("AXIS")
MG.Draw("L3")
dummyHist.Draw("AXIGSAME")

dummyHist.GetXaxis().SetTitle("m_{H} (GeV)")
dummyHist.GetXaxis().SetRangeUser(min(OBSmasses)-OFFSETLOW,max(OBSmasses)+OFFSETHIGH)
if options.doRatio:
 dummyHist.GetYaxis().SetRangeUser(RANGEYRAT[0],RANGEYRAT[1])
 dummyHist.GetYaxis().SetTitle("\sigma(H#rightarrow #gamma #gamma)_{95%%CL} / \sigma(H#rightarrow #gamma #gamma)_{%s}"%extraString)
else: 
 dummyHist.GetYaxis().SetRangeUser(RANGEYABS[0],RANGEYABS[1])
 dummyHist.GetYaxis().SetTitle("\sigma #times BR(H#rightarrow #gamma #gamma)_{95%CL} (pb)")
dummyHist.GetYaxis().SetTitleOffset(1.2)

MG.SetTitle("#int L = %s"%intlumi)
mytext = ROOT.TLatex()
mytext.SetTextSize(0.04)

mytext.SetNDC()
mytext.SetTextSize(0.03)
if options.addLine:
  mytext.DrawLatex(0.58,0.85,"CMS preliminary")
  mytext.DrawLatex(0.58,0.77,"#splitline{#sqrt{s} = 7 TeV L = 5.1 fb^{-1} (2011)}{#sqrt{s} = 8 TeV L = 1.5 fb^{-1} (2012)}")
else:
  if options.is2011: mytext.DrawLatex(0.58,0.77,"#splitline{CMS preliminary}{#sqrt{s} = 7 TeV L = 5.1 fb^{-1}}")
  else: mytext.DrawLatex(0.58,0.77,"#splitline{CMS preliminary}{#sqrt{s} = 8 TeV L = 1.5 fb^{-1}}")
  
leg.Draw()

#Make a bunch of extensions to the plots
if options.doRatio:
 C.SaveAs("limit_%s_%s_ratio.pdf"%(options.doSmooth and "smooth" or "",Method))
 C.SaveAs("limit_%s_%s_ratio.gif"%(options.doSmooth and "smooth" or "",Method))
 C.SaveAs("limit_%s_%s_ratio.eps"%(options.doSmooth and "smooth" or "",Method))
 C.SaveAs("limit_%s_%s_ratio.ps"%(options.doSmooth and "smooth" or "",Method))
else:
 C.SaveAs("limit_%s_%s.pdf"%(options.doSmooth and "smooth" or "",Method))
 C.SaveAs("limit_%s_%s.gif"%(options.doSmooth and "smooth" or "",Method))
 C.SaveAs("limit_%s_%s.eps"%(options.doSmooth and "smooth" or "",Method))
 C.SaveAs("limit_%s_%s.ps"%(options.doSmooth and "smooth" or "",Method))

if options.outputLimits:
  print "Writing Limits To ROOT file --> ",options.outputLimits
  OUTTgraphs = ROOT.TFile(options.outputLimits,"RECREATE")
  graphMed.SetName("median")
  graphMed.Write()
  graphObs.SetName("observed")
  graphObs.Write()
  graph68.SetName("sig1")
  graph68.Write()
  graph95.SetName("sig2")
  graph95.Write()
  OUTTgraphs.Write()

