#define LDEBUG 0 

#include "LoopAll.h"
#include "Tools.h"

#include <iostream>
#include <math.h>
#include "stdlib.h"

using namespace std;

#include "GeneralFunctions_cc.h"

#include "PhotonAnalysis/PhotonAnalysisReducedOutputTree.h"
#include "PhotonAnalysis/PhotonAnalysisFunctions_cc.h"

void LoopAll::SetTypeRun(int t, const char* name) {
  typerun = t;
  cout << "Type Run =" <<t <<endl; 
  if (t == 1) {
    makeOutputTree = 1;
    outputFileName = TString(name);
    histFileName  = TString("");
  } else {
    makeOutputTree = 0;
    outputFileName = TString("");
    histFileName  = TString(name);
  }
  
  if (makeOutputTree) {
    cout << "CREATE " << outputFileName<<endl;
    outputFile=new TFile(outputFileName,"recreate");
    outputFile->cd();
    if(outputFile) 
      if (DEBUG)
	cout<<"output file defined"<<endl;
  }
  
  int hasoutputfile=0;
  
  if(typerun == 0 || typerun == 2) {
    if(DEBUG) cout<<"typerun is " << typerun <<endl;
    hasoutputfile=0;
  } else if(typerun == 1) {
    if(DEBUG) cout<<"typerun is " << typerun <<endl;
    
    hasoutputfile=1;
    outputTree = new TTree("event","Reduced tree"); 
    outputTreePar = new TTree("global_variables","Parameters");
    
    PhotonAnalysisReducedOutputTree();

    outputParParameters = new std::vector<std::string>;
    outputParJobMaker = new std::string;
    outputTreePar->Branch("tot_events", &outputParTot_Events, "tot_events/I");
    outputTreePar->Branch("sel_events", &outputParSel_Events, "sel_events/I");
    outputTreePar->Branch("type", &outputParType, "type/I");
    outputTreePar->Branch("version", &outputParVersion, "version/I");
    outputTreePar->Branch("parameters", "std::vector<string>", &outputParParameters);
    outputTreePar->Branch("jobmaker", &outputParJobMaker, "jobmaker/C");
    //outputTreePar->Branch("job_maker", "std::string", &outputParJobMaker);
    outputTreePar->Branch("reductions", &outputParReductions, "reductions/I");
    outputTreePar->Branch("red_events", &outputParRed_Events, "red_events[reductions]/I");

  } 
}

void LoopAll::ReadInput(int t) {
  // FIXME make this variable global ?
  typerun = t;

  
  if (typerun == 1) {
    makeOutputTree = 1;
  } else {
    makeOutputTree = 0;
  }
  
  if (makeOutputTree) {
    cout << "CREATE " << outputFileName<<endl;
    outputFile=new TFile(outputFileName,"recreate");
    outputFile->cd();
    if(outputFile) 
      if (DEBUG)	cout<<"output file defined"<<endl;
  }
  
  int hasoutputfile=0;
  
  if(typerun == 0 || typerun == 2) {
    if(DEBUG) cout<<"typerun is " << typerun <<endl;
    hasoutputfile=0;
  } else if(typerun == 1) {
    if(DEBUG) cout<<"typerun is " << typerun <<endl;
    
    hasoutputfile=1;
    outputTree = new TTree("event","Reduced tree"); 
    outputTreePar = new TTree("global_variables","Parameters");
    
    PhotonAnalysisReducedOutputTree();
  } 
  
} 
void LoopAll::DefineSamples(const char *filesshortnam,
                            int type,
                            int histtoindfromfiles,
                            int histoplotit,
                            int nred,
                            long long ntot,
                            float intlumi,
                            float lumi,
                            float xsec,
                            float kfactor,
                            float scale) {
  
  //look in map for type as a key already
  int sample_is_defined = -1;
  for (unsigned int s=0; s<sampleContainer.size(); s++) {
    if (type == sampleContainer[s].itype) {
      sample_is_defined = s;
      break;
    }
  }
  
  if (sample_is_defined != -1) {
    sampleContainer[sample_is_defined].ntot += ntot;
    sampleContainer[sample_is_defined].nred += nred;
    sampleContainer[sample_is_defined].computeWeight(intlumi);
  } else {
    sampleContainer.push_back(SampleContainer());
    sampleContainer.back().ntot = ntot;
    sampleContainer.back().nred = nred;
    sampleContainer.back().histoplotit = histoplotit;
    sampleContainer.back().filesshortnam = filesshortnam;
    sampleContainer.back().lumi = lumi;
    sampleContainer.back().xsec = xsec;
    sampleContainer.back().kfactor = kfactor;
    sampleContainer.back().scale = scale;
    sampleContainer.back().computeWeight(intlumi);
  }
}

