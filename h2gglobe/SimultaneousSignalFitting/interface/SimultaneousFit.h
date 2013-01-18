#ifndef SimultaneousFit_h 
#define SimultaneousFit_h

#include <iostream>
#include <vector>
#include <string>

#include "TFile.h"
#include "TH1F.h"
#include "TGraph.h"
#include "TFormula.h"

#include "RooRealVar.h"
#include "RooConstVar.h"
#include "RooFormulaVar.h"
#include "RooGaussian.h"
#include "RooAddPdf.h"
#include "RooDataSet.h"
#include "RooSimultaneous.h"
#include "RooCategory.h"
#include "RooHistFunc.h"
#include "RooExtendPdf.h"
#include "RooWorkspace.h"

#include "Normalization_8TeV.h"

using namespace std;
using namespace RooFit;

class SimultaneousFit {
  
  public:
    
    SimultaneousFit(string infilename, string outfilename, int mhLow=110, int mhHigh=150, int verbose=0, int nInclusiveCats=4, int nExclusiveCats=5);
    ~SimultaneousFit();

    void runFit(string proc="ggh", int cat=0, int nGaussians=3, int dmOrder=1, int sigmaOrder=1, int fracOrder=0, bool recursive=false);
    void setInitialFit(bool);
    void setSimultaneousFit(bool);
    void setMHDependentFit(bool);
    void setForceFracUnity(bool);
    void saveExtra(string name);
    
    vector<int> getAllMH();
    void setVerbose(int);

    string getDataSetName(string proc, int mh, int cat);
    string getWeightedDataSetName(string proc, int mh, int cat);
    RooDataSet* putMHinDataset(RooDataSet *data, int thisMH);
    RooDataSet* putWTinDataset(RooDataSet *data, int thisMH);
    RooDataSet *getMHDepData(string name, string proc, int cat);

    RooAddPdf* getSumOfGaussians(int nGaussians, bool recursive=false);
    RooAddPdf *getSumOfGaussiansMHDependent(string name, int nGaussians, bool recursive, string proc, int cat);
    RooAddPdf *getSumOfGaussiansEachMH(int mh, int nGaussians, bool recursive, string proc, int cat);
    RooAddPdf *buildFinalPdf(string name, int nGaussians, bool recursive, string proc, int cat);

    void fitTH1F(string name, int order, string proc, int cat);
    void fitTH1Fs(int nGaussians, int dmOrder, int sigmaOrder, int fracOrder, string proc, int cat);
  
    RooRealVar* getPolParam(string);
    RooRealVar* findPolParam(string);
    RooFormulaVar* findFormVar(string);
    RooConstVar* getConstPolParam(string);
    void setParamAttributes(RooRealVar*);
    void constructFormulaVar(string prefix, int order);
    void constructFormulaVar(string prefix, int order, int mh);
    void constructFormulaVars(int nGaussians, int dmOrder, int sigmaOrder, int fracOrder, string proc, int cat);
    void constructConstFormulaVar(string prefix, int order);
    void constructConstFormulaVars(int nGaussians, int dmOrder, int sigmaOrder, int fracOrder, string proc, int cat);
    
    void revertStartingVals(int mh);
    void addFitResultToMap(int mh);

    void plotPolParam(string name, int order, string proc, int cat);
    void plotPolParams(int nGaussians, int dmOrder, int sigmaOrder, int fracOrder, string proc, int cat);

    TGraph* makeEffAccGraph(RooWorkspace*, string proc, int cat);
    void makeEffAccFunc(RooWorkspace *inWS, string proc, int cat);
    void plotXS(string proc="");
    void plotBR();
    void plotPDF(string proc, int cat);

    void makeExtendPdf(string proc, int cat);
    void putDataInWS(RooWorkspace *inWS, string proc, int cat);

    bool isExclusiveCat(int cat);
    bool isInclusiveCat(int cat);

    void loadVariablesMap();
    void dumpVariablesMap();
    void loadSmearVals();
    void dumpStartVals();
    void dumpPolParams();
    void dumpPolParams(string,string);
    void dumpFormVars();
    void dumpConstVars();
    void freezePolParams();
    void setupSystematics(int cat);
    void setFork(int);

    void clean();
    
  private:
    
    int mhLow_;
    int mhHigh_;
    vector<int> allMH_;
    int verbose_;
    int nInclusiveCats_;
    int nExclusiveCats_;
    bool initialFit_;
    bool simultaneousFit_;
    bool mhDependentFit_;
    bool forceFracUnity_;
    TFile *inFile;
    TFile *outFile;
    RooWorkspace *outWS;

    // observables
    RooRealVar *mass;
    RooRealVar *MH;
    RooRealVar *weight;
    RooRealVar *intLumi;
   
    // gaussian params (dm, sigma, frac, mean)
    map<string,double> startVals;
    map<string,RooRealVar*> fitParams;
    map<string,RooFormulaVar*> meanParams;
    map<string,RooFormulaVar*> recfracParams;
    map<string,RooGaussian*> initialGaussians;
    map<string,RooGaussian*> finalGaussians;

    // th1fs from initial fit
    map<string,TH1F*> th1fs;
    map<string,double> polStartVals;

    // tgraphs from final fit
    map<string,TFormula*> tforms;
    map<string,TGraph*> tgraphs;

    // polynomial params
    map<string,RooRealVar*> polParams;
    map<string,RooFormulaVar*> formVars;
    map<string,RooConstVar*> constParams;

    // map for pol variables
    map<string,string> variablesMap;

    // mh cats
    RooCategory *mhCategory;
    
    // data
    map<string,RooDataSet*> allData;
    map<string,RooDataSet*> simFitDataMap;
    RooDataSet *simFitCombData;
    RooDataSet *mhFitCombData;

    // pdfs
    map<string,RooAbsPdf*> simFitPdfMap;
    RooSimultaneous *simFitCombPdf;
    RooAddPdf *mhFitCombPdf;
    RooAddPdf *finalPdf;
    RooExtendPdf *extendPdfRel;

    // systematics
    RooRealVar *globalScale;
    RooRealVar *categoryScale;
    RooConstVar *categorySmear;
    RooRealVar *categoryResolution;
    map<string,double> categorySmears;

    // utilities
    Normalization_8TeV *normalization;
    map<string,TGraph*> xsecsGraph;
    TGraph* brGraph;
    RooHistFunc *funcEffAcc;
    RooHistFunc *funcEffAccRel;
    bool systematicsSet_;
    int fork_;
    bool saveExtraFile_;
    TFile *extraFile_;

};
#endif
