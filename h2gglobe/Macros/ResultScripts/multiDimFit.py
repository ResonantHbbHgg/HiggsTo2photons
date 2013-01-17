#!/bin/env python

from optparse import OptionParser, make_option
import sys, os, glob

def extract_par_limits(pars, model_name, mass, cl=0.05):
    
    par_limits = { }
    
    mystruct = "struct Entry{ float quantileExpected; "
    for m in pars:
        mystruct += "float %s;" % m
        par_limits[ m ] = [0.,99999.,-99999.]
    mystruct += "};"

    ROOT.gROOT.ProcessLine(mystruct)
    from ROOT import Entry
    entry = Entry()
    
    tin = ROOT.TFile.Open("higgsCombine%s_single.MultiDimFit.mH%s.root" % ( model_name, mass ) )
    limit = tin.Get("limit")
    limit.SetBranchAddress("quantileExpected", ROOT.AddressOf(entry,"quantileExpected" ) )
    for p in pars:
        limit.SetBranchAddress( p, ROOT.AddressOf(entry,p) )

    for i in range(limit.GetEntries()):
        limit.GetEntry(i)
        for p in pars:
            val = getattr(entry, p)
            qt =  getattr(entry, "quantileExpected")
            if qt == 1.:
                par_limits[p][0] = val
            elif qt >= cl:
                par_limits[p][1] = min( val, par_limits[p][1] )
                par_limits[p][2] = max( val, par_limits[p][2] )
    tin.Close()
    
    return par_limits

def system(cmd):
    print cmd
    os.system(cmd)
    
def main(options, args):

    combine_args = ""
    txt2ws_args  = ""
    if options.statOnly:
        combine_args += "-S 0"
        txt2ws_args  += "--stat"
    

    model_args  = { "ggHqqH" : "-P HiggsAnalysis.CombinedLimit.PhysicsModel:floatingXSHiggs --PO modes=ggH,qqH",
                    "cVcF"   : "-P HiggsAnalysis.CombinedLimit.HiggsCouplingsLOSM:cVcF",
                    "rVrF"   : "-P HiggsAnalysis.CombinedLimit.PhysicsModel:rVrFXSHiggs",
                    "rV"     : "-P HiggsAnalysis.CombinedLimit.PhysicsModel:rVrFXSHiggs",
                    "rVprf"  : "-P HiggsAnalysis.CombinedLimit.PhysicsModel:rVrFXSHiggs",
                    "mumH"   : "-P HiggsAnalysis.CombinedLimit.PhysicsModel:floatingHiggsMass",
                    "mH"     : "-P HiggsAnalysis.CombinedLimit.PhysicsModel:rVrFXSHiggs --PO higgsMassRange=120,130",
                    }

    model_combine_args  = { "ggHqqH" : "",
                            "cVcF"   : "",
                            "rVrF"   : "",
                            "rV"     : "--floatOtherPOIs=0 -P RV",
                            "rVprf"  : "--floatOtherPOIs=1 -P RV",
                            "mH"     : "--floatOtherPOIs=1 -P MH",
                            "mumH"   : "-P r -P MH",
                            }
   
    model_pars  = { "ggHqqH" : ["r_ggH","r_qqH"],
                    "cVcF"   : ["CV","CF"],
                    "rVrF"   : ["RV","RF"],
                    "rV"     : ["RV"],
                    "mH"     : ["MH"],
                    "mumH"     : ["r","MH"],
                    }

    pars_ranges = { "r_ggH"  : (0.,999999.,False),
                    "r_qqH"  : (0.,999999.,False),
                    "CV"     : (0.,2.,False),
                    "CF"     : (0.,3.,True),
                    "RV"     : (0.,999999.,False),
                    "RF"     : (0.,999999.,False),
                    "MH"     : (120.,130.,False),
                    "r"      : (-20,20.,False),
                    }

    parallel = "%s/parallel" % ( os.path.abspath(os.path.dirname(sys.argv[0])) )
    os.chdir( options.workdir )
    
    combine = "combine --preFitValue=0. --saveNLL %s %s " % ( combine_args, model_combine_args[options.model] )
    if options.expected > 0.:
        combine += " -t -1 --expectSignal=%f" % options.expected

    ## generate the model
    mass = "%1.5g" % options.mH
    model_name = "%s%1.5g" % ( options.model, options.mH )
    model = "%s.root" % model_name
    if not os.path.isfile(model) or options.forceRedoWorkspace:
        system("text2workspace.py %s %s -o %s -m %1.5g %s" % ( txt2ws_args, options.datacard, model, options.mH, model_args[options.model] ) )

    ## best fit
    system("%s -M MultiDimFit %s --algo=singles -v2 -n %s_single -m %s | tee combine_%s_single.log"  % (
        combine, model, model_name, mass, model_name ) )

    par_limits = extract_par_limits(model_pars[options.model],model_name, mass)


    ## clone model limiting the parameters range
    ROOT.gSystem.Load("libHiggsAnalysisCombinedLimit")
    fmod = ROOT.TFile.Open(model)
    ws = fmod.Get("w")
    
    for name, vals in par_limits.iteritems():
        best, down, up = vals
        pmin = max( best - options.scanrange*( best - down ), pars_ranges[name][0] )
        pmax = min( best + options.scanrange*( up   - best ), pars_ranges[name][1] )
        if pars_ranges[name][2]:
            pmax = max( abs(pmin), abs(pmax) )
            pmin = -pmax
        print name, pmin, pmax
        ws.var(name).setMin(pmin)
        ws.var(name).setMax(pmax)

    ws.writeToFile("%s_grid_test.root" % model_name)
    fmod.Close()

    if options.singleOnly:
        return

    ## run the NLL scan
    jobs=""
    step = options.npoints / 16 
    for ip in range(options.npoints/step+1):
        jobs="%s %d %d " % ( jobs, ip*step, (ip+1)*step-1 )
    print jobs
    system( "%s -N2 --eta '%s -M MultiDimFit %s_grid_test.root -m %s -v2 -n %s_grid{1} --algo=grid --points=%d --firstPoint={1} --lastPoint={2} | tee combine_%s_grid{1}.log' ::: %s " % ( parallel, combine, model_name, mass, model_name, options.npoints, model_name, jobs ) )

    hadd = "hadd higgsCombine%s_grid.MultiDimFit.mH%s.root" % (model_name,mass)
    for f in glob.glob("higgsCombine%s_grid[0-9]*.MultiDimFit.mH%s.root" % (model_name,mass) ):
        hadd += " %s" % f
    system(hadd)
    