void LoopAll::AddFile(std::string name,int type) {
  if(DEBUG) 
    cout << "Adding file:  " << name << " of type " << type << endl;
  files.push_back(name);
  itype.push_back(type);
  
  nfiles++;
}

void LoopAll::LoopAndFillHistos(TString treename) {

  int i=0;
  
  if (DEBUG)
    cout<<"LoopAndFillHistos: calling InitReal " << endl;
  
  InitReal(typerun);
  
  Files.resize(files.size());  
  Trees.resize(files.size());  
  TreesPar.resize(files.size());  

  std::vector<std::string>::iterator it;
  std::vector<TTree*>::iterator it_tree;
  std::vector<TTree*>::iterator it_treepar;
  std::vector<TFile*>::iterator it_file;
  
  it = files.begin();
  it_file = Files.begin();
  it_tree	= Trees.begin();
  it_treepar = TreesPar.begin();  
  
  for (;it!=files.end()
         ;it_file++,it_tree++,it_treepar++,it++){ 
    
    this->current = i;
    cout<<"LoopAndFillHistos: opening " << i << " " << files[i]<<endl;
    
    *it_file = TFile::Open((*it).c_str());
    //Files[i] = TFile::Open(files[i]);
    tot_events=1;
    sel_events=1;
    if(typerun == 1) { //this is a reduce job
      
      if(*it_file)
        *it_treepar=(TTree*) (*it_file)->Get("global_variables");
      
      if(*it_treepar) {
        TBranch        *b_tot_events;
        TBranch        *b_sel_events;
        (*it_treepar)->SetBranchAddress("tot_events",&tot_events, &b_tot_events);
        (*it_treepar)->SetBranchAddress("sel_events",&sel_events, &b_sel_events);
        b_tot_events->GetEntry(0);
        b_sel_events->GetEntry(0);
      } else {
        cout<<"REDUCE JOB, no global_variables tree ... the C-step job must have crashed ... SKIP THE FILE"<<endl;
        tot_events=0;
        sel_events=0;
      }
    }
    
    if(tot_events!=0) {
      
      if(*it_file)
        *it_tree=(TTree*) (*it_file)->Get(treename);
      
      Init(typerun, *it_tree);
    }

    Loop(i);

    if(tot_events != 0) {
      (*it_tree)->Delete("");
    }
   
    // EDIT - Cannot close the first file since it is in use after 
    // file 0 
    if(*it_file && i>0)
      (*it_file)->Close();
    
    i++;
  }
  
  //now close the first File
  if(Files[0])
    Files[0]->Close();

  TermReal(typerun);
}

void LoopAll::Term(){
  outputFile->Close();
}

LoopAll::LoopAll(TTree *tree) :
    vtxAna(vtxAlgoParams), vtxConv(vtxAlgoParams)
{  
#include "branchdef/newclonesarray.h"
#ifndef __CINT__
#include "branchdef/branchdict.h"
#endif
//  rooContainer = new RooContainer();
}

LoopAll::~LoopAll() {
  if (!fChain)
    return;
  //delete fChain->GetCurrentFile();
}

