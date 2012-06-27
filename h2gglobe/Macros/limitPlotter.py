# Original Authors - Nicholas Wardle, Nancy Marinelli, Doug Berry

# Major cleanup from limit-plotter-complete.py
# Standard Imports and calculators
import ROOT
import array,sys,numpy
ROOT.gROOT.ProcessLine(".L medianCalc.C++")
ROOT.gROOT.ProcessLine(".x tdrstyle.cc")

from ROOT import medianCalc
from ROOT import FrequentistLimits
from optparse import OptionParser

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)
ROOT.gStyle.SetOptFit(0)

#-------------------------------------------------------------------------
# Configuration for the Plotter
OBSmasses = []
EXPmasses = []

OBSmassesT = numpy.arange(110,150.5,0.5)
EXPmassesT = numpy.arange(110,150.5,0.5)

for i in OBSmassesT:
	if i == 145 or i==145.5: continue
	OBSmasses.append(i)
	EXPmasses.append(i)

# Plotting Styles --------------------------------------------------------
OFFSETLOW=0
OFFSETHIGH=0
FONTSIZE=0.027
FILLSTYLE=1001
SMFILLSTYLE=3244
FILLCOLOR_95=ROOT.kYellow
FILLCOLOR_68=ROOT.kGreen
RANGEYABS=[0.0,0.6]
RANGEYRAT=[0.0,4]
MINPV = 10E-9
MAXPV = 1.0
Lines = [1.,2.,3.,4.,5.,6.]
MINMH=int(min(EXPmasses))
MAXMH=int(max(EXPmasses))
#-------------------------------------------------------------------------
# UserInput
parser=OptionParser()
parser.add_option("-M","--Method",dest="Method")
parser.add_option("-r","--doRatio",action="store_true")
parser.add_option("-s","--doSmooth",action="store_true")
parser.add_option("-b","--bayes",dest="bayes")
parser.add_option("-o","--outputLimits",dest="outputLimits")
parser.add_option("-e","--expectedOnly",action="store_true")
parser.add_option("-p","--path",dest="path",default="",type="str")
parser.add_option("","--pval",action="store_true")
parser.add_option("","--lumistr",type="str",default="L = 5.1 fb^{-1} (2011) + 5.1 fb^{-1} (2012)")
parser.add_option("","--egrstr",type="str",default="#sqrt{s} = 7-8 TeV")
(options,args)=parser.parse_args()

# ------------------------------------------------------------------------
# SM Signal Normalizer
if not options.doRatio:
	ROOT.gROOT.ProcessLine(".L ../libLoopAll.so")
	signalNormalizer = ROOT.Normalization_8TeV()
extraString = "SM"
# ------------------------------------------------------------------------
if options.pval:
	 EXPmasses=[]
	 options.doRatio=True

if options.bayes:
  BayesianFile =ROOT.TFile(options.bayes) 
  bayesObs = BayesianFile.Get("observed")
 
Method = options.Method
if not options.path: options.path=Method

EXPName = options.path+"/higgsCombineEXPECTED."+Method
if Method == "Asymptotic" or Method == "AsymptoticNew":  EXPName = options.path+"/higgsCombineTest."+Method  # everyhting contained here
if Method == "ProfileLikelihood" or Method=="Asymptotic" or Method=="AsymptoticNew":
  OBSName = options.path+"/higgsCombineTest."+Method
if Method == "Bayesian":
  OBSName = options.path+"/higgsCombineOBSERVED.MarkovChainMC"
if Method == "HybridNew":
  OBSName = options.path+"/higgsCombineOBSERVED.HybridNew"


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
      EXPfiles.append(ROOT.TFile(EXPName+".mH%d.root"%m))
    else:
      EXPfiles.append(ROOT.TFile(EXPName+".mH%.1f.root"%m))

else:
  EXPfiles = [ROOT.TFile(EXPName+".mH%d.root"%m) for m in EXPmasses]

# Get the observed limits - Currently only does up to 1 decimal mass points
OBSfiles = []
if not options.expectedOnly:
  for m in OBSmasses:
    if int(m)==m:
      OBSfiles.append(ROOT.TFile(OBSName+".mH%d.root"%m))
    else:
      OBSfiles.append(ROOT.TFile(OBSName+".mH%.1f.root"%m))
  if Method == "Asymptotic" or Method =="AsymptoticNew":  obs = [getOBSERVED(O,5) for O in OBSfiles] # observed is last entry in these files
  else: obs = [getOBSERVED(O) for O in OBSfiles]