if __name__ == "__main__":
    parser = OptionParser(option_list=[
        make_option("-w", "--workdir",
                    action="store", type="string", dest="workdir",
                    default="",
                    help="default : [%default]", metavar="WORKING_DIR"
                    ),
        make_option("-f", "--forceRedoWorkspace",
                    action="store_true",  dest="forceRedoWorkspace",
                    default=False,
                    help="default : [%default]",
                    ),
        make_option("-d", "--datacard",
                    action="store", type="string", dest="datacard",
                    default="datacard_fine.txt",
                    help="default : [%default]", metavar="DATACARD"
                    ),
        make_option("-r", "--scanrange",
                    action="store", type="float", dest="scanrange",
                    default=4.,
                    help="default : [%default]", metavar="SCANRANGE"
                    ),
        make_option("-m", "--mass",
                    action="store", type="float", dest="mH",
                    default=125.,
                    help="default : [%default]", metavar=""
                    ),
        make_option("-n", "--npoints",
                    action="store", type="int", dest="npoints",
                    default=10000,
                    help="default : [%default]", metavar="NPOINTS"
                    ),
        make_option("-M", "--model",
                    action="store", type="string", dest="model",
                    default="ggHqqH",
                    help="default : [%default]", metavar=""
                    ),
        make_option("-s", "--singleOnly",
                    action="store_true", dest="singleOnly",
                    default=False,
                    help="default : [%default]", metavar=""
                    ),
        make_option("-S", "--statOnly",
                    action="store_true", dest="statOnly",
                    default=False,
                    help="default : [%default]", metavar=""
                    ),
        make_option("-e", "--expected",
                    action="store", type="float", dest="expected",
                    default=0.,
                    help="default : [%default]", metavar=""
                    ),
        ])
    
    (options, args) = parser.parse_args()
    if options.workdir == "":
        options.workdir = args.pop(1)

    sys.argv.append("-b")
    import ROOT

    main( options, args )

