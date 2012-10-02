#include "../interface/PhotonAnalysis.h"


#include "PhotonReducedInfo.h"
#include "Sorters.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <algorithm>

#include "JetAnalysis/interface/JetHandler.h"
#include "CMGTools/External/interface/PileupJetIdentifier.h"

#define PADEBUG 0

using namespace std;

// ----------------------------------------------------------------------------------------------------
PhotonAnalysis::PhotonAnalysis()  : 
    runStatAnalysis(false), doTriggerSelection(false),
    name_("PhotonAnalysis"),
    vtxAna_(vtxAlgoParams), vtxConv_(vtxAlgoParams),
    tmvaPerVtxMethod("BDTG"),
    tmvaPerVtxWeights(""),
    tmvaPerEvtMethod("evtBTG"),
    tmvaPerEvtWeights(""),
    energyCorrectionMethod("DaunceyAndKenzie"), energyCorrected(0), energyCorrectedError(0)
{
    addConversionToMva=true;
    mvaVertexSelection=false;
    useDefaultVertex=false;
    forcedRho = -1.;

    reRunVtx = false;
    rematchConversions = true;
    
    keepPP = true;
    keepPF = true;
    keepFF = true; 

    doSystematics = true;    

    zero_ = 0.; 
    
    recomputeBetas = false;
    recorrectJets = false;
    rerunJetMva = false;
    recomputeJetWp = false;
    jetHandler_ = 0;

    reComputeCiCPF = false;
    skimOnDiphoN = true;

    splitEscaleSyst = false;
    scale_offset_corr_error_file = "";
    splitEresolSyst = false;
    corr_smearing_file = "";

    dataIs2011 = false;
    
    emulateBeamspot = false;
    reweighBeamspot = false;
    beamspotWidth   = 0.;
    emulatedBeamspotWidth = 0.;
    targetsigma=1.0;
    sourcesigma=1.0;
    rescaleDZforVtxMVA=false;

    mvaVbfSelection=false;
    mvaVbfUseDiPhoPt=true;
    mvaVbfUsePhoPt=true;
    bookDiPhoCutsInVbf=false;
    multiclassVbfSelection=false;
}