Int_t LoopAll::GetEntry(Long64_t entry) {
  // Read contents of entry.
  if (!fChain) return 0;
  return fChain->GetEntry(entry);
}


Long64_t LoopAll::LoadTree(Long64_t entry) {

  // Set the environment to read one entry
  if (!fChain) return -5;
  Int_t centry = fChain->LoadTree(entry);
  if (centry < 0) return centry;
  if (fChain->IsA() != TChain::Class()) 
    return centry;
  TChain *chain = (TChain*)fChain;
  Notify();
  
  return centry;
}

Bool_t LoopAll::Notify() {
  return kTRUE;
}

void LoopAll::Show(Long64_t entry) {
  // Print contents of entry.
  // If entry is not specified, print current entry
  if (!fChain) return;
  fChain->Show(entry);
}

void LoopAll::InitHistos(){

  for(int ind=0; ind<sampleContainer.size(); ind++) {
    SampleContainer thisSample = (SampleContainer) sampleContainer.at(ind);
    HistoContainer temp(ind);
    temp.total_scale = thisSample.weight*thisSample.scale*thisSample.kfactor;
    histoContainer.push_back(temp);
  }

}

void LoopAll::InitReal(Int_t typerunpass) {

  // Set branch addresses
  typerun = typerunpass;
  hfile = (TFile*)gROOT->FindObject(histFileName); 
  if (hfile) 
    hfile->Close();

  //for(int ind=0; ind<sampleContainer.size(); ind++) {
  // HistoContainer temp(ind);
  //  histoContainer.push_back(temp);
 // }

  if(LDEBUG) cout << "doing InitRealPhotonAnalysis" << endl;
  InitRealPhotonAnalysis(typerun);
  if(LDEBUG) cout << "finished InitRealPhotonAnalysis" << endl;

  if (makeOutputTree) 
    outputFile->cd();
  cout<< "LoopAll::InitReal END" <<endl;
  
}

void LoopAll::TermReal(Int_t typerunpass) {

  typerun=typerunpass;

  TermRealPhotonAnalysis(typerun);
  
  if (makeOutputTree){ 
    outputFile->cd();
    outputParReductions++;
    outputTreePar->Fill();
    outputTreePar->Write();
  }
}


void LoopAll::Init(Int_t typerunpass, TTree *tree) {
  
  // Set branch addresses
  typerun=typerunpass;
  
  fChain = tree;
  if (tree == 0) 
    return;

  fChain->SetMakeClass(1);

  mySetBranchAddressRedPhotonAnalysis();

  Notify();
}


