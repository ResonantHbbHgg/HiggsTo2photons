// ---------------------------------------------------------------------------------------------------------------------------------------------
void LoopAll::GlobeCtIsol(int mode, TLorentzVector* p4, float ptCut, float drCutMin, float drCutMax, Int_t & nIsol, Float_t & ptIsol, Float_t & angle1, Float_t & angle2, Float_t & angle3) {
  nIsol=0;
  ptIsol=0.;
  angle1=10.;
  angle2=10.;
  angle3=10.;

  //must put a track selection

  for (int i=0; i<ct_n; i++) {
    TLorentzVector * tempp4= (TLorentzVector *) ct_p4->At(i);
    if(tempp4->Et()<ptCut) continue;
    double dr=p4->DeltaR(*tempp4);
    if(dr<drCutMin) continue;
    if(dr<angle1) {
      angle3=angle2;
      angle2=angle1;
      angle1=dr;
    }
    else if (dr<angle2) {
      angle3=angle2;
      angle2=dr;
    }
    else if (dr<angle3) {
      angle3=dr;
    }
    if(dr>drCutMax) continue;
    nIsol++;
    ptIsol+=tempp4->Et();
  }
}


// ---------------------------------------------------------------------------------------------------------------------------------------------
int LoopAll::GlobeMatchIsl(TLorentzVector* p4, Float_t & deltaR) {
  deltaR=10.;
  int imatch=-1;

  for (int i=0; i<sc_islbar_n; i++) {
    TLorentzVector * tempp4= (TLorentzVector *) sc_islbar_p4->At(i);
    double dr=p4->DeltaR(*tempp4);
    if(dr<deltaR) {
      deltaR=dr;
      imatch=i;
    }
  }
  if(imatch==-1) {
    cout<<"ERROR GlobeMatchIsl found no match!!!! "<<endl;
  }
  else if(deltaR>0.3) {
    cout<<"Strange, GlobeMatchIsl deltaR="<<deltaR<<" etapho "<<p4->Eta()<<endl;
  }
  return imatch;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
#include "eIDCuts.h"
std::pair<bool, bool> LoopAll::ElectronId(int index, eIDLevel type) { 

  std::pair<bool, bool> isoIDResult(true, true);

  //TLorentzVector* p4 = (TLorentzVector*)sc_p4->At(el_std_scind[index]);
  TLorentzVector* p4 = (TLorentzVector*)el_std_sc->At(index);
  float eta = fabs(p4->Eta());
  int eb = 0, bin = 0;
  float see = 0;

  if (p4->Et() < 20.) 
    bin = 2;
  else if (p4->Et() > 30.)
    bin = 0;
  else
    bin =1;

  //#ifndef CMSSW3
  //if (eta < 1.479) {
  //see = sc_sieie[el_std_scind[index]];
  //eb = 0;
  //} else {
  //eb = 1; 
  //see = bc_sieie[sc_bcseedind[el_std_scind[index]]];
  //}
  //#else
  if (eta < 1.479) {
    see = el_std_sieiesc[index];
    eb = 0;
  } else {
    eb = 1; 
    see = el_std_sieie[index];
  }
  //#endif

  float eseedopincor = el_std_eseedopin[index] + el_std_fbrem[index];

  if(el_std_fbrem[index]<0) 
    eseedopincor = el_std_eseedopin[index]; 

  //#ifndef CMSSW3
  //float sip = sipCalculator(index);
  //#else
  float sip = fabs(el_std_ip_gsf[index]);
  //#endif

  int cat = ElectronClassification(index);
  
  float corr_tk_iso   = el_std_tkiso03[  index];
  float corr_ecal_iso = el_std_ecaliso04[index];
  float corr_hcal_iso = el_std_hcaliso04[index];  

  corr_tk_iso   = corr_tk_iso  *pow(40/p4->Et(), 2); 
  corr_ecal_iso = corr_ecal_iso*pow(40/p4->Et(), 2);
  corr_hcal_iso = corr_hcal_iso*pow(40/p4->Et(), 2);
  
  if ((corr_tk_iso > cutisotk[bin][type][cat]) ||
      (corr_ecal_iso > cutisoecal[bin][type][cat]) ||
      (corr_hcal_iso > cutisohcal[bin][type][cat]))

    isoIDResult.first = false;
  
  if (el_std_fbrem[index] < -2) {
    isoIDResult.second = false;
    return isoIDResult;
  }

  if (el_std_hoe[index]  > cuthoe[bin][type][cat]) {
    isoIDResult.second = false;
    return isoIDResult;
  }

  if (see > cutsee[bin][type][cat]) {
    isoIDResult.second = false;
    return isoIDResult;
  }

  if (fabs(el_std_dphiin[index]) > cutdphi[bin][type][cat]) {
    isoIDResult.second = false;
    return isoIDResult;  
  }                            

  if (fabs(el_std_detain[index]) > cutdeta[bin][type][cat]) {
    isoIDResult.second = false;
    return isoIDResult;
  }

  if (eseedopincor < cuteopin[bin][type][cat]) {
    isoIDResult.second = false;
    return isoIDResult;
  }                      

  if (sip > cutip[bin][type][cat]) {
    isoIDResult.second = false;
    return isoIDResult;
  }

  if (el_std_hp_expin[index]  > cutmishits[bin][type][cat]) {
    isoIDResult.second = false;
    return isoIDResult;
  }

  return isoIDResult;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
int LoopAll::ElectronClassification(int index) {
  TLorentzVector* p4 = (TLorentzVector*) el_std_sc->At(index);

  int cat = -1;
  float eta = fabs(p4->Eta());

  if (eta < 1.479) {       // BARREL
    if(el_std_fbrem[index]<0.12)
      cat=1;
    else if (el_std_eopin[index] < 1.2 && el_std_eopin[index] > 0.9) 
      cat=0;
    else 
      cat=2;
  } else {                // ENDCAP
    if(el_std_fbrem[index]<0.2)
      cat=4;
    else if (el_std_eopin[index] < 1.22 && el_std_eopin[index] > 0.82) 
      cat=3;
    else 
      cat=5;
  }

  return cat;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
Float_t LoopAll::sipCalculator(int index) {

  Float_t ip = 0;

  if (el_std_tkind[index] != -1) {
    TLorentzVector* tk = (TLorentzVector*)tk_p4->At(el_std_tkind[index]);
    TVector3* my_tk_pos = (TVector3*)tk_vtx_pos->At(el_std_tkind[index]);

    // FIXME to handle the case of multiple vertices

    if (vtx_std_n != 0) {
      TVector3* my_vtx_pos = (TVector3*)vtx_std_xyz->At(0);

      // this is d0 "corrected" for the vertex...
      ip = fabs((-(my_tk_pos->X()-my_vtx_pos->X())*tk->Y()+(my_tk_pos->Y()-my_vtx_pos->Y()) * tk->X())/tk->Pt());
    } else {
      ip = fabs((-(my_tk_pos->X())*tk->Y()+(my_tk_pos->Y()) * tk->X())/tk->Pt());
    }

  }

  return ip;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
void LoopAll::eIDInfo(Int_t index, Int_t& iso_result, Int_t& id_result, Int_t eIDMaxLevel) {

  iso_result = 0;
  id_result = 0;

  // FIXME add GetEntry functions
  for(Int_t i=0; i<eIDMaxLevel; ++i) {
    std::pair<bool, bool> result = ElectronId(index, LoopAll::eIDLevel(i));

    if (result.first) 
      iso_result = i;

    if (result.second)
      id_result = i;
  }
}


// ---------------------------------------------------------------------------------------------------------------------------------------------
// Vertex Analysis
// ---------------------------------------------------------------------------------------------------------------------------------------------
class GlobeVertexInfo : public VertexInfoAdapter
{
public:
	GlobeVertexInfo(LoopAll &);
	
	virtual int nvtx() const    { return lo_.vtx_std_n; };
	virtual int ntracks() const { return lo_.tk_n; };
	
	virtual bool hasVtxTracks() const { return true; }
	virtual const unsigned short * vtxTracks(int ii) const { return &(*lo_.vtx_std_tkind)[ii][0]; };
	virtual int vtxNTracks(int ii) const { return lo_.vtx_std_ntks[ii]; };
	virtual const float * vtxTkWeights(int ii) const { return &(*lo_.vtx_std_tkweight)[ii][0]; };

	virtual float tkpx(int ii) const { return ((TLorentzVector*)lo_.tk_p4->At(ii))->Px(); };
	virtual float tkpy(int ii) const { return ((TLorentzVector*)lo_.tk_p4->At(ii))->Py(); };
	virtual float tkpz(int ii) const { return ((TLorentzVector*)lo_.tk_p4->At(ii))->Pz(); };
	
	virtual float tkPtErr(int ii) const { return lo_.tk_pterr[ii]; };
	virtual int   tkVtxId(int ii) const { return -1; };

	virtual float tkWeight(int ii, int jj) const { return (*lo_.vtx_std_tkweight)[jj][ii]; };
	
	virtual float vtxx(int ii) const { return ((TVector3*)lo_.vtx_std_xyz->At(ii))->X(); };
	virtual float vtxy(int ii) const { return ((TVector3*)lo_.vtx_std_xyz->At(ii))->Y(); };
	virtual float vtxz(int ii) const { return ((TVector3*)lo_.vtx_std_xyz->At(ii))->Z(); };

	virtual float tkd0(int ii, int jj) const { return 0.; } // FIXME
	virtual float tkd0Err(int ii, int jj) const { return 0.; };  // FIXME

	virtual float tkdz(int ii, int jj) const { return 0.; };  // FIXME
	virtual float tkdzErr(int ii, int jj) const { return 0.; };  // FIXME

	virtual bool tkIsHighPurity(int ii) const { return ( lo_.tk_quality[ii] & (1<<2) ) >> 2; };

	virtual ~GlobeVertexInfo();
	
private:
	LoopAll & lo_;
};


// ---------------------------------------------------------------------------------------------------------------------------------------------
GlobeVertexInfo::GlobeVertexInfo(LoopAll & lo) : lo_(lo) {};

// ---------------------------------------------------------------------------------------------------------------------------------------------
GlobeVertexInfo::~GlobeVertexInfo() {};


// ---------------------------------------------------------------------------------------------------------------------------------------------
void LoopAll::vertexAnalysis(HggVertexAnalyzer & vtxAna, int p1, int p2)
{
        GlobeVertexInfo vinfo(*this); 
	PhotonInfo
	  pho1(p1,*((TVector3*)pho_calopos->At(p1)),((TLorentzVector*)pho_p4->At(p1))->Energy()),
	  pho2(p2,*((TVector3*)pho_calopos->At(p2)),((TLorentzVector*)pho_p4->At(p2))->Energy());
	vtxAna.analyze(vinfo,pho1,pho2);
}


// ---------------------------------------------------------------------------------------------------------------------------------------------
PhotonInfo LoopAll::fillPhotonInfos(int p1, bool useAllConvs) 
{
	if(LDEBUG)  cout << "  LoopAll::fillPhotonInfos with index " << p1 <<  endl;
	
	int iConv1 = useAllConvs ? matchPhotonToConversion(p1) : -1;
	if(LDEBUG)  cout << " I have matched and the index is " << iConv1 << endl;
	
	if ( iConv1 >= 0) {
		// conversions infos
		return PhotonInfo(p1,
				  *((TVector3*)pho_calopos->At(p1)),
				  *((TVector3*) bs_xyz),
				  *((TVector3*) conv_vtx->At(iConv1)),
				  ((TLorentzVector*)pho_p4->At(p1))->Energy(),
				  pho_isEB[p1],
				  conv_ntracks[iConv1],
				  conv_validvtx[iConv1],
				  conv_chi2_probability[iConv1],
				  conv_eoverp[iConv1]
			);
	}
	
	return PhotonInfo(p1, 
			  *((TVector3*)pho_calopos->At(p1)),                                                                                                                
			  *((TVector3*) bs_xyz),                                                                                                                            
			  *((TVector3*) pho_conv_vtx->At(p1)),                                                                                                              
			  ((TLorentzVector*)pho_p4->At(p1))->Energy(),                                                                                                      
			  pho_isEB[p1],                                                                                                                                     
			  pho_conv_ntracks[p1],                                                                                                                             
			  pho_conv_validvtx[p1],                                                                                                                            
			  pho_conv_chi2_probability[p1] ,                                                                                                                   
			  pho_conv_eoverp[p1]                                                                                                                               
		);
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
std::vector<int> LoopAll::vertexSelection(HggVertexAnalyzer & vtxAna, HggVertexFromConversions & vtxAnaFromConv, 
					  PhotonInfo & pho1, PhotonInfo & pho2, std::vector<std::string> & vtxVarNames)
{
	int p1 = pho1.id(), p2 = pho2.id();
	assert( p1 == vtxAna.pho1() && p2 == vtxAna.pho2() );

	// preselect vertices : all vertices
        std::vector<int> preselAll;
        for(int i=0; i<vtx_std_n ; i++) {
          preselAll.push_back(i); 
        }

        float zconv = 0; 
        float dzconv = 0;
        std::vector<int> preselConv;

        if ( (pho1.isAConversion() || pho2.isAConversion() ) )  {
	  
          if (pho1.isAConversion()  && !pho2.isAConversion() ){
            zconv  = vtxAnaFromConv.vtxZ(pho1);
            dzconv = vtxAnaFromConv.vtxdZ(pho1);
          }
	  
          if (pho2.isAConversion() && !pho1.isAConversion()){
            zconv  = vtxAnaFromConv.vtxZ(pho2);
            dzconv = vtxAnaFromConv.vtxdZ(pho2);
          }
	  
          if ( pho1.isAConversion() && pho2.isAConversion()){
            float z1  = vtxAnaFromConv.vtxZ(pho1);
            float dz1 = vtxAnaFromConv.vtxdZ(pho1);
            
            float z2  = vtxAnaFromConv.vtxZ(pho2);
            float dz2 = vtxAnaFromConv.vtxdZ(pho2);
            
            zconv  = sqrt ( 1./(1./dz1/dz1 + 1./dz2/dz2 )*(z1/dz1/dz1 + z2/dz2/dz2) ) ;  // weighted average
            dzconv = sqrt( 1./(1./dz1/dz1 + 1./dz2/dz2)) ;
          }
	  
	  // preselect vertices : only vertices in a window zconv +/- dzconv
	  for(int i=0; i < vtx_std_n; i++) {
	    TVector3 * vtxpos= (TVector3 *) vtx_std_xyz->At(i);
	    if ( fabs(zconv - vtxpos->Z() ) < dzconv ) 
              preselConv.push_back(i); 
          }
	  
        }
	
	// preselection 
	if ( preselConv.size()==0 ) 
          vtxAna.preselection(preselAll);
        else 
          vtxAna.preselection(preselConv);

	std::vector<int> rankprod = vtxAna.rankprod(vtxVarNames);
	
	return rankprod;
}


double LoopAll::phiNorm(float &phi) {

  const float pi = 3.1415927;
  const float twopi = 2.0*pi;

  if(phi >  pi) {phi = phi - twopi;}
  if(phi < -pi) {phi = phi + twopi;}

  return phi;
}


double LoopAll::etaTransformation(  float EtaParticle , float Zvertex)  {

  //---Definitions
  const float pi = 3.1415927;

  //---Definitions for ECAL
  const float R_ECAL           = 136.5;
  const float Z_Endcap         = 328.0;
  const float etaBarrelEndcap  = 1.479; 
   
  //---ETA correction

  float Theta = 0.0  ; 
  float ZEcal = R_ECAL*sinh(EtaParticle)+Zvertex;

  if(ZEcal != 0.0) Theta = atan(R_ECAL/ZEcal);
  if(Theta<0.0) Theta = Theta+pi ;
  double ETA = - log(tan(0.5*Theta));
         
  if( fabs(ETA) > etaBarrelEndcap )
    {
      float Zend = Z_Endcap ;
      if(EtaParticle<0.0 )  Zend = -Zend ;
      float Zlen = Zend - Zvertex ;
      float RR = Zlen/sinh(EtaParticle); 
      Theta = atan(RR/Zend);
      if(Theta<0.0) Theta = Theta+pi ;
      ETA = - log(tan(0.5*Theta));		      
    } 
  //---Return the result
  return ETA;
  //---end
}


int  LoopAll::matchPhotonToConversion( int lpho) {

  int result=-99;
  double conv_eta=-999.;
  double conv_phi=-999.;
  
  float sc_eta  = ((TVector3 *) pho_calopos->At(lpho))->Eta();
  float sc_phi  = ((TVector3 *) pho_calopos->At(lpho))->Phi();
  
  TLorentzVector * p4 = (TLorentzVector *) pho_p4->At(lpho);
  float et = p4->Energy() / cosh(sc_eta);
  //  cout << " photon index " << lpho << " eta " <<sc_eta<< " phi " << sc_phi << " et " << et << endl; 
  
  float detaMin=999.;
  float dphiMin=999.;   
  float dRMin = 999.;

  float mconv_pt=-999999;
  int iMatch=-1;     

  if(LDEBUG)  cout << "   LoopAll::matchPhotonToConversion conv_n " << conv_n << endl; 
  for(int iconv=0; iconv<conv_n; iconv++) {
    vector<TVector3> refittedPairMomentum;
    refittedPairMomentum.push_back(*((TVector3*) conv_refitted_momentum->At(iconv)));
    float conv_pt =  refittedPairMomentum[iconv].Pt();
    if (conv_pt < 1 ) continue;    
    if ( ! conv_validvtx[iconv] ) continue;

    conv_phi  = ((TVector3 *) conv_refitted_momentum->At(iconv))->Phi();
    float eta  = ((TVector3 *) conv_refitted_momentum->At(iconv))->Eta();
    conv_eta = etaTransformation(eta, conv_zofprimvtxfromtrks[iconv] );

    //    cout << " conversion index " << iconv << " eta " <<conv_eta<<  " norm phi " << conv_phi << " PT " << conv_pt << endl; 

    float dPhi =conv_phi - sc_phi;       
    double delta_phi = phiNorm (dPhi);
    double delta_eta = conv_eta - sc_eta;
 
    //cout << " delta_eta " << delta_eta << " delta_phi " << delta_phi << endl;
    //delta_phi=pow(delta_phi,2);
    //delta_eta=pow(delta_eta,2);
    //float dR = sqrt( delta_phi+delta_eta); 
    
    if ( fabs(delta_eta) < detaMin && fabs(delta_phi) < dphiMin ) {
    //    if ( dR < dRMin ) {
      detaMin=  fabs(delta_eta);
      dphiMin=  fabs(delta_phi);
      //dRMin=dR;
      iMatch=iconv;
      mconv_pt = conv_pt;
    }

  }
  
  //  cout << " minimized conversion index " << iMatch << " eta " <<conv_eta<< " phi " << conv_phi <<endl; 

  if ( detaMin < 0.1 && dphiMin < 0.1 ) {
  //  if ( dRMin< 0.1 ) {
    if(LDEBUG)    cout << " matched conversion index " << iMatch << " eta " <<conv_eta<< " phi " << conv_phi << " pt " << mconv_pt << endl; 	
    result = iMatch;
  } else {
    result = -1;
  }
 
  return result;
  

}


// ---------------------------------------------------------------------------------------------------------------------------------------------
TLorentzVector LoopAll::get_pho_p4(int ipho, int ivtx)
{
	PhotonInfo p(ipho, *((TVector3*)pho_calopos->At(ipho)),((TLorentzVector*)pho_p4->At(ipho))->Energy());
	TVector3 * vtx = (TVector3*) vtx_std_xyz->At(ivtx);
	return p.p4( vtx->X(), vtx->Y(), vtx->Z() );
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
void LoopAll::set_pho_p4(int ipho, int ivtx)
{
	PhotonInfo p(ipho, *((TVector3*)pho_calopos->At(ipho)),((TLorentzVector*)pho_p4->At(ipho))->Energy());
	TVector3 * vtx = (TVector3*) vtx_std_xyz->At(ivtx);
	TLorentzVector p4 = p.p4( vtx->X(), vtx->Y(), vtx->Z() );
	*((TLorentzVector*)pho_p4->At(ipho)) = p4;
}


// ---------------------------------------------------------------------------------------------------------------------------------------------
void LoopAll::FillCICInputs()
{
	pho_tkiso_recvtx_030_002_0000_10_01->clear(); pho_tkiso_recvtx_030_002_0000_10_01->resize(pho_n,std::vector<float>(vtx_std_n,0.));
	
	for(int ipho=0;ipho<pho_n;++ipho){
		TLorentzVector * phop4 = (TLorentzVector*)pho_p4->At(ipho);
		pho_tkiso_badvtx_040_002_0000_10_01[ipho] = WorstSumTrackPtInCone(phop4, 0,0, 0.40, 0.02, 0.0, 1.0, 0.1);
		pho_drtotk_25_99[ipho] = DeltaRToTrack(ipho, vtx_std_sel, 2.5, 99.);
		
		for(int ivtx=0;ivtx<vtx_std_n;++ivtx) {
			TLorentzVector p4 = get_pho_p4( ipho, ivtx );
			(*pho_tkiso_recvtx_030_002_0000_10_01)[ipho][ivtx] = SumTrackPtInCone(phop4, ivtx, 0, 0.30, 0.02, 0.0, 1.0, 0.1);
		}
	}
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
void LoopAll::FillCIC()
{
	// 6 categories
	pho_cic6passcuts_lead->clear(); pho_cic6passcuts_lead->resize( pho_n, std::vector<UInt_t>(phoNCUTLEVELS,0) ); 
	pho_cic6cutlevel_lead->clear(); pho_cic6cutlevel_lead->resize( pho_n, 0 );
	pho_cic6passcuts_sublead->clear(); pho_cic6passcuts_sublead->resize( pho_n, std::vector<UInt_t>(phoNCUTLEVELS,0) ); 
	pho_cic6cutlevel_sublead->clear(); pho_cic6cutlevel_sublead->resize( pho_n, 0 );
	std::vector<std::vector<bool> > cic6_passcut_lead, cic6_passcut_sublead;
	// 4 categories
	pho_cic4passcuts_lead->clear(); pho_cic4passcuts_lead->resize( pho_n, std::vector<UInt_t>(phoNCUTLEVELS,0) ); 
	pho_cic4cutlevel_lead->clear(); pho_cic4cutlevel_lead->resize( pho_n, 0 );
	pho_cic4passcuts_sublead->clear(); pho_cic4passcuts_sublead->resize( pho_n, std::vector<UInt_t>(phoNCUTLEVELS,0) ); 
	pho_cic4cutlevel_sublead->clear(); pho_cic4cutlevel_sublead->resize( pho_n, 0 );
	std::vector<std::vector<bool> > cic4_passcut_lead, cic4_passcut_sublead;
	
	for(int ipho=0;ipho<pho_n;++ipho){
		// 6 categories
		int cic6_level_lead = PhotonCiCSelectionLevel(ipho, cic6_passcut_lead, 6, 0);
		int cic6_level_sublead = PhotonCiCSelectionLevel(ipho, cic6_passcut_sublead, 6, 1);
		(*pho_cic6cutlevel_lead)[ipho] = cic6_level_lead;
		(*pho_cic6cutlevel_sublead)[ipho] = cic6_level_sublead;
		// 4 categories
		int cic4_level_lead = PhotonCiCSelectionLevel(ipho, cic4_passcut_lead, 4, 0);
		int cic4_level_sublead = PhotonCiCSelectionLevel(ipho, cic4_passcut_sublead, 4, 1);
		(*pho_cic4cutlevel_lead)[ipho] = cic4_level_lead;
		(*pho_cic4cutlevel_sublead)[ipho] = cic4_level_sublead;
		
		for(int iCUTLEVEL=0;iCUTLEVEL!=(int)phoNCUTLEVELS;++iCUTLEVEL) {
			// 6 categories
			UInt_t cic6_leadw=0, cic6_subleadw=0;
			for(size_t icut=0; icut<cic6_passcut_lead.size(); ++icut) {
				cic6_leadw |= ( (!cic6_passcut_lead[iCUTLEVEL][icut] & 0x1) << icut);
			}
			(*pho_cic6passcuts_lead)[ipho][iCUTLEVEL] = cic6_leadw;
			for(size_t icut=0; icut<cic6_passcut_sublead.size(); ++icut) {
				cic6_subleadw |= ( (!cic6_passcut_sublead[iCUTLEVEL][icut] & 0x1) << icut);
			}
			(*pho_cic6passcuts_sublead)[ipho][iCUTLEVEL] = cic6_subleadw;
			// 4 categories
			UInt_t cic4_leadw=0, cic4_subleadw=0;
			for(size_t icut=0; icut<cic4_passcut_lead.size(); ++icut) {
				cic4_leadw |= ( (!cic4_passcut_lead[iCUTLEVEL][icut] & 0x1) << icut);
			}
			(*pho_cic4passcuts_lead)[ipho][iCUTLEVEL] = cic4_leadw;
			for(size_t icut=0; icut<cic4_passcut_sublead.size(); ++icut) {
				cic4_subleadw |= ( (!cic4_passcut_sublead[iCUTLEVEL][icut] & 0x1) << icut);
			}
			(*pho_cic4passcuts_sublead)[ipho][iCUTLEVEL] = cic4_subleadw;
			
		}
	}
}


// CiC SELECTION CODE BEGIN - SSIMON
// ---------------------------------------------------------------------------------------------------------------------------------------------
void LoopAll::SetPhotonCutsInCategories(phoCiCIDLevel cutlevel, float * cic6_allcuts_lead, float * cic6_allcuts_sublead, float * cic4_allcuts_lead, float * cic4_allcuts_sublead) {

  //thresholds are in this order below
  // isosumoet[]
  // isosumoetbad[]
  // trkisooetom[]
  // sieie[]
  // hovere[]
  // r9[]
  // drtotk_25_99[]
  // pixel[]

// 6 categories
// phoNOCUTS      - thresholds so all photons pass
// phoLOOSE       - sob value=0.0002         - iteration 8 - eff=0.947448  fake=0.0783937
// phoMEDIUM      - sob value=0.0004         - iteration 6 - eff=0.928017  fake=0.0572683
// phoTIGHT       - sob value=0.0008         - iteration 6 - eff=0.895238  fake=0.0392572
// phoSUPERTIGHT  - sob value=0.0016         - iteration 6 - eff=0.849812  fake=0.0256949
// phoHYPERTIGHT1 - sob value=0.0032         - iteration 6 - eff=0.784283  fake=0.016346
// phoHYPERTIGHT2 - sob value=0.00625        - iteration 6 - eff=0.699128  fake=0.00991561
// phoHYPERTIGHT3 - sob value=0.0125         - iteration 6 - eff=0.573171  fake=0.00520159
// phoHYPERTIGHT4 - sob value=0.025          - iteration 6 - eff=0.41176   fake=0.00217666


// 4 categories
// phoNOCUTS      - thresholds so all photons pass
// phoLOOSE       - sob value=0.0002         - iteration 8 - eff=0.939229  fake=0.0815158
// phoMEDIUM      - sob value=0.0004         - iteration 6 - eff=0.91754   fake=0.0581047
// phoTIGHT       - sob value=0.0008         - iteration 6 - eff=0.886869  fake=0.041063
// phoSUPERTIGHT  - sob value=0.0016         - iteration 6 - eff=0.844314  fake=0.0286033
// phoHYPERTIGHT1 - sob value=0.0032         - iteration 6 - eff=0.774552  fake=0.0191603
// phoHYPERTIGHT2 - sob value=0.00625        - iteration 6 - eff=0.67859   fake=0.0121262
// phoHYPERTIGHT3 - sob value=0.0125         - iteration 6 - eff=0.521328  fake=0.00626966
// phoHYPERTIGHT4 - sob value=0.025          - iteration 6 - eff=0.381192  fake=0.00357649



  const unsigned int ncuts = 8;
  const unsigned int ncat_cic6 = 6;
  const unsigned int ncat_cic4 = 4;
  switch(cutlevel) {
    case(phoNOCUTS) : {
                        float cic6_allcuts_temp_lead[] = { 
                          1e+09,     1e+09,     1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,     1e+09,     1e+09,
                          -1e+09,    -1e+09,    -1e+09,    -1e+09,    -1e+09,    -1e+09,
                          -1e+09,    -1e+09,    -1e+09,    -1e+09,    -1e+09,    -1e+09,
                          1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                        float cic6_allcuts_temp_sublead[] = { 
                          1e+09,     1e+09,     1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,     1e+09,     1e+09,
                          -1e+09,    -1e+09,    -1e+09,    -1e+09,    -1e+09,    -1e+09,
                          -1e+09,    -1e+09,    -1e+09,    -1e+09,    -1e+09,    -1e+09,
                          1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                        float cic4_allcuts_temp_lead[] = { 
                          1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,
                          -1e+09,    -1e+09,    -1e+09,    -1e+09,
                          -1e+09,    -1e+09,    -1e+09,    -1e+09,
                          1.5,         1.5,         1.5,         1.5 };
                        float cic4_allcuts_temp_sublead[] = { 
                          1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,
                          1e+09,     1e+09,     1e+09,     1e+09,
                          -1e+09,    -1e+09,    -1e+09,    -1e+09,
                          -1e+09,    -1e+09,    -1e+09,    -1e+09,
                          1.5,         1.5,         1.5,         1.5 };
                        for(int i=0;i!=ncuts*ncat_cic6;++i) { cic6_allcuts_lead[i]=cic6_allcuts_temp_lead[i];
                          cic6_allcuts_sublead[i]=cic6_allcuts_temp_sublead[i]; }
                          for(int i=0;i!=ncuts*ncat_cic4;++i) { cic4_allcuts_lead[i]=cic4_allcuts_temp_lead[i];
                            cic4_allcuts_sublead[i]=cic4_allcuts_temp_sublead[i]; }
                      } break;
    case(phoLOOSE) : {
                       float cic6_allcuts_temp_lead[] = { 
                         14.1278,     11.7187,     9.78826,     10.9814,     9.21945,     8.89621,
                         72.5178,     59.1506,     85.1822,     93.8969,     74.2109,     14.4058,
                         7.89015,     5.61652,     4.45536,     5.87563,     4.24725,     2.96206,
                         0.0114196,   0.0109898,   0.0100549,    0.029265,   0.0290002,   0.0279397,
                         0.0907646,   0.0791189,   0.0835245,    0.102617,   0.0596196,    0.098899,
                         0.94,    0.899976,    0.262285,    0.94,    0.90,    0.276953,
                         12.0314,     98.0038,  0.00968623,  0.00636153,  0.00476398,  0.00610842,
                         1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                       float cic6_allcuts_temp_sublead[] = { 
                         14.1278,     11.7187,     9.78826,     10.9814,     9.21945,     8.89621,
                         72.5178,     59.1506,     85.1822,     93.8969,     74.2109,     14.4058,
                         7.89015,     5.61652,     4.45536,     5.87563,     4.24725,     2.96206,
                         0.0114196,   0.0109898,   0.0100549,    0.029265,   0.0290002,   0.0279397,
                         0.0907646,   0.0791189,   0.0835245,    0.102617,   0.0596196,    0.098899,
                         0.94,    0.899976,    0.262285,    0.94,    0.90,    0.276953,
                         12.0314,     98.0038,  0.00968623,  0.00636153,  0.00476398,  0.00610842,
                         1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                       float cic4_allcuts_temp_lead[] = { 
                         8.2468,     4.16322,     5.42672,     2.58495,
                         66.7249,     69.6142,     84.8978,     7.20773,
                         7.54866,     4.56393,     5.24503,     2.52207,
                         0.0111626,   0.0102519,   0.0290281,   0.0282623,
                         0.0903079,   0.0893613,    0.101156,   0.0730662,
                         0.94,    0.307556,    0.921175,    0.287359,
                         3.8468,     1.78477,  0.00621473,  0.00547537,
                         1.5,         1.5,         1.5,         1.5 };
                       float cic4_allcuts_temp_sublead[] = { 
                         8.2468,     4.16322,     5.42672,     2.58495,
                         66.7249,     69.6142,     84.8978,     7.20773,
                         7.54866,     4.56393,     5.24503,     2.52207,
                         0.0111626,   0.0102519,   0.0290281,   0.0282623,
                         0.0903079,   0.0893613,    0.101156,   0.0730662,
                         0.94,    0.307556,    0.921175,    0.287359,
                         3.8468,     1.78477,  0.00621473,  0.00547537,
                         1.5,         1.5,         1.5,         1.5 };
                       for(int i=0;i!=ncuts*ncat_cic6;++i) { cic6_allcuts_lead[i]=cic6_allcuts_temp_lead[i];
                         cic6_allcuts_sublead[i]=cic6_allcuts_temp_sublead[i]; }
                         for(int i=0;i!=ncuts*ncat_cic4;++i) { cic4_allcuts_lead[i]=cic4_allcuts_temp_lead[i];
                           cic4_allcuts_sublead[i]=cic4_allcuts_temp_sublead[i]; }
                     } break;
    case(phoMEDIUM) : {
                        float cic6_allcuts_temp_lead[] = {  
                          12.5084,      10.156,     9.23141,     10.0482,     8.34498,     8.73704,
                          70.9011,     50.0742,     21.9926,     24.2436,     18.7884,     12.6882,
                          6.58797,     4.68564,     4.38815,     5.67876,     2.41162,     2.19991,
                          0.0110266,   0.0106749,  0.00983011,   0.0287021,   0.0286817,   0.0272739,
                          0.0891215,   0.0763711,   0.0798623,   0.0911974,   0.0511163,   0.0627764,
                          0.94,    0.90,    0.274434,    0.94,    0.90,    0.276953,
                          96.5654,     98.9721,   0.0119942,   0.0111399,  0.00855448,    0.012159,
                          1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                        float cic6_allcuts_temp_sublead[] = {  
                          12.5084,      10.156,     9.23141,     10.0482,     8.34498,     8.73704,
                          70.9011,     50.0742,     21.9926,     24.2436,     18.7884,     12.6882,
                          6.58797,     4.68564,     4.38815,     5.67876,     2.41162,     2.19991,
                          0.0110266,   0.0106749,  0.00983011,   0.0287021,   0.0286817,   0.0272739,
                          0.0891215,   0.0763711,   0.0798623,   0.0911974,   0.0511163,   0.0627764,
                          0.94,    0.90,    0.274434,    0.94,    0.90,    0.276953,
                          96.5654,     98.9721,   0.0119942,   0.0111399,  0.00855448,    0.012159,
                          1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                        float cic4_allcuts_temp_lead[] = {  
                          6.48191,     3.25515,     3.44913,     2.15119,
                          63.7204,     11.7537,     12.9501,     3.47679,
                          6.43647,      3.5981,     3.78298,     2.10611,
                          0.0108868,   0.0100955,   0.0286898,   0.0278915,
                          0.0889167,   0.0888774,    0.090159,   0.0610415,
                          0.94,    0.309849,     0.93949,    0.287359,
                          96.3363,     1.78477,   0.0109295,   0.0110844,
                          1.5,         1.5,         1.5,         1.5 };
                        float cic4_allcuts_temp_sublead[] = {  
                          6.48191,     3.25515,     3.44913,     2.15119,
                          63.7204,     11.7537,     12.9501,     3.47679,
                          6.43647,      3.5981,     3.78298,     2.10611,
                          0.0108868,   0.0100955,   0.0286898,   0.0278915,
                          0.0889167,   0.0888774,    0.090159,   0.0610415,
                          0.94,    0.309849,     0.93949,    0.287359,
                          96.3363,     1.78477,   0.0109295,   0.0110844,
                          1.5,         1.5,         1.5,         1.5 };
                        for(int i=0;i!=ncuts*ncat_cic6;++i) { cic6_allcuts_lead[i]=cic6_allcuts_temp_lead[i];
                          cic6_allcuts_sublead[i]=cic6_allcuts_temp_sublead[i]; }
                          for(int i=0;i!=ncuts*ncat_cic4;++i) { cic4_allcuts_lead[i]=cic4_allcuts_temp_lead[i];
                            cic4_allcuts_sublead[i]=cic4_allcuts_temp_sublead[i]; }
                      } break;
    case(phoTIGHT) : {
                       float cic6_allcuts_temp_lead[] = { 
                         11.1845,     9.28445,     8.98759,     9.19055,     7.94171,     8.16991,
                         70.7835,     16.7873,     13.7361,     15.6259,     13.2407,     10.3932,
                         5.76122,     3.97439,     2.89137,     4.62749,     2.34848,      1.9302,
                         0.010781,   0.0104673,  0.00965497,   0.0284936,    0.028082,   0.0270328,
                         0.0844869,   0.0703749,    0.060775,   0.0881813,   0.0502974,    0.060877,
                         0.94,    0.90,       0.321,    0.94,    0.90,    0.279956,
                         98.9318,     98.9992,   0.0146256,   0.0207672,     34.1809,   0.0261029,
                         1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                       float cic6_allcuts_temp_sublead[] = { 
                         11.1845,     9.28445,     8.98759,     9.19055,     7.94171,     8.16991,
                         70.7835,     16.7873,     13.7361,     15.6259,     13.2407,     10.3932,
                         5.76122,     3.97439,     2.89137,     4.62749,     2.34848,      1.9302,
                         0.010781,   0.0104673,  0.00965497,   0.0284936,    0.028082,   0.0270328,
                         0.0844869,   0.0703749,    0.060775,   0.0881813,   0.0502974,    0.060877,
                         0.94,    0.90,       0.321,    0.94,    0.90,    0.279956,
                         98.9318,     98.9992,   0.0146256,   0.0207672,     34.1809,   0.0261029,
                         1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                       float cic4_allcuts_temp_lead[] = { 
                         4.62434,     2.81143,     2.50887,     1.45731,
                         62.3241,     5.45376,     7.32095,     2.48286,
                         4.76431,     2.87144,     3.78298,      1.6321,
                         0.0107094,  0.00995029,   0.0284777,    0.027055,
                         0.0865544,   0.0875894,   0.0871544,   0.0499186,
                         0.94,     0.33192,    0.94,    0.287359,
                         98.9254,     1.78477,   0.0207514,   0.0278736,
                         1.5,         1.5,         1.5,         1.5 };
                       float cic4_allcuts_temp_sublead[] = { 
                         4.62434,     2.81143,     2.50887,     1.45731,
                         62.3241,     5.45376,     7.32095,     2.48286,
                         4.76431,     2.87144,     3.78298,      1.6321,
                         0.0107094,  0.00995029,   0.0284777,    0.027055,
                         0.0865544,   0.0875894,   0.0871544,   0.0499186,
                         0.94,     0.33192,    0.94,    0.287359,
                         98.9254,     1.78477,   0.0207514,   0.0278736,
                         1.5,         1.5,         1.5,         1.5 };
                       for(int i=0;i!=ncuts*ncat_cic6;++i) { cic6_allcuts_lead[i]=cic6_allcuts_temp_lead[i];
                         cic6_allcuts_sublead[i]=cic6_allcuts_temp_sublead[i]; }
                         for(int i=0;i!=ncuts*ncat_cic4;++i) { cic4_allcuts_lead[i]=cic4_allcuts_temp_lead[i];
                           cic4_allcuts_sublead[i]=cic4_allcuts_temp_sublead[i]; }
                     } break;
    case(phoSUPERTIGHT) : {
                            float cic6_allcuts_temp_lead[] = { 
                              10.0171,     8.81037,     8.74909,     8.47393,     7.94171,     7.47883,
                              54.9366,     14.3545,     11.5208,      12.939,     10.2496,      9.7095,
                              4.11252,     3.35092,     2.49296,     2.05592,     1.67021,     1.66678,
                              0.0106315,   0.0101656,  0.00950936,   0.0283215,   0.0276216,   0.0263378,
                              0.0823828,   0.0598641,   0.0494497,   0.0706222,   0.0502974,    0.060877,
                              0.94,    0.90,       0.321,    0.94,    0.90,    0.282153,
                              98.9981,          99,   0.0216484,     96.2292,     97.1855,     96.2294,
                              1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                            float cic6_allcuts_temp_sublead[] = { 
                              10.0171,     8.81037,     8.74909,     8.47393,     7.94171,     7.47883,
                              54.9366,     14.3545,     11.5208,      12.939,     10.2496,      9.7095,
                              4.11252,     3.35092,     2.49296,     2.05592,     1.67021,     1.66678,
                              0.0106315,   0.0101656,  0.00950936,   0.0283215,   0.0276216,   0.0263378,
                              0.0823828,   0.0598641,   0.0494497,   0.0706222,   0.0502974,    0.060877,
                              0.94,    0.90,       0.321,    0.94,    0.90,    0.282153,
                              98.9981,          99,   0.0216484,     96.2292,     97.1855,     96.2294,
                              1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                            float cic4_allcuts_temp_lead[] = { 
                              3.77459,     2.18305,     1.76811,     1.30029,
                              11.5519,     3.48306,     3.87394,     1.89038,
                              3.47103,      2.1822,     2.26931,     1.43769,
                              0.0105631,  0.00974116,   0.0282572,   0.0265096,
                              0.0834648,   0.0821447,   0.0648449,   0.0476437,
                              0.94,    0.355935,    0.94,    0.316358,
                              98.9979,     1.94497,     96.2292,     96.2294,
                              1.5,         1.5,         1.5,         1.5 };
                            float cic4_allcuts_temp_sublead[] = { 
                              3.77459,     2.18305,     1.76811,     1.30029,
                              11.5519,     3.48306,     3.87394,     1.89038,
                              3.47103,      2.1822,     2.26931,     1.43769,
                              0.0105631,  0.00974116,   0.0282572,   0.0265096,
                              0.0834648,   0.0821447,   0.0648449,   0.0476437,
                              0.94,    0.355935,    0.94,    0.316358,
                              98.9979,     1.94497,     96.2292,     96.2294,
                              1.5,         1.5,         1.5,         1.5 };
                            for(int i=0;i!=ncuts*ncat_cic6;++i) { cic6_allcuts_lead[i]=cic6_allcuts_temp_lead[i];
                              cic6_allcuts_sublead[i]=cic6_allcuts_temp_sublead[i]; }
                              for(int i=0;i!=ncuts*ncat_cic4;++i) { cic4_allcuts_lead[i]=cic4_allcuts_temp_lead[i];
                                cic4_allcuts_sublead[i]=cic4_allcuts_temp_sublead[i]; }
                          } break;
    case(phoHYPERTIGHT1) : {
                             float cic6_allcuts_temp_lead[] = { 
                               9.14323,     8.13617,     7.43416,     7.97795,     5.88227,     6.60691,
                               16.4126,     10.7813,     10.1764,     11.3829,     8.63128,     8.75289,
                               3.49873,     2.93013,     2.00419,     1.60673,     1.36163,     1.36132,
                               0.0105033,  0.00999387,  0.00946607,   0.0282088,   0.0273334,   0.0256399,
                               0.0782034,   0.0598641,   0.0273668,   0.0553324,   0.0502974,   0.0465477,
                               0.94,    0.90,    0.347653,    0.94,    0.90,    0.301546,
                               98.9999,          99,     1.92089,     98.9224,     98.9492,     98.9224,
                               1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                             float cic6_allcuts_temp_sublead[] = { 
                               9.14323,     8.13617,     7.43416,     7.97795,     5.88227,     6.60691,
                               16.4126,     10.7813,     10.1764,     11.3829,     8.63128,     8.75289,
                               3.49873,     2.93013,     2.00419,     1.60673,     1.36163,     1.36132,
                               0.0105033,  0.00999387,  0.00946607,   0.0282088,   0.0273334,   0.0256399,
                               0.0782034,   0.0598641,   0.0273668,   0.0553324,   0.0502974,   0.0465477,
                               0.94,    0.90,    0.347653,    0.94,    0.90,    0.301546,
                               98.9999,          99,     1.92089,     98.9224,     98.9492,     98.9224,
                               1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                             float cic4_allcuts_temp_lead[] = { 
                               3.16373,     1.76295,     1.38862,     1.10001,
                               6.07834,      2.7333,     2.84236,    0.769064,
                               3.36688,     1.86825,     1.66995,     1.43769,
                               0.0104201,  0.00944203,    0.027691,   0.0245544,
                               0.0763125,   0.0299019,   0.0469716,   0.0433841,
                               0.94,    0.409296,    0.94,    0.342725,
                               98.9999,     96.2831,     98.9224,     98.9224,
                               1.5,         1.5,         1.5,         1.5 };
                             float cic4_allcuts_temp_sublead[] = { 
                               3.16373,     1.76295,     1.38862,     1.10001,
                               6.07834,      2.7333,     2.84236,    0.769064,
                               3.36688,     1.86825,     1.66995,     1.43769,
                               0.0104201,  0.00944203,    0.027691,   0.0245544,
                               0.0763125,   0.0299019,   0.0469716,   0.0433841,
                               0.94,    0.409296,    0.94,    0.342725,
                               98.9999,     96.2831,     98.9224,     98.9224,
                               1.5,         1.5,         1.5,         1.5 };
                             for(int i=0;i!=ncuts*ncat_cic6;++i) { cic6_allcuts_lead[i]=cic6_allcuts_temp_lead[i];
                               cic6_allcuts_sublead[i]=cic6_allcuts_temp_sublead[i]; }
                               for(int i=0;i!=ncuts*ncat_cic4;++i) { cic4_allcuts_lead[i]=cic4_allcuts_temp_lead[i];
                                 cic4_allcuts_sublead[i]=cic4_allcuts_temp_sublead[i]; }
                           } break;
    case(phoHYPERTIGHT2) : {
                             float cic6_allcuts_temp_lead[] = { 
                               8.57184,     6.64014,     6.82022,     7.13109,     5.88011,      6.2565,
                               13.4065,     10.4316,     9.18551,     9.30193,     7.51729,     7.30382,
                               2.73319,     2.93013,     1.55723,     1.54876,     1.05254,     1.36132,
                               0.0103615,  0.00978982,  0.00940152,   0.0279141,   0.0260354,   0.0241246,
                               0.0572816,   0.0232443,   0.0173437,   0.0553324,   0.0365276,   0.0465477,
                               0.94,    0.90,    0.367082,    0.94,    0.90,    0.579434,
                               99,          99,     96.2824,     98.9978,     98.9986,     98.9978,
                               1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                             float cic6_allcuts_temp_sublead[] = { 
                               8.57184,     6.64014,     6.82022,     7.13109,     5.88011,      6.2565,
                               13.4065,     10.4316,     9.18551,     9.30193,     7.51729,     7.30382,
                               2.73319,     2.93013,     1.55723,     1.54876,     1.05254,     1.36132,
                               0.0103615,  0.00978982,  0.00940152,   0.0279141,   0.0260354,   0.0241246,
                               0.0572816,   0.0232443,   0.0173437,   0.0553324,   0.0365276,   0.0465477,
                               0.94,    0.90,    0.367082,    0.94,    0.90,    0.579434,
                               99,          99,     96.2824,     98.9978,     98.9986,     98.9978,
                               1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                             float cic4_allcuts_temp_lead[] = { 
                               2.62247,     1.39105,     1.32741,    0.901527,
                               5.07275,     1.65372,     1.37996,  -0.0456764,
                               2.71641,      1.6007,     1.55087,     1.43769,
                               0.0101475,  0.00924974,   0.0273546,   0.0230396,
                               0.0484683,   0.0189641,   0.0320885,  0.00121448,
                               0.94,    0.483342,     0.94,    0.551242,
                               99,     98.9239,     98.9978,     98.9978,
                               1.5,         1.5,         1.5,         1.5 };
                             float cic4_allcuts_temp_sublead[] = { 
                               2.62247,     1.39105,     1.32741,    0.901527,
                               5.07275,     1.65372,     1.37996,  -0.0456764,
                               2.71641,      1.6007,     1.55087,     1.43769,
                               0.0101475,  0.00924974,   0.0273546,   0.0230396,
                               0.0484683,   0.0189641,   0.0320885,  0.00121448,
                               0.94,    0.483342,     0.94,    0.551242,
                               99,     98.9239,     98.9978,     98.9978,
                               1.5,         1.5,         1.5,         1.5 };
                             for(int i=0;i!=ncuts*ncat_cic6;++i) { cic6_allcuts_lead[i]=cic6_allcuts_temp_lead[i];
                               cic6_allcuts_sublead[i]=cic6_allcuts_temp_sublead[i]; }
                               for(int i=0;i!=ncuts*ncat_cic4;++i) { cic4_allcuts_lead[i]=cic4_allcuts_temp_lead[i];
                                 cic4_allcuts_sublead[i]=cic4_allcuts_temp_sublead[i]; }
                           } break;
    case(phoHYPERTIGHT3) : {
                             float cic6_allcuts_temp_lead[] = { 
                               7.97897,     6.64014,     6.60332,     5.14765,     5.02192,     5.72775,
                               11.3476,     8.93788,     8.36279,     7.88566,     5.83093,     6.66771,
                               2.348,     2.59173,     1.55158,     1.54876,     0.98618,     1.06927,
                               0.0100676,  0.00971589,  0.00932669,   0.0279141,    0.025781,   0.0229432,
                               0.0372854,   0.0215628,   0.0132992,   0.0412051,   0.0322458,   0.0465477,
                               0.94,    0.90,    0.375623,    0.94,    0.90,    0.579434,
                               99,          99,     98.9239,     98.9999,     98.9997,     98.9987,
                               1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                             float cic6_allcuts_temp_sublead[] = { 
                               7.97897,     6.64014,     6.60332,     5.14765,     5.02192,     5.72775,
                               11.3476,     8.93788,     8.36279,     7.88566,     5.83093,     6.66771,
                               2.348,     2.59173,     1.55158,     1.54876,     0.98618,     1.06927,
                               0.0100676,  0.00971589,  0.00932669,   0.0279141,    0.025781,   0.0229432,
                               0.0372854,   0.0215628,   0.0132992,   0.0412051,   0.0322458,   0.0465477,
                               0.94,    0.90,    0.375623,    0.94,    0.90,    0.579434,
                               99,          99,     98.9239,     98.9999,     98.9997,     98.9987,
                               1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                             float cic4_allcuts_temp_lead[] = { 
                               1.84128,     1.02327,      1.2096,   -0.247273,
                               3.70571,    0.948406,     1.37996,   -0.756893,
                               1.94586,     1.53214,      1.4844,    0.111795,
                               0.00990065,  0.00914404,   0.0272978,   0.0229497,
                               0.0422667,   0.0185691,    0.022612,  0.000437212,
                               0.94,    0.692359,    0.965613,    0.551242,
                               99,     98.9979,     98.9999,     98.9999,
                               1.5,         1.5,         1.5,         1.5 };
                             float cic4_allcuts_temp_sublead[] = { 
                               1.84128,     1.02327,      1.2096,   -0.247273,
                               3.70571,    0.948406,     1.37996,   -0.756893,
                               1.94586,     1.53214,      1.4844,    0.111795,
                               0.00990065,  0.00914404,   0.0272978,   0.0229497,
                               0.0422667,   0.0185691,    0.022612,  0.000437212,
                               0.94,    0.692359,    0.965613,    0.551242,
                               99,     98.9979,     98.9999,     98.9999,
                               1.5,         1.5,         1.5,         1.5 };
                             for(int i=0;i!=ncuts*ncat_cic6;++i) { cic6_allcuts_lead[i]=cic6_allcuts_temp_lead[i];
                               cic6_allcuts_sublead[i]=cic6_allcuts_temp_sublead[i]; }
                               for(int i=0;i!=ncuts*ncat_cic4;++i) { cic4_allcuts_lead[i]=cic4_allcuts_temp_lead[i];
                                 cic4_allcuts_sublead[i]=cic4_allcuts_temp_sublead[i]; }
                           } break;
    case(phoHYPERTIGHT4) : {
                             float cic6_allcuts_temp_lead[] = { 
                               6.53539,     6.07874,     5.51521,     4.78731,     5.00511,     4.90969,
                               9.30747,      8.0574,     7.70153,     7.43339,     5.44326,     6.66771,
                               1.96543,     1.78829,    0.819072,     1.54876,     0.98618,    0.255192,
                               0.0100676,  0.00919753,  0.00911379,   0.0278098,   0.0249354,   0.0221531,
                               0.03099,   0.0153957,   0.0132992,   0.0214415,   0.0322458,   0.0138186,
                               0.94,    0.90,    0.397401,    0.94,    0.90,     0.68715,
                               99,          99,     98.9979,          99,          99,     98.9987,
                               1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                             float cic6_allcuts_temp_sublead[] = { 
                               6.53539,     6.07874,     5.51521,     4.78731,     5.00511,     4.90969,
                               9.30747,      8.0574,     7.70153,     7.43339,     5.44326,     6.66771,
                               1.96543,     1.78829,    0.819072,     1.54876,     0.98618,    0.255192,
                               0.0100676,  0.00919753,  0.00911379,   0.0278098,   0.0249354,   0.0221531,
                               0.03099,   0.0153957,   0.0132992,   0.0214415,   0.0322458,   0.0138186,
                               0.94,    0.90,    0.397401,    0.94,    0.90,     0.68715,
                               99,          99,     98.9979,          99,          99,     98.9987,
                               1.5,         1.5,         1.5,         1.5,         1.5,         1.5 };
                             float cic4_allcuts_temp_lead[] = { 
                               1.30284,    0.283147,       1.155,   -0.247273,
                               1.74788,    0.657085,      1.1369,    -1.01141,
                               1.45901,    0.842083,     1.47572,    0.111795,
                               0.00976527,   0.0089837,   0.0257602,   0.0229497,
                               0.0365257,  0.000519817,   0.0197826,  1.22391e-05,
                               0.94,    0.692359,    0.967428,    0.551242,
                               99,     98.9999,          99,          99,
                               1.5,         1.5,         1.5,         1.5 };
                             float cic4_allcuts_temp_sublead[] = { 
                               1.30284,    0.283147,       1.155,   -0.247273,
                               1.74788,    0.657085,      1.1369,    -1.01141,
                               1.45901,    0.842083,     1.47572,    0.111795,
                               0.00976527,   0.0089837,   0.0257602,   0.0229497,
                               0.0365257,  0.000519817,   0.0197826,  1.22391e-05,
                               0.94,    0.692359,    0.967428,    0.551242,
                               99,     98.9999,          99,          99,
                               1.5,         1.5,         1.5,         1.5 };
                             for(int i=0;i!=ncuts*ncat_cic6;++i) { cic6_allcuts_lead[i]=cic6_allcuts_temp_lead[i];
                               cic6_allcuts_sublead[i]=cic6_allcuts_temp_sublead[i]; }
                               for(int i=0;i!=ncuts*ncat_cic4;++i) { cic4_allcuts_lead[i]=cic4_allcuts_temp_lead[i];
                                 cic4_allcuts_sublead[i]=cic4_allcuts_temp_sublead[i]; }
                           } break;
    default:std::cout << "UNKNOWN phoCiCIDLevel: " << cutlevel << std::endl;

  }
}



// ---------------------------------------------------------------------------------------------------------------------------------------------
std::pair<int,int> LoopAll::DiphotonCiCSelection( phoCiCIDLevel LEADCUTLEVEL, phoCiCIDLevel SUBLEADCUTLEVEL, Float_t leadPtMin, Float_t subleadPtMin, int ncategories, bool applyPtoverM) {

  //rho=0;// CAUTION SETTING RHO TO 0 FOR 2010 DATA FILES (RHO ISN'T IN THESE FILES)
  int selected_lead_index = -1;
  int selected_sublead_index = -1;
  float selected_lead_pt = -1;
  float selected_sublead_pt = -1;

  std::vector<std::vector<bool> > ph_passcut;
  for(int ipho=0;ipho!=pho_n;++ipho) {
    TLorentzVector * iphop4 = (TLorentzVector*)pho_p4->At(ipho);
    if(iphop4->Et() < leadPtMin || fabs(iphop4->Eta()) > 2.5)continue;

    if(PhotonCiCSelectionLevel(ipho, ph_passcut, ncategories, 0) < LEADCUTLEVEL)continue;

    for(int iipho=0;iipho!=pho_n;++iipho) {
      if(iipho == ipho)continue;
      TLorentzVector * iiphop4 = (TLorentzVector*)pho_p4->At(iipho);
      if(iiphop4->Et() < subleadPtMin || fabs(iiphop4->Eta()) > 2.5)continue;
      if(iiphop4->Et() > iphop4->Et())continue;
      float m_gamgam = (*iphop4+*iiphop4).M();
      float L_ptom = iphop4->Et()/m_gamgam;
      float S_ptom = iiphop4->Et()/m_gamgam;
      if(applyPtoverM && (L_ptom < 0.33 || S_ptom<0.25))continue;

      if(PhotonCiCSelectionLevel(iipho, ph_passcut, ncategories, 1) < SUBLEADCUTLEVEL )continue;
      // if here, diphoton passed all cuts.
      //std::cout << "FOUND DIPHOTON" << std::endl;
      if( (iphop4->Et()>selected_lead_pt) || (ipho==selected_lead_index&&iiphop4->Et()>selected_sublead_pt) ) {
	      selected_lead_pt = iphop4->Et();
	      selected_sublead_pt = iiphop4->Et();
	      selected_lead_index = ipho;
	      selected_sublead_index = iipho;
      }
      
    }// end photon loop (iipho), aka sublead
  }// end photon loop (ipho), aka lead

  std::pair<int,int> dipho_inds(selected_lead_index,selected_sublead_index);
  return dipho_inds;

}


// ---------------------------------------------------------------------------------------------------------------------------------------------
int LoopAll::PhotonCiCSelectionLevel( int photon_index, std::vector<std::vector<bool> > & ph_passcut, int ncategories, int doSublead) {

  int cutlevelpassed = -1;

  int n_r9_categories = -1;
  int n_eta_categories = -1;
  if(ncategories==6) {
    n_r9_categories = 3;
    n_eta_categories = 2;
  } else if(ncategories==4) {
    n_r9_categories = 2;
    n_eta_categories = 2;
  } else {
    std::cout << "UNKNOWN ncategories must be 4 or 6, not " << ncategories << std::endl;
  }
  int photon_category = PhotonCategory(photon_index,n_r9_categories,n_eta_categories);

  TLorentzVector * phop4 = (TLorentzVector*)pho_p4->At(photon_index);

  float val_tkiso = (*pho_tkiso_recvtx_030_002_0000_10_01)[photon_index][vtx_std_sel];
  float val_ecaliso = pho_ecalsumetconedr03[photon_index];
  float val_hcaliso = pho_hcalsumetconedr04[photon_index];
  float val_ecalisobad = pho_ecalsumetconedr04[photon_index];
  float val_hcalisobad = pho_hcalsumetconedr04[photon_index];
  float val_tkisobad = pho_tkiso_badvtx_040_002_0000_10_01[photon_index];
  float val_sieie = pho_sieie[photon_index];
  float val_hoe = pho_hoe[photon_index];
  float val_r9 = pho_r9[photon_index];
  float val_drtotk_25_99 = pho_drtotk_25_99[photon_index];
  float val_pixel = (float)pho_haspixseed[photon_index];

  float isosumconst = 5.;
  float isosumconstbad = 7.;
  if(ncategories==4) {
    isosumconst = 0.;
    isosumconstbad = 0.;
  }

  float rhofacbad=0.40, rhofac=0.05;
  float val_isosumoet=(val_tkiso+val_ecaliso+val_hcaliso+isosumconst-rho*rhofac)*50./phop4->Et();
  float val_isosumoetbad=(val_tkisobad+val_ecalisobad+val_hcalisobad+isosumconstbad-rho*rhofacbad)*50./phop4->Et();
  float val_trkisooet=(val_tkiso)*50./phop4->Et();

  ph_passcut.clear();
  ph_passcut.resize(phoNCUTLEVELS,std::vector<bool>(8,true) );
  if(!doSublead) {
    for(int iCUTLEVEL=0;iCUTLEVEL!=(int)phoNCUTLEVELS;++iCUTLEVEL) {
      switch(ncategories) {
        case(6) :
          ph_passcut[iCUTLEVEL][0] = (val_isosumoet        <=   cic6_cut_lead_isosumoet[iCUTLEVEL][photon_category]     );
          ph_passcut[iCUTLEVEL][1] = (val_isosumoetbad     <=   cic6_cut_lead_isosumoetbad[iCUTLEVEL][photon_category]  );
          ph_passcut[iCUTLEVEL][2] = (val_trkisooet        <=   cic6_cut_lead_trkisooet[iCUTLEVEL][photon_category]     );
          ph_passcut[iCUTLEVEL][3] = (val_sieie            <=   cic6_cut_lead_sieie[iCUTLEVEL][photon_category]         );
          ph_passcut[iCUTLEVEL][4] = (val_hoe              <=   cic6_cut_lead_hovere[iCUTLEVEL][photon_category]        );
          ph_passcut[iCUTLEVEL][5] = (val_r9             >=     cic6_cut_lead_r9[iCUTLEVEL][photon_category]            );// gt cut
          ph_passcut[iCUTLEVEL][6] = (val_drtotk_25_99   >=     cic6_cut_lead_drtotk_25_99[iCUTLEVEL][photon_category]  );// gt cut
          ph_passcut[iCUTLEVEL][7] = (val_pixel            <=   cic6_cut_lead_pixel[iCUTLEVEL][photon_category]         );
          break;
        case(4) :
          ph_passcut[iCUTLEVEL][0] = (val_isosumoet        <=   cic4_cut_lead_isosumoet[iCUTLEVEL][photon_category]     );
          ph_passcut[iCUTLEVEL][1] = (val_isosumoetbad     <=   cic4_cut_lead_isosumoetbad[iCUTLEVEL][photon_category]  );
          ph_passcut[iCUTLEVEL][2] = (val_trkisooet        <=   cic4_cut_lead_trkisooet[iCUTLEVEL][photon_category]     );
          ph_passcut[iCUTLEVEL][3] = (val_sieie            <=   cic4_cut_lead_sieie[iCUTLEVEL][photon_category]         );
          ph_passcut[iCUTLEVEL][4] = (val_hoe              <=   cic4_cut_lead_hovere[iCUTLEVEL][photon_category]        );
          ph_passcut[iCUTLEVEL][5] = (val_r9             >=     cic4_cut_lead_r9[iCUTLEVEL][photon_category]            );// gt cut
          ph_passcut[iCUTLEVEL][6] = (val_drtotk_25_99   >=     cic4_cut_lead_drtotk_25_99[iCUTLEVEL][photon_category]  );// gt cut
          ph_passcut[iCUTLEVEL][7] = (val_pixel            <=   cic4_cut_lead_pixel[iCUTLEVEL][photon_category]         );
          break;
      }
      bool ph_passcut_all = true;
      for(int icut=0;icut!=8;++icut) {
	ph_passcut_all = ph_passcut_all && ph_passcut[iCUTLEVEL][icut];
      }
      if(ph_passcut_all) {
	if( cutlevelpassed != iCUTLEVEL - 1 ) {
	  std::cerr << "photon " << photon_index << " (category " << photon_category << ") in run/event " << run << "/" << event << " passed CiC cut level " 
		    << iCUTLEVEL << " but not "  << iCUTLEVEL - 1 << ". Did you load your cut values correctly? "<< std::endl;
	  /// assert( 0 );
	}
	cutlevelpassed=iCUTLEVEL;
      }
    }
  } else if(doSublead) {
    for(int iCUTLEVEL=0;iCUTLEVEL!=(int)phoNCUTLEVELS;++iCUTLEVEL) {
      switch(ncategories) {
        case(6) :
          ph_passcut[iCUTLEVEL][0] = (val_isosumoet        <=   cic6_cut_sublead_isosumoet[iCUTLEVEL][photon_category]     );
          ph_passcut[iCUTLEVEL][1] = (val_isosumoetbad     <=   cic6_cut_sublead_isosumoetbad[iCUTLEVEL][photon_category]  );
          ph_passcut[iCUTLEVEL][2] = (val_trkisooet        <=   cic6_cut_sublead_trkisooet[iCUTLEVEL][photon_category]     );
          ph_passcut[iCUTLEVEL][3] = (val_sieie            <=   cic6_cut_sublead_sieie[iCUTLEVEL][photon_category]         );
          ph_passcut[iCUTLEVEL][4] = (val_hoe              <=   cic6_cut_sublead_hovere[iCUTLEVEL][photon_category]        );
          ph_passcut[iCUTLEVEL][5] = (val_r9             >=     cic6_cut_sublead_r9[iCUTLEVEL][photon_category]            );// gt cut
          ph_passcut[iCUTLEVEL][6] = (val_drtotk_25_99   >=     cic6_cut_sublead_drtotk_25_99[iCUTLEVEL][photon_category]  );// gt cut
          ph_passcut[iCUTLEVEL][7] = (val_pixel            <=   cic6_cut_sublead_pixel[iCUTLEVEL][photon_category]         );
          break;
        case(4) :
          ph_passcut[iCUTLEVEL][0] = (val_isosumoet        <=   cic4_cut_sublead_isosumoet[iCUTLEVEL][photon_category]     );
          ph_passcut[iCUTLEVEL][1] = (val_isosumoetbad     <=   cic4_cut_sublead_isosumoetbad[iCUTLEVEL][photon_category]  );
          ph_passcut[iCUTLEVEL][2] = (val_trkisooet        <=   cic4_cut_sublead_trkisooet[iCUTLEVEL][photon_category]     );
          ph_passcut[iCUTLEVEL][3] = (val_sieie            <=   cic4_cut_sublead_sieie[iCUTLEVEL][photon_category]         );
          ph_passcut[iCUTLEVEL][4] = (val_hoe              <=   cic4_cut_sublead_hovere[iCUTLEVEL][photon_category]        );
          ph_passcut[iCUTLEVEL][5] = (val_r9             >=     cic4_cut_sublead_r9[iCUTLEVEL][photon_category]            );// gt cut
          ph_passcut[iCUTLEVEL][6] = (val_drtotk_25_99   >=     cic4_cut_sublead_drtotk_25_99[iCUTLEVEL][photon_category]  );// gt cut
          ph_passcut[iCUTLEVEL][7] = (val_pixel            <=   cic4_cut_sublead_pixel[iCUTLEVEL][photon_category]         );
          break;
      }
      bool ph_passcut_all = true;
      for(int icut=0;icut!=8;++icut) {
	ph_passcut_all = ph_passcut_all && ph_passcut[iCUTLEVEL][icut];
      }
      if(ph_passcut_all) {
	if( cutlevelpassed != iCUTLEVEL - 1 ) {
	  std::cerr << "photon " << photon_index << " (category " << photon_category << ") in run/event " << run << "/" << event << " passed CiC cut level " 
		    << iCUTLEVEL << " but not "  << iCUTLEVEL - 1 << ". Did you load your cut values correctly? " << std::endl;
	  //// assert( 0 );
	}
	cutlevelpassed=iCUTLEVEL;
      }
    }
  }

  return cutlevelpassed;

}


// ---------------------------------------------------------------------------------------------------------------------------------------------
Float_t LoopAll::DeltaRToTrack(Int_t photonind, Int_t vtxind, Float_t PtMin, Float_t dzmax, Float_t dxymax, int maxlosthits){
  if(LDEBUG)std::cout << "DeltaRToTrack BEGIN" << std::endl;
  int elind = -1;
  float eldr = 99.;
  TLorentzVector * photon_p4 = (TLorentzVector*)pho_p4->At(photonind);
  TVector3 * photon_calopos = (TVector3*)pho_calopos->At(photonind);
  for(int iel=0;iel!=el_std_n;++iel) {
    if(el_std_hp_expin[iel]>maxlosthits)continue;
    if(el_std_scind[iel] == pho_scind[photonind]) {
      elind = iel;
      break;
    }
  }
  if(elind >= 0)
    eldr = sqrt(el_std_detain[elind]*el_std_detain[elind] + el_std_dphiin[elind]*el_std_dphiin[elind]);
  return eldr;
}


// ---------------------------------------------------------------------------------------------------------------------------------------------
Float_t LoopAll::IsoEcalHitsSumEtNumCrystal( TVector3 *calopos, Float_t innerConeDR, Float_t outerConeDR, Float_t stripEtaHalfWidth, Float_t stripHalfLength) {
  if(LDEBUG)std::cout << "IsoEcalHitsSumEtNumCrystal begin" << std::endl;
  Float_t ecalhitsSumEt=0.;
  for(int i=0; i!= ecalhit_n; ++i) {
    //if(ecalhit_type[i]==2)continue;// don't use preshower (still want this?? - crashes without it)
    TLorentzVector * rechitp4 = (TLorentzVector *) ecalhit_p4->At(i);
    Float_t etaclus = calopos->Eta();
    Float_t phiclus = calopos->Phi();
    Float_t eta = rechitp4->Eta();
    Float_t phi = rechitp4->Phi();
    Float_t etaDiff = eta - etaclus;
    Float_t phiDiff = phi - phiclus;
    if(phiDiff>TMath::Pi())phiDiff = TMath::TwoPi() - phiDiff;
    Float_t deltaR = calopos->DeltaR(rechitp4->Vect());
    Float_t deltaEta = fabs(rechitp4->Eta() - calopos->Eta());
    if(sqrt(etaDiff*etaDiff + phiDiff*phiDiff)>outerConeDR)continue;
    if(fabs(etaclus) < 1.479) {
      if(fabs(etaDiff) < 0.0174*stripEtaHalfWidth)continue;
      if(sqrt(etaDiff*etaDiff + phiDiff*phiDiff) < 0.0174*innerConeDR)continue;
    } else {
      if(fabs(etaDiff) < 0.00864*fabs(sinh(eta))*stripEtaHalfWidth)continue;
      if(sqrt(etaDiff*etaDiff + phiDiff*phiDiff) < 0.00864*fabs(sinh(eta))*innerConeDR)continue;
    }
    if(fabs(rechitp4->E())>0.08 && fabs(rechitp4->Et()) > 0.)ecalhitsSumEt+=rechitp4->Et();
  }
  if(LDEBUG)std::cout << "IsoEcalHitsSumEtNumCrystal end" << std::endl;
  return ecalhitsSumEt;
}


// ---------------------------------------------------------------------------------------------------------------------------------------------
Float_t LoopAll::WorstSumTrackPtInCone(TLorentzVector *photon_p4, Int_t returnvtxind, Float_t PtMin, Float_t OuterConeRadius, Float_t InnerConeRadius, Float_t EtaStripHalfWidth, Float_t dzmax, Float_t dxymax) {

  Int_t worstvtxind = -1;
  Float_t maxisosum = -100;
  for(int ivtx=0;ivtx!=vtx_std_n;++ivtx) {
    Float_t thisvtxisosum = SumTrackPtInCone(photon_p4, ivtx, PtMin, OuterConeRadius, InnerConeRadius, EtaStripHalfWidth, dzmax, dxymax);
    if(thisvtxisosum > maxisosum) {
      maxisosum = thisvtxisosum;
      worstvtxind = ivtx;
    }
  }

  if(returnvtxind == 1) {
    return 0.5+(float)worstvtxind;
  } else {
    return maxisosum;
  }

}


// ---------------------------------------------------------------------------------------------------------------------------------------------
Float_t LoopAll::SumTrackPtInCone(TLorentzVector *photon_p4, Int_t vtxind, Float_t PtMin, Float_t OuterConeRadius, Float_t InnerConeRadius, Float_t EtaStripHalfWidth, Float_t dzmax, Float_t dxymax) {
  // TRACKER Isolation
  if(vtxind<0)return -99;
  TVector3 * vtxpos= (TVector3 *) vtx_std_xyz->At(vtxind);
  float SumTrackPt=0;
  for(unsigned int itk=0; itk!=tk_n; itk++) {
    TLorentzVector * tkp4= (TLorentzVector *) tk_p4->At(itk);
    if(tkp4->Pt() < PtMin)continue;
    TVector3 * tkpos= (TVector3 *) tk_vtx_pos->At(itk);
    double deltaz = fabs(vtxpos->Z() - tkpos->Z());
    if(deltaz > dzmax)continue;
    double dxy = ( -(tkpos->X() - vtxpos->X())*tkp4->Py() + (tkpos->Y() - vtxpos->Y())*tkp4->Px()) / tkp4->Pt();
    if(fabs(dxy) > dxymax)continue;
    double tk_eta = tkp4->Eta();
    double tk_phi = tkp4->Phi();
    double deta = fabs(photon_p4->Eta() - tk_eta);
    double dphi = fabs(photon_p4->Phi() - tk_phi);
    if(dphi > TMath::Pi())dphi = TMath::TwoPi() - dphi;
    double deltaR = sqrt(deta*deta + dphi*dphi);
    if(deltaR < OuterConeRadius && deltaR >= InnerConeRadius && deta >= EtaStripHalfWidth)SumTrackPt+=tkp4->Pt();
  }
  return SumTrackPt;
}


// CiC SELECTION CODE END - SSIMON

//
// Generate dictionary entries for branches from GeneralFunctions_h 
//
// ---------------------------------------------------------------------------------------------------------------------------------------------
void LoopAll::DefineUserBranches() 
{
#ifndef __CINT__

	BRANCH_DICT(gv_n  );
	BRANCH_DICT(gv_pos);
	BRANCH_DICT(pu_n);
	BRANCH_DICT(pu_zpos);
	BRANCH_DICT(pu_sumpt_lowpt);
	BRANCH_DICT(pu_sumpt_highpt);
	BRANCH_DICT(pu_ntrks_lowpt);
	BRANCH_DICT(pu_ntrks_highpt);
	
	BRANCH_DICT(rho);
	
	BRANCH_DICT(vtx_std_sel);
	BRANCH_DICT(vtx_std_ranked_list);

	BRANCH_DICT(pho_tkiso_recvtx_030_002_0000_10_01);
	BRANCH_DICT(pho_tkiso_badvtx_040_002_0000_10_01);
	BRANCH_DICT(pho_drtotk_25_99);

	BRANCH_DICT(pho_cic6cutlevel_lead);
	BRANCH_DICT(pho_cic6passcuts_lead);
	BRANCH_DICT(pho_cic6cutlevel_sublead);
	BRANCH_DICT(pho_cic6passcuts_sublead);

	BRANCH_DICT(pho_matchingConv);

  //BRANCH_DICT(dipho_n);
  //BRANCH_DICT(dipho_leadind);
  //BRANCH_DICT(dipho_subleadind);
  //BRANCH_DICT(dipho_vtxind);
  //BRANCH_DICT(dipho_leadet);
  //BRANCH_DICT(dipho_subleadet);
  //BRANCH_DICT(dipho_leadeta);
  //BRANCH_DICT(dipho_subleadeta);
  //BRANCH_DICT(dipho_leadci6cindex);
  //BRANCH_DICT(dipho_subleadci6cindex);
  //BRANCH_DICT(dipho_leadci4cindex);
  //BRANCH_DICT(dipho_subleadci4cindex);
  //BRANCH_DICT(dipho_mass);
  //BRANCH_DICT(dipho_pt);
  //BRANCH_DICT(dipho_eta);
  //BRANCH_DICT(dipho_phi);
  //BRANCH_DICT(dipho_cts);

	BRANCH_DICT(pho_matchingConv);

#endif
}