// ----------------------------------------------------------------------------------------------------
PhotonAnalysis::~PhotonAnalysis() 
{
    if( jetHandler_ != 0 ) delete jetHandler_;
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::Term(LoopAll& l) 
{}

// ----------------------------------------------------------------------------------------------------
void readEnergyScaleOffsets(const std::string &fname, EnergySmearer::energySmearingParameters::eScaleVector &escaleOffsets, 
                            EnergySmearer::energySmearingParameters::phoCatVector &photonCategories, bool data=true
                            )
{
    // read in energy scale corrections to be applied in run ranges
    std::fstream in(fname.c_str());
    assert( in );
    char line[200];
    float EBHighR9, EBLowR9, EBm4HighR9, EBm4LowR9, EEHighR9, EELowR9; 
    char catname[200];
    float mineta, maxeta, minr9, maxr9, offset, err;
    int type; 
    int  first, last;
    do {
        in.getline( line, 200, '\n' );

        if( sscanf(line,"%d %d %f %f %f %f %f %f",&first, &last, &EBHighR9, &EBLowR9, &EBm4HighR9, &EBm4LowR9, &EEHighR9, &EELowR9) == 8 ) { 
            std::cerr << "Energy scale by run " <<  first<< " " <<  last<< " " <<  EBHighR9<< " " <<  EBLowR9 << " " <<  EBm4HighR9<< " " <<  EBm4LowR9<< " " <<  EEHighR9<< " " <<  EELowR9 << std::endl;
            
            assert( ! data );
            escaleOffsets.push_back(EnergyScaleOffset(first,last));
            escaleOffsets.back().scale_offset["EBHighR9"] = -1.*EBHighR9;
            escaleOffsets.back().scale_offset["EBLowR9"]  = -1.*EBLowR9;
            escaleOffsets.back().scale_offset["EBm4HighR9"] = -1.*EBm4HighR9;
            escaleOffsets.back().scale_offset["EBm4LowR9"]  = -1.*EBm4LowR9;
            escaleOffsets.back().scale_offset["EEHighR9"] = -1.*EEHighR9;
            escaleOffsets.back().scale_offset["EELowR9"]  = -1.*EELowR9;
            escaleOffsets.back().scale_offset_error["EBHighR9"] = 0.;
            escaleOffsets.back().scale_offset_error["EBLowR9"]  = 0.;
            escaleOffsets.back().scale_offset_error["EBm4HighR9"] = 0.;
            escaleOffsets.back().scale_offset_error["EBm4LowR9"]  = 0.;
            escaleOffsets.back().scale_offset_error["EEHighR9"] = 0.;
            escaleOffsets.back().scale_offset_error["EELowR9"]  = 0.;
        } else if( sscanf(line,"%s %d %f %f %f %f %d %d %f %f", &catname, &type, &mineta, &maxeta, &minr9, &maxr9, &first, &last, &offset, &err  ) == 10 ) { 
        std::cerr << "Energy scale (or smering) by run " <<  catname << " " << type << " " << mineta << " " << maxeta << " " << minr9 << " " << maxr9 << " " << first << " " << last << " " << offset << " " << err << std::endl;
        
        assert( type>=0 && type<=2 );

            EnergySmearer::energySmearingParameters::eScaleVector::reverse_iterator escaleOffset = 
                find(escaleOffsets.rbegin(),escaleOffsets.rend(),std::make_pair(first,last));
            if( escaleOffset == escaleOffsets.rend() ) {
                std::cerr << "  adding new range range " << first << " " << last << std::endl;
                escaleOffsets.push_back(EnergyScaleOffset(first,last));
                escaleOffset = escaleOffsets.rbegin();
            }
            // chck if the category is already defined
            if( find(photonCategories.begin(), photonCategories.end(), std::string(catname) ) == photonCategories.end() ) {
                std::cerr << "  defining new category" << std::endl;
                photonCategories.push_back(PhotonCategory(mineta,maxeta,minr9,maxr9,(PhotonCategory::photon_type_t)type,catname));
            }
            // assign the scale offset and error for this category and this run range 
            escaleOffset->scale_offset[catname] = data ? -offset : offset;
            escaleOffset->scale_offset_error[catname] = err;
        }

    } while( in );
    
    in.close();
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::loadPuMap(const char * fname, TDirectory * dir, TH1 * target)
{
    std::fstream in(fname);
    assert( in );
    char line[200];
    int typid;
    char dname[200];
    do {
        in.getline( line, 200, '\n' );

        if( sscanf(line,"%d %s",&typid,dname) != 2 ) { continue; } 
        std::cerr << "Reading PU weights for sample " << typid << " from " << dname << std::endl;
        TDirectory * subdir = (TDirectory *)dir->Get(dname);
        assert( subdir != 0 );
        cout << "loadPuMap " << typid << " from subdir " << endl; 
        loadPuWeights(typid, subdir, target);
    } while ( in );
    in.close();
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::loadPuWeights(int typid, TDirectory * puFile, TH1 * target)
{
    cout<<"Loading pileup weights for typid " << typid <<endl;

    TH1 * hweigh = (TH1*) puFile->Get("pileup_weights");
    if( hweigh == 0 ) {
    if( PADEBUG ) std::cout << "pileup_weights not found: trying weights " << endl;
    hweigh = (TH1*) puFile->Get("weights");
    }
    TH1 * gen_pu = (TH1*)puFile->Get("pileup");
    if( gen_pu == 0 ) {
    if( PADEBUG ) std::cout << "pileup not found: trying generated_pu " << endl;
    gen_pu = (TH1*)puFile->Get("generated_pu");
    }
    
    assert( gen_pu != 0 );
    bool delHwei = false;
    if( target != 0 ) {
    // compute weights on the fly based on the target pileup scenario
    if( hweigh == 0 ) { 
        hweigh = (TH1*)gen_pu->Clone(); 
        delHwei = true;
    }
    hweigh->Reset("ICE");
    for( int ii=1; ii<hweigh->GetNbinsX(); ++ii ) {
        hweigh->SetBinContent( ii, target->GetBinContent( target->FindBin( hweigh->GetBinCenter(ii) ) ) );
    }
    hweigh->Divide(hweigh, gen_pu, 1., 1./gen_pu->Integral() );
    } else { 
    // Normalize weights such that the total cross section is unchanged
    TH1 * eff = (TH1*)hweigh->Clone("eff");
    eff->Multiply(gen_pu);
    hweigh->Scale( gen_pu->Integral() / eff->Integral()  );
    delete eff;
    }
    weights[typid].clear();
    for( int ii=1; ii<hweigh->GetNbinsX(); ++ii ) {
    weights[typid].push_back(hweigh->GetBinContent(ii)); 
    }

    std::cout << "pile-up weights: ["<<typid<<"]";
    std::copy(weights[typid].begin(), weights[typid].end(), std::ostream_iterator<double>(std::cout,","));
    std::cout << std::endl;
    
    if( delHwei ) { delete hweigh; }
}

// ----------------------------------------------------------------------------------------------------
float PhotonAnalysis::getPuWeight(int n_pu, int sample_type, SampleContainer* container, bool warnMe)
{
    if ( sample_type !=0 && puHist != "") {
        bool hasSpecificWeight = weights.find( sample_type ) != weights.end() ; 
    if( ! hasSpecificWeight && container != 0 && container->pileup != 0 ) {
        std::cout << "On-the fly pileup reweighing typeid " << sample_type << " " << container->pileup << std::endl;
        TFile * samplePu = TFile::Open(container->pileup.c_str());
        loadPuWeights(sample_type, samplePu, puTargetHist);
        samplePu->Close();
        hasSpecificWeight = true;
    } else if( sample_type < 0 && !hasSpecificWeight && warnMe ) {
            std::cerr  << "WARNING no pu weights specific for sample " << sample_type << std::endl;
        }
        std::vector<double> & puweights = hasSpecificWeight ? weights[ sample_type ] : weights[0]; 
        if(n_pu<puweights.size()){
            return puweights[n_pu]; 
        }    
        else{ //should not happen as we have a weight for all simulated n_pu multiplicities!
            cout <<"n_pu ("<< n_pu<<") too big ("<<puweights.size()<<") ["<< sample_type <<"], event will not be reweighted for pileup"<<endl;
        }
    }
    return 1.;
} 

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::applyGenLevelSmearings(double & genLevWeight, const TLorentzVector & gP4, int npu, int sample_type, BaseGenLevelSmearer * sys, float syst_shift)
{
    static int nwarnings=10;
    for(std::vector<BaseGenLevelSmearer*>::iterator si=genLevelSmearers_.begin(); si!=genLevelSmearers_.end(); si++){
    float genWeight=1;
    if( sys != 0 && *si == *sys ) { 
        (*si)->smearEvent(genWeight, gP4, npu, sample_type, syst_shift );
    } else {
        (*si)->smearEvent(genWeight, gP4, npu, sample_type, 0. );
    }
    if( genWeight < 0. ) {
        if( syst_shift == 0. ) {
        std::cerr << "Negative weight from smearer " << (*si)->name() << std::endl;
        assert(0);
        } else { 
        if( nwarnings-- > 0 ) {
            std::cout <<  "WARNING: negative during systematic scan in " << (*si)->name() << " " << genWeight << std::endl;
        }
        genWeight = 0.;
        }
    }
    genLevWeight*=genWeight;
    }
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::applySinglePhotonSmearings(std::vector<float> & smeared_pho_energy, std::vector<float> & smeared_pho_r9, std::vector<float> & smeared_pho_weight,
                        int cur_type, const LoopAll & l, const float * energyCorrected, const float * energyCorrectedError,
                        BaseSmearer * sys, float syst_shift
                        
    )
{
    static int nwarnings = 10;
    static int cache_run = -1, cache_lumis = -1, cache_event = -1;
    bool fillInfo = false;
    if( l.run != cache_run || l.lumis != cache_lumis || l.event != cache_event ) {
	fillInfo = true;
	cache_run   = l.run  ;
	cache_lumis = l.lumis;
	cache_event = l.event;
    }
    
    if( fillInfo ) {
	photonInfoCollection.clear();
    }
    smeared_pho_energy.resize(l.pho_n,0.);
    smeared_pho_r9.resize(l.pho_n,0.);
    smeared_pho_weight.resize(l.pho_n,0.);
    for(int ipho=0; ipho<l.pho_n; ++ipho ) { 
    
        std::vector<std::vector<bool> > p;
	if( fillInfo ) {
	    PhotonReducedInfo info (
		*((TVector3*)     l.sc_xyz->At(l.pho_scind[ipho])), 
		((TLorentzVector*)l.pho_p4->At(ipho))->Energy(), 
		energyCorrected[ipho],
		l.pho_isEB[ipho], l.pho_r9[ipho],
		true, // WARNING  setting pass photon ID flag for all photons. This is safe as long as only selected photons are used
		(energyCorrectedError!=0?energyCorrectedError[ipho]:0)
		);
	    photonInfoCollection.push_back(info);
	} else {
	    photonInfoCollection[ipho].reset();
	}
	PhotonReducedInfo & phoInfo = photonInfoCollection[ipho];
	
	int ieta, iphi;
	l.getIetaIPhi(ipho,ieta,iphi);
	phoInfo.addSmearingSeed( (unsigned int)l.sc_raw[l.pho_scind[ipho]] + abs(ieta) + abs(iphi) + l.run + l.event + l.lumis ); 
	phoInfo.setSphericalPhoton(l.CheckSphericalPhoton(ieta,iphi));
	
	// FIXME add seed to syst smearings
	
        float pweight = 1.;
        // smear MC. But apply energy corrections and scale adjustement to data 
        if( cur_type != 0 && doMCSmearing ) {
            for(std::vector<BaseSmearer *>::iterator si=photonSmearers_.begin(); si!= photonSmearers_.end(); ++si ) {
                float sweight = 1.;
		if( sys != 0 && *si == *sys ) {
		    // move the smearer under study by syst_shift
		    (*si)->smearPhoton(phoInfo,sweight,l.run,syst_shift);
		} else {
		    // for the other use the nominal points
		    (*si)->smearPhoton(phoInfo,sweight,l.run,0.);
		}
		if( sweight < 0. ) {
		    if( syst_shift == 0. ) {
			std::cerr << "Negative weight from smearer " << (*si)->name() << std::endl;
			assert(0);
		    } else { 
			if( nwarnings-- > 0 ) {
			    std::cout <<  "WARNING: negative during systematic scan in " << (*si)->name() << std::endl;
			}
			sweight = 0.;
		    }
		}
		pweight *= sweight;
            }
        } else if( cur_type == 0 ) {
            float sweight = 1.;
            if( doEcorrectionSmear )  { 
                eCorrSmearer->smearPhoton(phoInfo,sweight,l.run,0.); 
            }
            eScaleDataSmearer->smearPhoton(phoInfo,sweight,l.run,0.);
            pweight *= sweight;
        }
        smeared_pho_energy[ipho] = phoInfo.energy();
        smeared_pho_r9[ipho]     = phoInfo.r9();
        smeared_pho_weight[ipho] = pweight;
        
    }
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::fillDiphoton(TLorentzVector & lead_p4, TLorentzVector & sublead_p4, TLorentzVector & Higgs,
                  float & lead_r9, float & sublead_r9, TVector3 *& vtx, const float * energy,
                  const LoopAll & l, int diphoton_id)
{
    fillDiphoton(lead_p4, sublead_p4, Higgs, lead_r9, sublead_r9, vtx, energy,
                  l, l.dipho_leadind[diphoton_id], l.dipho_subleadind[diphoton_id], l.dipho_vtxind[diphoton_id]);
}
    
// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::fillDiphoton(TLorentzVector & lead_p4, TLorentzVector & sublead_p4, TLorentzVector & Higgs,
                  float & lead_r9, float & sublead_r9, TVector3 *& vtx, const float * energy,
                  const LoopAll & l, int leadind, int subleadind, int myvtx)
{
    int vtx_ind = myvtx;
    
    int cur_type = l.itype[l.current];
    bool isCorrectVertex;
    if (cur_type!=0) isCorrectVertex= (*((TVector3*)l.vtx_std_xyz->At(vtx_ind))-*((TVector3*)l.gv_pos->At(0))).Mag() < 1.;
   
    TLorentzVector lead_p4_bef = l.get_pho_p4( leadind, vtx_ind, energy);
    TLorentzVector sublead_p4_bef = l.get_pho_p4( subleadind, vtx_ind, energy);
    TLorentzVector Higgs_bef = lead_p4_bef + sublead_p4_bef;
    
    //TRandom3 rand;
    //rand.SetSeed(0);
    bool changed=false;
    if (emulateBeamspot && cur_type!=0 && (lastRun!=l.run || lastEvent!=l.event || lastLumi!=l.lumis)){
        double genVtxZ = ((TVector3*)l.gv_pos->At(0))->Z();
        double myVtxZ = ((TVector3*)l.vtx_std_xyz->At(vtx_ind))->Z();
        ((TVector3*)l.vtx_std_xyz->At(vtx_ind))->SetZ(genVtxZ+(targetsigma/sourcesigma)*(myVtxZ-genVtxZ));
        changed=true;
    }

        /*
    if (emulateBeamspot) isCorrectVertex=(*((TVector3*)l.vtx_std_xyz->At(vtx_ind)) - *((TVector3*)l.gv_pos->At(0))).Mag() < (sourcesigma/targetsigma);
    else isCorrectVertex=(*((TVector3*)l.vtx_std_xyz->At(vtx_ind))- *((TVector3*)l.gv_pos->At(0))).Mag() < 1.;
    if (emulateBeamspot && cur_type!=0 && !isCorrectVertex && (lastRun!=l.run || lastEvent!=l.event || lastLumi!=l.lumis)){
	// FIXME use beam spot from event/pass parameter. Do not hardcode.
        double beamspotZ = 0.4145;
        double randVtxZ = -100;
        bool tooClose=true;
        while(tooClose){
            randVtxZ = rand.Gaus(beamspotZ,emulatedBeamspotWidth);
            ((TVector3*)l.vtx_std_xyz->At(vtx_ind))->SetZ(randVtxZ);
            tooClose = (*((TVector3*)l.vtx_std_xyz->At(vtx_ind))-*((TVector3*)l.gv_pos->At(0))).Mag() < 1.;
        }
        double genVtxZ = ((TVector3*)l.gv_pos->At(0))->Z();
        double myVtxZ = ((TVector3*)l.vtx_std_xyz->At(vtx_ind))->Z();
        ((TVector3*)l.vtx_std_xyz->At(vtx_ind))->SetZ(genVtxZ+(targetsigma/sourcesigma)*(myVtxZ-genVtxZ));
        changed=true;
    }
        */

    lead_p4 = l.get_pho_p4( leadind, vtx_ind, energy);
    sublead_p4 = l.get_pho_p4( subleadind, vtx_ind, energy);
    lead_r9    = l.pho_r9[leadind];
    sublead_r9 = l.pho_r9[subleadind];
    Higgs = lead_p4 + sublead_p4;    
    vtx = (TVector3*)l.vtx_std_xyz->At(vtx_ind);

    if (changed && PADEBUG){
        cout << "emBS: " << emulateBeamspot << " rwBS: " << reweighBeamspot << " tS: " << targetsigma << " sS: " << sourcesigma << " bW: " << beamspotWidth << endl;
        cout << "Before (eta1,eta2,mh): " << lead_p4_bef.Eta() << " " << sublead_p4_bef.Eta() << " " << Higgs_bef.M() << endl;
        cout << "After  (eta1,eta2,mh): " << lead_p4.Eta() << " " << sublead_p4.Eta() << " " << Higgs.M() << endl;
    }
    
    lastRun = l.run;
    lastEvent = l.event;
    lastLumi = l.lumis;
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::applyDiPhotonSmearings(TLorentzVector & Higgs, TVector3 & vtx, int category, int cur_type, const TVector3 & truevtx, 
                        float & evweight, float & idmva1, float & idmva2,
                        BaseDiPhotonSmearer * sys, float syst_shift)
{
    static int nwarnings=10;
    float pth = Higgs.Pt();
    for(std::vector<BaseDiPhotonSmearer *>::iterator si=diPhotonSmearers_.begin(); si!= diPhotonSmearers_.end(); ++si ) {
        float rewei=1.;
    if( sys != 0 && *si == *sys ) { 
        (*si)->smearDiPhoton( Higgs, vtx, rewei, category, cur_type, truevtx, idmva1, idmva2, syst_shift );
    } else {
        (*si)->smearDiPhoton( Higgs, vtx, rewei, category, cur_type, truevtx, idmva1, idmva2, 0. );
    }
        if( rewei < 0. ) {
        if( syst_shift == 0. ) {
        std::cerr << "Negative weight from smearer " << (*si)->name() << std::endl;
        assert(0);
        } else { 
        if( nwarnings-- > 0 ) {
            std::cout <<  "WARNING: negative during systematic scan in " << (*si)->name() << std::endl;
        }
        rewei = 0.;
        }
        }
        evweight *= rewei;
    }
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::Init(LoopAll& l) 
{
    if(PADEBUG) 
        cout << "InitRealPhotonAnalysis START"<<endl;

    if(energyCorrectionMethod=="DaunceyAndKenzie"){
        energyCorrected     = (l.pho_residCorrEnergy);
        energyCorrectedError= (l.pho_residCorrResn);
    }else if(energyCorrectionMethod=="Bendavid"){
        energyCorrected     = (l.pho_regr_energy);
        energyCorrectedError= (l.pho_regr_energyerr);
    }else if(energyCorrectionMethod=="BendavidOTF"){
        energyCorrected     = (l.pho_regr_energy_otf);
        energyCorrectedError= (l.pho_regr_energyerr_otf);

        //  }else if(energyCorrectionMethod=="PFRegression"){
    }else{
        assert(doEcorrectionSmear==false);
    }
    if (doEcorrectionSmear) std::cout << "using energy correction type: " << energyCorrectionMethod << std::endl;
    else                    std::cout << "NOT using energy correction (sbattogiu)"<< std::endl;

    if( vtxVarNames.empty() ) {
        vtxVarNames.push_back("ptbal"), vtxVarNames.push_back("ptasym"), vtxVarNames.push_back("logsumpt2");
    }
    
    /// // trigger

    // /cdaq/physics/Run2011/5e32/v4.2/HLT/V2
    triggerSelections.push_back(TriggerSelection(160404,161176));
    triggerSelections.back().addpath("HLT_Photon26_CaloIdL_IsoVL_Photon18_CaloIdL_IsoVL_v");

    // /cdaq/physics/Run2011/5e32/v6.1/HLT/V1
    triggerSelections.push_back(TriggerSelection(161216,165633));
    triggerSelections.back().addpath("HLT_Photon26_CaloIdL_IsoVL_Photon18_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon26_CaloIdL_IsoVL_Photon18_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id_Photon18_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon20_R9Id_Photon18_R9Id_v");

    // /cdaq/physics/Run2011/1e33/v2.3/HLT/V1
    triggerSelections.push_back(TriggerSelection(165970,166967));
    triggerSelections.back().addpath("HLT_Photon26_CaloIdL_IsoVL_Photon18_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon26_CaloIdL_IsoVL_Photon18_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id_Photon18_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id_Photon18_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloIdL_IsoVL_Photon22_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloId_IsoVL_Photon22_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id_Photon22_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id_Photon22_R9Id_v");

    // /cdaq/physics/Run2011/1e33/v2.3/HLT/V1
    triggerSelections.push_back(TriggerSelection(167039,173198));
    triggerSelections.back().addpath("HLT_Photon26_CaloIdL_IsoVL_Photon18_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon26_CaloIdL_IsoVL_Photon18_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id_Photon18_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id_Photon18_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloIdL_IsoVL_Photon22_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloIdL_IsoVL_Photon22_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id_Photon22_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id_Photon22_R9Id_v");

    // /cdaq/physics/Run2011/1e33/v2.3/HLT/V1
    triggerSelections.push_back(TriggerSelection(165970,166967));
    triggerSelections.back().addpath("HLT_Photon26_CaloIdL_IsoVL_Photon18_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon26_CaloIdL_IsoVL_Photon18_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id_Photon18_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id_Photon18_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloIdL_IsoVL_Photon22_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloId_IsoVL_Photon22_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id_Photon22_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id_Photon22_R9Id_v");

    // /cdaq/physics/Run2011/1e33/v2.3/HLT/V3
    triggerSelections.push_back(TriggerSelection(167039,173198));
    triggerSelections.back().addpath("HLT_Photon26_CaloIdL_IsoVL_Photon18_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon26_CaloIdL_IsoVL_Photon18_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id_Photon18_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id_Photon18_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloIdL_IsoVL_Photon22_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloIdL_IsoVL_Photon22_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id_Photon22_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id_Photon22_R9Id_v");

    // /cdaq/physics/Run2011/3e33/v1.1/HLT/V1
    triggerSelections.push_back(TriggerSelection(173236,178380));
    triggerSelections.back().addpath("HLT_Photon26_CaloIdXL_IsoXL_Photon18_CaloIdXL_IsoXL_v");
    triggerSelections.back().addpath("HLT_Photon26_CaloIdXL_IsoXL_Photon18_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id_Photon18_CaloIdXL_IsoXL_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id_Photon18_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloIdL_IsoVL_Photon22_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloIdL_IsoVL_Photon22_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id_Photon22_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id_Photon22_R9Id_v");

    // /cdaq/physics/Run2011/5e33/v1.4/HLT/V3
    triggerSelections.push_back(TriggerSelection(178420,190455));
    triggerSelections.back().addpath("HLT_Photon26_CaloIdXL_IsoXL_Photon18_CaloIdXL_IsoXL_Mass60_v");
    triggerSelections.back().addpath("HLT_Photon26_CaloIdXL_IsoXL_Photon18_R9IdT_Mass60_v");
    triggerSelections.back().addpath("HLT_Photon26_R9IdT_Photon18_CaloIdXL_IsoXL_Mass60_v");
    triggerSelections.back().addpath("HLT_Photon26_R9IdT_Photon18_R9IdT_Mass60_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloIdL_IsoVL_Photon22_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloIdL_IsoVL_Photon22_R9Id_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id_Photon22_CaloIdL_IsoVL_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id_Photon22_R9Id_v");
    
    triggerSelections.push_back(TriggerSelection(190456,194269));
    triggerSelections.back().addpath("HLT_Photon26_R9Id85_OR_CaloId10_Iso50_Photon18_R9Id85_OR_CaloId10_Iso50_Mass60_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id85_OR_CaloId10_Iso50_Photon22_R9Id85_OR_CaloId10_Iso50_v");

    triggerSelections.back().addpath("HLT_Photon26_CaloId10_Iso50_Photon18_CaloId10_Iso50_Mass60_v");
    triggerSelections.back().addpath("HLT_Photon26_CaloId10_Iso50_Photon18_R9Id85_Mass60_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id85_OR_CaloId10_Iso50_Photon18_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id85_Photon18_CaloId10_Iso50_Mass60_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id85_Photon18_R9Id85_Mass60_v");
    
    triggerSelections.back().addpath("HLT_Photon36_CaloId10_Iso50_Photon22_CaloId10_Iso50_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloId10_Iso50_Photon22_R9Id85_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id85_OR_CaloId10_Iso50_Photon22_R9Id85_OR_CaloId10_Iso50_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id85_OR_CaloId10_Iso50_Photon22_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id85_Photon22_CaloId10_Iso50_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id85_Photon22_R9Id85_v");

    triggerSelections.push_back(TriggerSelection(194270,-1));
    triggerSelections.back().addpath("HLT_Photon26_R9Id85_OR_CaloId10_Iso50_Photon18_R9Id85_OR_CaloId10_Iso50_Mass60_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id85_OR_CaloId10_Iso50_Photon18_R9Id85_OR_CaloId10_Iso50_Mass70_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id85_OR_CaloId10_Iso50_Photon22_R9Id85_OR_CaloId10_Iso50_v");

    triggerSelections.back().addpath("HLT_Photon26_CaloId10_Iso50_Photon18_CaloId10_Iso50_Mass60_v");
    triggerSelections.back().addpath("HLT_Photon26_CaloId10_Iso50_Photon18_R9Id85_Mass60_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id85_OR_CaloId10_Iso50_Photon18_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id85_Photon18_CaloId10_Iso50_Mass60_v");
    triggerSelections.back().addpath("HLT_Photon26_R9Id85_Photon18_R9Id85_Mass60_v");
    
    triggerSelections.back().addpath("HLT_Photon36_CaloId10_Iso50_Photon22_CaloId10_Iso50_v");
    triggerSelections.back().addpath("HLT_Photon36_CaloId10_Iso50_Photon22_R9Id85_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id85_OR_CaloId10_Iso50_Photon22_R9Id85_OR_CaloId10_Iso50_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id85_OR_CaloId10_Iso50_Photon22_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id85_Photon22_CaloId10_Iso50_v");
    triggerSelections.back().addpath("HLT_Photon36_R9Id85_Photon22_R9Id85_v");

    // n-1 plots for VBF tag 2011 
    l.SetCutVariables("cut_VBFLeadJPt",         &myVBFLeadJPt);
    l.SetCutVariables("cut_VBFSubJPt",          &myVBFSubJPt); 
    l.SetCutVariables("cut_VBF_Mjj",            &myVBF_Mjj);   
    l.SetCutVariables("cut_VBF_dEta",           &myVBFdEta);   
    l.SetCutVariables("cut_VBF_Zep",            &myVBFZep);    
    l.SetCutVariables("cut_VBF_dPhi",           &myVBFdPhi);   
    l.SetCutVariables("cut_VBF_Mgg0",           &myVBF_Mgg);   
    l.SetCutVariables("cut_VBF_Mgg2",           &myVBF_Mgg);   
    l.SetCutVariables("cut_VBF_Mgg4",           &myVBF_Mgg);   
    l.SetCutVariables("cut_VBF_Mgg10",          &myVBF_Mgg);   
    l.SetCutVariables("cut_VBF_Mgg4_100_180",   &myVBF_Mgg);   
    l.SetCutVariables("cut_VBF_Mgg2_100_180",   &myVBF_Mgg);
    
    if( mvaVbfSelection || multiclassVbfSelection || bookDiPhoCutsInVbf  ) {
        l.SetCutVariables("cut_VBF_DiPhoPtOverM",   &myVBFDiPhoPtOverM);
        l.SetCutVariables("cut_VBF_LeadPhoPtOverM", &myVBFLeadPhoPtOverM);
        l.SetCutVariables("cut_VBF_SubPhoPtOverM",  &myVBFSubPhoPtOverM);
    }

    if( mvaVbfSelection || multiclassVbfSelection ) {
	
	tmvaVbfReader_ = new TMVA::Reader( "!Color:!Silent" );

	tmvaVbfReader_->AddVariable("jet1pt"              , &myVBFLeadJPt);
	tmvaVbfReader_->AddVariable("jet2pt"	          , &myVBFSubJPt);
	tmvaVbfReader_->AddVariable("abs(jet1eta-jet2eta)", &myVBFdEta);
	tmvaVbfReader_->AddVariable("mj1j2"		  , &myVBF_Mjj);
	tmvaVbfReader_->AddVariable("zepp"		  , &myVBFZep);
	tmvaVbfReader_->AddVariable("dphi"		  , &myVBFdPhi);
	if( mvaVbfUseDiPhoPt ) {
	    tmvaVbfReader_->AddVariable("diphopt/diphoM"      , &myVBFDiPhoPtOverM); 
	}
	if( mvaVbfUsePhoPt   ) {
	    tmvaVbfReader_->AddVariable("pho1pt/diphoM"	  , &myVBFLeadPhoPtOverM);
	    tmvaVbfReader_->AddVariable("pho2pt/diphoM"       , &myVBFSubPhoPtOverM);
	}
	
	tmvaVbfReader_->BookMVA( mvaVbfMethod, mvaVbfWeights );
    
    }

    
    

    // n-1 plots for VH hadronic tag 2011
    l.SetCutVariables("cut_VHhadLeadJPt",      &myVHhadLeadJPt);
    l.SetCutVariables("cut_VHhadSubJPt",       &myVHhadSubJPt);
    l.SetCutVariables("cut_VHhad_Mjj",         &myVHhad_Mjj);
    l.SetCutVariables("cut_VHhad_dEta",        &myVHhaddEta);
    l.SetCutVariables("cut_VHhad_Zep",         &myVHhadZep);
    l.SetCutVariables("cut_VHhad_dPhi",        &myVHhaddPhi);
    l.SetCutVariables("cut_VHhad_Mgg0",        &myVHhad_Mgg);
    l.SetCutVariables("cut_VHhad_Mgg2",        &myVHhad_Mgg);
    l.SetCutVariables("cut_VHhad_Mgg4",        &myVHhad_Mgg);
    l.SetCutVariables("cut_VHhad_Mgg10",        &myVHhad_Mgg);
    l.SetCutVariables("cut_VHhad_Mgg2_100_160",        &myVHhad_Mgg);
    l.SetCutVariables("cut_VHhad_Mgg4_100_160",        &myVHhad_Mgg);

    // n-1 plot for ClassicCats
    l.SetCutVariables("cutnm1hir9EB_r9",             &sublead_r9);
    l.SetCutVariables("cutnm1hir9EB_isoOverEt",      &sublead_isoOverEt);
    l.SetCutVariables("cutnm1hir9EB_badisoOverEt",   &sublead_badisoOverEt);
    l.SetCutVariables("cutnm1hir9EB_trkisooet",      &sublead_trkisooet);
    l.SetCutVariables("cutnm1hir9EB_sieie",          &sublead_sieie);
    l.SetCutVariables("cutnm1hir9EB_drtotk",         &sublead_drtotk);
    l.SetCutVariables("cutnm1hir9EB_hovere",         &sublead_hovere);
    l.SetCutVariables("cutnm1hir9EB_Mgg",            &sublead_mgg);

    l.SetCutVariables("cutnm1lor9EB_r9",             &sublead_r9);
    l.SetCutVariables("cutnm1lor9EB_isoOverEt",      &sublead_isoOverEt);
    l.SetCutVariables("cutnm1lor9EB_badisoOverEt",   &sublead_badisoOverEt);
    l.SetCutVariables("cutnm1lor9EB_trkisooet",      &sublead_trkisooet);
    l.SetCutVariables("cutnm1lor9EB_sieie",          &sublead_sieie);
    l.SetCutVariables("cutnm1lor9EB_drtotk",         &sublead_drtotk);
    l.SetCutVariables("cutnm1lor9EB_hovere",         &sublead_hovere);
    l.SetCutVariables("cutnm1lor9EB_Mgg",            &sublead_mgg);

    l.SetCutVariables("cutnm1hir9EE_r9",             &sublead_r9);
    l.SetCutVariables("cutnm1hir9EE_isoOverEt",      &sublead_isoOverEt);
    l.SetCutVariables("cutnm1hir9EE_badisoOverEt",   &sublead_badisoOverEt);
    l.SetCutVariables("cutnm1hir9EE_trkisooet",      &sublead_trkisooet);
    l.SetCutVariables("cutnm1hir9EE_sieie",          &sublead_sieie);
    l.SetCutVariables("cutnm1hir9EE_drtotk",         &sublead_drtotk);
    l.SetCutVariables("cutnm1hir9EE_hovere",         &sublead_hovere);
    l.SetCutVariables("cutnm1hir9EE_Mgg",            &sublead_mgg);

    l.SetCutVariables("cutnm1lor9EE_r9",             &sublead_r9);
    l.SetCutVariables("cutnm1lor9EE_isoOverEt",      &sublead_isoOverEt);
    l.SetCutVariables("cutnm1lor9EE_badisoOverEt",   &sublead_badisoOverEt);
    l.SetCutVariables("cutnm1lor9EE_trkisooet",      &sublead_trkisooet);
    l.SetCutVariables("cutnm1lor9EE_sieie",          &sublead_sieie);
    l.SetCutVariables("cutnm1lor9EE_drtotk",         &sublead_drtotk);
    l.SetCutVariables("cutnm1lor9EE_hovere",         &sublead_hovere);
    l.SetCutVariables("cutnm1lor9EE_Mgg",            &sublead_mgg);

    if(includeVHlep) {
        l.SetCutVariables("cutEl_leptonSig",    &myEl_leptonSig);
        l.SetCutVariables("cutEl_elpt",         &myEl_elpt);
        l.SetCutVariables("cutEl_oEsuboP",      &myEl_oEsuboP);
        l.SetCutVariables("cutEl_D0",           &myEl_D0     );
        l.SetCutVariables("cutEl_DZ",           &myEl_DZ     );
        l.SetCutVariables("cutEl_mishit",       &myEl_mishit );
        l.SetCutVariables("cutEl_conv",         &myEl_conv   );
        l.SetCutVariables("cutEl_detain",       &myEl_detain );
        l.SetCutVariables("cutEl_dphiin",       &myEl_dphiin );
        l.SetCutVariables("cutEl_sieie",        &myEl_sieie  );
        l.SetCutVariables("cutEl_sieie2",       &myEl_sieie  );
        l.SetCutVariables("cutEl_hoe",          &myEl_hoe    );
        l.SetCutVariables("cutEl_drlead",       &myEl_drlead );
        l.SetCutVariables("cutEl_drsub",        &myEl_drsub  );
        l.SetCutVariables("cutEl_melead",       &myEl_melead );
        l.SetCutVariables("cutEl_meleadveto10", &myEl_meleadveto10 );
        l.SetCutVariables("cutEl_meleadveto15", &myEl_meleadveto15 );
        l.SetCutVariables("cutEl_mesub",        &myEl_mesub  );
        l.SetCutVariables("cutEl_mesubveto5",   &myEl_mesubveto5  );
        l.SetCutVariables("cutEl_mesubveto10",  &myEl_mesubveto10  );
        l.SetCutVariables("cutEl_reliso",       &myEl_reliso );
        l.SetCutVariables("cutEl_iso",          &myEl_iso    );
        l.SetCutVariables("cutEl_mvaNonTrig",   &myEl_mvaNonTrig);
        l.SetCutVariables("cutEl_dZ_ee",        &myEl_dZ_ee);
        l.SetCutVariables("cutEl_mass_ee",      &myEl_mass_ee);
        l.SetCutVariables("cutEl_inwindow_ee",  &myEl_inwindow_ee);
        l.SetCutVariables("cutEl_ptlead",       &myEl_ptlead    );
        l.SetCutVariables("cutEl_ptsub",        &myEl_ptsub     );
        l.SetCutVariables("cutEl_ptleadom",       &myEl_ptleadom    );
        l.SetCutVariables("cutEl_ptsubom",        &myEl_ptsubom     );
        l.SetCutVariables("cutEl_elvetolead",   &myEl_elvetolead);
        l.SetCutVariables("cutEl_elvetosub",    &myEl_elvetosub );
        l.SetCutVariables("cutEl_ptgg",         &myEl_ptgg      );
        l.SetCutVariables("cutEl_phomaxeta",    &myEl_phomaxeta );
        l.SetCutVariables("cutEl_sumpt3",       &myEl_sumpt3    );
        l.SetCutVariables("cutEl_sumpt4",       &myEl_sumpt4    );
        l.SetCutVariables("cutEl_dRtklead",     &myEl_dRtklead  );
        l.SetCutVariables("cutEl_dRtksub",      &myEl_dRtksub   );
        l.SetCutVariables("cutEl_MVAlead",      &myEl_MVAlead   );
        l.SetCutVariables("cutEl_MVAsub",       &myEl_MVAsub    );
        l.SetCutVariables("cutEl_diphomva",     &myEl_diphomva  );
        l.SetCutVariables("cutEl_CiClead",      &myEl_CiClead   );
        l.SetCutVariables("cutEl_CiCsub",       &myEl_CiCsub    );
        l.SetCutVariables("cutEl_mgg",          &myEl_mgg       );
        l.SetCutVariables("cutEl_MET",          &myEl_MET       );
        l.SetCutVariables("cutEl_METphi",       &myEl_METphi    );
        l.SetCutVariables("cutEl_presellead",   &myEl_presellead );
        l.SetCutVariables("cutEl_matchellead",  &myEl_matchellead);
        l.SetCutVariables("cutEl_preselsub",    &myEl_preselsub  );
        l.SetCutVariables("cutEl_matchelsub",   &myEl_matchelsub );
        l.SetCutVariables("cutEl_category",     &myEl_category );
        l.SetCutVariables("cutEl_ElePho",       &myEl_ElePho );
        l.SetCutVariables("cutEl_passelcuts",   &myEl_passelcuts );
    }


    // CiC initialization
    // FIXME should move this to GeneralFunctions
    l.runCiC = true;
    const int phoNCUTS = LoopAll::phoNCUTS;
    const int phoCiC6NCATEGORIES = LoopAll::phoCiC6NCATEGORIES;
    const int phoCiC4NCATEGORIES = LoopAll::phoCiC4NCATEGORIES;
    const int phoNCUTLEVELS = LoopAll::phoNCUTLEVELS;

    for(int iLevel=0; iLevel<phoNCUTLEVELS; ++iLevel) {
        float cic6_cuts_lead[phoNCUTS][phoCiC6NCATEGORIES];
        float cic6_cuts_sublead[phoNCUTS][phoCiC6NCATEGORIES];
        float cic4_cuts_lead[phoNCUTS][phoCiC4NCATEGORIES];
        float cic4_cuts_sublead[phoNCUTS][phoCiC4NCATEGORIES];
        float cic4pf_cuts_lead[phoNCUTS][phoCiC4NCATEGORIES];
        float cic4pf_cuts_sublead[phoNCUTS][phoCiC4NCATEGORIES];

        // get the cut values for the current photon CIC level 
        l.SetPhotonCutsInCategories((LoopAll::phoCiCIDLevel)iLevel, 
                &cic6_cuts_lead[0][0], &cic6_cuts_sublead[0][0], 
                &cic4_cuts_lead[0][0], &cic4_cuts_sublead[0][0],
                &cic4pf_cuts_lead[0][0], &cic4pf_cuts_sublead[0][0]);
        
        // rearrange the returned values to arrays with more meaningful names
        float * cic6_cuts_arrays_lead[phoNCUTS] = {
            &l.cic6_cut_lead_isosumoet[0][0], 
            &l.cic6_cut_lead_isosumoetbad[0][0], 
            &l.cic6_cut_lead_trkisooet[0][0], 
            &l.cic6_cut_lead_sieie[0][0],
            &l.cic6_cut_lead_hovere[0][0],
            &l.cic6_cut_lead_r9[0][0], 
            &l.cic6_cut_lead_drtotk_25_99[0][0], 
            &l.cic6_cut_lead_pixel[0][0] 
        };
        
        float * cic6_cuts_arrays_sublead[phoNCUTS] = {
            &l.cic6_cut_sublead_isosumoet[0][0], 
            &l.cic6_cut_sublead_isosumoetbad[0][0], 
            &l.cic6_cut_sublead_trkisooet[0][0], 
            &l.cic6_cut_sublead_sieie[0][0], 
            &l.cic6_cut_sublead_hovere[0][0], 
            &l.cic6_cut_sublead_r9[0][0],
            &l.cic6_cut_sublead_drtotk_25_99[0][0], 
            &l.cic6_cut_sublead_pixel[0][0]
        };

        float * cic4_cuts_arrays_lead[phoNCUTS] = {
            &l.cic4_cut_lead_isosumoet[0][0], 
            &l.cic4_cut_lead_isosumoetbad[0][0], 
            &l.cic4_cut_lead_trkisooet[0][0], 
            &l.cic4_cut_lead_sieie[0][0],
            &l.cic4_cut_lead_hovere[0][0], 
            &l.cic4_cut_lead_r9[0][0], 
            &l.cic4_cut_lead_drtotk_25_99[0][0], 
            &l.cic4_cut_lead_pixel[0][0] 
        };
        
        float * cic4_cuts_arrays_sublead[phoNCUTS] = {
            &l.cic4_cut_sublead_isosumoet[0][0], 
            &l.cic4_cut_sublead_isosumoetbad[0][0], 
            &l.cic4_cut_sublead_trkisooet[0][0], 
            &l.cic4_cut_sublead_sieie[0][0], 
            &l.cic4_cut_sublead_hovere[0][0], 
            &l.cic4_cut_sublead_r9[0][0],
            &l.cic4_cut_sublead_drtotk_25_99[0][0], 
            &l.cic4_cut_sublead_pixel[0][0]
        };
    
    float * cic4pf_cuts_arrays_lead[phoNCUTS] = {
            &l.cic4pf_cut_lead_isosumoet[0][0], 
            &l.cic4pf_cut_lead_isosumoetbad[0][0], 
            &l.cic4pf_cut_lead_trkisooet[0][0], 
            &l.cic4pf_cut_lead_sieie[0][0],
            &l.cic4pf_cut_lead_hovere[0][0], 
            &l.cic4pf_cut_lead_r9[0][0], 
            &l.cic4pf_cut_lead_drtotk_25_99[0][0], 
            &l.cic4pf_cut_lead_pixel[0][0] 
        };
        
        float * cic4pf_cuts_arrays_sublead[phoNCUTS] = {
            &l.cic4pf_cut_sublead_isosumoet[0][0], 
            &l.cic4pf_cut_sublead_isosumoetbad[0][0], 
            &l.cic4pf_cut_sublead_trkisooet[0][0], 
            &l.cic4pf_cut_sublead_sieie[0][0], 
            &l.cic4pf_cut_sublead_hovere[0][0], 
            &l.cic4pf_cut_sublead_r9[0][0],
            &l.cic4pf_cut_sublead_drtotk_25_99[0][0], 
            &l.cic4pf_cut_sublead_pixel[0][0]
        };
        for(int iCut=0; iCut<phoNCUTS; ++iCut) {
            for(int iCat=0; iCat<phoCiC6NCATEGORIES; ++iCat) {
                cic6_cuts_arrays_lead[iCut][iLevel*phoCiC6NCATEGORIES+iCat] = cic6_cuts_lead[iCut][iCat];
                cic6_cuts_arrays_sublead[iCut][iLevel*phoCiC6NCATEGORIES+iCat] = cic6_cuts_sublead[iCut][iCat];
            }
            for(int iCat=0; iCat<phoCiC4NCATEGORIES; ++iCat) {
                cic4_cuts_arrays_lead[iCut][iLevel*phoCiC4NCATEGORIES+iCat] = cic4_cuts_lead[iCut][iCat];
                cic4_cuts_arrays_sublead[iCut][iLevel*phoCiC4NCATEGORIES+iCat] = cic4_cuts_sublead[iCut][iCat];
            }
            for(int iCat=0; iCat<phoCiC4NCATEGORIES; ++iCat) {
                cic4pf_cuts_arrays_lead[iCut][iLevel*phoCiC4NCATEGORIES+iCat] = cic4pf_cuts_lead[iCut][iCat];
                cic4pf_cuts_arrays_sublead[iCut][iLevel*phoCiC4NCATEGORIES+iCat] = cic4pf_cuts_sublead[iCut][iCat];
        }
        }
    } // end of loop over all photon cut levels
    
    //--------------------

    if( tmvaPerVtxWeights != ""  ) {
	if( tmvaPerVtxVariables.empty() ) {
	    tmvaPerVtxVariables.push_back("ptbal"), tmvaPerVtxVariables.push_back("ptasym"), tmvaPerVtxVariables.push_back("logsumpt2");
	    if( addConversionToMva ) {
		tmvaPerVtxVariables.push_back("limPullToConv");
		tmvaPerVtxVariables.push_back("nConv");
	    }
	}
        tmvaPerVtxReader_ = new TMVA::Reader( "!Color:!Silent" );
        HggVertexAnalyzer::bookVariables( *tmvaPerVtxReader_, tmvaPerVtxVariables );
        tmvaPerVtxReader_->BookMVA( tmvaPerVtxMethod, tmvaPerVtxWeights );
    } else {
        tmvaPerVtxReader_ = 0;
    }
    if( tmvaPerEvtWeights != "" ) {
        tmvaPerEvtReader_ = new TMVA::Reader( "!Color:!Silent" );
        HggVertexAnalyzer::bookPerEventVariables( *tmvaPerEvtReader_ );
        tmvaPerEvtReader_->BookMVA( tmvaPerEvtMethod, tmvaPerEvtWeights );
    } else {
        tmvaPerEvtReader_ = 0;
    }
    assert( !mvaVertexSelection || tmvaPerVtxReader_ != 0 );

    eSmearDataPars.categoryType = "2CatR9_EBEBm4EE";
    eSmearDataPars.byRun = true;
    //eSmearDataPars.n_categories = 4; //GF
    eSmearDataPars.n_categories = 6; //GF
    std::cerr << "Reading energy scale offsets " << scale_offset_file << std::endl;
    readEnergyScaleOffsets(scale_offset_file, eSmearDataPars.scale_offset_byrun, eSmearDataPars.photon_categories);
    // if the scale offset file defines the categories set the category type to automatic
    if( ! eSmearDataPars.photon_categories.empty() ) {
        eSmearDataPars.categoryType = "Automagic";
        eSmearDataPars.n_categories = -1;
    }
    // E resolution smearing NOT applied to data 
    eSmearDataPars.smearing_sigma["EBHighR9"] = 0.;
    eSmearDataPars.smearing_sigma["EBLowR9"]  = 0.;
    eSmearDataPars.smearing_sigma["EBm4HighR9"] = 0.;
    eSmearDataPars.smearing_sigma["EBm4LowR9"]  = 0.;
    eSmearDataPars.smearing_sigma["EEHighR9"] = 0.;
    eSmearDataPars.smearing_sigma["EELowR9"]  = 0.;
    // E resolution systematics NOT applied to data 
    eSmearDataPars.smearing_sigma_error["EBHighR9"] = 0.;
    eSmearDataPars.smearing_sigma_error["EBLowR9"]  = 0.;
    eSmearDataPars.smearing_sigma_error["EBm4HighR9"] = 0.;
    eSmearDataPars.smearing_sigma_error["EBm4LowR9"]  = 0.;
    eSmearDataPars.smearing_sigma_error["EEHighR9"] = 0.;
    eSmearDataPars.smearing_sigma_error["EELowR9"]  = 0.;
    
    // energy scale corrections to Data
    eScaleDataSmearer = new EnergySmearer( eSmearDataPars );
    eScaleDataSmearer->name("E_scale_data");
    eScaleDataSmearer->doEnergy(true);
    eScaleDataSmearer->scaleOrSmear(true);
    
    if( scale_offset_error_file.empty() ) {
        //eSmearPars.categoryType = "2CatR9_EBEE"; //GF
        eSmearPars.categoryType = "2CatR9_EBEBm4EE";
        eSmearPars.byRun = false;
        //eSmearPars.n_categories = 4; //GF
        eSmearPars.n_categories = 6;
        // E scale is shifted for data, NOT for MC 
        eSmearPars.scale_offset["EBHighR9"] = 0.;
        eSmearPars.scale_offset["EBLowR9"]  = 0.;
        eSmearPars.scale_offset["EBm4HighR9"] = 0.;
        eSmearPars.scale_offset["EBm4LowR9"]  = 0.;
        eSmearPars.scale_offset["EEHighR9"] = 0.;
        eSmearPars.scale_offset["EELowR9"]  = 0.;
        // E scale systematics are applied to MC, NOT to data
        eSmearPars.scale_offset_error["EBHighR9"] = scale_offset_error_EBHighR9;
        eSmearPars.scale_offset_error["EBLowR9"]  = scale_offset_error_EBLowR9;
        eSmearPars.scale_offset_error["EBm4HighR9"] = scale_offset_error_EBHighR9;
        eSmearPars.scale_offset_error["EBm4LowR9"]  = scale_offset_error_EBLowR9;
        eSmearPars.scale_offset_error["EEHighR9"] = scale_offset_error_EEHighR9;
        eSmearPars.scale_offset_error["EELowR9"]  = scale_offset_error_EELowR9;
        // E resolution smearing applied to MC 
        eSmearPars.smearing_sigma["EBHighR9"] = smearing_sigma_EBHighR9;
        eSmearPars.smearing_sigma["EBLowR9"]  = smearing_sigma_EBLowR9;
        eSmearPars.smearing_sigma["EBm4HighR9"] = smearing_sigma_EBm4HighR9;
        eSmearPars.smearing_sigma["EBm4LowR9"]  = smearing_sigma_EBm4LowR9;
        eSmearPars.smearing_sigma["EEHighR9"] = smearing_sigma_EEHighR9;
        eSmearPars.smearing_sigma["EELowR9"]  = smearing_sigma_EELowR9;
        // E resolution systematics applied to MC 
        eSmearPars.smearing_sigma_error["EBHighR9"] = smearing_sigma_error_EBHighR9;
        eSmearPars.smearing_sigma_error["EBLowR9"]  = smearing_sigma_error_EBLowR9;
        eSmearPars.smearing_sigma_error["EBm4HighR9"] = smearing_sigma_error_EBm4HighR9;
        eSmearPars.smearing_sigma_error["EBm4LowR9"]  = smearing_sigma_error_EBm4LowR9;
        eSmearPars.smearing_sigma_error["EEHighR9"] = smearing_sigma_error_EEHighR9;
        eSmearPars.smearing_sigma_error["EELowR9"]  = smearing_sigma_error_EELowR9;
        // error on photon corrections set to a fraction of the correction itself; number below is tentative (GF: push it to .dat)  
        eSmearPars.corrRelErr  = 0.5;
    } else {
        // Read energy scale errors and energy smaerings from dat files
        assert( ! scale_offset_error_file.empty() && ! smearing_file.empty() );

        // Use the same format used for the run-dependent energy corrections
        EnergySmearer::energySmearingParameters::eScaleVector tmp_scale_offset, tmp_smearing;
        EnergySmearer::energySmearingParameters::phoCatVector tmp_scale_cat, tmp_smearing_cat;
        readEnergyScaleOffsets(scale_offset_error_file, tmp_scale_offset, tmp_scale_cat,false);
        readEnergyScaleOffsets(smearing_file, tmp_smearing, tmp_smearing_cat,false);

        // make sure that the scale correction and smearing info is as expected
        assert( tmp_scale_offset.size() == 1); assert( tmp_smearing.size() == 1 );
        assert( ! tmp_smearing_cat.empty() );
        /// assert( tmp_smearing_cat == tmp_scale_cat );

        // copy the read info to the smarer parameters
        eSmearPars.categoryType = "Automagic";
        eSmearPars.byRun = false;
        eSmearPars.n_categories = tmp_smearing_cat.size();
        eSmearPars.photon_categories = tmp_smearing_cat;
        
        eSmearPars.scale_offset = tmp_scale_offset[0].scale_offset;
        eSmearPars.scale_offset_error = tmp_scale_offset[0].scale_offset_error;
        
        eSmearPars.smearing_sigma = tmp_smearing[0].scale_offset;
        eSmearPars.smearing_sigma_error = tmp_smearing[0].scale_offset_error;
    }
    
    // energy scale systematics to MC
    eScaleSmearer = new EnergySmearer( eSmearPars );
    eScaleSmearer->name("E_scale");
    eScaleSmearer->doEnergy(true);
    eScaleSmearer->scaleOrSmear(true);
    eScaleSmearer->syst_only(true);
    
    eResolSmearer = new EnergySmearer( eSmearPars );
    eResolSmearer->name("E_res");
    eResolSmearer->doEnergy(false);
    eResolSmearer->scaleOrSmear(false);

    if( doEcorrectionSmear ) {
        eCorrSmearer = new EnergySmearer( eSmearPars );
        eCorrSmearer->name("E_corr");
        // activating pho corrections to this instance of EnergySmearer, implies that it won't touch Escale and Eresolution
        eCorrSmearer->doCorrections(true); 
    }
    
    if (l.typerun == 2 || l.typerun == 1) {
    }
    // MassResolution 
    massResolutionCalculator = new MassResolution();    
    /* -------------------------------------------------------------------------------------------
       Pileup Reweighting
       https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupReweighting
       ----------------------------------------------------------------------------------------------  */
    if (puHist != "" && puHist != "auto" ) {
        if(PADEBUG) 
            cout << "Opening PU file"<<endl;
        TFile* puFile = TFile::Open( puHist );
        if (puFile) {
        TH1 * target = 0;
        
        if( puTarget != "" ) {
        TFile * puTargetFile = TFile::Open( puTarget ); 
        assert( puTargetFile != 0 );
        target = (TH1*)puTargetFile->Get("pileup");
        if( target == 0 ) { target = (TH1*)puTargetFile->Get("target_pileup"); }
        target->Scale( 1. / target->Integral() );
        }
        
            if( puMap != "" ) {
                loadPuMap(puMap, puFile, target); 
            } else {
                loadPuWeights(0, puFile, target);
            }
            puFile->Close();
        }
        else {
            cout<<"Error opening " <<puHist<<" pileup reweighting histogram, using 1.0"<<endl; 
            weights[0].resize(50);
            for (unsigned int i=0; i<weights[0].size(); i++) weights[0][i] = 1.0;
        }
        if(PADEBUG) 
            cout << "Opening PU file END"<<endl;
    } else if ( puHist == "auto" ) {
    TFile * puTargetFile = TFile::Open( puTarget ); 
    assert( puTargetFile != 0 );
    puTargetHist = (TH1*)puTargetFile->Get("pileup");
    if( puTargetHist == 0 ) { puTargetHist = (TH1*)puTargetFile->Get("target_pileup"); }
    puTargetHist = (TH1*)puTargetHist->Clone();
    puTargetHist->SetDirectory(0);
    puTargetHist->Scale( 1. / puTargetHist->Integral() );
    puTargetFile->Close();
    }

    if( recomputeBetas || recorrectJets || rerunJetMva || recomputeJetWp ) {
	std::cout << "JetHandler: \n" 
		  << "recomputeBetas " << recomputeBetas << "\n" 
		  << "recorrectJets " << recorrectJets << "\n" 
		  << "rerunJetMva " << rerunJetMva << "\n" 
		  << "recomputeJetWp " << recomputeJetWp 
		  << std::endl;
	jetHandler_ = new JetHandler(jetHandlerCfg, l);
    }

    if( emulateBeamspot || reweighBeamspot ) {
	assert( emulatedBeamspotWidth != 0. );
	beamspotWidth = emulatedBeamspotWidth;
    }
    if( beamspotWidth == 0. ) {
	beamspotWidth = (dataIs2011 ? 5.8 : 4.8);
    }
    
    // Load up instances of PhotonFix for local coordinate calculations
    /*  
        PhotonFix::initialise("4_2",photonFixDat);  
        std::cout << "Regression corrections from -> " << regressionFile.c_str() << std::endl;
        fgbr = new TFile(regressionFile.c_str(),"READ");
        fReadereb = (GBRForest*)fgbr->Get("EBCorrection");
        fReaderebvariance = (GBRForest*)fgbr->Get("EBUncertainty");  
        fReaderee = (GBRForest*)fgbr->Get("EECorrection");
        fReadereevariance = (GBRForest*)fgbr->Get("EEUncertainty");      
        fgbr->Close();
    */
    
    // -------------------------------------------------------------------- 
    if(PADEBUG) 
        cout << "InitRealPhotonAnalysis END"<<endl;

    // FIXME book of additional variables

}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::setupEscaleSmearer()
{
    if( splitEscaleSyst ) {
        EnergySmearer::energySmearingParameters::eScaleVector tmp_scale_offset;
	EnergySmearer::energySmearingParameters::phoCatVector tmp_scale_cat;
	readEnergyScaleOffsets(scale_offset_corr_error_file, tmp_scale_offset, tmp_scale_cat,false);
	assert( tmp_scale_offset.size() == 1); assert( ! tmp_scale_cat.empty() );
	
	eScaleCorrPars.categoryType = "Automagic";
	eScaleCorrPars.byRun = false;
        eScaleCorrPars.n_categories = tmp_scale_cat.size();
        eScaleCorrPars.photon_categories = tmp_scale_cat;

	eScaleCorrPars.scale_offset = tmp_scale_offset[0].scale_offset;
        eScaleCorrPars.scale_offset_error = tmp_scale_offset[0].scale_offset_error;

        eScaleCorrPars.smearing_sigma = tmp_scale_offset[0].scale_offset;
        eScaleCorrPars.smearing_sigma_error = tmp_scale_offset[0].scale_offset_error;
        
	EnergySmearer::energySmearingParameters::phoCatVectorIt icat = tmp_scale_cat.begin();
	for( ; icat != tmp_scale_cat.end(); ++icat ) {
	    EnergySmearer * theSmear = new EnergySmearer( eScaleSmearer, EnergySmearer::energySmearingParameters::phoCatVector(1,*icat) );
	    theSmear->name( eScaleSmearer->name()+"_"+icat->name );
	    theSmear->syst_only(true);
	    std::cout << "Uncorrelated single photon category smearer " << theSmear->name() << std::endl; 
	    photonSmearers_.push_back(theSmear);
	    eScaleSmearers_.push_back(theSmear);
	}

	eScaleCorrSmearer = new EnergySmearer( eScaleCorrPars );
	eScaleCorrSmearer->name("E_scaleCorr");
	eScaleCorrSmearer->doEnergy(true);
	eScaleCorrSmearer->scaleOrSmear(true);
	eScaleCorrSmearer->syst_only(true);
	photonSmearers_.push_back(eScaleCorrSmearer);
	eScaleSmearers_.push_back(eScaleCorrSmearer);

	std::cout << "Uncorrelated single photon category smearer " << eScaleCorrSmearer->name() << std::endl; 
	
    } else {
	photonSmearers_.push_back(eScaleSmearer);
	eScaleSmearers_.push_back(eScaleSmearer);
    }
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::setupEscaleSyst(LoopAll &l)
{
    for(std::vector<EnergySmearer*>::iterator ei=eScaleSmearers_.begin(); ei!=eScaleSmearers_.end(); ++ei){
	systPhotonSmearers_.push_back( *ei );
	std::vector<std::string> sys(1,(*ei)->name());
	std::vector<int> sys_t(1,-1);   // -1 for signal, 1 for background 0 for both
	l.rooContainer->MakeSystematicStudy(sys,sys_t);
    }
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::setupEresolSmearer()
{
    if( splitEresolSyst ) {
        // Use the same format used for the run-dependent energy corrections
        EnergySmearer::energySmearingParameters::eScaleVector tmp_smearing;
        EnergySmearer::energySmearingParameters::phoCatVector tmp_smearing_cat;
        readEnergyScaleOffsets(corr_smearing_file, tmp_smearing, tmp_smearing_cat,false);

        // make sure that the scale correction and smearing info is as expected
        assert( tmp_smearing.size() == 1 );
        assert( ! tmp_smearing_cat.empty() );

        // copy the read info to the smarer parameters
        eResolCorrPars.categoryType = "Automagic";
        eResolCorrPars.byRun = false;
        eResolCorrPars.n_categories = tmp_smearing_cat.size();
        eResolCorrPars.photon_categories = tmp_smearing_cat;
        
        eResolCorrPars.scale_offset = tmp_smearing[0].scale_offset;
        eResolCorrPars.scale_offset_error = tmp_smearing[0].scale_offset_error;
        
        eResolCorrPars.smearing_sigma = tmp_smearing[0].scale_offset;
        eResolCorrPars.smearing_sigma_error = tmp_smearing[0].scale_offset_error;

	EnergySmearer::energySmearingParameters::phoCatVectorIt icat = tmp_smearing_cat.begin();
	for( ; icat != tmp_smearing_cat.end(); ++icat ) {
	    EnergySmearer * theSmear = new EnergySmearer( eResolSmearer, EnergySmearer::energySmearingParameters::phoCatVector(1,*icat) );
	    theSmear->name( eResolSmearer->name()+"_"+icat->name );
	    std::cout << "Uncorrelated single photon category smearer " << theSmear->name() << std::endl; 
	    photonSmearers_.push_back(theSmear);
	    eResolSmearers_.push_back(theSmear);
	}
	
	eResolCorrSmearer = new EnergySmearer( eResolCorrPars );
	eResolCorrSmearer->name("E_resCorr");
	eResolCorrSmearer->doEnergy(true);
	eResolCorrSmearer->scaleOrSmear(true);
	eResolCorrSmearer->syst_only(true);
	photonSmearers_.push_back(eResolCorrSmearer);
	eResolSmearers_.push_back(eResolCorrSmearer);
	std::cout << "Uncorrelated single photon category smearer " << eResolCorrSmearer->name() << std::endl; 

    } else {
	photonSmearers_.push_back(eResolSmearer);
	eResolSmearers_.push_back(eResolSmearer);
    }
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::setupEresolSyst(LoopAll &l)
{
    for(std::vector<EnergySmearer*>::iterator ei=eResolSmearers_.begin(); ei!=eResolSmearers_.end(); ++ei){
	systPhotonSmearers_.push_back( *ei );
	std::vector<std::string> sys(1,(*ei)->name());
	std::vector<int> sys_t(1,-1);   // -1 for signal, 1 for background 0 for both
	l.rooContainer->MakeSystematicStudy(sys,sys_t);
    }
}

// ----------------------------------------------------------------------------------------------------
bool PhotonAnalysis::Analysis(LoopAll& l, Int_t jentry) 
{
    if(PADEBUG) 
        cout << "Analysis START"<<endl;
    pho_presel.clear();

    //remove process ID 18 from gamma+jet to avoid double counting with born+box
    if (l.itype[l.current]==3 && l.process_id==18) return false;

    //apply pileup reweighting
    unsigned int n_pu = l.pu_n;
    float weight =1.;
    if (l.itype[l.current] !=0 && puHist != "") {
        std::vector<double> & puweights = weights.find( l.itype[l.current] ) != weights.end() ? weights[ l.itype[l.current] ] : weights[0]; 
        if(n_pu<puweights.size()){
            weight *= puweights[n_pu];
        }    
        else{ //should not happen as we have a weight for all simulated n_pu multiplicities!
            cout<<"n_pu ("<< n_pu<<") too big ("<<puweights.size()<<"), event will not be reweighted for pileup"<<endl;
        }
    }

    if( pho_presel.empty() ) { 
        PreselectPhotons(l,jentry);
    }

    if( pho_acc.size() < 2 || pho_et[ pho_acc[0] ] < presel_scet1 ) return false;

    int leadLevel=LoopAll::phoSUPERTIGHT, subLevel=LoopAll::phoSUPERTIGHT;

    //Fill histograms to use as denominator (pre-selection only) and numerator (selection applied)
    //for photon ID efficiency calculation.  To avoid ambiguities concerning vertex choice, use only 
    //events with one diphoton pair (close to 100% of signal events)
    if (l.dipho_n==1) {

        int ivtx = l.dipho_vtxind[0];
        int lead = l.dipho_leadind[0];
        int sublead = l.dipho_subleadind[0];

        TLorentzVector lead_p4 = l.get_pho_p4(lead,ivtx,&corrected_pho_energy[0]); 
        TLorentzVector sublead_p4 = l.get_pho_p4(sublead,ivtx,&corrected_pho_energy[0]); 
        float leadEta = ((TVector3 *)l.sc_xyz->At(l.pho_scind[lead]))->Eta();
        float subleadEta = ((TVector3 *)l.sc_xyz->At(l.pho_scind[sublead]))->Eta();

        //apply pre-selection
        bool passpresel = true;
        if(lead_p4.Pt() < 40. || sublead_p4.Pt() < 30. || 
           fabs(leadEta) > 2.5 || fabs(subleadEta) > 2.5 || 
           ( fabs(leadEta) > 1.4442 && fabs(leadEta) < 1.566 ) || ( fabs(subleadEta) > 1.4442 && fabs(subleadEta) < 1.566 ))
            passpresel = false;
        if (lead != sublead && passpresel) {

            int leadpho_category = l.PhotonCategory(lead, 2, 2);
            int subleadpho_category = l.PhotonCategory(sublead, 2, 2);

            //Fill eta and pt distributions after pre-selection only (efficiency denominator)
            l.FillHist("pho1_pt_presel",0,lead_p4.Pt(), weight);
            l.FillHist("pho2_pt_presel",0,sublead_p4.Pt(), weight);
            l.FillHist("pho1_eta_presel",0,leadEta, weight);
            l.FillHist("pho2_eta_presel",0,subleadEta, weight);

            l.FillHist("pho1_pt_presel",leadpho_category+1,lead_p4.Pt(), weight);
            l.FillHist("pho2_pt_presel",subleadpho_category+1,sublead_p4.Pt(), weight);
            l.FillHist("pho1_eta_presel",leadpho_category+1,leadEta, weight);
            l.FillHist("pho2_eta_presel",subleadpho_category+1,subleadEta, weight);

            //Apply single photon CiC selection and fill eta and pt distributions (efficiency numerator)
            std::vector<std::vector<bool> > ph_passcut;
            if( l.PhotonCiCSelectionLevel(lead, ivtx, ph_passcut, 4, 0, &corrected_pho_energy[0]) >=  (LoopAll::phoCiCIDLevel) leadLevel) {
                l.FillHist("pho1_pt_sel",0,lead_p4.Pt(), weight);
                l.FillHist("pho1_eta_sel",0,leadEta, weight);
                l.FillHist("pho1_pt_sel",leadpho_category+1,lead_p4.Pt(), weight);
                l.FillHist("pho1_eta_sel",leadpho_category+1,leadEta, weight);
            }
            if( l.PhotonCiCSelectionLevel(sublead, ivtx, ph_passcut, 4, 1, &corrected_pho_energy[0]) >=  (LoopAll::phoCiCIDLevel) subLevel ) {
                l.FillHist("pho2_pt_sel",0,sublead_p4.Pt(), weight);
                l.FillHist("pho2_eta_sel",0,subleadEta, weight);
                l.FillHist("pho2_pt_sel",subleadpho_category+1,sublead_p4.Pt(), weight);
                l.FillHist("pho2_eta_sel",subleadpho_category+1,subleadEta, weight);
            }
        }
    }

    //Apply diphoton CiC selection
    int dipho_id = l.DiphotonCiCSelection((LoopAll::phoCiCIDLevel) leadLevel, (LoopAll::phoCiCIDLevel) subLevel, 40., 30.0, 4, applyPtoverM, &corrected_pho_energy[0]);

    if (dipho_id > -1){
        std::pair<int,int> diphoton_index = std::make_pair(l.dipho_leadind[dipho_id],l.dipho_subleadind[dipho_id]);
        TLorentzVector lead_p4 = l.get_pho_p4( l.dipho_leadind[dipho_id], l.dipho_vtxind[dipho_id], &corrected_pho_energy[0]);
        TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[dipho_id], l.dipho_vtxind[dipho_id], &corrected_pho_energy[0]);
        TLorentzVector Higgs = lead_p4 + sublead_p4;    
      
        //// TLorentzVector *lead_p4 = (TLorentzVector*)l.pho_p4->At(diphoton_index.first);
        //// TLorentzVector *sublead_p4 = (TLorentzVector*)l.pho_p4->At(diphoton_index.second);
        //// TLorentzVector Higgs = *lead_p4 + *sublead_p4;

        // Calculate the Rest Frame Quantities:
        TLorentzVector *rest_lead_p4 = (TLorentzVector*) lead_p4.Clone();
        TLorentzVector *rest_sublead_p4 = (TLorentzVector*) sublead_p4.Clone();
        TVector3 higgsBoostVector = Higgs.BoostVector();
        rest_lead_p4->Boost(higgsBoostVector);        // lead photon in Higgs Decay Frame
        rest_sublead_p4->Boost(higgsBoostVector);    // sub-lead photon in Higgs Decay Frame

        // Calculate variables to be filled
        float decayAngle  = l.DeltaPhi(lead_p4.Phi(),sublead_p4.Phi());
        float helicityAngle = fabs(lead_p4.E()-sublead_p4.E())/Higgs.P();
        float maxeta = lead_p4.Eta();
        if (fabs(sublead_p4.Eta())>fabs(lead_p4.Eta())) maxeta = sublead_p4.Eta();
        float HT = lead_p4.Pt() + sublead_p4.Pt();
        float mH = Higgs.M();


        //Fill histograms according to diphoton or single photon category, as appropriate

        int dipho_category = l.DiphotonCategory(diphoton_index.first, diphoton_index.second, Higgs.Pt(), 2, 2, 2);
        int leadpho_category = l.PhotonCategory(diphoton_index.first, 2, 2);
        int subleadpho_category = l.PhotonCategory(diphoton_index.second, 2, 2);
      
    
        //Only fill histograms for QCD and GJet if bkgCat is not promptprompt
        //if ((l.itype[l.current]==1 || l.itype[l.current]==2 || l.itype[l.current]==3) && bkgCat==promptprompt) return;

        l.FillHist("pt",0, Higgs.Pt(), weight);
        l.FillHist("ptOverM",0, Higgs.Pt()/Higgs.M(), weight);
        l.FillHist("eta",0, Higgs.Eta(), weight);
        l.FillHist("decayAngle",0, decayAngle, weight);
        l.FillHist("helicityAngle",0, helicityAngle, weight);
        l.FillHist("pho1_pt",0,lead_p4.Pt(), weight);
        l.FillHist("pho2_pt",0,sublead_p4.Pt(), weight);
        l.FillHist("pho1_ptOverM",0,lead_p4.Pt()/Higgs.M(), weight);
        l.FillHist("pho2_ptOverM",0,sublead_p4.Pt()/Higgs.M(), weight);
        l.FillHist("pho1_eta",0,lead_p4.Eta(), weight);
        l.FillHist("pho2_eta",0,sublead_p4.Eta(), weight);
        l.FillHist("pho_r9",0,l.pho_r9[diphoton_index.first], weight);
        l.FillHist("pho_r9",0,l.pho_r9[diphoton_index.second], weight);
        l.FillHist("maxeta",0,maxeta, weight);
        l.FillHist("ht",0, HT, weight);
        l.FillHist2D("ht_vs_m",0, HT, mH, weight);

        l.FillHist("pt",dipho_category+1, Higgs.Pt(), weight);
        l.FillHist("ptOverM",dipho_category+1, Higgs.Pt()/Higgs.M(), weight);
        l.FillHist("eta",dipho_category+1, Higgs.Eta(), weight);
        l.FillHist("decayAngle",dipho_category+1, decayAngle, weight);
        l.FillHist("helicityAngle",dipho_category+1, helicityAngle, weight);
        l.FillHist("pho1_pt",dipho_category+1,lead_p4.Pt(), weight);
        l.FillHist("pho2_pt",dipho_category+1,sublead_p4.Pt(), weight);
        l.FillHist("pho1_ptOverM",dipho_category+1,lead_p4.Pt()/Higgs.M(), weight);
        l.FillHist("pho2_ptOverM",dipho_category+1,sublead_p4.Pt()/Higgs.M(), weight);
        l.FillHist("pho1_eta",dipho_category+1,lead_p4.Eta(), weight);
        l.FillHist("pho2_eta",dipho_category+1,sublead_p4.Eta(), weight);
        l.FillHist("pho_r9",dipho_category+1,l.pho_r9[diphoton_index.first], weight);
        l.FillHist("pho_r9",dipho_category+1,l.pho_r9[diphoton_index.second], weight);
        l.FillHist("maxeta",dipho_category+1,maxeta, weight);
        l.FillHist("ht",dipho_category+1, HT, weight);
        l.FillHist2D("ht_vs_m",dipho_category+1, mH, HT, weight);

        //Fill separately for low and high mass sidebands and for signal region

        if (mH>100 && mH<110) {
            l.FillHist("pt_mlow",0, Higgs.Pt(), weight);
            l.FillHist("ptOverM_mlow",0, Higgs.Pt()/Higgs.M(), weight);
            l.FillHist("eta_mlow",0, Higgs.Eta(), weight);
            l.FillHist("decayAngle_mlow",0, decayAngle, weight);
            l.FillHist("helicityAngle_mlow",0, helicityAngle, weight);
            l.FillHist("pho1_pt_mlow",0,lead_p4.Pt(), weight);
            l.FillHist("pho2_pt_mlow",0,sublead_p4.Pt(), weight);
            //l.FillHist("pho1_pt_mlow",0,lead_p4.Pt()*R_low, weight);
            //l.FillHist("pho2_pt_mlow",0,sublead_p4.Pt()*R_low, weight);
            l.FillHist("pho1_ptOverM_mlow",0,lead_p4.Pt()/Higgs.M(), weight);
            l.FillHist("pho2_ptOverM_mlow",0,sublead_p4.Pt()/Higgs.M(), weight);
            l.FillHist("pho1_eta_mlow",0,lead_p4.Eta(), weight);
            l.FillHist("pho2_eta_mlow",0,sublead_p4.Eta(), weight);
            l.FillHist("pho_r9_mlow",0,l.pho_r9[diphoton_index.first], weight);
            l.FillHist("pho_r9_mlow",0,l.pho_r9[diphoton_index.second], weight);
            l.FillHist("maxeta_mlow",0,maxeta, weight);
            l.FillHist("ht_mlow",0, HT, weight);

            l.FillHist("pt_mlow",dipho_category+1, Higgs.Pt(), weight);
            l.FillHist("ptOverM_mlow",dipho_category+1, Higgs.Pt()/Higgs.M(), weight);
            l.FillHist("eta_mlow",dipho_category+1, Higgs.Eta(), weight);
            l.FillHist("decayAngle_mlow",dipho_category+1, decayAngle, weight);
            l.FillHist("helicityAngle_mlow",dipho_category+1, helicityAngle, weight);
            l.FillHist("pho1_pt_mlow",dipho_category+1,lead_p4.Pt(), weight);
            l.FillHist("pho2_pt_mlow",dipho_category+1,sublead_p4.Pt(), weight);
            l.FillHist("pho1_ptOverM_mlow",dipho_category+1,lead_p4.Pt()/Higgs.M(), weight);
            l.FillHist("pho2_ptOverM_mlow",dipho_category+1,sublead_p4.Pt()/Higgs.M(), weight);
            l.FillHist("pho1_eta_mlow",dipho_category+1,lead_p4.Eta(), weight);
            l.FillHist("pho2_eta_mlow",dipho_category+1,sublead_p4.Eta(), weight);
            l.FillHist("pho_r9_mlow",dipho_category+1,l.pho_r9[diphoton_index.first], weight);
            l.FillHist("pho_r9_mlow",dipho_category+1,l.pho_r9[diphoton_index.second], weight);
            l.FillHist("maxeta_mlow",dipho_category+1,maxeta, weight);
            l.FillHist("ht_mlow",dipho_category+1, HT, weight);
        } else if (mH>126 && mH<145) {
            l.FillHist("pt_mhigh",0, Higgs.Pt(), weight);
            l.FillHist("ptOverM_mhigh",0, Higgs.Pt()/Higgs.M(), weight);
            l.FillHist("eta_mhigh",0, Higgs.Eta(), weight);
            l.FillHist("decayAngle_mhigh",0, decayAngle, weight);
            l.FillHist("helicityAngle_mhigh",0, helicityAngle, weight);
            l.FillHist("pho1_pt_mhigh",0,lead_p4.Pt(), weight);
            l.FillHist("pho2_pt_mhigh",0,sublead_p4.Pt(), weight);
            //l.FillHist("pho1_pt_mhigh",0,lead_p4.Pt()*R_high, weight);
            //l.FillHist("pho2_pt_mhigh",0,sublead_p4.Pt()*R_high, weight);
            l.FillHist("pho1_ptOverM_mhigh",0,lead_p4.Pt()/Higgs.M(), weight);
            l.FillHist("pho2_ptOverM_mhigh",0,sublead_p4.Pt()/Higgs.M(), weight);
            l.FillHist("pho1_eta_mhigh",0,lead_p4.Eta(), weight);
            l.FillHist("pho2_eta_mhigh",0,sublead_p4.Eta(), weight);
            l.FillHist("pho_r9_mhigh",0,l.pho_r9[diphoton_index.first], weight);
            l.FillHist("pho_r9_mhigh",0,l.pho_r9[diphoton_index.second], weight);
            l.FillHist("maxeta_mhigh",0,maxeta, weight);
            l.FillHist("ht_mhigh",0, HT, weight);

            l.FillHist("pt_mhigh",dipho_category+1, Higgs.Pt(), weight);
            l.FillHist("ptOverM_mhigh",dipho_category+1, Higgs.Pt()/Higgs.M(), weight);
            l.FillHist("eta_mhigh",dipho_category+1, Higgs.Eta(), weight);
            l.FillHist("decayAngle_mhigh",dipho_category+1, decayAngle, weight);
            l.FillHist("helicityAngle_mhigh",dipho_category+1, helicityAngle, weight);
            l.FillHist("pho1_pt_mhigh",dipho_category+1,lead_p4.Pt(), weight);
            l.FillHist("pho2_pt_mhigh",dipho_category+1,sublead_p4.Pt(), weight);
            l.FillHist("pho1_ptOverM_mhigh",dipho_category+1,lead_p4.Pt()/Higgs.M(), weight);
            l.FillHist("pho2_ptOverM_mhigh",dipho_category+1,sublead_p4.Pt()/Higgs.M(), weight);
            l.FillHist("pho1_eta_mhigh",dipho_category+1,lead_p4.Eta(), weight);
            l.FillHist("pho2_eta_mhigh",dipho_category+1,sublead_p4.Eta(), weight);
            l.FillHist("pho_r9_mhigh",dipho_category+1,l.pho_r9[diphoton_index.first], weight);
            l.FillHist("pho_r9_mhigh",dipho_category+1,l.pho_r9[diphoton_index.second], weight);
            l.FillHist("maxeta_mhigh",dipho_category+1,maxeta, weight);
            l.FillHist("ht_mhigh",dipho_category+1, HT, weight);
        } else if (mH>=110 && mH<=126) {
            l.FillHist("pt_msig",0, Higgs.Pt(), weight);
            l.FillHist("ptOverM_msig",0, Higgs.Pt()/Higgs.M(), weight);
            l.FillHist("eta_msig",0, Higgs.Eta(), weight);
            l.FillHist("decayAngle_msig",0, decayAngle, weight);
            l.FillHist("helicityAngle_msig",0, helicityAngle, weight);
            l.FillHist("pho1_pt_msig",0,lead_p4.Pt(), weight);
            l.FillHist("pho2_pt_msig",0,sublead_p4.Pt(), weight);
            l.FillHist("pho1_ptOverM_msig",0,lead_p4.Pt()/Higgs.M(), weight);
            l.FillHist("pho2_ptOverM_msig",0,sublead_p4.Pt()/Higgs.M(), weight);
            l.FillHist("pho1_eta_msig",0,lead_p4.Eta(), weight);
            l.FillHist("pho2_eta_msig",0,sublead_p4.Eta(), weight);
            l.FillHist("pho_r9_msig",0,l.pho_r9[diphoton_index.first], weight);
            l.FillHist("pho_r9_msig",0,l.pho_r9[diphoton_index.second], weight);
            l.FillHist("maxeta_msig",0,maxeta, weight);
            l.FillHist("ht_msig",0, HT, weight);

            l.FillHist("pt_msig",dipho_category+1, Higgs.Pt(), weight);
            l.FillHist("ptOverM_msig",dipho_category+1, Higgs.Pt()/Higgs.M(), weight);
            l.FillHist("eta_msig",dipho_category+1, Higgs.Eta(), weight);
            l.FillHist("decayAngle_msig",dipho_category+1, decayAngle, weight);
            l.FillHist("helicityAngle_msig",dipho_category+1, helicityAngle, weight);
            l.FillHist("pho1_pt_msig",dipho_category+1,lead_p4.Pt(), weight);
            l.FillHist("pho2_pt_msig",dipho_category+1,sublead_p4.Pt(), weight);
            l.FillHist("pho1_ptOverM_msig",dipho_category+1,lead_p4.Pt()/Higgs.M(), weight);
            l.FillHist("pho2_ptOverM_msig",dipho_category+1,sublead_p4.Pt()/Higgs.M(), weight);
            l.FillHist("pho1_eta_msig",dipho_category+1,lead_p4.Eta(), weight);
            l.FillHist("pho2_eta_msig",dipho_category+1,sublead_p4.Eta(), weight);
            l.FillHist("pho_r9_msig",dipho_category+1,l.pho_r9[diphoton_index.first], weight);
            l.FillHist("pho_r9_msig",dipho_category+1,l.pho_r9[diphoton_index.second], weight);
            l.FillHist("maxeta_msig",dipho_category+1,maxeta, weight);
            l.FillHist("ht_msig",dipho_category+1, HT, weight);
        }
    }

    if(PADEBUG) 
        cout<<"myFillHistRed END"<<endl;
    
    return true;
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::GetBranches(TTree *t, std::set<TBranch *>& s ) 
{
    vtxAna_.setBranchAdresses(t,"vtx_std_");
    vtxAna_.getBranches(t,"vtx_std_",s);
}


// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::PreselectPhotons(LoopAll& l, int jentry) 
{
    // Photon preselection
    pho_acc.clear();
    pho_presel.clear();
    pho_presel_lead.clear();
    pho_et.clear();
    l.pho_matchingConv->clear();

    // Nominal smearing
    corrected_pho_energy.clear(); corrected_pho_energy.resize(l.pho_n,0.); 
    int cur_type = l.itype[l.current];

    // EDIT - 4 Dec 2011 NWardle Latest Ntuple Production uses New and Correct Regression so no need to calculate on the FLY corrections
    // TEMPORARY FIX TO CALCULATE CORRECTED ENERGIES SINCE REGRESSION WAS NOT STORED IN NTUPLES 
    // The following Fills the arrays with the ON-THE-FLY calculations
    //GetRegressionCorrections(l);  // need to pass LoopAll
    // -------------------------------------------------------------------------------------------//

    
    ////// std::vector<float> smeared_pho_r9, smeared_pho_weight;
    ////// 
    ////// applySinglePhotonSmearings(smeared_pho_energy, smeared_pho_r9, smeared_pho_weight,
    //////                    cur_type, l, energyCorrected, energyCorrectedError);
    
    for(int ipho=0; ipho<l.pho_n; ++ipho ) { 
        std::vector<std::vector<bool> > p;
        PhotonReducedInfo phoInfo (
                                   *((TVector3*)l.sc_xyz->At(l.pho_scind[ipho])),
                                   ((TLorentzVector*)l.pho_p4->At(ipho))->Energy(),
                                   energyCorrected[ipho],
                                   l.pho_isEB[ipho],
                                   l.pho_r9[ipho],
                                   false,
                                   (energyCorrectedError!=0?energyCorrectedError[ipho]:0)
                                   );
        float pweight = 1.;
        float sweight = 1.;
        float eta = fabs(((TVector3 *)l.sc_xyz->At(l.pho_scind[ipho]))->Eta());
        
        if( doEcorrectionSmear )  { 
            eCorrSmearer->smearPhoton(phoInfo,sweight,l.run,0.); 
        } 
        /// FIXME deterministic smearing on MC photons
        if( cur_type == 0 ) {          // correct energy scale in data
            float ebefore = phoInfo.energy();
            eScaleDataSmearer->smearPhoton(phoInfo,sweight,l.run,0.);
            pweight *= sweight;
        }
        // apply mc-derived photon corrections, to data and MC alike
        corrected_pho_energy[ipho] = phoInfo.energy();
    }

    for(int ipho=0; ipho<l.pho_n; ++ipho) {

        l.pho_genmatched[ipho]=GenMatchedPhoton( l, ipho);

        // match all photons in the original tree with the conversions from the merged collection and save the indices
        int iConv  =l.matchPhotonToConversion(ipho);
        if ( iConv>=0 )
            (*l.pho_matchingConv).push_back(l.matchPhotonToConversion(ipho));
        else
            (*l.pho_matchingConv).push_back(-1);

        TLorentzVector p4 = l.get_pho_p4(ipho,0,&corrected_pho_energy[0]);
        float eta = fabs(((TVector3 *)l.sc_xyz->At(l.pho_scind[ipho]))->Eta());
        // photon et wrt 0,0,0
        float et = p4.Pt();
        pho_et.push_back(et);
    
	if( (eta>1.4442 && eta<1.566) || eta > 2.5 ) { 
            continue;  
        }
        pho_acc.push_back(ipho);
      
        pho_presel.push_back(ipho);
    } 

    std::sort(pho_acc.begin(),pho_acc.end(),
              SimpleSorter<float,std::greater<float> >(&pho_et[0]));
    std::sort(pho_presel.begin(),pho_presel.end(),
              SimpleSorter<float,std::greater<float> >(&pho_et[0]));

    if( pho_presel.size() > 1 ) {
        for(size_t ipho=0; ipho<pho_presel.size()-1; ++ipho ) {
            assert( pho_et[pho_presel[ipho]] >= pho_et[pho_presel[ipho+1]] );
        }
    }
    if( pho_acc.size()>1 ) {
        for(size_t ipho=0; ipho<pho_acc.size()-1; ++ipho ) {
            assert( pho_et[pho_acc[ipho]] >= pho_et[pho_acc[ipho+1]] );
        }
    }
    
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::FillReductionVariables(LoopAll& l, int jentry) 
{
    if(PADEBUG) 
        cout<<"myFillReduceVar START"<<endl;
    
    PreselectPhotons(l,jentry);
    
    /*
    for (int i=0; i<l.pho_n; i++) {
	l.pho_ncrys[i] = 0;
	for (int j=0; j<l.sc_nbc[l.pho_scind[i]]; j++) {
	    l.pho_ncrys[i] += l.bc_nhits[l.sc_bcind[l.pho_scind[i]][j]];
	}
    }
    */
  
    if(PADEBUG) 
        cout<<"myFillReduceVar END"<<endl;

}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::postProcessJets(LoopAll & l, int vtx) 
{
    for(int ijet=0; ijet<l.jet_algoPF1_n; ++ijet) {
	int minv = 0, maxv = l.vtx_std_n;
	if( l.version > 14 && maxv > 10 ) {
	    maxv = 10;
	}
	if( vtx!=-1 ){
	    minv = vtx;
	    maxv = vtx+1;
	}
	if( recorrectJets ) {
	    jetHandler_->recomputeJec(ijet, true);
	}
	for(int ivtx=minv;ivtx<maxv; ++ivtx) {
	    if( recomputeBetas ) {
		jetHandler_->computeBetas(ijet, ivtx);
	    }
	    if( rerunJetMva ) {
		jetHandler_->computeMva(ijet, ivtx);
	    } else if ( recomputeJetWp ) {
		jetHandler_->computeWp(ijet, ivtx);
	    }
	}
    }
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::switchJetIdVertex(LoopAll &l, int ivtx) 
{
    if( l.version > 14 && ivtx > 10 ) {                                                                                                    
	std::cout << "WARNING choosen vertex beyond 10 and jet ID was not computed. Falling back to vertex 0." << std::endl;
	ivtx = 0;
    }               
    
    for(int ii=0; ii<l.jet_algoPF1_n; ++ii) {
	l.jet_algoPF1_beta[ii]              = (*l.jet_algoPF1_beta_ext)[ii][ivtx];
        l.jet_algoPF1_betaStar[ii]          = (*l.jet_algoPF1_betaStar_ext)[ii][ivtx];
        l.jet_algoPF1_betaStarClassic[ii]   = (*l.jet_algoPF1_betaStarClassic_ext)[ii][ivtx];
        l.jet_algoPF1_simple_mva[ii]        = (*l.jet_algoPF1_simple_mva_ext)[ii][ivtx];
        l.jet_algoPF1_full_mva[ii]          = (*l.jet_algoPF1_full_mva_ext)[ii][ivtx];
        l.jet_algoPF1_simple_wp_level[ii]   = (*l.jet_algoPF1_simple_wp_level_ext)[ii][ivtx];
        l.jet_algoPF1_full_wp_level[ii]     = (*l.jet_algoPF1_full_wp_level_ext)[ii][ivtx];
        l.jet_algoPF1_cutbased_wp_level[ii] = (*l.jet_algoPF1_cutbased_wp_level_ext)[ii][ivtx];
    }
}


// ----------------------------------------------------------------------------------------------------
bool PhotonAnalysis::SelectEventsReduction(LoopAll& l, int jentry) 
{

    if(PADEBUG)  cout << " ****************** SelectEventsReduction " << endl;
    // require at least two reconstructed photons to store the event

    if( pho_acc.size() < 2 ) { return false; }
    
    vtxAna_.clear();
    l.vtx_std_ranked_list->clear();
    l.dipho_vtx_std_sel->clear();
    l.vtx_std_ranked_list->clear();
    l.vtx_std_evt_mva->clear();
    l.vtx_std_sel=0;
    float maxSumPt = 0.;
    l.dipho_n = 0;
    bool oneKinSelected = false;
    
    // fill ID variables
    if( forcedRho >= 0. ) {
        l.rho = forcedRho;
    } else if ( l.rho == 0. ) {
        l.rho = l.rho_algo1;
    }
    l.FillCICInputs();
    if(reComputeCiCPF) { l.FillCICPFInputs(); }
    l.FillCIC();

    if(l.itype[l.current]<0) {
        bool foundHiggs=FindHiggsObjects(l);
        if(PADEBUG)  cout << " foundHiggs? "<<foundHiggs<<std::endl;
    } else {
        SetNullHiggs(l);
    }
    /// Jet matching
    // pfJets ak5
    l.doJetMatching(*l.jet_algoPF1_p4,*l.genjet_algo1_p4,l.jet_algoPF1_genMatched,l.jet_algoPF1_vbfMatched,l.jet_algoPF1_genPt,l.jet_algoPF1_genDr);
    // pfJets ak7
    //l.doJetMatching(*l.jet_algoPF2_p4,*l.genjet_algo2_p4,l.jet_algoPF2_genMatched,l.jet_algoPF2_vbfMatched,l.jet_algoPF2_genPt,l.jet_algoPF2_genDr);
    // CHS ak5
    l.doJetMatching(*l.jet_algoPF3_p4,*l.genjet_algo1_p4,l.jet_algoPF3_genMatched,l.jet_algoPF3_vbfMatched,l.jet_algoPF3_genPt,l.jet_algoPF3_genDr);

    postProcessJets(l);

    if( pho_presel.size() < 2 ) {
        // zero or one photons, can't determine a vertex based on photon pairs
        l.vtx_std_ranked_list->push_back( std::vector<int>() );
        for(int ii=0;ii<l.vtx_std_n; ++ii) { l.vtx_std_ranked_list->back().push_back(ii); }
        l.vtx_std_sel = 0;
    } else {
        // fully combinatorial vertex selection
        std::vector<std::pair<int,int> > diphotons;
        for(size_t ip=0; ip<pho_presel.size(); ++ip) {
            for(size_t jp=ip+1; jp<pho_presel.size(); ++jp) {
                diphotons.push_back( std::make_pair( pho_presel[ip], pho_presel[jp] ) );
            }
        }
        l.dipho_n = 0;
        for(size_t id=0; id<diphotons.size(); ++id ) {
            
	    if( l.dipho_n >= MAX_DIPHOTONS-1 ) { continue; }
            int ipho1 = diphotons[id].first;
            int ipho2 = diphotons[id].second;

            if(PADEBUG)        cout << " SelectEventsReduction going to fill photon info " << endl;
            PhotonInfo pho1=l.fillPhotonInfos(ipho1,vtxAlgoParams.useAllConversions,&corrected_pho_energy[0]);
            PhotonInfo pho2=l.fillPhotonInfos(ipho2,vtxAlgoParams.useAllConversions,&corrected_pho_energy[0]);
            if(PADEBUG) cout << " SelectEventsReduction done with fill photon info " << endl;
            
            l.vertexAnalysis(vtxAna_, pho1, pho2 );
            std::vector<int> vtxs = l.vertexSelection(vtxAna_, vtxConv_, pho1, pho2, vtxVarNames, mvaVertexSelection, 
                              tmvaPerVtxReader_, tmvaPerVtxMethod);
        
            TLorentzVector lead_p4 = l.get_pho_p4( ipho2, vtxs[0], &corrected_pho_energy[0] ).Pt();
            TLorentzVector sublead_p4 = l.get_pho_p4( ipho1, vtxs[0], &corrected_pho_energy[0] ).Pt();

            if(sublead_p4.Pt()  > lead_p4.Pt() ) {
                std::swap( diphotons[id].first,  diphotons[id].second );
                std::swap( lead_p4,  sublead_p4 );
            }
        
            if( lead_p4.Pt() < presel_scet1 || sublead_p4.Pt() < presel_scet2 || 
                fabs(lead_p4.Eta()) > presel_maxeta || fabs(sublead_p4.Eta()) > presel_maxeta ) {
                vtxAna_.discardLastDipho();
                continue;
            }
	    oneKinSelected = true;

            if( ! l.PhotonMITPreSelection(ipho1, vtxs[0], &corrected_pho_energy[0] )
                || ! l.PhotonMITPreSelection(ipho2, vtxs[0], &corrected_pho_energy[0] ) ) {
                vtxAna_.discardLastDipho();
                continue;
            }
            
            l.vtx_std_ranked_list->push_back(vtxs);
            if( tmvaPerEvtReader_ ) {
                float vtxEvtMva = vtxAna_.perEventMva( *tmvaPerEvtReader_, tmvaPerEvtMethod, l.vtx_std_ranked_list->back() );
                l.vtx_std_evt_mva->push_back(vtxEvtMva);
            }
            if( l.vtx_std_ranked_list->back().size() != 0 && ! useDefaultVertex ) {  
                l.dipho_vtx_std_sel->push_back( (l.vtx_std_ranked_list)->back()[0] );
            } else {
                l.dipho_vtx_std_sel->push_back(0);
                std::cerr << "NO VERTEX SELECTED " << l.event << " " << l.run << " " << diphotons[id].first << " " << diphotons[id].second << std::endl;
            }

            l.dipho_leadind[l.dipho_n] = diphotons[id].first;
            l.dipho_subleadind[l.dipho_n] = diphotons[id].second;
            l.dipho_vtxind[l.dipho_n] = l.dipho_vtx_std_sel->back();
            
            l.dipho_sumpt[l.dipho_n] = lead_p4.Pt() + sublead_p4.Pt();
            
            if( l.dipho_sumpt[l.dipho_n] > maxSumPt ) {
                l.vtx_std_sel = l.dipho_vtx_std_sel->back();
                maxSumPt = l.dipho_sumpt[l.dipho_n];
            }
	    
            // make sure that vertex analysis indexes are in synch 
            assert( l.dipho_n == vtxAna_.pairID(ipho1,ipho2) );
            
            l.dipho_n++;
        }
       
       MetCorrections2012( l );
    }
    

    return oneKinSelected;
}

// ----------------------------------------------------------------------------------------------------

void PhotonAnalysis::MetCorrections2012(LoopAll& l)
{
    //shift met (reduction step)
    //in mc smearing should be applied first and then shifting
    //both these are performed at analysis step
    TLorentzVector unpfMET;
    unpfMET.SetPxPyPzE (l.met_pfmet*cos(l.met_phi_pfmet),
           l.met_pfmet*sin(l.met_phi_pfmet),
           0,
           sqrt(l.met_pfmet*cos(l.met_phi_pfmet) * l.met_pfmet*cos(l.met_phi_pfmet) 
           + l.met_pfmet*sin(l.met_phi_pfmet) * l.met_pfmet*sin(l.met_phi_pfmet))); 
    
    bool isMC = l.itype[l.current]!=0;
    
    TLorentzVector shiftMET_corr = l.shiftMet(&unpfMET,isMC);
    l.shiftMET_pt = shiftMET_corr.Pt();
    l.shiftMET_phi = shiftMET_corr.Phi();
    l.shiftMET_eta = shiftMET_corr.Eta();
    l.shiftMET_e = shiftMET_corr.Energy();
}


void PhotonAnalysis::MetCorrections2012_Simple(LoopAll& l,TLorentzVector lead_p4 ,TLorentzVector sublead_p4)
{
    // mc: scaling and shifting, data: scaling (analysis step)
    TLorentzVector unpfMET;
    unpfMET.SetPxPyPzE (l.met_pfmet*cos(l.met_phi_pfmet),
           l.met_pfmet*sin(l.met_phi_pfmet),
           0,
           sqrt(l.met_pfmet*cos(l.met_phi_pfmet) * l.met_pfmet*cos(l.met_phi_pfmet) 
           + l.met_pfmet*sin(l.met_phi_pfmet) * l.met_pfmet*sin(l.met_phi_pfmet))); 
     
     bool isMC = l.itype[l.current]!=0;
     
     //take shifted met for data
     TLorentzVector shiftedMET;
     double shiftedMETpt = l.shiftMET_pt;
     double shiftedMETe = l.shiftMET_e;
     double shiftedMETeta = l.shiftMET_eta;
     double shiftedMETphi = l.shiftMET_phi;
     
     shiftedMET.SetPtEtaPhiE(shiftedMETpt,shiftedMETeta,shiftedMETphi,shiftedMETe);
     
     if (isMC) {
       //smear raw met for mc
       TLorentzVector smearMET_corr = l.correctMet_Simple( lead_p4, sublead_p4 , &unpfMET, true, false);
       l.smearMET_pt = smearMET_corr.Pt();
       l.smearMET_phi = smearMET_corr.Phi();
       //shift smeared met for mc
       TLorentzVector shiftsmearMET_corr = l.shiftMet(&smearMET_corr,isMC);
       l.shiftsmearMET_pt = shiftsmearMET_corr.Pt();
       l.shiftsmearMET_phi = shiftsmearMET_corr.Phi();
       l.correctedpfMET = l.shiftsmearMET_pt;
       l.correctedpfMET_phi = l.shiftsmearMET_phi;
     } else {
       //scale shifted met for data
       TLorentzVector shiftscaleMET_corr = l.correctMet_Simple( lead_p4, sublead_p4 , &shiftedMET, false , true);
       l.shiftscaleMET_pt = shiftscaleMET_corr.Pt();
       l.shiftscaleMET_phi = shiftscaleMET_corr.Phi();
       l.correctedpfMET = l.shiftscaleMET_pt;
       l.correctedpfMET_phi = l.shiftscaleMET_phi;
     }
}


bool PhotonAnalysis::SkimEvents(LoopAll& l, int jentry)
{
    if( dataIs2011 ) { l.version=12; }

    l.b_pho_n->GetEntry(jentry);
    if( l.pho_n < 2 ) {
        return false;
    }
    
    if( skimOnDiphoN && l.typerun == l.kFill ) {
	l.b_dipho_n->GetEntry(jentry);
	if( l.dipho_n < 1 ) {
	    return false;
	}
    }

    // do not run trigger selection on MC
    int filetype = l.itype[l.current];
    bool skipTrigger = !doTriggerSelection || filetype != 0 || triggerSelections.empty();

    if( ! skipTrigger ) {
        // get the trigger selection for this run 
        l.b_run->GetEntry(jentry);
        std::vector<TriggerSelection>::iterator isel = find(triggerSelections.begin(), triggerSelections.end(), l.run );
        if(isel == triggerSelections.end() ) {
            std::cerr << "No trigger selection for run " << l.run << "defined" << std::endl;
            return true;
        }
	
	// get the trigger data
	if( l.version < 13 ) { 
	    l.b_hlt1_bit->GetEntry(jentry); 
	    l.b_hlt_path_names_HLT1->GetEntry(jentry);
	    if( !  isel->pass( *(l.hlt_path_names_HLT1), *(l.hlt1_bit) ) ) {
		return false;
	    }
	} else {  
	    l.b_hlt_bit->GetEntry(jentry); 
	    l.b_hlt_path_names_HLT->GetEntry(jentry);
	    if( !  isel->pass( *(l.hlt_path_names_HLT), *(l.hlt_bit) ) ) {
		return false;
	    }
	}
	//l.countersred[trigCounter_]++;
    }

    if( l.typerun == l.kReduce || l.typerun == l.kFillReduce ) {
        //// if( filetype == 2 ) { // photon+jet
        ////    l.b_process_id->GetEntry(jentry);
        ////    if( l.process_id == 18 ) {
        ////        return false;
        ////    }
        //// }
        
        if( filetype != 0 && ! (keepPP && keepPF && keepFF) ) {
            l.b_gp_n->GetEntry(jentry);
            l.b_gp_mother->GetEntry(jentry);
            l.b_gp_status->GetEntry(jentry);
            l.b_gp_pdgid->GetEntry(jentry);
            l.b_gp_p4->GetEntry(jentry);
            
            int np = 0;
            for(int ip=0;ip<l.gp_n;++ip) {
                if( l.gp_status[ip] != 1 || l.gp_pdgid[ip] != 22 ) { 
                    continue; 
                }
                TLorentzVector * p4 = (TLorentzVector*) l.gp_p4->At(ip);
                if( p4->Pt() < 20. || fabs(p4->Eta()) > 3. ) { continue; }
                int mother_id = abs( l.gp_pdgid[ l.gp_mother[ip] ] );
                if( mother_id <= 25 ) { ++np; }
                if( np >= 2 ) { break; }
            }
            if( np >= 2 && ! keepPP ) { return false; }
            if( np == 1 && ! keepPF ) { return false; }
            if( np == 0 && ! keepFF ) { return false; }
        }
    }

    return true;
}

// ----------------------------------------------------------------------------------------------------

bool PhotonAnalysis::SelectEvents(LoopAll& l, int jentry) 
{
    return true;
}

// ----------------------------------------------------------------------------------------------------
void PhotonAnalysis::ReducedOutputTree(LoopAll &l, TTree * outputTree) 
{
    if( outputTree ) { vtxAna_.branches(outputTree,"vtx_std_"); }

    l.pho_matchingConv = new  std::vector<int>();
    if( outputTree ) { l.Branch_pho_matchingConv(outputTree); }
    
    l.vtx_std_evt_mva = new std::vector<float>();
    l.vtx_std_ranked_list = new std::vector<std::vector<int> >();
    l.pho_tkiso_recvtx_030_002_0000_10_01 = new std::vector<std::vector<float> >();
    l.pho_ZeeVal_tkiso_recvtx_030_002_0000_10_01 = new std::vector<std::vector<float> >();
    l.pho_cic6cutlevel_lead = new std::vector<std::vector<Short_t> >();
    l.pho_cic6passcuts_lead = new std::vector<std::vector<std::vector<UInt_t> > >();
    l.pho_cic6cutlevel_sublead = new std::vector<std::vector<Short_t> >();
    l.pho_cic6passcuts_sublead = new std::vector<std::vector<std::vector<UInt_t> > >();
    l.pho_cic4cutlevel_lead = new std::vector<std::vector<Short_t> >();
    l.pho_cic4passcuts_lead = new std::vector<std::vector<std::vector<UInt_t> > >();
    l.pho_cic4cutlevel_sublead = new std::vector<std::vector<Short_t> >();
    l.pho_cic4passcuts_sublead = new std::vector<std::vector<std::vector<UInt_t> > >();
    l.pho_cic4pfcutlevel_lead = new std::vector<std::vector<Short_t> >();
    l.pho_cic4pfpasscuts_lead = new std::vector<std::vector<std::vector<UInt_t> > >();
    l.pho_cic4pfcutlevel_sublead = new std::vector<std::vector<Short_t> >();
    l.pho_cic4pfpasscuts_sublead = new std::vector<std::vector<std::vector<UInt_t> > >();
    l.dipho_vtx_std_sel =  new std::vector<int>();

    if( outputTree ) {
	//l.Branch_pho_ncrys(outputTree);

	l.Branch_vtx_std_evt_mva(outputTree);
	l.Branch_vtx_std_ranked_list(outputTree);
	l.Branch_vtx_std_sel(outputTree);
	l.Branch_pho_tkiso_recvtx_030_002_0000_10_01(outputTree);
	l.Branch_pho_tkiso_badvtx_040_002_0000_10_01(outputTree);
	l.Branch_pho_tkiso_badvtx_id(outputTree);
	l.Branch_pho_pfiso_charged_badvtx_04(outputTree);
	l.Branch_pho_pfiso_charged_badvtx_id(outputTree);
	l.Branch_pho_ZeeVal_tkiso_recvtx_030_002_0000_10_01(outputTree);
	l.Branch_pho_ZeeVal_tkiso_badvtx_040_002_0000_10_01(outputTree);
	l.Branch_pho_ZeeVal_tkiso_badvtx_id(outputTree);
	l.Branch_pho_mitmva(outputTree);
	l.Branch_pho_drtotk_25_99(outputTree);
	
	l.Branch_dipho_n(outputTree);
	l.Branch_dipho_leadind(outputTree);
	l.Branch_dipho_subleadind(outputTree);
	l.Branch_dipho_vtxind(outputTree);
	l.Branch_dipho_sumpt(outputTree);
	
	l.Branch_pho_cic6cutlevel_lead( outputTree );
	l.Branch_pho_cic6passcuts_lead( outputTree );
	l.Branch_pho_cic6cutlevel_sublead( outputTree );
	l.Branch_pho_cic6passcuts_sublead( outputTree );
	l.Branch_pho_cic4cutlevel_lead( outputTree );
	l.Branch_pho_cic4passcuts_lead( outputTree );
	l.Branch_pho_cic4cutlevel_sublead( outputTree );
	l.Branch_pho_cic4passcuts_sublead( outputTree );
	l.Branch_pho_cic4pfcutlevel_lead( outputTree );
	l.Branch_pho_cic4pfpasscuts_lead( outputTree );
	l.Branch_pho_cic4pfcutlevel_sublead( outputTree );
	l.Branch_pho_cic4pfpasscuts_sublead( outputTree );
	
	l.Branch_pho_genmatched(outputTree);
	l.Branch_pho_regr_energy_otf(outputTree);
	l.Branch_pho_regr_energyerr_otf(outputTree);
	
	l.Branch_jet_algoPF1_genMatched(outputTree);
	l.Branch_jet_algoPF1_vbfMatched(outputTree);
	l.Branch_jet_algoPF1_genPt(outputTree);
	l.Branch_jet_algoPF1_genDr(outputTree);
	
	//l.Branch_jet_algoPF2_genMatched(outputTree);
	//l.Branch_jet_algoPF2_vbfMatched(outputTree);
	//l.Branch_jet_algoPF2_genPt(outputTree);
	//l.Branch_jet_algoPF2_genDr(outputTree);
	
	l.Branch_jet_algoPF3_genMatched(outputTree);
	l.Branch_jet_algoPF3_vbfMatched(outputTree);
	l.Branch_jet_algoPF3_genPt(outputTree);
	l.Branch_jet_algoPF3_genDr(outputTree);
	
	//correctMETinRED
	l.Branch_shiftMET_pt(outputTree);
	l.Branch_shiftMET_phi(outputTree);
	l.Branch_smearMET_pt(outputTree);
	l.Branch_smearMET_phi(outputTree);
	l.Branch_shiftsmearMET_pt(outputTree);
	l.Branch_shiftsmearMET_phi(outputTree);
	l.Branch_shiftscaleMET_pt(outputTree);
	l.Branch_shiftscaleMET_phi(outputTree);
	l.Branch_shiftMET_eta(outputTree);
	l.Branch_shiftMET_e(outputTree);
	l.Branch_shiftscaleMET_eta(outputTree);
	l.Branch_shiftscaleMET_e(outputTree);
    }
    
    l.gh_higgs_p4 = new TClonesArray("TLorentzVector", 1); 
    l.gh_higgs_p4->Clear();
    ((*l.gh_higgs_p4)[0]) = new TLorentzVector();

    l.gh_pho1_p4 = new TClonesArray("TLorentzVector", 1); 
    l.gh_pho1_p4->Clear();
    ((*l.gh_pho1_p4)[0]) = new TLorentzVector();

    l.gh_pho2_p4 = new TClonesArray("TLorentzVector", 1); 
    l.gh_pho2_p4->Clear();
    ((*l.gh_pho2_p4)[0]) = new TLorentzVector();

    l.gh_vbfq1_p4 = new TClonesArray("TLorentzVector", 1); 
    l.gh_vbfq1_p4->Clear();
    ((*l.gh_vbfq1_p4)[0]) = new TLorentzVector();

    l.gh_vbfq2_p4 = new TClonesArray("TLorentzVector", 1); 
    l.gh_vbfq2_p4->Clear();
    ((*l.gh_vbfq2_p4)[0]) = new TLorentzVector();

    l.gh_vh1_p4 = new TClonesArray("TLorentzVector", 1); 
    l.gh_vh1_p4->Clear();
    ((*l.gh_vh1_p4)[0]) = new TLorentzVector();
    
    l.gh_vh2_p4 = new TClonesArray("TLorentzVector", 1); 
    l.gh_vh2_p4->Clear();
    ((*l.gh_vh2_p4)[0]) = new TLorentzVector();
    
//    l.METcorrected = new TClonesArray("TLorentzVector", 1);     //met at analysis step
//    l.METcorrected->Clear();                    //met at analysis step
//    ((*l.METcorrected)[0]) = new TLorentzVector();        //met at analysis step
    
    
    if( outputTree ) {
    l.Branch_gh_gen2reco1( outputTree );
    l.Branch_gh_gen2reco2( outputTree );
    l.Branch_gh_vbfq1_pdgid( outputTree );
    l.Branch_gh_vbfq2_pdgid( outputTree );
    l.Branch_gh_vh_pdgid( outputTree );
    l.Branch_gh_vh1_pdgid( outputTree );
    l.Branch_gh_vh2_pdgid( outputTree );
//    l.Branch_METcorrected( outputTree );  //met at analysis step
    l.Branch_gh_higgs_p4( outputTree );
    l.Branch_gh_pho1_p4( outputTree );
    l.Branch_gh_pho2_p4( outputTree );
    l.Branch_gh_vbfq1_p4( outputTree );
    l.Branch_gh_vbfq2_p4( outputTree );
    l.Branch_gh_vh1_p4( outputTree );
    l.Branch_gh_vh2_p4( outputTree );
    }
}


// ---------------------------------------------------------------------------------------------------------------------------------------------
int PhotonAnalysis::DiphotonMVASelection(LoopAll &l, HggVertexAnalyzer & vtxAna, Float_t & diphoMVA,  
                                         Float_t minTightMVA, Float_t minLooseMVA, Float_t leadPtMin, 
                                         Float_t subleadPtMin, std::string type, int ncategories,
                                         bool applyPtoverM, float *pho_energy_array, bool split) {

    //rho=0;// CAUTION SETTING RHO TO 0 FOR 2010 DATA FILES (RHO ISN'T IN THESE FILES)
    int selected_lead_index = -1;
    int selected_sublead_index = -1;
    float selected_lead_pt = -1;
    float selected_sublead_pt = -1;
  
    std::vector<int> passingLoose_dipho;
    std::vector<int> passingTight_dipho;
    std::vector<float> passingLoose_sumpt;
    std::vector<float> passingTight_sumpt;
    std::map<int,float> passing_diphomva;
    //std::vector<float> passing_leadphomva;
    //std::vector<float> passing_subphomva;
    if(PADEBUG)  std::cout <<"type "<< type<<std::endl;
    for(int idipho = 0; idipho < l.dipho_n; ++idipho ) {
        int ivtx = l.dipho_vtxind[idipho];
        int lead = l.dipho_leadind[idipho];
        int sublead = l.dipho_subleadind[idipho];
      
        if( lead == sublead ) { continue; }

        TLorentzVector lead_p4 = l.get_pho_p4(lead,ivtx,pho_energy_array); 
        TLorentzVector sublead_p4 = l.get_pho_p4(sublead,ivtx,pho_energy_array); 
      
        float leadEta = fabs(((TVector3 *)l.sc_xyz->At(l.pho_scind[lead]))->Eta());
        float subleadEta = fabs(((TVector3 *)l.sc_xyz->At(l.pho_scind[sublead]))->Eta());
        float m_gamgam = (lead_p4+sublead_p4).M();
        float pt_gamgam = (lead_p4+sublead_p4).Pt();

        // FIXME call to (*vtx_std_evt_mva)[ivtx] crashes program
        //std::cout<<"(*vtx_std_evt_mva)["<<ivtx<<"] "<< (*vtx_std_evt_mva)[ivtx]<<std::endl; 
        float vtxProb = vtxAna.vertexProbability((*l.vtx_std_evt_mva)[idipho], l.vtx_std_n);
        //float vtxProb = vtxAna.vertexProbability(0.0);
        //float vtxProb = 0.3;

        if( leadEta > 2.5 || subleadEta > 2.5 || 
            ( leadEta > 1.4442 && leadEta < 1.566 ) ||
            ( subleadEta > 1.4442 && subleadEta < 1.566 ) ) { continue; }
     
        // VBF cuts smoothly on lead pt/M but on straight pt on sublead to save sig eff and avoid HLT turn-on
        if(split){
            if ( lead_p4.Pt()/m_gamgam < leadPtMin/120. || sublead_p4.Pt()< subleadPtMin ) { continue; }
        }else{
            if( applyPtoverM ) {
                if ( lead_p4.Pt()/m_gamgam < leadPtMin/120. || sublead_p4.Pt()/m_gamgam < subleadPtMin/120. ) { continue; }
            } else {
                if ( lead_p4.Pt() < leadPtMin || sublead_p4.Pt() < subleadPtMin ) { continue; }
            }
        }

        if(PADEBUG)  std::cout << "getting photon ID MVA" << std::endl;
        std::vector<std::vector<bool> > ph_passcut;
        float leadmva = l.photonIDMVA(lead, ivtx, lead_p4, type.c_str()); 
        float submva = l.photonIDMVA(sublead, ivtx, sublead_p4, type.c_str());
        if(PADEBUG)  std::cout << "lead  leadmva  sublead  submva  "<<lead<<"  "<<leadmva<<"  "<<sublead<<"  "<<submva << std::endl;
        if(PADEBUG)  std::cout << "lead_p4.Pt()  leadEta  sublead_p4.Pt()  subleadEta  "<<lead_p4.Pt()<<"  "<<leadEta<<"  "<<sublead_p4.Pt()<<"  "<<subleadEta << std::endl;
        if(PADEBUG)  std::cout << "mass "<<m_gamgam<<std::endl;
        if(leadmva>minLooseMVA && submva>minLooseMVA) {
            passingLoose_dipho.push_back(idipho);
            passingLoose_sumpt.push_back(lead_p4.Et()+sublead_p4.Et());
            if(leadmva>minTightMVA && submva>minTightMVA) {
                passingTight_dipho.push_back(idipho);
                passingTight_sumpt.push_back(lead_p4.Et()+sublead_p4.Et());
            }
        } else { continue; } 

        if(PADEBUG)  std::cout << "got photon ID MVA" << std::endl;
     
        if(PADEBUG)  std::cout << "getting di-photon MVA" << std::endl;

        massResolutionCalculator->Setup(l,&photonInfoCollection[lead],&photonInfoCollection[sublead],idipho,eSmearPars,nR9Categories,nEtaCategories,beamspotSigma);
        //massResolutionCalculator->Setup(l,&lead_p4,&sublead_p4,lead,sublead,idipho,pt_gamgam, m_gamgam,eSmearPars,nR9Categories,nEtaCategories);

        float sigmaMrv = massResolutionCalculator->massResolutionCorrVtx();
        float sigmaMwv = massResolutionCalculator->massResolutionWrongVtx();
        float sigmaMeonly = massResolutionCalculator->massResolutionEonly();
    
        // FIXME  These things will move to MassResolution eventually...
        if(type=="UCSD") {
            float leta=fabs( ((TLorentzVector*)l.pho_p4->At(lead))->Eta() );
            float seta=fabs( ((TLorentzVector*)l.pho_p4->At(sublead))->Eta() );

            //MARCO FIXED
            float leadErr = GetSmearSigma(leta,l.pho_r9[lead]);
            float subleadErr = GetSmearSigma(seta,l.pho_r9[sublead]);

            Float_t t_dmodz = l.getDmOverDz(lead, sublead, pho_energy_array);
            float z_gg = ((TVector3*)(l.vtx_std_xyz->At(ivtx)))->Z();
            // will be updated to this
            //sigmaMwv = fabs(t_dmodz)*(sqrt(pow(double(l.bs_sigmaZ), 2) + pow(double(z_gg), 2)))/m_gamgam;
            sigmaMwv = (t_dmodz)*(sqrt(pow(double(l.bs_sigmaZ), 2) + pow(double(z_gg), 2)));
      
            sigmaMeonly = 0.5*sqrt( pow(l.pho_regr_energyerr[lead]/l.pho_regr_energy[lead],2)+pow(leadErr,2)+pow(l.pho_regr_energyerr[sublead]/l.pho_regr_energy[sublead],2)+pow(subleadErr,2) )*m_gamgam;
        }

        passing_diphomva[idipho]=
            (l.diphotonMVA(lead, sublead, ivtx, 
                           vtxProb, lead_p4, sublead_p4, 
                           sigmaMrv, sigmaMwv, sigmaMeonly, type.c_str()));
        if(PADEBUG)  std::cout << "got di-photon MVA" << std::endl;
        //passing_leadphomva.push_back(leadmva);
        //passing_subphomva.push_back(submva);
    }
  
    if( passingLoose_dipho.empty() ) { return -1; }

    if( passingTight_dipho.empty() ) {
        std::sort(passingLoose_dipho.begin(),passingLoose_dipho.end(),
                  SimpleSorter<float,std::greater<double> >(&passingLoose_sumpt[0]));
        // set reference to diphoton MVA
        diphoMVA = passing_diphomva[passingLoose_dipho[0]];

        if(PADEBUG)  std::cout<<"exiting DiphotonMVAsel"<<std::endl;
        return passingLoose_dipho[0];
    } else {
        std::sort(passingTight_dipho.begin(),passingTight_dipho.end(),
                  SimpleSorter<float,std::greater<double> >(&passingTight_sumpt[0]));
        // set reference to diphoton MVA
        diphoMVA = passing_diphomva[passingTight_dipho[0]];

        if(PADEBUG)  std::cout<<"exiting DiphotonMVAsel"<<std::endl;
        return passingTight_dipho[0];
    }

}

// ---------------------------------------------------------------------------------------------------------------------------------------------
int PhotonAnalysis::DiphotonMVAEventClass(LoopAll &l, float diphoMVA, int nCat, std::string type, int EBEB){
    if(PADEBUG)  std::cout<<"DiphotonMVAEventClass 1"<<std::endl;
  
    int eventClass = -1;

    float class5threshMIT[4]  = { -0.05,  0.5, 0.71, 0.88 };
    float class6threshUCSD[6] = { -0.4, -0.0356,  0.3889, 0.592, 0.6669,  0.7583 };
    // first 2 for (ebee+eeee) and last 6 for ebeb
    float class8threshUCSD[8]={-0.7, -0.11, -0.4, -0.0356,  0.3889, 0.592, 0.6669,  0.7583 };
    float class2threshUCSD[2]={-0.2, 0.3 };

    if(PADEBUG)  std::cout<<"DiphotonMVAEventClass diphoMVA:  "<<diphoMVA<<std::endl;
    if(PADEBUG)  std::cout<<"DiphotonMVAEventClass nCat:  "<<nCat<<std::endl;
    if(PADEBUG)  std::cout<<"DiphotonMVAEventClass type:  "<<type<<std::endl;
    if(PADEBUG)  std::cout<<"DiphotonMVAEventClass EBEB:  "<<EBEB<<std::endl;
  
    if(nCat==4){
        if(PADEBUG)  std::cout<<"DiphotonMVAEventClass 3"<<std::endl;
        for(int ithresh=0; ithresh<nCat; ithresh++){
            if(PADEBUG)  std::cout <<"eventClass "<<eventClass
                                   <<"\tclass5threshMIT["<<ithresh<<"] "<<class5threshMIT[ithresh]<<std::endl;
            if(class5threshMIT[ithresh]<diphoMVA && type=="MIT") eventClass++;
        }
    } else if(nCat==6){
        for(int ithresh=0; ithresh<nCat; ithresh++){
            if(class6threshUCSD[ithresh]<diphoMVA && type=="UCSD") eventClass++;
        }
    } else if(nCat==8){
        if(type=="UCSD") {
            for(int ithresh=0; ithresh<nCat; ithresh++){
                if(class8threshUCSD[ithresh]<diphoMVA && EBEB==1 && ithresh>1) {
                    if(PADEBUG)  std::cout <<"passing EBEB eventClass "<<eventClass<<std::endl;
                    eventClass++;
                }
        
                if(class8threshUCSD[ithresh]<diphoMVA && EBEB!=1 && ithresh<2) {
                    if(PADEBUG)  std::cout <<"passing !EBEB eventClass "<<eventClass<<std::endl;
                    eventClass++;
                }
            }
            if(eventClass!=-1 && EBEB==1) eventClass=eventClass+2;
        }
    } else if(nCat==2) {
        for(int ithresh=0; ithresh<nCat; ithresh++){
            if(class2threshUCSD[ithresh]<diphoMVA && type=="UCSD") eventClass++;
        }
    }

 

    if(PADEBUG)  std::cout<<"eventClass "<<eventClass<<std::endl;
    return eventClass;
}

void PhotonAnalysis::ResetAnalysis(){
}

float PhotonAnalysis::GetSmearSigma(float eta, float r9, int epoch){
    // not using epoch for now
    // r9cat + nr9Cat*etaCat
    float smear_nov14[] = {0.99e-2, 1.00e-2, 1.57e-2, 2.17e-2, 3.30e-2, 3.26e-2, 3.78e-2, 3.31e-2}; 
    float sigma = -1;
    if(epoch==0) {
        int nr9Cat=2;
        int r9Cat=(int)(r9<0.94);
        int nEtaCat=4;
        int etaCat=(int)(eta>1.0) + (int)(eta>1.5) + (int)(eta>2.0);
        sigma=smear_nov14[(int)(r9Cat + nr9Cat*etaCat)];
    }

    return sigma;
}
    
    
void PhotonAnalysis::SetNullHiggs(LoopAll& l){
  
    ((TLorentzVector *)l.gh_higgs_p4->At(0))->SetXYZT(0,0,0,0);
  
    ((TLorentzVector *)l.gh_pho1_p4->At(0))->SetXYZT(0,0,0,0);
    ((TLorentzVector *)l.gh_pho2_p4->At(0))->SetXYZT(0,0,0,0);

    ((TLorentzVector *)l.gh_vbfq1_p4->At(0))->SetXYZT(0,0,0,0);
    ((TLorentzVector *)l.gh_vbfq2_p4->At(0))->SetXYZT(0,0,0,0);
    l.gh_vbfq1_pdgid=0;
    l.gh_vbfq2_pdgid=0;
  
    ((TLorentzVector *)l.gh_vh1_p4->At(0))->SetXYZT(0,0,0,0);
    ((TLorentzVector *)l.gh_vh2_p4->At(0))->SetXYZT(0,0,0,0);
    l.gh_vh_pdgid=0;
    l.gh_vh1_pdgid=0;
    l.gh_vh2_pdgid=0;

}


bool PhotonAnalysis::FindHiggsObjects(LoopAll& l){
  
    int higgsind=-1;
    int mc1=-1;
    int mc2=-1;
    int i1=-1;
    int i2=-1;

    l.FindMCHiggsPhotons( higgsind,  mc1,  mc2,  i1,  i2 );

  
    if(higgsind!=-1) {
        TLorentzVector * TheHiggs = (TLorentzVector *) l.gp_p4->At(higgsind);
        ((TLorentzVector *)l.gh_higgs_p4->At(0))->SetXYZT(TheHiggs->Px(),TheHiggs->Py(),TheHiggs->Pz(),TheHiggs->E()); 
    } else { ((TLorentzVector *)l.gh_higgs_p4->At(0))->SetXYZT(0,0,0,0); }
  
    l.gh_gen2reco1=i1;
    l.gh_gen2reco2=i2;
  
  
    if(mc1!=-1) {
        TLorentzVector * mcpho1 = (TLorentzVector *) l.gp_p4->At(mc1);
        ((TLorentzVector *)l.gh_pho1_p4->At(0))->SetXYZT(mcpho1->Px(),mcpho1->Py(),mcpho1->Pz(),mcpho1->E()); 
    } else { ((TLorentzVector *)l.gh_pho1_p4->At(0))->SetXYZT(0,0,0,0); }

    if(mc2!=-1) {
        TLorentzVector * mcpho2 = (TLorentzVector *) l.gp_p4->At(mc2);
        ((TLorentzVector *)l.gh_pho2_p4->At(0))->SetXYZT(mcpho2->Px(),mcpho2->Py(),mcpho2->Pz(),mcpho2->E()); 
    } else { ((TLorentzVector *)l.gh_pho2_p4->At(0))->SetXYZT(0,0,0,0); }


    int vbfq1=-100;
    int vbfq2=-100;

    int vh=-100;
    int vh1=-100;
    int vh2=-100;

    if(higgsind!=-1){
        l.FindMCVBF(higgsind,vbfq1,vbfq2);
        l.FindMCVH(higgsind,vh,vh1,vh2);

        //std::cout<<"higgsind vbfq1 vbfq2 vh gp_pdgid[vh] vh1 vh2  "<<higgsind<<"  "<<vbfq1<<"  "<<vbfq2<<"  "<<vh<<"  "<<l.gp_pdgid[vh]<<"  "<<vh1<<"  "<<vh2<<std::endl;
    }
  
  
    if(vh==-100) l.gh_vh_pdgid=-10000;
    else l.gh_vh_pdgid=l.gp_pdgid[vh];

    if(vh1==-100) l.gh_vh1_pdgid=-10000;
    else l.gh_vh1_pdgid=l.gp_pdgid[vh1];

    if(vh2==-100) l.gh_vh2_pdgid=-10000;
    else l.gh_vh2_pdgid=l.gp_pdgid[vh2];
  
    if(vh1!=-100) {
        TLorentzVector * mcvh1 = (TLorentzVector *) l.gp_p4->At(vh1);
        ((TLorentzVector *)l.gh_vh1_p4->At(0))->SetXYZT(mcvh1->Px(),mcvh1->Py(),mcvh1->Pz(),mcvh1->E()); 
    } else { ((TLorentzVector *)l.gh_vh1_p4->At(0))->SetXYZT(0,0,0,0); }
    
    if(vh2!=-100) {
        TLorentzVector * mcvh2 = (TLorentzVector *) l.gp_p4->At(vh2);
        ((TLorentzVector *)l.gh_vh2_p4->At(0))->SetXYZT(mcvh2->Px(),mcvh2->Py(),mcvh2->Pz(),mcvh2->E()); 
    } else { ((TLorentzVector *)l.gh_vh2_p4->At(0))->SetXYZT(0,0,0,0); }


  
    if(vbfq1==-100) l.gh_vbfq1_pdgid=0;
    else l.gh_vbfq1_pdgid=l.gp_pdgid[vbfq1];

    if(vbfq2==-100) l.gh_vbfq2_pdgid=0;
    else l.gh_vbfq2_pdgid=l.gp_pdgid[vbfq2];

    if(vbfq1!=-100) {
        TLorentzVector * mcvbfq1 = (TLorentzVector *) l.gp_p4->At(vbfq1);
        ((TLorentzVector *)l.gh_vbfq1_p4->At(0))->SetXYZT(mcvbfq1->Px(),mcvbfq1->Py(),mcvbfq1->Pz(),mcvbfq1->E()); 
    } else { ((TLorentzVector *)l.gh_vbfq1_p4->At(0))->SetXYZT(0,0,0,0); }
    
    if(vbfq2!=-100) {
        TLorentzVector * mcvbfq2 = (TLorentzVector *) l.gp_p4->At(vbfq2);
        ((TLorentzVector *)l.gh_vbfq2_p4->At(0))->SetXYZT(mcvbfq2->Px(),mcvbfq2->Py(),mcvbfq2->Pz(),mcvbfq2->E()); 
    } else { ((TLorentzVector *)l.gh_vbfq2_p4->At(0))->SetXYZT(0,0,0,0); }

    return (higgsind != -1);

}



Bool_t PhotonAnalysis::GenMatchedPhoton(LoopAll& l, int ipho){
    Bool_t is_prompt = false;
    TLorentzVector* phop4 = (TLorentzVector*) l.pho_p4->At(ipho);

    for(int ip=0;ip<l.gp_n;++ip) {
        if( l.gp_status[ip] != 1 || l.gp_pdgid[ip] != 22 ) {
            continue;
        }
        TLorentzVector * p4 = (TLorentzVector*) l.gp_p4->At(ip);
        if( p4->Pt() < 20. || fabs(p4->Eta()) > 3. ) { continue; }
        int mother_id = abs( l.gp_pdgid[ l.gp_mother[ip] ] );
        if( mother_id <= 25 ) {
            float dr = phop4->DeltaR(*p4);
            if (dr<0.3 && fabs((p4->Pt()-phop4->Pt())/p4->Pt()) < 0.5) {
                is_prompt = true;
                break;
            }
        }
    }
    return is_prompt;

}


bool PhotonAnalysis::ClassicCatsNm1Plots(LoopAll& l, int diphoton_nm1_id, float* smeared_pho_energy, float eventweight, float myweight){
    
    bool pass = false;
    if(diphoton_nm1_id==-1) return pass;

    TLorentzVector lead_p4 = l.get_pho_p4( l.dipho_leadind[diphoton_nm1_id], l.dipho_vtxind[diphoton_nm1_id], &smeared_pho_energy[0]);
    TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphoton_nm1_id], l.dipho_vtxind[diphoton_nm1_id], &smeared_pho_energy[0]);
    TLorentzVector diphoton = lead_p4+sublead_p4;
    
    int photon_category   = l.PhotonCategory(l.dipho_subleadind[diphoton_nm1_id],2,2);
    sublead_r9            = l.pho_r9[l.dipho_subleadind[diphoton_nm1_id]];
    sublead_trkisooet     = (*l.pho_tkiso_recvtx_030_002_0000_10_01)[l.dipho_subleadind[diphoton_nm1_id]][l.dipho_vtxind[diphoton_nm1_id]]*50/sublead_p4.Et();
    sublead_isoOverEt     = (l.pho_hcalsumetconedr04[l.dipho_subleadind[diphoton_nm1_id]]
                             +  l.pho_ecalsumetconedr03[l.dipho_subleadind[diphoton_nm1_id]]
                             +  (*l.pho_tkiso_recvtx_030_002_0000_10_01)[l.dipho_subleadind[diphoton_nm1_id]][l.dipho_vtxind[diphoton_nm1_id]]
                             - 0.17*l.rho)*50/sublead_p4.Et();
    sublead_badisoOverEt  = (l.pho_hcalsumetconedr04[l.dipho_subleadind[diphoton_nm1_id]]
                             +  l.pho_ecalsumetconedr04[l.dipho_subleadind[diphoton_nm1_id]]
                             +  l.pho_tkiso_badvtx_040_002_0000_10_01[l.dipho_subleadind[diphoton_nm1_id]]
                             - 0.52*l.rho)*50/sublead_p4.Et();
    
    sublead_sieie         = l.pho_sieie[l.dipho_subleadind[diphoton_nm1_id]];
    sublead_drtotk        = l.pho_drtotk_25_99[l.dipho_subleadind[diphoton_nm1_id]];
    sublead_hovere        = l.pho_hoe[l.dipho_subleadind[diphoton_nm1_id]];
    sublead_mgg           = diphoton.M();
    
    int applyCutsType = 15 + photon_category;
    
    pass = l.ApplyCutsFill(0,applyCutsType, eventweight, myweight);

    return pass;
}


bool PhotonAnalysis::ElectronTag2011(LoopAll& l, int diphotonVHlep_id, float* smeared_pho_energy, bool nm1, float eventweight, float myweight){
    bool tag = false;

    if(diphotonVHlep_id==-1) return tag;
    TLorentzVector lead_p4 = l.get_pho_p4( l.dipho_leadind[diphotonVHlep_id], l.dipho_vtxind[diphotonVHlep_id], &smeared_pho_energy[0]);
    TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphotonVHlep_id], l.dipho_vtxind[diphotonVHlep_id], &smeared_pho_energy[0]);
    
    int elInd = l.ElectronSelection(lead_p4, sublead_p4, l.dipho_vtxind[diphotonVHlep_id]);
    if(elInd!=-1) tag = true;
    
    return tag;
}
	        
bool PhotonAnalysis::ElectronTag2012(LoopAll& l, int diphotonVHlep_id, float* smeared_pho_energy, ofstream& lep_sync, bool nm1, float eventweight, float myweight){
    bool tag = false;

    if(diphotonVHlep_id==-1) return tag;
    TLorentzVector lead_p4 = l.get_pho_p4( l.dipho_leadind[diphotonVHlep_id], l.dipho_vtxind[diphotonVHlep_id], &smeared_pho_energy[0]);
    TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphotonVHlep_id], l.dipho_vtxind[diphotonVHlep_id], &smeared_pho_energy[0]);
    
    int elInd = l.ElectronSelection2012(lead_p4, sublead_p4, l.dipho_vtxind[diphotonVHlep_id]);
    if(elInd!=-1) tag = true;

        TLorentzVector dipho_p4 = lead_p4+sublead_p4;
        lep_sync<<"run="<<l.run<<"\t";
        lep_sync<<"lumis"<<l.lumis<<"\t";
        lep_sync<<"event="<<(unsigned int) l.event<<"\t";
        lep_sync<<"pt1="<<lead_p4.Pt()<<"\t";
        lep_sync<<"eta1="<<lead_p4.Eta()<<"\t";
        lep_sync<<"pt2="<<sublead_p4.Pt()<<"\t";
        lep_sync<<"eta2="<<sublead_p4.Eta()<<"\t";
        lep_sync<<"ptgg="<<dipho_p4.Pt()<<"\t";
    if(tag){
        TLorentzVector* el_p4 = (TLorentzVector*) l.el_std_p4->At(elInd);
        lep_sync<<"elpt="<<el_p4->Pt()<<"\t";
        lep_sync<<"eleta="<<el_p4->Eta()<<"  ELTAG\n";
    } else {
        lep_sync<<"ELNOTAG\n";
    }

    return tag;
}
	        
            

bool PhotonAnalysis::ElectronTag2012B(LoopAll& l, int& diphotonVHlep_id, int& elVtx, int& el_cat, float* smeared_pho_energy, ofstream& lep_sync, bool mvaselection, float phoidMvaCut){
    bool tag = false;
    float elptcut=20;

    int el_ind=l.ElectronSelectionMVA2012(elptcut);
    if(el_ind!=-1) {
        TLorentzVector* myel = (TLorentzVector*) l.el_std_p4->At(el_ind);
        TLorentzVector* myelsc = (TLorentzVector*) l.el_std_sc->At(el_ind);
        elVtx=l.FindElectronVertex(el_ind);
   
        float drtoveto = 0.5;
        std::vector<bool> veto_indices;
        veto_indices.clear();
        l.PhotonsToVeto(myelsc, drtoveto, veto_indices);

        if(mvaselection) {
            diphotonVHlep_id = l.DiphotonMITPreSelection(leadEtVHlepCut,subleadEtVHlepCut,phoidMvaCut,
                applyPtoverM, &smeared_pho_energy[0], elVtx, false, false, veto_indices);
        } else {
            diphotonVHlep_id = l.DiphotonCiCSelection( l.phoSUPERTIGHT, l.phoSUPERTIGHT, leadEtVHlepCut,subleadEtVHlepCut, 4, 
                applyPtoverM, &smeared_pho_energy[0], true, elVtx, veto_indices);
        }
       
        if(diphotonVHlep_id!=-1){
            TLorentzVector lead_p4 = l.get_pho_p4( l.dipho_leadind[diphotonVHlep_id], elVtx, &smeared_pho_energy[0]);
            TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphotonVHlep_id], elVtx, &smeared_pho_energy[0]);

            // need to check again for d0 and dZ (couldn't before because we didn't have the vertex)
            if(l.ElectronMVACuts(el_ind, elVtx)){
                if(l.ElectronPhotonCuts2012B(lead_p4, sublead_p4, *myel)){
                    tag=true;
                    el_cat=(int)(abs(lead_p4.Eta())<1.5 && abs(sublead_p4.Eta())<1.5); 
                } else {
                    diphotonVHlep_id=-1;
                }
            }

            if(tag){
                double Aeff=0.;
                float thiseta = fabs(myelsc->Eta());
                if(thiseta<1.0)                   Aeff=0.135;
                if(thiseta>=1.0 && thiseta<1.479) Aeff=0.168;
                if(thiseta>=1.479 && thiseta<2.0) Aeff=0.068;
                if(thiseta>=2.0 && thiseta<2.2)   Aeff=0.116;
                if(thiseta>=2.2 && thiseta<2.3)   Aeff=0.162;
                if(thiseta>=2.3 && thiseta<2.4)   Aeff=0.241;
                if(thiseta>=2.4)                  Aeff=0.23;
                float thisiso=l.el_std_pfiso_charged[el_ind]+std::max(l.el_std_pfiso_neutral[el_ind]+l.el_std_pfiso_photon[el_ind]-l.rho*Aeff,0.);
                TLorentzVector elead_p4 = *myel + lead_p4;
                TLorentzVector esub_p4 = *myel + sublead_p4;
                
                l.FillHist("ElectronTag_elpt",          el_cat, myel->Pt());
                l.FillHist("ElectronTag_eleta",         el_cat, myelsc->Eta());
                l.FillHist("ElectronTag_elPhi",         el_cat, myelsc->Phi());
                l.FillHist("ElectronTag_elmva",         el_cat, l.el_std_mva_nontrig[el_ind]);
                l.FillHist("ElectronTag_elisoopt",      el_cat, thisiso/myel->Pt());
                l.FillHist("ElectronTag_missinghits",   el_cat, l.el_std_hp_expin[el_ind]);
                l.FillHist("ElectronTag_passconvveto",  el_cat, l.el_std_conv[el_ind]);
                l.FillHist("ElectronTag_drellead",      el_cat, myel->DeltaR(lead_p4));
                l.FillHist("ElectronTag_drelsub",       el_cat, myel->DeltaR(sublead_p4));
                l.FillHist("ElectronTag_d0",            el_cat, l.el_std_D0Vtx[el_ind][elVtx]);
                l.FillHist("ElectronTag_dZ",            el_cat, l.el_std_DZVtx[el_ind][elVtx]);
                l.FillHist("ElectronTag_Melsub",        el_cat, esub_p4.M());
                l.FillHist("ElectronTag_Mellead",       el_cat, elead_p4.M());
                l.FillHist("ElectronTag_dMelsub",       el_cat, abs(esub_p4.M()-91.2));
                l.FillHist("ElectronTag_dMellead",      el_cat, abs(elead_p4.M()-91.2));
                
                // vertex plots
                TVector3* vtx = (TVector3*) l.vtx_std_xyz->At(elVtx);
                int cur_type = l.itype[l.current];
                if(cur_type!=0){
                    l.FillHist("ElectronTag_dZtogen",        el_cat, (float)((*vtx - *((TVector3*)l.gv_pos->At(0))).Z()));
                }
                l.FillHist("ElectronTag_sameVtx",        el_cat, (float)(elVtx==l.dipho_vtxind[diphotonVHlep_id]));

                TLorentzVector dipho_p4 = lead_p4 + sublead_p4;
                lep_sync<<"run="<<l.run<<"\t";
                lep_sync<<"lumis"<<l.lumis<<"\t";
                lep_sync<<"event="<<(unsigned int) l.event<<"\t";
                lep_sync<<"pt1="<<lead_p4.Pt()<<"\t";
                lep_sync<<"eta1="<<lead_p4.Eta()<<"\t";
                lep_sync<<"pt2="<<sublead_p4.Pt()<<"\t";
                lep_sync<<"eta2="<<sublead_p4.Eta()<<"\t";
                lep_sync<<"ptgg="<<dipho_p4.Pt()<<"\t";
                lep_sync<<"elpt="<<myel->Pt()<<"\t";
                lep_sync<<"eleta="<<myel->Eta()<<"  ELTAG\n";
            }
        }
    }
        
    return tag;
}


void PhotonAnalysis::ZWithFakeGammaCS(LoopAll& l, float* smeared_pho_energy){
    int elVtx=-1;
    bool tag = false;

    if(l.met_pfmet > 30) return;

    int el_ind=l.ElectronSelectionMVA2012();
    if(el_ind==-1) return;
    elVtx=l.FindElectronVertex(el_ind);
    
    int selphoind=-1;
    int selphopt=0;
    TLorentzVector* selel_p4= (TLorentzVector*) l.el_std_p4->At(el_ind);
    for(int ipho=0; ipho<l.pho_n; ipho++){
        float phoEta = fabs(((TVector3 *)l.sc_xyz->At(l.pho_scind[ipho]))->Eta());
        if( phoEta > 2.5 || ( phoEta > 1.4442 && phoEta < 1.566 )){
            continue;
        }
        TLorentzVector thispho = l.get_pho_p4(ipho,elVtx,smeared_pho_energy); 
        if(selel_p4->DeltaR(thispho)<0.2) continue;
        float phoPt = thispho.Pt();
        if(phoPt<25) continue;
	    if (!l.PhotonMITPreSelection2011(ipho, elVtx,  smeared_pho_energy)) continue; 
        float phoMVA= l.photonIDMVANew(ipho, elVtx, thispho, bdtTrainingPhilosophy.c_str() );
        if(phoMVA<-0.1) continue;
        if(phoPt>selphopt){
            selphopt=phoPt;
            selphoind=ipho;
            tag=true;
        }
    }            

    if(!tag) return;
    TLorentzVector selpho_p4=l.get_pho_p4(selphoind,elVtx,smeared_pho_energy);

    TLorentzVector eg_p4 = selpho_p4+*selel_p4;
    int cat= (fabs(selpho_p4.Eta())<1.5 && fabs(selel_p4->Eta())<1.5);

    if(selphopt>40){
        l.FillHist("Meg_fakelead",cat,eg_p4.M());
        l.FillHist("r9_fakelead",cat,l.pho_r9[selphoind]);
    } else {
        l.FillHist("Meg_fakesub",cat,eg_p4.M());
        l.FillHist("r9_fakesub",cat,l.pho_r9[selphoind]);
    }
    
    return;
}


bool PhotonAnalysis::ElectronStudies2012B(LoopAll& l, float* smeared_pho_energy, bool mvaselection, float phoidMvaCut, float eventweight, float myweight, int jentry){
    int diphotonVHlep_id=-1;
    int elVtx=-1;
    bool tag = false;
 
    bool debuglocal=false;

    TLorentzVector* el_tag;
    TLorentzVector* el_sc;
    
    float leadptcut=30;
    float subleadptcut=20;
    float elptcut=10;
    
    int elInd=l.ElectronSelectionMVA2012(elptcut);
    if(elInd==-1) return tag;
    
    if(debuglocal)  std::cout<<"ElectronStudies2012B electron"<<elInd<<std::endl;

    el_tag = (TLorentzVector*) l.el_std_p4->At(elInd);
    if(debuglocal)  std::cout<<"ElectronStudies2012B got p4"<<std::endl;
    el_sc = (TLorentzVector*) l.el_std_sc->At(elInd);
    if(debuglocal)  std::cout<<"ElectronStudies2012B got sc p4"<<std::endl;
    elVtx=l.FindElectronVertex(elInd);
    
    float drtoveto = 0.2;
    std::vector<bool> veto_indices;
    veto_indices.clear();
    l.PhotonsToVeto(el_sc, drtoveto, veto_indices);

    if(mvaselection) {
        diphotonVHlep_id = l.DiphotonMITPreSelection(leadEtVHlepCut,subleadEtVHlepCut,phoidMvaCut,
            applyPtoverM, &smeared_pho_energy[0], elVtx, false, false, veto_indices);
    } else {
        diphotonVHlep_id = l.DiphotonCiCSelection( l.phoSUPERTIGHT, l.phoSUPERTIGHT, leadEtVHlepCut,subleadEtVHlepCut, 4, 
            applyPtoverM, &smeared_pho_energy[0], true, elVtx, veto_indices);
    }
       
    if(diphotonVHlep_id==-1) return tag;
    
    if(debuglocal)  std::cout<<"ElectronStudies2012B diphoton "<<diphotonVHlep_id<<std::endl;

    TLorentzVector lead_p4 = l.get_pho_p4( l.dipho_leadind[diphotonVHlep_id], elVtx, &smeared_pho_energy[0]);
    TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphotonVHlep_id], elVtx, &smeared_pho_energy[0]);

    if(debuglocal){
        std::cout<<"lead pt "<<lead_p4.Pt()<<std::endl;
        std::cout<<"sublead pt "<<sublead_p4.Pt()<<std::endl;
        std::cout<<"el pt "<<el_tag->Pt()<<std::endl;
    }
    // need to check again for d0 and dZ (couldn't before because we didn't have the vertex)
    myEl_passelcuts = l.ElectronMVACuts(elInd, elVtx);
    myEl_ElePho     = l.ElectronPhotonCuts2012B(lead_p4, sublead_p4, *el_tag);
    if(myEl_passelcuts){
        if(myEl_ElePho){
            tag=true;
        }
    }

    if(debuglocal){
        std::cout<<"ElectronStudies2012B "<<tag<<std::endl;
    }
   
    int el_cat=-1;
    if(fabs(el_sc->Eta())<1.5 && fabs(lead_p4.Eta())<1.5 && fabs(sublead_p4.Eta())<1.5) el_cat=0;
    else el_cat=1;

    if(debuglocal)  std::cout<<"ElectronStudies2012B el_cat "<<el_cat<<std::endl;

    TLorentzVector gg_p4 = lead_p4 + sublead_p4;
    if(debuglocal){
        std::cout<<"el_tag->Pt(),sublead_p4.Pt(),lead_p4.Pt()/gg_p4.M() "<<el_tag->Pt()<<","<<sublead_p4.Pt()<<","<<lead_p4.Pt()/gg_p4.M()<<std::endl;
        std::cout<<"pt cuts "<<tag<<std::endl;
    }

    float thiseta = fabs(el_sc->Eta());
    float thispt  = fabs(el_tag->Pt());

    double Aeff=0.;
    if(thiseta<1.0)                   Aeff=0.10;
    if(thiseta>=1.0 && thiseta<1.479) Aeff=0.12;
    if(thiseta>=1.479 && thiseta<2.0) Aeff=0.085;
    if(thiseta>=2.0 && thiseta<2.2)   Aeff=0.11;
    if(thiseta>=2.2 && thiseta<2.3)   Aeff=0.12;
    if(thiseta>=2.3 && thiseta<2.4)   Aeff=0.12;
    if(thiseta>=2.4)                  Aeff=0.13;

    float thisiso=l.el_std_pfiso_charged[elInd]+std::max(l.el_std_pfiso_neutral[elInd]+l.el_std_pfiso_photon[elInd]-l.rho*Aeff,0.);
    TLorentzVector elpho1 = *el_tag + lead_p4;
    TLorentzVector elpho2 = *el_tag + sublead_p4;
    
    int cur_type = l.itype[l.current];
    if( cur_type<0 && (abs(l.gh_vh_pdgid)==24 || abs(l.gh_vh_pdgid)==26) ) {
        myEl_leptonSig = (abs(l.gh_vh1_pdgid)>10 && abs(l.gh_vh1_pdgid)<19);
    } else {
        myEl_leptonSig = 1;
    }

    float overE_overP=fabs((1/l.el_std_pin[elInd])-(1/(l.el_std_pin[elInd]*l.el_std_eopin[elInd])));
    myEl_presellead =   HLTPhotonPreselection(l, &lead_p4, elVtx);
    int matchlead=-1;
    myEl_matchellead=   PhotonMatchElectron(l, &lead_p4, matchlead);
    myEl_preselsub  =   HLTPhotonPreselection(l, &sublead_p4, elVtx);
    int matchsub=-1;
    myEl_matchelsub =   PhotonMatchElectron(l, &sublead_p4, matchsub);
    myEl_ptlead     =   lead_p4.Pt();
    myEl_ptsub      =   sublead_p4.Pt();
    myEl_elvetolead =   l.pho_isconv[l.dipho_leadind[diphotonVHlep_id]];
    myEl_elvetosub  =   l.pho_isconv[l.dipho_subleadind[diphotonVHlep_id]];

    int lead_cat = abs(lead_p4.Eta())>1.5;
    int sublead_cat = abs(sublead_p4.Eta())>1.5;
    l.FillHist("PhoMatchEl",lead_cat,   myEl_matchellead);
    l.FillHist("PhoMatchEl",sublead_cat,myEl_matchelsub);

    if(myEl_matchellead==1){
        if(cur_type==0) std::cout<<"badphoton event,lumis,run:  "<<l.event<<","<<l.lumis<<","<<l.run<<std::endl;
        l.FillHist("MatchedPhoPt",      lead_cat, myEl_ptlead);
        l.FillHist("MatchedPhoElVeto",  lead_cat, myEl_elvetolead);
        l.FillHist("MatchedPhoElTrk",   lead_cat, l.el_std_tkdrv[matchlead]);
        l.FillHist("MatchedPhoElEcal",  lead_cat, l.el_std_ecaldrv[matchlead]);
        l.FillHist("MatchedPhoGenPho",  lead_cat, l.pho_genmatched[l.dipho_leadind[diphotonVHlep_id]]);
        l.FillHist("MatchedSCIndex",    lead_cat, (int)(l.el_std_scind[matchlead]==l.pho_scind[l.dipho_leadind[diphotonVHlep_id]]));
    }

    if(myEl_matchelsub==1){
        if(cur_type==0) std::cout<<"badphoton event,lumis,run:  "<<l.event<<","<<l.lumis<<","<<l.run<<std::endl;
        l.FillHist("MatchedPhoPt",      sublead_cat, myEl_ptsub);
        l.FillHist("MatchedPhoElVeto",  sublead_cat, myEl_elvetosub);
        l.FillHist("MatchedPhoElTrk",   sublead_cat, l.el_std_tkdrv[matchsub]);
        l.FillHist("MatchedPhoElEcal",  sublead_cat, l.el_std_ecaldrv[matchsub]);
        l.FillHist("MatchedPhoGenPho",  sublead_cat, l.pho_genmatched[l.dipho_subleadind[diphotonVHlep_id]]);
        l.FillHist("MatchedSCIndex",    sublead_cat, (int)(l.el_std_scind[matchsub]==l.pho_scind[l.dipho_subleadind[diphotonVHlep_id]]));
    }
    
    if(debuglocal)  std::cout<<"ElectronStudies2012B filled pho variables "<<std::endl;

    myEl_elpt       =   thispt;
    myEl_oEsuboP    =   overE_overP;
    myEl_D0         =   l.el_std_D0Vtx[elInd][elVtx];
    myEl_DZ         =   l.el_std_DZVtx[elInd][elVtx];
    myEl_conv       =   l.el_std_conv[elInd];
    myEl_mishit     =   l.el_std_hp_expin[elInd];
    myEl_reliso     =   thisiso/thispt;
    myEl_iso        =   thisiso;
    myEl_detain     =   l.el_std_detain[elInd];
    myEl_dphiin     =   l.el_std_dphiin[elInd];
    myEl_sieie      =   l.el_std_sieie[elInd];
    myEl_hoe        =   l.el_std_hoe[elInd];
    
    if(debuglocal)  std::cout<<"ElectronStudies2012B filled pho+ele variables "<<std::endl;
    
    myEl_drlead     =   lead_p4.DeltaR(*el_tag);
    myEl_drsub      =   sublead_p4.DeltaR(*el_tag);
    myEl_melead     =   elpho1.M();
    myEl_meleadveto15 = abs(myEl_melead-91.2)<15;
    myEl_meleadveto10 = abs(myEl_melead-91.2)<10;
    myEl_mesub      =   elpho2.M();
    myEl_mesubveto10=   abs(myEl_mesub -91.2)<10;
    myEl_mesubveto5 =   abs(myEl_mesub -91.2)<5;
    //myEl_mvaTrig    =   l.el_std_mva_trig[elInd];
    myEl_mvaNonTrig =   l.el_std_mva_nontrig[elInd];

    //std::cout<<"myEl_ElePho "<<myEl_ElePho<<std::endl;
            
    myEl_ptgg       =   gg_p4.Pt();
    myEl_mgg        =   gg_p4.M();
    myEl_ptleadom   =   myEl_ptlead/myEl_mgg;
    myEl_ptsubom    =   myEl_ptsub/myEl_mgg;
    myEl_phomaxeta  =   std::max(lead_p4.Eta(),sublead_p4.Eta());
    myEl_sumpt3     =   lead_p4.Pt()+sublead_p4.Pt()+el_tag->Pt();
    myEl_dRtklead   =   l.pho_drtotk_25_99[l.dipho_leadind[diphotonVHlep_id]];
    myEl_dRtksub    =   l.pho_drtotk_25_99[l.dipho_subleadind[diphotonVHlep_id]];

    if( debuglocal ) {
        if(myEl_dRtklead<1.0 || myEl_dRtksub<1.0 || myEl_matchellead==1 || myEl_matchelsub==1){
            std::cout<<"myEl_dRtklead,myEl_dRtksub,myEl_matchellead,myEl_matchelsub "<<
                    myEl_dRtklead<<","<<
                    myEl_dRtksub<<","<<
                    myEl_matchellead<<","<<
                    myEl_matchelsub<<std::endl;
        }
    }

    myEl_MVAlead    = ( dataIs2011 ? 
                    l.photonIDMVA(l.dipho_leadind[diphotonVHlep_id], elVtx,
                        lead_p4,bdtTrainingPhilosophy.c_str()) :
                    l.photonIDMVANew(l.dipho_leadind[diphotonVHlep_id], elVtx,
                        lead_p4,bdtTrainingPhilosophy.c_str()) ); //photonIDMVAShift_EB );
    myEl_MVAsub     = ( dataIs2011 ?   
                    l.photonIDMVA(l.dipho_subleadind[diphotonVHlep_id], elVtx,
                        sublead_p4,bdtTrainingPhilosophy.c_str()) : 
                    l.photonIDMVANew(l.dipho_subleadind[diphotonVHlep_id], elVtx,
                        sublead_p4,bdtTrainingPhilosophy.c_str()) ); // photonIDMVAShift_EE );
    
    if(debuglocal && myEl_ElePho){
        std::cout<<"myEl_mvaNonTrig,myEl_MVAlead,myEl_MVAsub "
            <<myEl_mvaNonTrig<<","<<myEl_MVAlead<<","<<myEl_MVAsub<<std::endl;
    }
    
    if( debuglocal ) std::cout<<"test01"<<std::endl;

    // Mass Resolution of the Event
    massResolutionCalculator->Setup(l,&photonInfoCollection[l.dipho_leadind[diphotonVHlep_id]],&photonInfoCollection[l.dipho_subleadind[diphotonVHlep_id]],elVtx,eSmearPars,nR9Categories,nEtaCategories,beamspotSigma,true);
//    float vtx_mva  = l.vtx_std_evt_mva->at(diphotonVHlep_id);
    //for(int rankind=0; rankind<l.vtx_std_ranked_list->size(); rankind++){
    //    if(l.vtx_std_ranked_list[rankind]==elVtx){
    //        l.vtx_std_ranked_list->erase(l.vtx_std_ranked_list->begin()+rankind);
    //    }
    //}
    //l.vtx_std_ranked_list->insert(l.vtx_std_ranked_list->begin(),elVtx);
    float vtx_mva = -0.9;// vtxAna_.perEventMva( *tmvaPerEvtReader_, tmvaPerEvtMethod, l.vtx_std_ranked_list->back()  );
    float sigmaMrv = massResolutionCalculator->massResolutionEonly();
    float sigmaMwv = massResolutionCalculator->massResolutionWrongVtx();
    float sigmaMeonly = massResolutionCalculator->massResolutionEonly();
    // easy to calculate vertex probability from vtx mva output
    float vtxProb   = 1.-0.49*(vtx_mva+1.0); /// should better use this: vtxAna_.setPairID(diphoton_id); vtxAna_.vertexProbability(vtx_mva); PM
    if( debuglocal ) std::cout<<"test02"<<std::endl;

	float diphobdt_output = l.diphotonMVA(l.dipho_leadind[diphotonVHlep_id], l.dipho_subleadind[diphotonVHlep_id], elVtx,
					      vtxProb,lead_p4,sublead_p4,sigmaMrv,sigmaMwv,sigmaMeonly,
					      bdtTrainingPhilosophy.c_str(),
					      myEl_MVAlead,myEl_MVAsub);

    myEl_diphomva    =   diphobdt_output; 

    std::vector<std::vector<bool> > ph_passcut;
    if( debuglocal ) std::cout<<"test03"<<std::endl;
    myEl_CiClead    =   l.PhotonCiCPFSelectionLevel(l.dipho_leadind[diphotonVHlep_id], elVtx, ph_passcut, 4, 0, smeared_pho_energy );
    if( debuglocal ) std::cout<<"test04"<<std::endl;
    myEl_CiCsub     =   l.PhotonCiCPFSelectionLevel(l.dipho_subleadind[diphotonVHlep_id], elVtx, ph_passcut, 4, 1, smeared_pho_energy );
    if( debuglocal ) std::cout<<"test05"<<std::endl;
    myEl_MET        =   l.met_pfmet;
    myEl_METphi     =   l.met_phi_pfmet;

    int elcat = (thiseta>1.5 || fabs(lead_p4.Eta())>1.5 || fabs(sublead_p4.Eta())>1.5);
    if(myEl_diphomva>0.05 && (myEl_meleadveto15 || myEl_mesubveto10)) {
        l.FillHist("NearZ_leadr9",    elcat,   (float)l.pho_r9[l.dipho_leadind[diphotonVHlep_id]]);
        if(l.pho_r9[l.dipho_leadind[diphotonVHlep_id]]>0.94){
            l.FillHist("M_elead_hir9",    elcat,   myEl_melead);
        } else {
            l.FillHist("M_elead_lor9",    elcat,   myEl_melead);
        }
        l.FillHist("NearZ_subleadr9",    elcat,   (float)l.pho_r9[l.dipho_subleadind[diphotonVHlep_id]]);
        if(l.pho_r9[l.dipho_subleadind[diphotonVHlep_id]]>0.94){
            l.FillHist("M_esublead_hir9",    elcat,   myEl_mesub);
        } else {
            l.FillHist("M_esublead_lor9",    elcat,   myEl_mesub);
        }
    }
    myEl_category=(int)(fabs(lead_p4.Eta())>1.5)+2*(int)(fabs(sublead_p4.Eta())>1.5) +4*(int)(thiseta>1.5);
    if( debuglocal ) std::cout<<"test08"<<std::endl;
    tag = l.ApplyCutsFill(elcat,10, eventweight, myweight);
    if(tag&&cur_type==0) std::cout<<"selected electron tag event,lumis,run:  "<<l.event<<","<<l.lumis<<","<<l.run<<std::endl;
    if( debuglocal ) std::cout<<"test09"<<std::endl;

    //plots for comparing selection

    if(diphotonVHlep_id!=-1){
        l.FillHist("ElectronTag_MethodA_MethodB_samevtx", 0,(float)(l.dipho_vtxind[diphotonVHlep_id]==elVtx));
    }
    
    return tag;
}


bool PhotonAnalysis::ElectronTagStudies2012(LoopAll& l, int diphotonVHlep_id, float* smeared_pho_energy, bool nm1, float eventweight, float myweight, int jentry){
    bool tag = false;
 
    bool debuglocal=false;

    TLorentzVector* el_tag;
    TLorentzVector* el_sc;
    int elVtx = 0; 
    
    TLorentzVector lead_p4;
    int leadpho_ind=-1;

    TLorentzVector sublead_p4;
    int subleadpho_ind=-1;
    
    float leadptcut=30;
    float subleadptcut=20;
    float elptcut=10;

    int elInd=l.ElectronSelectionMVA2012(elptcut);
    if(elInd!=-1) {
        el_tag = (TLorentzVector*) l.el_std_p4->At(elInd);
        if(el_tag->Pt()>elptcut){
            el_sc = (TLorentzVector*) l.el_std_sc->At(elInd);
            elVtx=l.FindElectronVertex(elInd);

            float drtoveto = 0.2;
            std::vector<bool> veto_indices;
            veto_indices.clear();
            l.PhotonsToVeto(el_sc, drtoveto, veto_indices);

            diphotonVHlep_id = l.DiphotonMITPreSelection(leadptcut,subleadptcut,-0.2,
                    applyPtoverM, &smeared_pho_energy[0], elVtx, false, false, veto_indices);
            //    diphotonVHlep_id = l.DiphotonCiCSelection( l.phoLOOSE, l.phoLOOSE, leadEtVHlepCut,subleadEtVHlepCut, 4, 
            //        applyPtoverM, &smeared_pho_energy[0], true, elVtx, veto_indices);
       
            if(diphotonVHlep_id!=-1){
                lead_p4 = l.get_pho_p4( l.dipho_leadind[diphotonVHlep_id], elVtx, &smeared_pho_energy[0]);
                sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphotonVHlep_id], elVtx, &smeared_pho_energy[0]);
                tag=true;
            }
        }
    }

    if(!tag) return tag;

    if( debuglocal ) {
        std::cout<<"l.ElectronPhotonCuts(lead_p4, sublead_p4, *el_tag)"<<l.ElectronPhotonCuts(lead_p4, sublead_p4, *el_tag)<<std::endl;
    }
    // require tighter dr to electron track
    //if(l.pho_drtotk_25_99[l.dipho_leadind[diphotonVHlep_id]] < 1 || l.pho_drtotk_25_99[l.dipho_subleadind[diphotonVHlep_id]] < 1) return tag;



        
    float thiseta = fabs(el_sc->Eta());
    float thispt  = fabs(el_tag->Pt());

    double Aeff=0.;
    if(thiseta<1.0)                   Aeff=0.10;
    if(thiseta>=1.0 && thiseta<1.479) Aeff=0.12;
    if(thiseta>=1.479 && thiseta<2.0) Aeff=0.085;
    if(thiseta>=2.0 && thiseta<2.2)   Aeff=0.11;
    if(thiseta>=2.2 && thiseta<2.3)   Aeff=0.12;
    if(thiseta>=2.3 && thiseta<2.4)   Aeff=0.12;
    if(thiseta>=2.4)                  Aeff=0.13;

    float thisiso=l.el_std_pfiso_charged[elInd]+std::max(l.el_std_pfiso_neutral[elInd]+l.el_std_pfiso_photon[elInd]-l.rho*Aeff,0.);
    TLorentzVector elpho1 = *el_tag + lead_p4;
    TLorentzVector elpho2 = *el_tag + sublead_p4;
    
    int cur_type = l.itype[l.current];
    if( cur_type<0 && (abs(l.gh_vh_pdgid)==24 || abs(l.gh_vh_pdgid)==26) ) {
        myEl_leptonSig = (abs(l.gh_vh1_pdgid)>10 && abs(l.gh_vh1_pdgid)<19);
    } else {
        myEl_leptonSig = 1;
    }

    float overE_overP=fabs((1/l.el_std_pin[elInd])-(1/(l.el_std_pin[elInd]*l.el_std_eopin[elInd])));
    myEl_presellead =   HLTPhotonPreselection(l, &lead_p4, elVtx);
    int matchlead=-1;
    myEl_matchellead=   PhotonMatchElectron(l, &lead_p4, matchlead);
    myEl_preselsub  =   HLTPhotonPreselection(l, &sublead_p4, elVtx);
    int matchsub=-1;
    myEl_matchelsub =   PhotonMatchElectron(l, &sublead_p4, matchsub);
    myEl_ptlead     =   lead_p4.Pt();
    myEl_ptsub      =   sublead_p4.Pt();
    myEl_elvetolead =   l.pho_isconv[leadpho_ind];
    myEl_elvetosub  =   l.pho_isconv[subleadpho_ind];

    int lead_cat = abs(lead_p4.Eta())>1.5;
    int sublead_cat = abs(sublead_p4.Eta())>1.5;
    l.FillHist("PhoMatchEl",lead_cat,   myEl_matchellead);
    l.FillHist("PhoMatchEl",sublead_cat,myEl_matchelsub);

    if(myEl_matchellead==1){
        if(cur_type==0) std::cout<<"badphoton event,lumis,run:  "<<l.event<<","<<l.lumis<<","<<l.run<<std::endl;
        l.FillHist("MatchedPhoPt",      lead_cat, myEl_ptlead);
        l.FillHist("MatchedPhoElVeto",  lead_cat, myEl_elvetolead);
        l.FillHist("MatchedPhoElTrk",   lead_cat, l.el_std_tkdrv[matchlead]);
        l.FillHist("MatchedPhoElEcal",  lead_cat, l.el_std_ecaldrv[matchlead]);
        l.FillHist("MatchedPhoGenPho",  lead_cat, l.pho_genmatched[leadpho_ind]);
        l.FillHist("MatchedSCIndex",    lead_cat, (int)(l.el_std_scind[matchlead]==l.pho_scind[leadpho_ind]));
    }

    if(myEl_matchelsub==1){
        if(cur_type==0) std::cout<<"badphoton event,lumis,run:  "<<l.event<<","<<l.lumis<<","<<l.run<<std::endl;
        l.FillHist("MatchedPhoPt",      sublead_cat, myEl_ptsub);
        l.FillHist("MatchedPhoElVeto",  sublead_cat, myEl_elvetosub);
        l.FillHist("MatchedPhoElTrk",   sublead_cat, l.el_std_tkdrv[matchsub]);
        l.FillHist("MatchedPhoElEcal",  sublead_cat, l.el_std_ecaldrv[matchsub]);
        l.FillHist("MatchedPhoGenPho",  sublead_cat, l.pho_genmatched[subleadpho_ind]);
        l.FillHist("MatchedSCIndex",    sublead_cat, (int)(l.el_std_scind[matchsub]==l.pho_scind[subleadpho_ind]));
    }

    myEl_elpt       =   thispt;
    myEl_oEsuboP    =   overE_overP;
    myEl_D0         =   l.el_std_D0Vtx[elInd][elVtx];
    myEl_DZ         =   l.el_std_DZVtx[elInd][elVtx];
    myEl_conv       =   l.el_std_conv[elInd];
    myEl_mishit     =   l.el_std_hp_expin[elInd];
    myEl_reliso     =   thisiso/thispt;
    myEl_iso        =   thisiso;
    myEl_detain     =   l.el_std_detain[elInd];
    myEl_dphiin     =   l.el_std_dphiin[elInd];
    myEl_sieie      =   l.el_std_sieie[elInd];
    myEl_hoe        =   l.el_std_hoe[elInd];
    
    myEl_drlead     =   lead_p4.DeltaR(*el_tag);
    myEl_drsub      =   sublead_p4.DeltaR(*el_tag);
    myEl_melead     =   elpho1.M();
    myEl_meleadveto15 = abs(myEl_melead-91.2)<15;
    myEl_meleadveto10 = abs(myEl_melead-91.2)<10;
    myEl_mesub      =   elpho2.M();
    myEl_mesubveto10=   abs(myEl_mesub -91.2)<10;
    myEl_mesubveto5 =   abs(myEl_mesub -91.2)<5;
    //myEl_mvaTrig    =   l.el_std_mva_trig[elInd];
    myEl_mvaNonTrig =   l.el_std_mva_nontrig[elInd];

    myEl_ElePho     =   l.ElectronPhotonCuts2012B(lead_p4, sublead_p4, *el_tag);
    //std::cout<<"myEl_ElePho "<<myEl_ElePho<<std::endl;
            
    TLorentzVector gg_p4 = lead_p4+sublead_p4;
    myEl_ptgg       =   gg_p4.Pt();
    myEl_mgg        =   gg_p4.M();
    myEl_ptleadom   =   myEl_ptlead/myEl_mgg;
    myEl_ptsubom    =   myEl_ptsub/myEl_mgg;
    myEl_phomaxeta  =   std::max(lead_p4.Eta(),sublead_p4.Eta());
    myEl_sumpt3     =   lead_p4.Pt()+sublead_p4.Pt()+el_tag->Pt();
    myEl_dRtklead   =   l.pho_drtotk_25_99[leadpho_ind];
    myEl_dRtksub    =   l.pho_drtotk_25_99[subleadpho_ind];

    if( debuglocal ) {
        if(myEl_dRtklead<1.0 || myEl_dRtksub<1.0 || myEl_matchellead==1 || myEl_matchelsub==1){
            std::cout<<"myEl_dRtklead,myEl_dRtksub,myEl_matchellead,myEl_matchelsub "<<
                    myEl_dRtklead<<","<<
                    myEl_dRtksub<<","<<
                    myEl_matchellead<<","<<
                    myEl_matchelsub<<std::endl;
        }
    }

    myEl_MVAlead    = ( dataIs2011 ? 
                    l.photonIDMVA(leadpho_ind, elVtx,
                        lead_p4,bdtTrainingPhilosophy.c_str()) :
                    l.photonIDMVANew(leadpho_ind, elVtx,
                        lead_p4,bdtTrainingPhilosophy.c_str()) ); //photonIDMVAShift_EB );
    myEl_MVAsub     = ( dataIs2011 ?   
                    l.photonIDMVA(subleadpho_ind, elVtx,
                        sublead_p4,bdtTrainingPhilosophy.c_str()) : 
                    l.photonIDMVANew(subleadpho_ind, elVtx,
                        sublead_p4,bdtTrainingPhilosophy.c_str()) ); // photonIDMVAShift_EE );
    
    if(debuglocal && myEl_ElePho){
        std::cout<<"myEl_mvaNonTrig,myEl_MVAlead,myEl_MVAsub "
            <<myEl_mvaNonTrig<<","<<myEl_MVAlead<<","<<myEl_MVAsub<<std::endl;
    }
    
    if( debuglocal ) std::cout<<"test01"<<std::endl;

    // Mass Resolution of the Event
    massResolutionCalculator->Setup(l,&photonInfoCollection[leadpho_ind],&photonInfoCollection[subleadpho_ind],elVtx,eSmearPars,nR9Categories,nEtaCategories,beamspotSigma,true);
