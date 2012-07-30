#!/bin/env python

import sys
import os
import ROOT

def setcolors(h,color):
    h.SetLineColor(color)
    h.SetFillColor(color)
    h.SetMarkerColor(color)

def legopt(h,opt):
    h.legopt = opt

def xrange(h,a,b):
    h.GetXaxis().SetRangeUser(a,b)

def xtitle(h,tit):
    h.GetXaxis().SetTitle(tit)

def ytitle(h,tit):
    h.GetYaxis().SetTitle(tit % { "binw" : h.GetBinWidth(0) } )

def applyModifs(h,modifs):
    for method in modifs:
        args = None
        print method, args
        if type(method) == tuple:
            method, args = method
        if type(method) == str:
            method = getattr(h,method)
        print method, args
        if args == None:            
            try:
                method(h)
            except:
                method()
        else:            
            if not ( type(args) == tuple or type(args) == list ):
                args = tuple([args])
            print method, args
            try:
                method(h,*args)
            except:
                method(*args)
            
            

def readPlot(fin, name, cat=0, which=["_sequential","_nminus1"], samples=["diphojet_8TeV","ggh_m125_8TeV","vbf_m125_8TeV",]):

    ret = []
    for p in which:
        hists = []
        for s in samples:
            nam = "%s%s_cat%d_%s" % ( name, p, cat, s )
            h = fin.Get(nam)
            h.GetXaxis().SetTitle(  h.GetXaxis().GetTitle().replace("@"," ") )
            ## print nam, h
            hists.append(h)
        ret.append(hists)
            
    return ret


def selectionControlPlots(fname):
    vars=[("vbf_mva",               [""]),
          ("cut_VBFLeadJPt",        ["_sequential","_nminus1"]),
          ("cut_VBFSubJPt",         ["_sequential","_nminus1"]),
          ("cut_VBF_dEta",          ["_sequential","_nminus1"]),
          ("cut_VBF_Zep",           ["_sequential","_nminus1"]),
          ("cut_VBF_Mjj",           ["_sequential","_nminus1"]),
          ("cut_VBF_dPhi",          ["_sequential","_nminus1"]),
          ("cut_VBF_DiPhoPtOverM",  ["_sequential","_nminus1"]),
          ("cut_VBF_LeadPhoPtOverM",["_sequential","_nminus1"]),
          ("cut_VBF_SubPhoPtOverM", ["_sequential","_nminus1"]),
          ]
    
    
    fin = ROOT.TFile.Open(fname)
    
    objs=[]
    for var,which in vars:
        plots = readPlot(fin,var,which=which)
        
        c = ROOT.TCanvas(var,var,800,400)
        objs.append(c)
        np = len(plots)
        if np > 1:
            c.Divide(2,np/2)
        for ip in range(np):
            ps=plots[ip]
            
            ps[0].SetLineColor(ROOT.kRed)
            ps[1].SetLineColor(ROOT.kMagenta)

            if np>1:
                c.cd(ip+1)

            p0=ps[0].Clone("%s_%d" % (var,ip) )
            objs.append(p0)
            p0.Reset("ICE")
            p0.SetEntries(0)
            p0.Draw()
            if var == "vbf_mva":
                p0.GetXaxis().SetRangeUser(-1,1)
            for p in ps:
                if var == "vbf_mva":
                    p.SetBinContent(1,0.)
                    p.Draw("hist same")
                else:
                    p.DrawNormalized("hist same")
            
            maximum=0.
            for p in ROOT.gPad.GetListOfPrimitives():
                if p.ClassName().startswith("TH1"):
                    pmax = p.GetMaximum()
                    if pmax>maximum:
                        maximum=pmax
            p0.GetYaxis().SetRangeUser(0,maximum*1.5)
            print var,p0,maximum, p0.GetYaxis().GetXmax()
            ROOT.gPad.Modified()
            ROOT.gPad.Update()
            
            for fmt in "C","png":
                c.SaveAs("%s.%s" % (c.GetName(),fmt))
                
    return objs

def eventYield(filenames,categories=[5,6],procs=["ggh_m125_8TeV","vbf_m125_8TeV","diphojet_8TeV"]):
    files = [ ROOT.TFile.Open(f) for f in filenames ]

    
    for fin in files:
        print fin.GetName()
        for cat in categories:
            plots=readPlot(fin, "mass", cat=cat, which=[""], samples=procs)[0]
            print "cat%d " % cat-1,
            for iproc in range(len(procs)):
                ## print "%1.3g" % plots[iproc].Integral(18,28), 115-135
                print "%1.3g" % plots[iproc].Integral(18,28),
            print