else:
  obs = [0 for O in OBSmasses]
  OBSfiles = obs[:]

# Set-up the GRAPHS

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

graph68.SetLineColor(1)
graph95.SetLineColor(1)
graph68.SetLineStyle(2)
graph95.SetLineStyle(2)
graph68.SetLineWidth(2)
graph95.SetLineWidth(2)


MG = ROOT.TMultiGraph()
#-------------------------------------------------------------------------
def MakePvalPlot(MG):

	legend=ROOT.TLegend(0.65,0.15,0.89,0.3)
	legend.SetFillColor(10)
	legend.SetTextFont(42)
	legend.SetTextSize(FONTSIZE)
	legend.AddEntry(graphObs,"Observed","L")

	c = ROOT.TCanvas("c","c",600,600)

	dhist = ROOT.TH1F("dh","dh",100,MINMH,MAXMH)
	dhist.GetYaxis().SetTitleOffset(1.4)
	dhist.GetXaxis().SetTitleOffset(1.2)
	dhist.GetYaxis().SetTitleSize(0.04)
	dhist.GetXaxis().SetTitleSize(0.04)
	dhist.GetYaxis().SetLabelSize(0.04)
	dhist.GetXaxis().SetLabelSize(0.04)
	dhist.GetXaxis().SetRangeUser(MINMH,MAXMH)
	dhist.GetYaxis().SetRangeUser(MINPV,MAXPV)
	dhist.GetXaxis().SetTitle("m_{H} (GeV)")
	dhist.GetYaxis().SetTitle("Local p-value")
	dhist.Draw("AXIS")

	MG.Draw("L")

	c.Update()
	text = ROOT.TLatex()
	text.SetTextColor(ROOT.kRed)
	text.SetTextSize(FONTSIZE)
	text.SetTextFont(42)

	Vals=[ROOT.RooStats.SignificanceToPValue(L) for L in Lines]
	TLines=[ROOT.TLine(MINMH,V,MAXMH,V) for V in Vals]

	for j,TL in enumerate(TLines):
		TL.SetLineStyle(1)
		TL.SetLineColor(2)
		TL.SetLineWidth(1)
		TL.Draw("same")
		text.DrawLatex(MAXMH+0.2,Vals[j]*0.88,"%d #sigma"%Lines[j])

	mytext= ROOT.TLatex()
	mytext.SetTextSize(FONTSIZE)
	mytext.SetTextFont(42)
	mytext.SetNDC()

	mytext.DrawLatex(0.18,0.22,"#splitline{CMS Preliminary, %s}{%s}"%(options.egrstr,options.lumistr))
	legend.Draw()
	c.SetLogy()
	ROOT.gPad.RedrawAxis();
	c.SaveAs("pvaluesplot.pdf")
	c.SaveAs("pvaluesplot.png")

def MakeLimitPlot(MG):

	leg=ROOT.TLegend(0.53,0.66,0.89,0.90)
	leg.SetFillColor(10)

	# Different entries for the different methods
	LegendEntry = ""
	if Method == "ProfileLikelihood": LegendEntry = "PL"
	if Method == "Bayesian": LegendEntry = "Bayesian"
	if Method == "HybridNew": LegendEntry = "CLs"
	if Method == "Asymptotic": LegendEntry = "CLs (Asymptotic)"
	if Method == "AsymptoticNew": LegendEntry = "CLs (Asymptotic)"

	if not options.expectedOnly: leg.AddEntry(graphObs,"Observed","L")
	if options.bayes and not options.expectedOnly: leg.AddEntry(bayesObs,"Observed Bayesian Limit","L")
	leg.AddEntry(graph68,"Expected #pm 1#sigma","FL")
	leg.AddEntry(graph95,"Expected #pm 2#sigma","FL")

	leg.SetTextFont(42)
	leg.SetTextSize(FONTSIZE)

	C = ROOT.TCanvas("c","c",600,600)