//    float vtx_mva  = l.vtx_std_evt_mva->at(diphotonVHlep_id);
    //for(int rankind=0; rankind<l.vtx_std_ranked_list->size(); rankind++){
    //    if(l.vtx_std_ranked_list[rankind]==elVtx){
    //        l.vtx_std_ranked_list->erase(l.vtx_std_ranked_list->begin()+rankind);
    //    }
    //}
    //l.vtx_std_ranked_list->insert(l.vtx_std_ranked_list->begin(),elVtx);
    float vtx_mva = -0.9;// vtxAna_.perEventMva( *tmvaPerEvtReader_, tmvaPerEvtMethod, l.vtx_std_ranked_list->back()  );
    float sigmaMrv = massResolutionCalculator->massResolutionEonly();
    float sigmaMwv = massResolutionCalculator->massResolutionWrongVtx();
    float sigmaMeonly = massResolutionCalculator->massResolutionEonly();
    // easy to calculate vertex probability from vtx mva output
    float vtxProb   = 1.-0.49*(vtx_mva+1.0); /// should better use this: vtxAna_.setPairID(diphoton_id); vtxAna_.vertexProbability(vtx_mva); PM
    if( debuglocal ) std::cout<<"test02"<<std::endl;

	float diphobdt_output = l.diphotonMVA(leadpho_ind, subleadpho_ind, elVtx,
					      vtxProb,lead_p4,sublead_p4,sigmaMrv,sigmaMwv,sigmaMeonly,
					      bdtTrainingPhilosophy.c_str(),
					      myEl_MVAlead,myEl_MVAsub);

    myEl_diphomva    =   diphobdt_output; 

    std::vector<std::vector<bool> > ph_passcut;
    if( debuglocal ) std::cout<<"test03"<<std::endl;
    myEl_CiClead    =   l.PhotonCiCPFSelectionLevel(leadpho_ind, elVtx, ph_passcut, 4, 0, smeared_pho_energy );
    if( debuglocal ) std::cout<<"test04"<<std::endl;
    myEl_CiCsub     =   l.PhotonCiCPFSelectionLevel(subleadpho_ind, elVtx, ph_passcut, 4, 1, smeared_pho_energy );
    if( debuglocal ) std::cout<<"test05"<<std::endl;
    myEl_MET        =   l.met_pfmet;
    myEl_METphi     =   l.met_phi_pfmet;

    
    int elcat = (thiseta>1.5 || fabs(lead_p4.Eta())>1.5 || fabs(sublead_p4.Eta())>1.5);
    myEl_category=(int)(fabs(lead_p4.Eta())>1.5)+2*(int)(fabs(sublead_p4.Eta())>1.5) +4*(int)(thiseta>1.5);
    if( debuglocal ) std::cout<<"test08"<<std::endl;
    tag = l.ApplyCutsFill(elcat,10, eventweight, myweight);
    if(tag&&cur_type==0) std::cout<<"selected electron tag event,lumis,run:  "<<l.event<<","<<l.lumis<<","<<l.run<<std::endl;
    if( debuglocal ) std::cout<<"test09"<<std::endl;
    //if(jentry>-1) {
    //    if(cur_type!=0){
    //        std::map<std::string,int> genbranchnames;
    //        const char* branchchar[] = {"gp_n",
    //            "gp_mother",
    //            "gp_status",
    //            "gp_pdgid",
    //            "gp_p4"};
    //        std::vector<std::string> branchstrings(branchchar,branchchar + 5);;
    //        
    //        for (int istr=0; istr<branchstrings.size(); istr++){
    //            genbranchnames[branchstrings[istr]]=1;
    //        }
    //   
    //        std::set<TBranch *> genbranches;
    //        genbranches.clear();
    //        l.GetBranches(genbranchnames, genbranches);
    //        l.SetBranchAddresses(genbranchnames);
    //        l.GetEntry(genbranches, jentry);
    //    }
    //    
    //    int el_gen_ind = -1;
    //    if( debuglocal ) std::cout<<"about to gen match el"<<std::endl;
    //    if(cur_type!=0){
    //        el_gen_ind=GenMatch(l, el_tag);
    //    }
    //    l.FillTree("elGenIndex",(float)el_gen_ind);
    //    l.FillTree("elEta",     (float)el_tag->Eta());
    //    l.FillTree("elPhi",     (float)el_tag->Phi());
    //    l.FillTree("elPt",      (float)el_tag->Pt());
    //    if(el_gen_ind!=-1){
    //        TLorentzVector* genel_p4=(TLorentzVector*) l.gp_p4->At(el_gen_ind);
    //        l.FillTree("elGenEta",  (float)genel_p4->Eta());
    //        l.FillTree("elGenPhi",  (float)genel_p4->Phi());
    //        l.FillTree("elGenPt",   (float)genel_p4->Pt());
    //        l.FillTree("elGenPDGID",(float)l.gp_pdgid[el_gen_ind]);
    //        float dr_min = el_tag->DeltaR(*genel_p4);
    //        float dpt_min = el_tag->Pt() - genel_p4->Pt();
    //        l.FillTree("elGenDR",   (float)dr_min);
    //        l.FillTree("elGenDPT",  (float)dpt_min);
    //    }
    //    l.FillTree("elmva", (float)myEl_mvaNonTrig);

    //    int pholead_gen_ind = -1;
    //    if(cur_type!=0){
    //        pholead_gen_ind=GenMatch(l, &lead_p4);
    //    }
    //    l.FillTree("leadGenIndex",(float)pholead_gen_ind);
    //    l.FillTree("leadEta",     (float)lead_p4.Eta());
    //    l.FillTree("leadPhi",     (float)lead_p4.Phi());
    //    l.FillTree("leadPt",      (float)lead_p4.Pt());
    //    l.FillTree("leadr9",      (float)l.pho_r9[leadpho_ind]);
    //    if(pholead_gen_ind!=-1){
    //        TLorentzVector* genlead_p4=(TLorentzVector*) l.gp_p4->At(pholead_gen_ind);
    //        l.FillTree("leadGenEta",  (float)genlead_p4->Eta());
    //        l.FillTree("leadGenPhi",  (float)genlead_p4->Phi());
    //        l.FillTree("leadGenPt",   (float)genlead_p4->Pt());
    //        l.FillTree("leadGenPDGID",(float)l.gp_pdgid[pholead_gen_ind]);
    //        float dr_min  = lead_p4.DeltaR(*genlead_p4);
    //        float dpt_min = lead_p4.Pt() - genlead_p4->Pt();
    //        l.FillTree("leadGenDR",   (float)dr_min);
    //        l.FillTree("leadGenDPT",  (float)dpt_min);
    //    }
    //    l.FillTree("lead_eleveto", (float)myEl_elvetolead);
    //    l.FillTree("sub_eleveto", (float)myEl_elvetosub);
    //    l.FillTree("leadCIC", (float)myEl_CiClead);
    //    l.FillTree("subCIC", (float)myEl_CiCsub);
    //    l.FillTree("lead_Elmatched", (float)myEl_matchellead);
    //    l.FillTree("sub_Elmatched", (float)myEl_matchelsub);
    //    l.FillTree("leadmva", (float)myEl_MVAlead);
    //    l.FillTree("submva", (float)myEl_MVAsub);

    //    int phosublead_gen_ind=-1;
    //    if(cur_type!=0){
    //        phosublead_gen_ind=GenMatch(l, &sublead_p4);
    //    }
    //    l.FillTree("subleadGenIndex",(float)phosublead_gen_ind);
    //    l.FillTree("subleadEta",     (float)sublead_p4.Eta());
    //    l.FillTree("subleadPhi",     (float)sublead_p4.Phi());
    //    l.FillTree("subleadPt",      (float)sublead_p4.Pt());
    //    l.FillTree("subleadr9",      (float)l.pho_r9[subleadpho_ind]);
    //    if(phosublead_gen_ind!=-1){
    //        TLorentzVector* gensublead_p4=(TLorentzVector*) l.gp_p4->At(phosublead_gen_ind);
    //        l.FillTree("subleadGenEta",  (float)gensublead_p4->Eta());
    //        l.FillTree("subleadGenPhi",  (float)gensublead_p4->Phi());
    //        l.FillTree("subleadGenPt",   (float)gensublead_p4->Pt());
    //        l.FillTree("subleadGenPDGID",(float)l.gp_pdgid[phosublead_gen_ind]);
    //        float dr_min  = sublead_p4.DeltaR(*gensublead_p4);
    //        float dpt_min = sublead_p4.Pt() - gensublead_p4->Pt();
    //        l.FillTree("subleadGenDR",   (float)dr_min);
    //        l.FillTree("subleadGenDPT",  (float)dpt_min);
    //    }


    //    l.FillTree("dipho_mva", (float)myEl_diphomva);
    //    l.FillTree("cur_type",  (int)cur_type);
    //    
    //    l.FillTree("tagged",    (float)tag);
    //    l.FillTree("weight",    (float)eventweight);
    //}

    //plots for comparing selection

    if(diphotonVHlep_id!=-1){
        l.FillHist("ElectronTag_MethodA_MethodB_samelead",0,(float)(l.dipho_leadind[diphotonVHlep_id]==leadpho_ind));
        l.FillHist("ElectronTag_MethodA_MethodB_samesub", 0,(float)(l.dipho_subleadind[diphotonVHlep_id]==subleadpho_ind));
        l.FillHist("ElectronTag_MethodA_MethodB_samevtx", 0,(float)(l.dipho_vtxind[diphotonVHlep_id]==elVtx));
    }

    return tag;
}
    

            
int PhotonAnalysis::GenMatch(LoopAll& l, TLorentzVector* recop4){
            
    int reco_gen_ind=-1;
    int reco_gen_pdgid=-1000;
    TLorentzVector* igp_p4;
    TLorentzVector* genmatch_p4;
    float dr_temp=999;
    float dr_min=999;
    float dpt_temp=5;
    float dpt_min=5;
    for(int igp=0; igp<l.gp_n; igp++){
        if(l.gp_status[igp] != 1) continue;
        igp_p4 = (TLorentzVector*) l.gp_p4->At(igp);
        dr_temp = recop4->DeltaR(*igp_p4);
        dpt_temp = recop4->Pt() - igp_p4->Pt();
        if(dr_temp<dr_min && abs(dpt_temp)*1.15<abs(dpt_min)){
            dr_min=dr_temp;
            dpt_min=dpt_temp;
            reco_gen_ind=igp;
            reco_gen_pdgid=l.gp_pdgid[igp];
            genmatch_p4=(TLorentzVector*) l.gp_p4->At(igp);
        }
        if(dr_min<0.05) break;
    }

    return reco_gen_ind;
}