void LoopAll::Loop(Int_t a) {
  
  makeOutputTree = makeOutputTree;
  if (makeOutputTree) {
    outputTree = outputTree;
    outputFile = outputFile;
    if(outputFile) 
      outputFile->cd();
  }

  Int_t nentries = 0;
  if(fChain) 
    nentries = Int_t(fChain->GetEntriesFast());

  Int_t nbytes = 0, nb = 0;

  outputEvents=0;

  int hasoutputfile=0;
  for (Int_t jentry=0; jentry<nentries;jentry++) {
    
    if(jentry%10000==0) 
      cout << "Entry: "<<jentry<<endl;
    
    if(LDEBUG) 
      cout<<"call LoadTree"<<endl;
    
    Int_t ientry = LoadTree(jentry);
  
    if (ientry < 0) 
      break;
   
      
    if(typerun == 1) {


        //nb = fChain->GetEntry(jentry,1);
	
      
    } else {
      //HERE NEED TO DO A GLOBAL GETENTRY
      nb=0;
    }

    nbytes += nb;

    if(LDEBUG) 
      cout<<"Call FillandReduce "<<endl;
      
    hasoutputfile = FillAndReduce(jentry);
    if(LDEBUG) 
      cout<<"Called FillandReduce "<<endl;
  }

  if(hasoutputfile) {
    if(outputFile) {
      outputFile->cd();
      if (LDEBUG)
	cout<<"LoopAll_cc writing outputTree"<<endl;
      outputTree->Write(0,TObject::kWriteDelete);
    }

    
    if(outputFile) {
      outputFile->cd();
      if(TreesPar[a]) {
        
        std::vector<std::string> *parameters = new std::vector<std::string>;
        std::string *job_maker = new std::string;
        Int_t tot_events, sel_events, type, version, reductions;
        Int_t red_events[20];
        
        TreesPar[a]->SetBranchAddress("tot_events", &tot_events);
        TreesPar[a]->SetBranchAddress("sel_events", &sel_events);
        TreesPar[a]->SetBranchAddress("type", &type);
        TreesPar[a]->SetBranchAddress("version", &version);
        TreesPar[a]->SetBranchAddress("parameters", &parameters);
        TreesPar[a]->SetBranchAddress("job_maker", &job_maker);
        if (TreesPar[a]->FindBranch("reductions")) {
          TreesPar[a]->SetBranchAddress("reductions", &reductions);
          TreesPar[a]->SetBranchAddress("red_events", &red_events);
        }
        
        TreesPar[a]->GetEntry(0);
        
        if (a == 0) {
          outputParTot_Events = tot_events;
          outputParSel_Events = sel_events;

        } else {
          outputParTot_Events += tot_events;
          outputParSel_Events += sel_events;
        }

        outputParType = type;
        outputParVersion = version;
        outputParParameters = &(*parameters);
        outputParJobMaker = job_maker;
        
        if (!TreesPar[a]->FindBranch("reductions")) {
          outputParReductions = 0;
          for (int i=0; i<20; i++) {
            outputParRed_Events[i] = -1;
          }
          outputParRed_Events[0] += (int)countersred[1];
        } else {
          outputParReductions = reductions;
          outputParRed_Events[reductions] += (int)countersred[1];
        }
      } else {
        std::cerr << "Cannot write Parameter tree." << std::endl;
      }
    }
  }
  
  int oldnentries=nentries;
  if (nentries == sel_events) {
    nentries = tot_events;
  }
  
  if(countersred[1] || oldnentries==0) {
    printf("red: %d_%d \n",(int)countersred[0], (int) countersred[1]);
  } else { 
    printf("norm: %d \n",(int)counters[0]);
  }
}

void LoopAll::WriteFits() {
  
  hfile = new TFile(histFileName, "RECREATE", "Globe ROOT file with histograms");

  hfile->cd();
  hfile->cd();
 // rooContainer->Save();
}
void LoopAll::WriteHist() {

  hfile = new TFile(histFileName, "RECREATE", "Globe ROOT file with histograms");

  hfile->cd();
  hfile->cd();
  for(unsigned int ind=0; ind<sampleContainer.size(); ind++) {
    histoContainer[ind].Save();
  }
  if (makeOutputTree) 
    outputFile->cd();
}