def readProc(fin,name,title,style,subproc,plot,plotmodifs,category):

    names = subproc.keys()
    histos = readPlot(fin,plot,which=[""],samples=names)[0]
    for iplot in range(len(histos)):
        h = histos[iplot]
        hname = names[iplot]
        applyModifs(h,subproc[hname])

    sum = histos[0].Clone(name)
    sum.SetTitle(title)

    for h in histos[1:]:
        sum.Add(h)

    applyModifs(sum,plotmodifs)
    applyModifs(sum,style)
    
    
    return sum

def makeCanvAndLeg(name,legPos):
    canv = ROOT.TCanvas(name)
    leg  = ROOT.TLegend(*legPos)

    leg.SetFillStyle(0), leg.SetLineColor(ROOT.kWhite)## , leg.SetShadowColor(ROOT.kWhite)

    return canv, leg

def makeStack(name,histos):
    stk = ROOT.THStack()
    for h in histos:
        stk.Add(h)
    return stk
    
def dataMcComparison(data, bkg, sig, plots, categories=[0], savefmts=["C","png","pdf"]):

    objs = []
    canvs = []
    for cat in categories:
        for plot in plots:

            plotname, plotmodifs, drawopts, legPos = plot
            dm, dataopt, bkgopt, sigopt = drawopts
            
            bkgfile, bkgprocs = bkg
            bkghists = [ readProc(bkgfile,*subprocs,plot=plotname,plotmodifs=plotmodifs,category=cat) for subprocs in bkgprocs ]
            
            sigfile, sigprocs = sig
            sighists = [ readProc(sigfile,*subprocs,plot=plotname,plotmodifs=plotmodifs,category=cat) for subprocs in sigprocs ]

            datafile, dataprocs = data
            datahists = [ readProc(datafile,*subprocs,plot=plotname,plotmodifs=plotmodifs,category=cat) for subprocs in dataprocs ]
            
            allhists = datahists+bkghists+sighists
            objs += allhists
            
            frame = allhists[0].Clone()
            frame.Reset("ICE")
            frame.SetEntries(0)
            objs.append(frame)
            ymax = 0.
            ymin = 0.
            
            canv,leg = makeCanvAndLeg("%s_cat%d" % ( plotname, cat), legPos )
            objs.append(canv)
            objs.append(leg)
            canvs.append(canv)
            
            frame.Draw()

            if len(bkghists) > 0:
                bkgstk = makeStack("bkg_%s_cat%d" % ( plotname, cat), bkghists)
                getattr(bkgstk,dm)("%s SAME" % bkgopt)
                ymax = max(ymax,bkgstk.GetMaximum(bkgopt))
                objs.append(bkgstk)
                
            
            if len(datahists) > 0:
                datastk = makeStack("data_%s_cat%d" % ( plotname, cat),datahists)
                getattr(datastk,dm)("%s SAME" % dataopt)
                ymax = max(ymax,datastk.GetMaximum())
                objs.append(datastk)
                

            if len(sighists) > 0:
                sigstk = makeStack("sig_%s_cat%d" % ( plotname, cat),sighists)
                getattr(sigstk,dm)("%s SAME" % sigopt)
                ymax = max(ymax,sigstk.GetMaximum(sigopt))
                objs.append(sigstk)

            for h in allhists:
                legopt = "f"
                if hasattr(h,"legopt"):
                    legopt = h.legopt
                leg.AddEntry(h,"",legopt)

            frame.GetYaxis().SetRangeUser(ymin,ymax*2)
            leg.Draw("same")

    for c in canvs:
        for fmt in savefmts:
            c.SaveAs("%s.%s" % (c.GetName(),fmt))
            
    return objs