bool PhotonAnalysis::HLTPhotonPreselection(LoopAll& l, TLorentzVector* phop4, int photon_index){
    bool pass = true;
    // denominator definition: only matching HLT related part of MIT preselection
           
    // cuts to emulate the trigger
    float HILTcuts_hoe[4]     = {0.082,0.075,0.075,0.075};                                        
    float HILTcuts_sieie[4]   = {0.014,0.014,0.034,0.034};                                        
    float HILTcuts_ecaliso[4] = {50,4,50,4};                                                      
    float HILTcuts_hcaliso[4] = {50,4,50,4};                                                      
    float HILTcuts_trkiso[4]  = {50,4,50,4};                                                      

    // pT cut
    float phopt = phop4->Et();
    if (phopt<20) pass = false;
    
    // eta cut
    if( phop4->Eta() > 2.5 || ( phop4->Eta()>1.4442 && phop4->Eta()<1.566 ) ) pass = false;
    
    // photon category
    int PhotonEtaCategory;
    if (l.pho_isEB[photon_index]) PhotonEtaCategory=0; 
    if (l.pho_isEE[photon_index]) PhotonEtaCategory=1; 
    int r9_category     = (int) (l.pho_r9[photon_index] <= 0.9);                                                      
    int photon_category = r9_category + 2*PhotonEtaCategory;
    
    // isolation and id cuts
    float val_hoe        = l.pho_hoe[photon_index];
    float val_sieie      = l.pho_sieie[photon_index];                                                          
    float val_ecaliso    = l.pho_ecalsumetconedr03[photon_index] - 0.012*phop4->Et();                              
    float val_hcaliso    = l.pho_hcalsumetconedr03[photon_index] - 0.005*phop4->Et(); 
    float val_trkiso     = l.pho_trksumpthollowconedr03[photon_index] - 0.002*phop4->Et();                
    if (val_hoe     >= HILTcuts_hoe[photon_category]     ) pass = false;                                           
    if (val_sieie   >= HILTcuts_sieie[photon_category]   ) pass = false;
    if (val_ecaliso >= HILTcuts_ecaliso[photon_category] ) pass = false;
    if (val_hcaliso >= HILTcuts_hcaliso[photon_category] ) pass = false;                                           
    if (val_trkiso  >= HILTcuts_trkiso[photon_category]  ) pass = false;
    
    int val_pho_isconv = l.pho_isconv[photon_index];
    if ( !val_pho_isconv ) pass = false;        
    
    return pass;
}