void LoopAll::WriteCounters() {
  if(LDEBUG) std::cout<<"LoopAll::myWriteCounters - START"<<std::endl;
  
  const int samples = sampleContainer.size();
   
  if(LDEBUG) std::cout<<"samples = "<<samples<<std::endl;
  
  stringstream fileLinesInfo[samples];
  stringstream fileLinesCatFile[samples];
  stringstream fileLinesCatCounts[samples];
  stringstream fileLinesCatEvents[samples];
  stringstream fileLinesCatSigma[samples];
  stringstream fileLinesCatEff[samples][3];
  
  if(LDEBUG) std::cout<<"initialize streams"<<std::endl;

  double counterNumerator_tot[samples];
  double counterDenominator_tot[samples][3];
  double counterEvents_tot[samples];
  double counterSigma_tot[samples];
    
  if (counterContainer.size() < 1)
    return;

  TString msName = histFileName.ReplaceAll(".root", 5, ".csv", 4);
  FILE *file;
  TString s2 = msName+".csv";
  file = fopen(s2, "w");

  if(LDEBUG) std::cout<<"msName is "<< msName <<std::endl;
  if(LDEBUG) std::cout<<"s2 is "<< s2 <<std::endl;
  
  for(unsigned int i=0; i<counterContainer[0].mapSize(); i++) {
    if(LDEBUG) std::cout<<"counterContainer[0].mapSize() is "<< counterContainer[0].mapSize() <<std::endl;
    fprintf(file, "Number, Sample, Counter Name, Counted, TotalEvents, Sigma, Selection 1, Eff, Selection 2, Eff, Selection 3, Eff");
    fprintf(file, ",indexfiles, namefile, scale, lumi, intlum, weight");
    fprintf(file, "\n");
    
    int ncat = counterContainer[0].ncat(i);

    fprintf(file, "Number, Sample, Counter Name, Categories");
    for (unsigned int iCat=0; iCat<ncat; iCat++) 
      fprintf(file, "Cat %d Counts", iCat);
    fprintf(file, ", TOT Cat Counts");
    for (unsigned int iCat=0; iCat<ncat; iCat++)
      fprintf(file, ", Cat %d Tot Events", iCat);
    fprintf(file, ", TOT Cat Tot Events");
    for (unsigned int iCat=0; iCat<ncat; iCat++)
      fprintf(file, ", Cat %d Tot Sigma", iCat);
    fprintf(file, ", TOT Cat Tot Sigma");
    for (unsigned int iEff=0; iEff<3; iEff++) {
      fprintf(file, ", Denominator Name");
      for (unsigned int iCat=0; iCat<ncat; iCat++)
	      fprintf(file, ", Cat %d Eff.", iCat);
      fprintf(file, ", TOT Cat Eff.");
    }
    fprintf(file, ",, indexfiles, namefile, scale, lumi, intlum, weight");
    fprintf(file, "\n");
    
    for (int c=0; c<ncat; c++) {
      if(LDEBUG) std::cout<<"cat is "<< c <<std::endl;
      for (unsigned int j=0; j<counterContainer.size(); j++) {
        if (c == 0) {
	  counterNumerator_tot[j] = 0;
	  counterEvents_tot[j] = 0;
	  counterSigma_tot[j] = 0;
	      }
	      
	      for (unsigned int iEff=0; iEff<3; iEff++)
	        counterDenominator_tot[j][iEff] = 0;
	      
	      int indexfiles = sampleContainer[j].itype;
	      //indexinfo =; // check matteo
	      float weight = sampleContainer[j].weight;
	      
	      if (c == 0) {
	        fileLinesCatFile[j] << j << sampleContainer[j].filesshortnam << counterContainer[j].name(i) << ncat;
	        //fileLinesInfo[j] << indexfiles << files << scale << lumireal << intlumi << weight; // check matteo
	        fileLinesInfo[j] << indexfiles << sampleContainer[j].scale << sampleContainer[j].lumireal << intlumi << sampleContainer[j].weight;
	      }

	      float counts = counterContainer[j][i][c];
	      
	      // CHECK MATTEO
	      counterNumerator_tot[j] += counts;
	      counterEvents_tot[j] += counts*weight;
	      counterSigma_tot[j] += counts*weight/intlumi;
      }
      if(LDEBUG) std::cout<<"end cat loop"<<std::endl;
    }
    if(LDEBUG) std::cout<<"end sample loop"<<std::endl;
  }
  fclose(file);
  if(LDEBUG) std::cout<<"LoopAll::myWriteCounters - END"<<std::endl;
}