#	C.SetGrid(True)

	dummyHist = ROOT.TH1D("dummy","",1,min(OBSmasses)-OFFSETLOW,max(OBSmasses)+OFFSETHIGH)
	dummyHist.SetTitleSize(0.04,"XY")
	dummyHist.Draw("AXIS")
	MG.Draw("L3")
	#dummyHist.Draw("AXIGSAME")

	dummyHist.GetXaxis().SetTitle("m_{H} (GeV)")
	dummyHist.GetXaxis().SetRangeUser(min(OBSmasses)-OFFSETLOW,max(OBSmasses)+OFFSETHIGH)
	if options.doRatio:
	 dummyHist.GetYaxis().SetRangeUser(RANGEYRAT[0],RANGEYRAT[1])
	 dummyHist.GetYaxis().SetTitle("\sigma(H#rightarrow #gamma #gamma)_{95%%CL} / \sigma(H#rightarrow #gamma #gamma)_{%s}"%extraString)
	else: 
	 dummyHist.GetYaxis().SetRangeUser(RANGEYABS[0],RANGEYABS[1])
	 dummyHist.GetYaxis().SetTitle("\sigma #times BR(H#rightarrow #gamma #gamma)_{95%CL} (pb)")

	dummyHist.GetYaxis().SetTitleOffset(1.2)
	dummyHist.GetXaxis().SetTitleOffset(1.25)

	MG.SetTitle("")
	mytext = ROOT.TLatex()
	mytext.SetTextSize(FONTSIZE)

	mytext.SetNDC()
	mytext.SetTextFont(42)
	mytext.SetTextSize(FONTSIZE)
	mytext.DrawLatex(0.14,0.85,"CMS Preliminary, %s"%options.egrstr)
	mytext.DrawLatex(0.14,0.77,"%s"%(options.lumistr))
  
	leg.Draw()
	ROOT.gPad.RedrawAxis();

	#Make a bunch of extensions to the plots
	if options.doRatio:
	 C.SaveAs("limit_%s_%s_ratio.pdf"%(options.doSmooth and "smooth" or "",Method))
	 C.SaveAs("limit_%s_%s_ratio.png"%(options.doSmooth and "smooth" or "",Method))
	 C.SaveAs("limit_%s_%s_ratio.gif"%(options.doSmooth and "smooth" or "",Method))
	 C.SaveAs("limit_%s_%s_ratio.eps"%(options.doSmooth and "smooth" or "",Method))
	 C.SaveAs("limit_%s_%s_ratio.ps"%(options.doSmooth and "smooth" or "",Method))
	else:
	 C.SaveAs("limit_%s_%s.pdf"%(options.doSmooth and "smooth" or "",Method))
	 C.SaveAs("limit_%s_%s.png"%(options.doSmooth and "smooth" or "",Method))
	 C.SaveAs("limit_%s_%s.gif"%(options.doSmooth and "smooth" or "",Method))
	 C.SaveAs("limit_%s_%s.eps"%(options.doSmooth and "smooth" or "",Method))
	 C.SaveAs("limit_%s_%s.ps"%(options.doSmooth and "smooth" or "",Method))

#-------------------------------------------------------------------------


#-------------------------------------------------------------------------

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

  if not options.doRatio: sm = signalNormalizer.GetBR(mass)*signalNormalizer.GetXsection(mass) 
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
    if not options.doRatio: sm = signalNormalizer.GetBR(M)*signalNormalizer.GetXsection(M)
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
graphOne.SetLineColor(ROOT.kRed)
graphOne.SetMarkerColor(ROOT.kRed)
graphObs.SetMarkerStyle(20)
graphObs.SetMarkerSize(2.0)
graphObs.SetLineColor(1)

graphMed.SetLineStyle(2)
graphMed.SetLineColor(ROOT.kBlack)
if not options.pval:MG.Add(graph95)
if not options.pval:MG.Add(graph68)
if not options.pval:MG.Add(graphMed)

if not options.expectedOnly:
  MG.Add(graphObs)
  if options.bayes:
   MG.Add(bayesObs)

if not options.pval: MG.Add(graphOne)

# Plot -------------------------------------
if options.pval: MakePvalPlot(MG)
else: MakeLimitPlot(MG) 
# ------------------------------------------
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