bool PhotonAnalysis::PhotonMatchElectron(LoopAll& l, TLorentzVector* pho_p4){
    int dummy=-1;

    return PhotonMatchElectron(l, pho_p4, dummy);
}

bool PhotonAnalysis::PhotonMatchElectron(LoopAll& l, TLorentzVector* pho_p4, int& el_match_ind){
    bool pass = false;

    // need to check if this photon matches an electron with 0 missing hits
    el_match_ind=-1;
    TLorentzVector* el_p4;
    for(int iel=0; iel<l.el_std_n; iel++){
        if(l.el_std_conv[iel]==1 || l.el_std_hp_expin[iel]!=0) continue;
        el_p4=(TLorentzVector*) l.el_std_sc->At(iel);
        float dr = el_p4->DeltaR(*pho_p4);
        float dpt = el_p4->Pt() - pho_p4->Pt();
        if(dr<0.2 && abs(dpt/pho_p4->Pt())<0.05) {
            el_match_ind=iel;
            pass=true;
            break;
        }
    }
    return pass;
}


bool PhotonAnalysis::MuonTag2011(LoopAll& l, int diphotonVHlep_id, float* smeared_pho_energy, bool nm1, float eventweight, float myweight){
    bool tag = false;

    if(diphotonVHlep_id==-1) return tag;
        
    TLorentzVector lead_p4 = l.get_pho_p4( l.dipho_leadind[diphotonVHlep_id], l.dipho_vtxind[diphotonVHlep_id], &smeared_pho_energy[0]);
    TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphotonVHlep_id], l.dipho_vtxind[diphotonVHlep_id], &smeared_pho_energy[0]);
    
    int muonInd = l.MuonSelection(lead_p4, sublead_p4, l.dipho_vtxind[diphotonVHlep_id]);
    if(muonInd!=-1) tag = true;
    
    return tag;
}