int LoopAll::FillAndReduce(int jentry) {

  int hasoutputfile = 0;
  if(typerun == 1) {
    hasoutputfile = 1;
    if(LDEBUG) 
      cout<<"call myReduce"<<endl;
    myGetEntryPhotonRedAnalysis(jentry);
    myReducePhotonAnalysis(jentry);
    if(LDEBUG) 
      cout<<"called myReduce"<<endl;
  } else if (typerun == 0) {
    hasoutputfile = 0;
    if(LDEBUG) 
      cout<<"call myFillHist -- really?"<<endl;
    myFillHistPhotonAnalysis(jentry);
    if(LDEBUG) 
      cout<<"called myFillHist"<<endl;
  } else if (typerun == 2) {
    hasoutputfile = 0;
    if(LDEBUG) 
      cout<<"call myFillHistRed"<<endl;
    myGetEntryPhotonRedAnalysis(jentry);
    myFillHistPhotonAnalysisRed(jentry);
    if(LDEBUG) 
      cout<<"called myFillHistRed"<<endl;
  } else if (typerun == 3) {
    hasoutputfile = 0;
    if(LDEBUG) 
      cout<<"call myFillHistRed"<<endl;
    myGetEntryPhotonRedAnalysis(jentry);
    myStatPhotonAnalysis(jentry);
    if(LDEBUG) 
      cout<<"called myFillHistStat"<<endl;
  }

  
  return hasoutputfile;
}

// ------------------------------------------------------------------------------------
void LoopAll::GetBranches(std::set<std::string> & names, std::set<TBranch *> & branches)
{
    for(std::set<std::string>::iterator it=names.begin(); it!=names.end(); ++it ) {
	const std::string & name = *it;
	branch_info_t & info = branchDict[ name ];
	assert( info.branch != 0 );
	*(info.branch) = fChain->GetBranch( name.c_str() );
	branches.insert( *(info.branch) );
    }
}

// ------------------------------------------------------------------------------------
void LoopAll::SetBranchAddresses(std::set<std::string> & names) {
    for(std::set<std::string>::iterator it=names.begin(); it!=names.end(); ++it ) {
	const std::string & name = *it;
	branch_info_t & info = branchDict[ name ];
	assert( info.read != 0 );
	(this->*(info.read)) (fChain);
    }
}

// ------------------------------------------------------------------------------------
void LoopAll::Branches(std::list<std::string> & names) {
   for(std::list<std::string>::iterator it=names.begin(); it!=names.end(); ++it ) {
	const std::string & name = *it;
	branch_info_t & info = branchDict[ name ];
	assert( info.write != 0 );
	(this->*(info.write)) (outputTree);
    }
}

// ------------------------------------------------------------------------------------
void LoopAll::GetEntry(std::set<TBranch *> & branches, int jentry)
{
   for(std::set<TBranch *>::iterator it=branches.begin(); it!=branches.end(); ++it ) {
       if( (*it)->GetReadEntry() != jentry ) {  (*it)->GetEntry(jentry); }
   }
}

void LoopAll::BookHisto(int h2d,
			int typplot,
			int typeplotall,
			int histoncat,
			int nbinsx,
			int nbinsy,
			float lowlim,
			float highlim,
			float lowlim2,
			float highlim2,
			char *name) {

  for(unsigned int ind=0; ind<sampleContainer.size(); ind++) {
    if (nbinsy == 0)
      histoContainer[ind].Add(name, histoncat, nbinsx, lowlim, highlim);
    if (nbinsy != 0)
      histoContainer[ind].Add(name, histoncat, nbinsx, lowlim, highlim, nbinsy, lowlim2, highlim2);
  }
}


// ------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------
void LoopAll::AddCut(char *cutnamesc, int ncatstmp, int ifromright, int ifinalcut, float *cutValuel, float *cutValueh) {

  if(LDEBUG) cout<<"InitCuts START"<<endl;
  
  Cut* this_cut = new Cut();
  this_cut->name = cutnamesc;
  this_cut->fromright = ifromright;
  this_cut->finalcut = ifinalcut;
  this_cut->ncat = ncatstmp;
  this_cut->cut.clear();
  this_cut->cutintervall.clear();
  this_cut->cutintervalh.clear();
  if(LDEBUG) cout<<"this_cut filled  "<<endl;
  for (int j=0; j<ncatstmp; j++) {
    if(ifromright == 2) {
      this_cut->cutintervall.push_back((float) cutValuel[j]);
      this_cut->cutintervalh.push_back((float) cutValueh[j]);
    } else {
      this_cut->cut.push_back((float) cutValuel[j]);
    }
  }
  if(LDEBUG) cout<<"push back cut  "<<endl;
  cutContainer.push_back(*this_cut);
  if(LDEBUG) cout<<"pushed back cut  "<<endl;
  if(LDEBUG) cout<<"InitCuts END"<<endl;
}