if __name__ == "__main__":
    
    
    ## objs=selectionControlPlots(sys.argv[1])
    fdata = ROOT.TFile.Open(sys.argv[1])
    fbkg  = ROOT.TFile.Open(sys.argv[2])
    fsig  = ROOT.TFile.Open(sys.argv[3])
    defdrawopt = ("Draw","e","hist","hist nostack")
    deflegPos  = (0.137,0.525,0.474,0.888)
    ROOT.gStyle.SetOptTitle(0)
    ROOT.gStyle.SetOptStat(0)
    
    outdir = sys.argv[4]
    try:
        os.mkdir(outdir)
        os.chdir(outdir)
    except:
        pass
    
    objs=dataMcComparison( data = [ fdata, [ ("data", "data",
                                              [("SetMarkerStyle",(ROOT.kFullCircle)),(legopt,"pl")],
                                              { "Data" : [] }
                                              )
                                             ]
                                    ],
                           bkg  = [ fbkg,  [ ("pp","2 Prompt #gamma",          ## process name
                                              [(setcolors,591),(legopt,"fl"),("Scale",1.4)], ## style
                                              { "diphojet_8TeV"      : [],     ## subprocesses to add, with possible manpulators
                                                "dipho_Box_25_8TeV"  : [],
                                                "dipho_Box_250_8TeV" : [],
                                                "gjet_20_8TeV_pp"    : [],
                                                "gjet_40_8TeV_pp"    : []
                                                }
                                              ),
                                             ("pp","1 Prompt #gamma 1 Fake #gamma",
                                              [(setcolors,406),(legopt,"fl"),("Scale",1.4)],
                                              { ## "qcd_30_8TeV_pf"   : [("Smooth",25)],
                                                ## "qcd_40_8TeV_pf"   : [("Smooth",25)],
                                                "gjet_20_8TeV_pf"  : [],
                                                "gjet_40_8TeV_pf"  : []
                                                }
                                              ),
                                             ]
                                    ],
                           
                           sig   = [ fsig,  [ ("ggH", "25 x ggH H #rightarrow #gamma #gamma",
                                               [(setcolors,ROOT.kMagenta),("SetLineWidth",2),("SetFillStyle",0),("Scale",25.),(legopt,"l")],
                                               { "ggh_m125_8TeV" : [] }
                                               ),
                                              ("qqH", "25 x qqH H #rightarrow #gamma #gamma",
                                               [(setcolors,ROOT.kRed+1),("SetLineWidth",2),("SetFillStyle",0),("Scale",25.),(legopt,"l")],
                                               { "vbf_m125_8TeV" : [] }
                                               )
                                              ]
                                     ],
                           
                           plots = [ ("vbf_mva",[("Rebin",5),(xrange,(-1.,1)),("SetBinContent",(1,0.)),
                                                 ("SetBinError",(1,0.)),(xtitle,"MVA"),(ytitle,"Events/%(binw)1.2g")],
                                      defdrawopt,deflegPos),
                                     
                                     ("cut_VBFLeadJPt_sequential",[],defdrawopt,deflegPos),
                                     ("cut_VBFSubJPt_sequential",[],defdrawopt,deflegPos),
                                     ("cut_VBF_dEta_sequential",[],defdrawopt,deflegPos),
                                     ("cut_VBF_Zep_sequential",[],defdrawopt,deflegPos),
                                     ("cut_VBF_Mjj_sequential",[],defdrawopt,deflegPos),
                                     ("cut_VBF_dPhi_sequential",[],defdrawopt,deflegPos),
                                     ("cut_VBF_DiPhoPtOverM_sequential",[],defdrawopt,deflegPos),
                                     ("cut_VBF_LeadPhoPtOverM_sequential",[],defdrawopt,deflegPos),
                                     ("cut_VBF_SubPhoPtOverM_sequential",[],defdrawopt,deflegPos),
                                     
                                     ("cut_VBFLeadJPt_nminus1",[],defdrawopt,deflegPos),
                                     ("cut_VBFSubJPt_nminus1",[],defdrawopt,deflegPos),
                                     ("cut_VBF_dEta_nminus1",[],defdrawopt,deflegPos),
                                     ("cut_VBF_Zep_nminus1",[],defdrawopt,deflegPos),
                                     ("cut_VBF_Mjj_nminus1",[],defdrawopt,deflegPos),
                                     ("cut_VBF_dPhi_nminus1",[],defdrawopt,deflegPos),
                                     ("cut_VBF_DiPhoPtOverM_nminus1",[],defdrawopt,deflegPos),
                                     ("cut_VBF_LeadPhoPtOverM_nminus1",[],defdrawopt,deflegPos),
                                     ("cut_VBF_SubPhoPtOverM_nminus1",[],defdrawopt,deflegPos),
                                     
                                     ] 
                           )

    
    ## eventYield(sys.argv[1:])