bool PhotonAnalysis::MuonTag2012(LoopAll& l, int diphotonVHlep_id, float* smeared_pho_energy, ofstream& lep_sync, bool nm1, float eventweight, float myweight){
    bool tag = false;

    if(diphotonVHlep_id==-1) return tag;
        
    TLorentzVector lead_p4 = l.get_pho_p4( l.dipho_leadind[diphotonVHlep_id], l.dipho_vtxind[diphotonVHlep_id], &smeared_pho_energy[0]);
    TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphotonVHlep_id], l.dipho_vtxind[diphotonVHlep_id], &smeared_pho_energy[0]);
    
    int muonInd = l.MuonSelection2012(lead_p4, sublead_p4, l.dipho_vtxind[diphotonVHlep_id]);
    if(muonInd!=-1) tag = true;
    
        TLorentzVector dipho_p4 = lead_p4+sublead_p4;
        lep_sync<<"run="<<l.run<<"\t";
        lep_sync<<"lumis"<<l.lumis<<"\t";
        lep_sync<<"event="<<(unsigned int) l.event<<"\t";
        lep_sync<<"pt1="<<lead_p4.Pt()<<"\t";
        lep_sync<<"eta1="<<lead_p4.Eta()<<"\t";
        lep_sync<<"pt2="<<sublead_p4.Pt()<<"\t";
        lep_sync<<"eta2="<<sublead_p4.Eta()<<"\t";
        lep_sync<<"ptgg="<<dipho_p4.Pt()<<"\t";
    if(tag){
        TLorentzVector* mu_p4 = (TLorentzVector*) l.mu_glo_p4->At(muonInd);
        lep_sync<<"mupt="<<mu_p4->Pt()<<"\t";
        lep_sync<<"mueta="<<mu_p4->Eta()<<"  MUTAG\n";
    } else {
        lep_sync<<"MUNOTAG\n";
    }
    
    return tag;
}