void LoopAll::InitCounters(){
  if(LDEBUG) cout<<"InitCounts BEGIN"<<endl;

  for(unsigned int i=0; i<sampleContainer.size(); i++)
     counterContainer.push_back(CounterContainer(i));

  if(LDEBUG) cout<<"InitCounts END"<<endl;
}

void LoopAll::AddCounter(int countersncat,
			 char *countername,
			 char *denomname0,
			 char *denomname1,
			 char *denomname2) {
    
  std::string* counternames_str = new std::string(countername);
  if(LDEBUG) cout<<" counternames_str"<<counternames_str<< endl; 
  //counternames_str.assign(counternames);
  if(LDEBUG) cout<<" *counternames_str"<<*counternames_str<< endl; 
  
  std::string* denomname0_str = new std::string(denomname0);
  if(LDEBUG) cout<<" denomname0_str"<<denomname0_str<< endl; 
  
  //std::string denomname1_str;
  std::string* denomname1_str =new std::string(denomname1);
  //denomname1_str.assign(denomname1);
  if(LDEBUG) cout<<" denomname1_str"<<denomname1_str<< endl; 
  
  std::string* denomname2_str = new std::string(denomname2);
  if(LDEBUG) cout<<" denomname2_str"<<denomname2_str<< endl; 
  
  for(unsigned int ind=0; ind<sampleContainer.size(); ind++) {
    if(LDEBUG) cout<<"adding to "<<ind<<" sampleContainer"<< endl; 
    counterContainer[ind].Add(*counternames_str
			      ,countersncat
			      ,*denomname0_str
			      ,*denomname1_str
			      ,*denomname2_str);
    if(LDEBUG) cout<<"added to "<<ind<<" sampleContainer"<< endl; 
  }
}

 
int LoopAll::ApplyCut(int icut, float var, int icat) {
  
  //returns 0 if not initialized
  if(cutContainer[icut].useit==0) return 1;
  
  if(cutContainer[icut].fromright==2) {
    if(var<cutContainer[icut].cutintervall[icat] || var>cutContainer[icut].cutintervalh[icat]) return 0;
  }
  else if (cutContainer[icut].fromright==1) {
    if(var>cutContainer[icut].cut[icat]) return 0;
  }
  else if (cutContainer[icut].fromright==0) {
    if(var<cutContainer[icut].cut[icat]) return 0;
  }
  return 1;
}

int LoopAll::ApplyCut(std::string cutname, float var, int icat) {
  for (unsigned int i=0; i<cutContainer.size(); i++) {
    if(cutContainer[i].name == cutname) {
      return ApplyCut(i, var, icat);
    }
  }
  
  //std::cout<<"ApplyCut: attention cutname "<<cutname<<" not found"<<endl;
  return 0;
}
 
void LoopAll::FillHist(std::string name, float y) {
  FillHist(name, 0, y);
}

void LoopAll::FillHist2D(std::string name, float x, float y) {
  FillHist2D(name, 0, x, y);
}

void LoopAll::FillHist(std::string name, int category, float y) {
  histoContainer[current].Fill(name, category, y);
}

void LoopAll::FillHist2D(std::string name, int category, float x, float y) {
  histoContainer[current].Fill2D(name, category, x, y);
}

void LoopAll::FillCounter(std::string name) {
  FillCounter(name, 0);
}

void LoopAll::FillCounter(std::string name, int category) {
  counterContainer[current].Fill(name, category);
}