bool PhotonAnalysis::MuonTag2012B(LoopAll& l, int& diphotonVHlep_id, int& muVtx, int& mu_cat, float* smeared_pho_energy, ofstream& lep_sync, bool mvaselection, float phoidMvaCut){
    bool tag = false;
    float muptcut=20.;

    int mu_ind=l.MuonSelection2012B(muptcut);
    if(mu_ind!=-1) {
        TLorentzVector* mymu = (TLorentzVector*) l.mu_glo_p4->At(mu_ind);
        muVtx=l.FindMuonVertex(mu_ind);
    
        if(mvaselection) {
            diphotonVHlep_id = l.DiphotonMITPreSelection(leadEtVHlepCut,subleadEtVHlepCut,phoidMvaCut,
                applyPtoverM, &smeared_pho_energy[0], muVtx, false);
        } else {
            diphotonVHlep_id = l.DiphotonCiCSelection( l.phoSUPERTIGHT, l.phoSUPERTIGHT, leadEtVHlepCut,subleadEtVHlepCut, 4, 
                applyPtoverM, &smeared_pho_energy[0], true, muVtx);
        }
       
        if(diphotonVHlep_id!=-1){ 
            TLorentzVector lead_p4 = l.get_pho_p4( l.dipho_leadind[diphotonVHlep_id], muVtx, &smeared_pho_energy[0]);
            TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphotonVHlep_id], muVtx, &smeared_pho_energy[0]);

            tag = l.MuonPhotonCuts2012B(lead_p4, sublead_p4, mymu);
            if(!tag) diphotonVHlep_id=-1;
            mu_cat=(int)(abs(lead_p4.Eta())<1.5 && abs(sublead_p4.Eta())<1.5); 
            TLorentzVector dipho_p4 = lead_p4+sublead_p4;
            if(tag){
                l.FillHist("MuonTag_mupt",      mu_cat, mymu->Pt());
                l.FillHist("MuonTag_mueta",     mu_cat, mymu->Eta());
                l.FillHist("MuonTag_muphi",     mu_cat, mymu->Phi());
                float thisiso=((l.mu_glo_nehadiso04[mu_ind]+l.mu_glo_photiso04[mu_ind])>l.mu_dbCorr[mu_ind]) ?
                l.mu_glo_chhadiso04[mu_ind]+l.mu_glo_nehadiso04[mu_ind]+l.mu_glo_photiso04[mu_ind]-l.mu_dbCorr[mu_ind] : l.mu_glo_chhadiso04[mu_ind];    
                l.FillHist("MuonTag_muisoopt",  mu_cat, thisiso/mymu->Pt());
                l.FillHist("MuonTag_drmulead",  mu_cat, mymu->DeltaR(lead_p4));
                l.FillHist("MuonTag_drmusub",   mu_cat, mymu->DeltaR(sublead_p4));
                l.FillHist("MuonTag_d0",        mu_cat, l.mu_glo_D0Vtx[mu_ind][muVtx]);
                l.FillHist("MuonTag_dZ",        mu_cat, l.mu_glo_DZVtx[mu_ind][muVtx]);
                
                // vertex plots
                TVector3* vtx = (TVector3*) l.vtx_std_xyz->At(muVtx);
                int cur_type = l.itype[l.current];
                if(cur_type!=0){
                    l.FillHist("MuonTag_dZtogen",   mu_cat, (float)((*vtx - *((TVector3*)l.gv_pos->At(0))).Z()));
                }
                l.FillHist("MuonTag_sameVtx",   mu_cat, (float)(muVtx==l.dipho_vtxind[diphotonVHlep_id]));


                lep_sync<<"run="<<l.run<<"\t";
                lep_sync<<"lumis"<<l.lumis<<"\t";
                lep_sync<<"event="<<(unsigned int) l.event<<"\t";
                lep_sync<<"pt1="<<lead_p4.Pt()<<"\t";
                lep_sync<<"eta1="<<lead_p4.Eta()<<"\t";
                lep_sync<<"pt2="<<sublead_p4.Pt()<<"\t";
                lep_sync<<"eta2="<<sublead_p4.Eta()<<"\t";
                lep_sync<<"ptgg="<<dipho_p4.Pt()<<"\t";
                lep_sync<<"mupt="<<mymu->Pt()<<"\t";
                lep_sync<<"mueta="<<mymu->Eta()<<"  MUTAG\n";
            }
        }
    }
    
    return tag;
}



bool PhotonAnalysis::VBFTag2011(LoopAll& l, int diphoton_id, float* smeared_pho_energy, bool nm1, float eventweight, float myweight){
    bool tag = false;

    if(diphoton_id==-1) return tag;
  
    TLorentzVector lead_p4    = l.get_pho_p4( l.dipho_leadind[diphoton_id], l.dipho_vtxind[diphoton_id], &smeared_pho_energy[0]);
    TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphoton_id], l.dipho_vtxind[diphoton_id], &smeared_pho_energy[0]);
          
    std::pair<int, int> jets = l.Select2HighestPtJets(lead_p4, sublead_p4 );
    if(jets.first==-1 or jets.second==-1) return tag;
    
    TLorentzVector diphoton = lead_p4+sublead_p4;
  
    TLorentzVector* jet1 = (TLorentzVector*)l.jet_algoPF1_p4->At(jets.first);
    TLorentzVector* jet2 = (TLorentzVector*)l.jet_algoPF1_p4->At(jets.second);
    TLorentzVector dijet = (*jet1) + (*jet2);
    
    myVBFLeadJPt= jet1->Pt();
    myVBFSubJPt = jet2->Pt();
    myVBF_Mjj   = dijet.M();
    myVBFdEta   = fabs(jet1->Eta() - jet2->Eta());
    myVBFZep    = fabs(diphoton.Eta() - 0.5*(jet1->Eta() + jet2->Eta()));
    myVBFdPhi   = fabs(diphoton.DeltaPhi(dijet));
    myVBF_Mgg   = diphoton.M();
  
    if(nm1){
        tag = l.ApplyCutsFill(0,1, eventweight, myweight);
    } else {
        tag = l.ApplyCuts(0,1);
    }
  
    return tag;
}

bool PhotonAnalysis::VBFTag2012(int & ijet1, int & ijet2, 
                LoopAll& l, int diphoton_id, float* smeared_pho_energy, bool nm1, float eventweight, 
                float myweight, bool * jetid_flags)
{
    static std::vector<unsigned char> id_flags;
    bool tag = false;
    
    if(diphoton_id==-1) return tag;
    
    if( jetid_flags == 0 ) { 
	switchJetIdVertex( l, l.dipho_vtxind[diphoton_id] );
	id_flags.resize(l.jet_algoPF1_n);
	for(int ijet=0; ijet<l.jet_algoPF1_n; ++ijet ) {
	    id_flags[ijet] = PileupJetIdentifier::passJetId(l.jet_algoPF1_cutbased_wp_level[ijet], PileupJetIdentifier::kLoose);
	}
	jetid_flags = (bool*)&id_flags[0];
    }
    
    TLorentzVector lead_p4    = l.get_pho_p4( l.dipho_leadind[diphoton_id], l.dipho_vtxind[diphoton_id], &smeared_pho_energy[0]);
    TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphoton_id], l.dipho_vtxind[diphoton_id], &smeared_pho_energy[0]);

    std::pair<int, int> jets;
    if(usePUjetveto){
        jets = l.Select2HighestPtJets(lead_p4, sublead_p4, jetid_flags );
    } else {
        jets = l.Select2HighestPtJets(lead_p4, sublead_p4);
    }

    if(jets.first==-1 || jets.second==-1) return tag;
    
    TLorentzVector diphoton = lead_p4+sublead_p4;
    
    ijet1 = jets.first; ijet2 = jets.second;
    TLorentzVector* jet1 = (TLorentzVector*)l.jet_algoPF1_p4->At(jets.first);
    TLorentzVector* jet2 = (TLorentzVector*)l.jet_algoPF1_p4->At(jets.second);
    TLorentzVector dijet = (*jet1) + (*jet2);
    
    myVBFLeadJPt= jet1->Pt();
    myVBFSubJPt = jet2->Pt();
    myVBF_Mjj   = dijet.M();
    myVBFdEta   = fabs(jet1->Eta() - jet2->Eta());
    myVBFZep    = fabs(diphoton.Eta() - 0.5*(jet1->Eta() + jet2->Eta()));
    myVBFdPhi   = fabs(diphoton.DeltaPhi(dijet));
    myVBF_Mgg   = diphoton.M();
    myVBFDiPhoPtOverM   = diphoton.Pt()   / myVBF_Mgg;
    myVBFLeadPhoPtOverM = lead_p4.Pt()    / myVBF_Mgg;
    myVBFSubPhoPtOverM  = sublead_p4.Pt() / myVBF_Mgg;
    myVBF_MVA  = -2.;
    myVBF_MVA0 = -2.;
    myVBF_MVA1 = -2.;
    myVBF_MVA2 = -2.;
    myVBF_deltaPhiJJ = jet1->DeltaPhi(*jet2);
    myVBF_deltaPhiGamGam = lead_p4.DeltaPhi(sublead_p4);
    myVBF_etaJJ = (jet1->Eta() + jet2->Eta())/2;

    TVector3 boost = diphoton.BoostVector();
    TLorentzVector jet1Boosted = *jet1, jet2Boosted = *jet2, leadingBoosted = lead_p4, subleadingBoosted = sublead_p4;
    jet1Boosted.Boost(-boost);
    jet2Boosted.Boost(-boost);
    leadingBoosted.Boost(-boost);
    subleadingBoosted.Boost(-boost);

    myVBF_thetaJ1 = leadingBoosted.Angle(jet1Boosted.Vect());
    myVBF_thetaJ2 = leadingBoosted.Angle(jet2Boosted.Vect());
    
    if( mvaVbfSelection ) { 
	if( myVBFLeadJPt>20. && myVBFSubJPt>20. && myVBF_Mjj > 100. ) { // FIXME hardcoded pre-selection thresholds
	    if(nm1 && myVBF_Mgg>massMin && myVBF_Mgg<massMax) { 
		l.FillCutPlots(0,1,"_nminus1",eventweight,myweight); 
	    }
	    if (!multiclassVbfSelection){
		myVBF_MVA = tmvaVbfReader_->EvaluateMVA(mvaVbfMethod);
		tag       = (myVBF_MVA > mvaVbfCatBoundaries.back());
	    }
	    else {
		myVBF_MVA0 = tmvaVbfReader_->EvaluateMulticlass(mvaVbfMethod)[0]; // signal vbf
		myVBF_MVA1 = tmvaVbfReader_->EvaluateMulticlass(mvaVbfMethod)[1]; // dipho
		myVBF_MVA2 = tmvaVbfReader_->EvaluateMulticlass(mvaVbfMethod)[2]; // gluglu
		//do transformation for bkg mvas
		myVBF_MVA1 = -myVBF_MVA1+1;
		myVBF_MVA2 = -myVBF_MVA2+1;
		tag        = (myVBF_MVA0 > multiclassVbfCatBoundaries0.back() && myVBF_MVA1 > multiclassVbfCatBoundaries1.back() && myVBF_MVA2 > multiclassVbfCatBoundaries2.back());
	    }
	    
	    // this is moved to StatAnalysis::fillControlPlots
	    // 	    if(nm1 && tag && myVBF_Mgg>massMin && myVBF_Mgg<massMax ) { 
	    // 		l.FillCutPlots(0,1,"_sequential",eventweight,myweight); 
	    // 	    }
	}
    } else {
	if(nm1){
	    tag = l.ApplyCutsFill(0,1, eventweight, myweight);
	} else {
	    tag = l.ApplyCuts(0,1);
	}
    }
    
    return tag;
}

bool PhotonAnalysis::VHhadronicTag2011(LoopAll& l, int diphotonVHhad_id, float* smeared_pho_energy, bool nm1, float eventweight, float myweight){
    bool tag = false;

    if(diphotonVHhad_id==-1) return tag;
  
    TLorentzVector lead_p4 = l.get_pho_p4( l.dipho_leadind[diphotonVHhad_id], l.dipho_vtxind[diphotonVHhad_id], &smeared_pho_energy[0]);
    TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphotonVHhad_id], l.dipho_vtxind[diphotonVHhad_id], &smeared_pho_energy[0]);
    
    std::pair<int, int> jets = l.Select2HighestPtJets(lead_p4, sublead_p4 );
    if(jets.first==-1 or jets.second==-1) return tag;
  
    TLorentzVector* jet1 = (TLorentzVector*)l.jet_algoPF1_p4->At(jets.first);
    TLorentzVector* jet2 = (TLorentzVector*)l.jet_algoPF1_p4->At(jets.second);
    TLorentzVector dijet = (*jet1) + (*jet2);
    
    TLorentzVector diphoton = lead_p4+sublead_p4;
    
    myVHhadLeadJPt = jet1->Pt();
    myVHhadSubJPt = jet2->Pt();
    myVHhad_Mjj = dijet.M();
    myVHhaddEta = fabs(jet1->Eta() - jet2->Eta());
    myVHhadZep  = fabs(diphoton.Eta() - 0.5*(jet1->Eta() + jet2->Eta()));
    myVHhaddPhi = fabs(diphoton.DeltaPhi(dijet));
    myVHhad_Mgg =diphoton.M();

    
    if(nm1){
        tag = l.ApplyCutsFill(0,2, eventweight, myweight);
    } else {
        tag = l.ApplyCuts(0,2);
    }
  
    return tag;
}



//met at analysis step
bool PhotonAnalysis::METTag2012(LoopAll& l, int& diphotonVHmet_id, float* smeared_pho_energy){
    bool tag = false;
    diphotonVHmet_id = l.DiphotonCiCSelection(l.phoSUPERTIGHT, l.phoSUPERTIGHT, leadEtVHmetCut, subleadEtVHmetCut, 4, false, &smeared_pho_energy[0], true);
    if(diphotonVHmet_id>-1) {
        TLorentzVector lead_p4 = l.get_pho_p4( l.dipho_leadind[diphotonVHmet_id], l.dipho_vtxind[diphotonVHmet_id] , &smeared_pho_energy[0]);
        TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphotonVHmet_id], l.dipho_vtxind[diphotonVHmet_id] , &smeared_pho_energy[0]);        
        TLorentzVector TwoPhoton_Vector = (lead_p4) + (sublead_p4);  
        float m_gamgam = TwoPhoton_Vector.M();
    
        MetCorrections2012_Simple( l, lead_p4 , sublead_p4 );
            
        if (m_gamgam>100 && m_gamgam<180) {
          met_sync << " run: " << l.run
            << "\tevent: " << l.event
            << "\tleadPt: " << lead_p4.Pt()
            << "\tsubleadPt: " << sublead_p4.Pt()
            << "\tdiphomass: " << m_gamgam
            << "\traw_met: " << l.met_pfmet
            << "\traw_met_phi: " << l.met_phi_pfmet
            << "\tshifted_met: " << l.shiftMET_pt
            << "\tcorrected_met: " << l.shiftscaleMET_pt
            << "\tcorrected_met_phi: " << l.shiftscaleMET_phi
            << "\tjet_algoPF1_n: " << l.jet_algoPF1_n
            << endl;
        }        
    }

    if(diphotonVHmet_id==-1) return tag;
    if( l.correctedpfMET > 70 ) tag = true;    
    
    return tag;
    
}


bool PhotonAnalysis::METTag2012B(LoopAll& l, int& diphotonVHmet_id, int& met_cat, float* smeared_pho_energy, ofstream& met_sync, bool mvaselection, float phoidMvaCut){
    bool tag = false;

    int metVtx=0;  // use default
    if(mvaselection) {
        diphotonVHmet_id = l.DiphotonMITPreSelection(leadEtVHmetCut,subleadEtVHmetCut,phoidMvaCut,
            applyPtoverM, &smeared_pho_energy[0], metVtx, false);
    } else {
        diphotonVHmet_id = l.DiphotonCiCSelection( l.phoSUPERTIGHT, l.phoSUPERTIGHT, leadEtVHmetCut,subleadEtVHmetCut, 4, 
            applyPtoverM, &smeared_pho_energy[0], true, metVtx);
    }
    
    if(diphotonVHmet_id!=-1){ 
        TLorentzVector lead_p4 = l.get_pho_p4( l.dipho_leadind[diphotonVHmet_id], metVtx, &smeared_pho_energy[0]);
        TLorentzVector sublead_p4 = l.get_pho_p4( l.dipho_subleadind[diphotonVHmet_id], metVtx, &smeared_pho_energy[0]);

        float MET = l.METCorrection2012B(lead_p4, sublead_p4);

        tag = l.METAnalysis2012B(MET);

        if(!tag) diphotonVHmet_id=-1;
        
        met_cat=(int)(abs(lead_p4.Eta())<1.5 && abs(sublead_p4.Eta())<1.5); 
        
        // for synchronization
        TLorentzVector dipho_p4 = lead_p4+sublead_p4;
        if(tag){
            met_sync<<"run="<<l.run<<"\t";
            met_sync<<"lumis"<<l.lumis<<"\t";
            met_sync<<"event="<<(unsigned int) l.event<<"\t";
            met_sync<<"pt1="<<lead_p4.Pt()<<"\t";
            met_sync<<"eta1="<<lead_p4.Eta()<<"\t";
            met_sync<<"pt2="<<sublead_p4.Pt()<<"\t";
            met_sync<<"eta2="<<sublead_p4.Eta()<<"\t";
            met_sync<<"ptgg="<<dipho_p4.Pt()<<"\t";
            met_sync<<"met="<<MET<<"\n";
        }
    }
    
    return tag;
}


void PhotonAnalysis::reVertex(LoopAll & l)
{
    l.vtx_std_ranked_list->clear();
    l.vtx_std_evt_mva->clear();
    std::vector<int> preselAll;

    for(int i=0; i<l.vtx_std_n ; i++) {
	preselAll.push_back(i); 
    }
    vtxAna_.preselection( preselAll );
    
    for(int id=0; id<l.dipho_n; ++id ) {
	
	vtxAna_.setPairID(id);
	
	if( rematchConversions ) {
	    vtxAna_.setNConv(0);
	    PhotonInfo p1 = l.fillPhotonInfos(l.dipho_leadind[id], true, 0); // WARNING using default photon energy: it's ok because we only re-do the conversion part
	    PhotonInfo p2 = l.fillPhotonInfos(l.dipho_subleadind[id], true, 0); // WARNING using default photon energy: it's ok because we only re-do the conversion part
	    
	    float zconv=0., szconv=0.;
	    vtxAna_.getZFromConvPair(zconv,  szconv, p1, p2);
	    
	    for(int vid=0; vid<l.vtx_std_n; ++vid) {
		if( vtxAna_.nconv(vid) > 0 ) {
		    assert( szconv > 0. );
		    vtxAna_.setPullToConv( vid, fabs(  ((TVector3 *)l.vtx_std_xyz->At(vid))->Z() - zconv ) / szconv );
		} else {
		    vtxAna_.setPullToConv( vid, -1. );
		}
	    }
	}
	
	l.vtx_std_ranked_list->push_back( vtxAna_.rank(*tmvaPerVtxReader_,tmvaPerVtxMethod) );
	l.dipho_vtxind[id] = l.vtx_std_ranked_list->back()[0];
	if( tmvaPerEvtReader_ ) {
	    float vtxEvtMva;
        if (rescaleDZforVtxMVA) vtxEvtMva = vtxAna_.perEventMva( *tmvaPerEvtReader_, tmvaPerEvtMethod, l.vtx_std_ranked_list->back(),targetsigma/sourcesigma );
        else vtxEvtMva = vtxAna_.perEventMva( *tmvaPerEvtReader_, tmvaPerEvtMethod, l.vtx_std_ranked_list->back());
	    l.vtx_std_evt_mva->push_back(vtxEvtMva);
	}
    }
}


float PhotonAnalysis::BeamspotReweight(double vtxZ, double genZ) {
    if (genZ<(-100)) return 1.0;
   
    float diffVar = vtxZ-genZ;
    if (TMath::Abs(diffVar)<0.02) return 1.0;

    float newBSmean1  = 9.9391e-02;
    float newBSmean2  = 1.8902e-01;
    float newBSnorm1  = 5.3210e+00;
    float newBSnorm2  = 4.1813e+01;
    float newBSsigma1 = 9.7530e-01;
    float newBSsigma2 = 7.0811e+00;
    
    float oldBSmean1  = 7.2055e-02;
    float oldBSmean2  = 4.9986e-01;
    float oldBSnorm1  = 3.5411e+00;
    float oldBSnorm2  = 4.0258e+01;
    float oldBSsigma1 = 7.9678e-01;
    float oldBSsigma2 = 8.5356e+00;
    
    float newBSgaus1 = newBSnorm1*exp(-0.5*pow((diffVar-newBSmean1)/newBSsigma1,2));
    float newBSgaus2 = newBSnorm2*exp(-0.5*pow((diffVar-newBSmean2)/newBSsigma2,2));
    float oldBSgaus1 = oldBSnorm1*exp(-0.5*pow((diffVar-oldBSmean1)/oldBSsigma1,2));
    float oldBSgaus2 = oldBSnorm2*exp(-0.5*pow((diffVar-oldBSmean2)/oldBSsigma2,2));
    
    float reweight = (newBSgaus1+newBSgaus2)/(oldBSgaus1+oldBSgaus2);

    //float sourceweight = exp(-pow(hardInterZ-beamspotZ,2)/2.0/sourcesigma/sourcesigma)/sourcesigma;
    //float targetweight = exp(-pow(hardInterZ-beamspotZ,2)/2.0/targetsigma/targetsigma)/targetsigma;

    //float reweight = targetweight/sourceweight;

    //if(PADEBUG) std::cout<<"hardInterZ targetweight/sourceweight reweight "<<hardInterZ<<" "<<targetweight<<"/"<<sourceweight<<" "<<reweight<<std::endl;
    if(PADEBUG) std::cout<< "vtxZ genZ newBSgaus1 newBSgaus2 oldBSgaus1 oldBSgaus2 reweight "<< vtxZ << " " << genZ << " " << newBSgaus1 << " " << newBSgaus2 << " " << oldBSgaus1 << " " << oldBSgaus2 << " " << reweight << " " << std::endl;

    return reweight;
}



// Local Variables:
// mode: c++
// c-basic-offset: 4
// End:
// vim: tabstop=4 expandtab shiftwidth=4 softtabstop=4
