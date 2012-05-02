   b_event = fChain->GetBranch("event"); 
   b_run = fChain->GetBranch("run"); 
   b_process_id = fChain->GetBranch("process_id"); 
   b_lumis = fChain->GetBranch("lumis"); 
   b_bx = fChain->GetBranch("bx"); 
   b_pthat = fChain->GetBranch("pthat"); 
   b_weight = fChain->GetBranch("weight"); 
   b_ct_n = fChain->GetBranch("ct_n"); 
   b_ct_p4 = fChain->GetBranch("ct_p4"); 
   b_ct_emEnergy = fChain->GetBranch("ct_emEnergy"); 
   b_ct_hadEnergy = fChain->GetBranch("ct_hadEnergy"); 
   b_ct_outerEnergy = fChain->GetBranch("ct_outerEnergy"); 
   b_ct_emL1 = fChain->GetBranch("ct_emL1"); 
   b_ct_hadL1 = fChain->GetBranch("ct_hadL1"); 
   b_ct_size = fChain->GetBranch("ct_size"); 
   b_sc_islbar_n = fChain->GetBranch("sc_islbar_n"); 
   b_sc_islbar_p4 = fChain->GetBranch("sc_islbar_p4"); 
   b_sc_islbar_xyz = fChain->GetBranch("sc_islbar_xyz"); 
   b_sc_islbar_raw = fChain->GetBranch("sc_islbar_raw"); 
   b_sc_islbar_nbc = fChain->GetBranch("sc_islbar_nbc"); 
   b_sc_islbar_seedenergy = fChain->GetBranch("sc_islbar_seedenergy"); 
   b_sc_islbar_bcseedind = fChain->GetBranch("sc_islbar_bcseedind"); 
   b_sc_islbar_bcind = fChain->GetBranch("sc_islbar_bcind"); 
   b_sc_n = fChain->GetBranch("sc_n"); 
   b_sc_islend_n = fChain->GetBranch("sc_islend_n"); 
   b_sc_hybrid_n = fChain->GetBranch("sc_hybrid_n"); 
   b_sc_p4 = fChain->GetBranch("sc_p4"); 
   b_sc_xyz = fChain->GetBranch("sc_xyz"); 
   b_sc_pre = fChain->GetBranch("sc_pre"); 
   b_sc_raw = fChain->GetBranch("sc_raw"); 
   b_sc_barrel = fChain->GetBranch("sc_barrel"); 
   b_sc_2xN = fChain->GetBranch("sc_2xN"); 
   b_sc_5xN = fChain->GetBranch("sc_5xN"); 
   b_sc_sieie = fChain->GetBranch("sc_sieie"); 
   b_sc_nbc = fChain->GetBranch("sc_nbc"); 
   b_sc_bcseedind = fChain->GetBranch("sc_bcseedind"); 
   b_sc_bcind = fChain->GetBranch("sc_bcind"); 
   b_sc_seta = fChain->GetBranch("sc_seta"); 
   b_sc_sphi = fChain->GetBranch("sc_sphi"); 
   b_bc_n = fChain->GetBranch("bc_n"); 
   b_bc_islbar_n = fChain->GetBranch("bc_islbar_n"); 
   b_bc_islend_n = fChain->GetBranch("bc_islend_n"); 
   b_bc_hybrid_n = fChain->GetBranch("bc_hybrid_n"); 
   b_bc_p4 = fChain->GetBranch("bc_p4"); 
   b_bc_xyz = fChain->GetBranch("bc_xyz"); 
   b_bc_nhits = fChain->GetBranch("bc_nhits"); 
   b_bc_s1 = fChain->GetBranch("bc_s1"); 
   b_bc_rook = fChain->GetBranch("bc_rook"); 
   b_bc_chx = fChain->GetBranch("bc_chx"); 
   b_bc_s4 = fChain->GetBranch("bc_s4"); 
   b_bc_s9 = fChain->GetBranch("bc_s9"); 
   b_bc_s25 = fChain->GetBranch("bc_s25"); 
   b_bc_spp = fChain->GetBranch("bc_spp"); 
   b_bc_see = fChain->GetBranch("bc_see"); 
   b_bc_sep = fChain->GetBranch("bc_sep"); 
   b_bc_type = fChain->GetBranch("bc_type"); 
   b_bc_sieie = fChain->GetBranch("bc_sieie"); 
   b_bc_seed = fChain->GetBranch("bc_seed"); 
   b_ecalhit_n = fChain->GetBranch("ecalhit_n"); 
   b_ecalhit_type = fChain->GetBranch("ecalhit_type"); 
   b_ecalhit_flag = fChain->GetBranch("ecalhit_flag"); 
   b_ecalhit_time = fChain->GetBranch("ecalhit_time"); 
   b_ecalhit_detid = fChain->GetBranch("ecalhit_detid"); 
   b_ecalhit_p4 = fChain->GetBranch("ecalhit_p4"); 
   b_el_std_n = fChain->GetBranch("el_std_n"); 
   b_el_std_sc = fChain->GetBranch("el_std_sc"); 
   b_el_std_p4 = fChain->GetBranch("el_std_p4"); 
   b_el_std_momvtx = fChain->GetBranch("el_std_momvtx"); 
   b_el_std_momvtxconst = fChain->GetBranch("el_std_momvtxconst"); 
   b_el_std_momcalo = fChain->GetBranch("el_std_momcalo"); 
   b_el_std_momout = fChain->GetBranch("el_std_momout"); 
   b_el_std_posvtx = fChain->GetBranch("el_std_posvtx"); 
   b_el_std_poscalo = fChain->GetBranch("el_std_poscalo"); 
   b_el_std_eopin = fChain->GetBranch("el_std_eopin"); 
   b_el_std_eseedopout = fChain->GetBranch("el_std_eseedopout"); 
   b_el_std_pout = fChain->GetBranch("el_std_pout"); 
   b_el_std_pin = fChain->GetBranch("el_std_pin"); 
   b_el_std_e1x5 = fChain->GetBranch("el_std_e1x5"); 
   b_el_std_e5x5 = fChain->GetBranch("el_std_e5x5"); 
   b_el_std_e2x5 = fChain->GetBranch("el_std_e2x5"); 
   b_el_std_spp = fChain->GetBranch("el_std_spp"); 
   b_el_std_see = fChain->GetBranch("el_std_see"); 
   b_el_std_sieie = fChain->GetBranch("el_std_sieie"); 
   b_el_std_sieiesc = fChain->GetBranch("el_std_sieiesc"); 
   b_el_std_eseed = fChain->GetBranch("el_std_eseed"); 
   b_el_std_enearbcopin = fChain->GetBranch("el_std_enearbcopin"); 
   b_el_std_eseedopin = fChain->GetBranch("el_std_eseedopin"); 
   b_el_std_fbrem = fChain->GetBranch("el_std_fbrem"); 
   b_el_std_nbrem = fChain->GetBranch("el_std_nbrem"); 
   b_el_std_hoe = fChain->GetBranch("el_std_hoe"); 
   b_el_std_hoed1 = fChain->GetBranch("el_std_hoed1"); 
   b_el_std_hoed2 = fChain->GetBranch("el_std_hoed2"); 
   b_el_std_detain = fChain->GetBranch("el_std_detain"); 
   b_el_std_dphiin = fChain->GetBranch("el_std_dphiin"); 
   b_el_std_detaout = fChain->GetBranch("el_std_detaout"); 
   b_el_std_dphiout = fChain->GetBranch("el_std_dphiout"); 
   b_el_std_class = fChain->GetBranch("el_std_class"); 
   b_el_std_crack = fChain->GetBranch("el_std_crack"); 
   b_el_std_nambtk = fChain->GetBranch("el_std_nambtk"); 
   b_el_std_scind = fChain->GetBranch("el_std_scind"); 
   b_el_std_z0 = fChain->GetBranch("el_std_z0"); 
   b_el_std_d0 = fChain->GetBranch("el_std_d0"); 
   b_el_std_D0Vtx = fChain->GetBranch("el_std_D0Vtx"); 
   b_el_std_DZVtx = fChain->GetBranch("el_std_DZVtx"); 
   b_el_std_qoverperr = fChain->GetBranch("el_std_qoverperr"); 
   b_el_std_pterr = fChain->GetBranch("el_std_pterr"); 
   b_el_std_etaerr = fChain->GetBranch("el_std_etaerr"); 
   b_el_std_phierr = fChain->GetBranch("el_std_phierr"); 
   b_el_std_z0err = fChain->GetBranch("el_std_z0err"); 
   b_el_std_d0err = fChain->GetBranch("el_std_d0err"); 
   b_el_std_chi2 = fChain->GetBranch("el_std_chi2"); 
   b_el_std_dof = fChain->GetBranch("el_std_dof"); 
   b_el_std_mva = fChain->GetBranch("el_std_mva"); 
   b_el_std_ch_gsf = fChain->GetBranch("el_std_ch_gsf"); 
   b_el_std_ch_scpix = fChain->GetBranch("el_std_ch_scpix"); 
   b_el_std_charge = fChain->GetBranch("el_std_charge"); 
   b_el_std_losthits = fChain->GetBranch("el_std_losthits"); 
   b_el_std_validhits = fChain->GetBranch("el_std_validhits"); 
   b_el_std_hp_expin = fChain->GetBranch("el_std_hp_expin"); 
   b_el_std_hp_expout = fChain->GetBranch("el_std_hp_expout"); 
   b_el_std_rohighe = fChain->GetBranch("el_std_rohighe"); 
   b_el_std_roloose = fChain->GetBranch("el_std_roloose"); 
   b_el_std_rotight = fChain->GetBranch("el_std_rotight"); 
   b_el_std_loose = fChain->GetBranch("el_std_loose"); 
   b_el_std_tight = fChain->GetBranch("el_std_tight"); 
   b_el_std_catbased = fChain->GetBranch("el_std_catbased"); 
   b_el_std_tkind = fChain->GetBranch("el_std_tkind"); 
   b_el_std_hcaliso03 = fChain->GetBranch("el_std_hcaliso03"); 
   b_el_std_ecaliso03 = fChain->GetBranch("el_std_ecaliso03"); 
   b_el_std_tkiso03 = fChain->GetBranch("el_std_tkiso03"); 
   b_el_std_hcaliso04 = fChain->GetBranch("el_std_hcaliso04"); 
   b_el_std_ecaliso04 = fChain->GetBranch("el_std_ecaliso04"); 
   b_el_std_tkiso04 = fChain->GetBranch("el_std_tkiso04"); 
   b_el_std_tkdrv = fChain->GetBranch("el_std_tkdrv"); 
   b_el_std_ecaldrv = fChain->GetBranch("el_std_ecaldrv"); 
   b_el_std_ip_ctf = fChain->GetBranch("el_std_ip_ctf"); 
   b_el_std_ip_gsf = fChain->GetBranch("el_std_ip_gsf"); 
   b_el_std_dist = fChain->GetBranch("el_std_dist"); 
   b_el_std_dcot = fChain->GetBranch("el_std_dcot"); 
   b_el_std_1pxb = fChain->GetBranch("el_std_1pxb"); 
   b_el_std_1pxf = fChain->GetBranch("el_std_1pxf"); 
   b_gp_n = fChain->GetBranch("gp_n"); 
   b_gp_p4 = fChain->GetBranch("gp_p4"); 
   b_gp_vtx = fChain->GetBranch("gp_vtx"); 
   b_gp_status = fChain->GetBranch("gp_status"); 
   b_gp_pdgid = fChain->GetBranch("gp_pdgid"); 
   b_gp_mother = fChain->GetBranch("gp_mother"); 
   b_genjet_algo1_n = fChain->GetBranch("genjet_algo1_n"); 
   b_genjet_algo1_p4 = fChain->GetBranch("genjet_algo1_p4"); 
   b_genjet_algo1_em = fChain->GetBranch("genjet_algo1_em"); 
   b_genjet_algo1_had = fChain->GetBranch("genjet_algo1_had"); 
   b_genjet_algo1_inv = fChain->GetBranch("genjet_algo1_inv"); 
   b_genjet_algo1_aux = fChain->GetBranch("genjet_algo1_aux"); 
   b_genjet_algo2_n = fChain->GetBranch("genjet_algo2_n"); 
   b_genjet_algo2_p4 = fChain->GetBranch("genjet_algo2_p4"); 
   b_genjet_algo2_em = fChain->GetBranch("genjet_algo2_em"); 
   b_genjet_algo2_had = fChain->GetBranch("genjet_algo2_had"); 
   b_genjet_algo2_inv = fChain->GetBranch("genjet_algo2_inv"); 
   b_genjet_algo2_aux = fChain->GetBranch("genjet_algo2_aux"); 
   b_genjet_algo3_n = fChain->GetBranch("genjet_algo3_n"); 
   b_genjet_algo3_p4 = fChain->GetBranch("genjet_algo3_p4"); 
   b_genjet_algo3_em = fChain->GetBranch("genjet_algo3_em"); 
   b_genjet_algo3_had = fChain->GetBranch("genjet_algo3_had"); 
   b_genjet_algo3_inv = fChain->GetBranch("genjet_algo3_inv"); 
   b_genjet_algo3_aux = fChain->GetBranch("genjet_algo3_aux"); 
   b_hc_p4 = fChain->GetBranch("hc_p4"); 
   b_hc_n = fChain->GetBranch("hc_n"); 
   b_hc_type = fChain->GetBranch("hc_type"); 
   b_hlt_bit = fChain->GetBranch("hlt_bit"); 
   b_hlt_path_names_HLT = fChain->GetBranch("hlt_path_names_HLT"); 
   b_hlt1_bit = fChain->GetBranch("hlt1_bit"); 
   b_hlt_path_names_HLT1 = fChain->GetBranch("hlt_path_names_HLT1"); 
   b_hlt2_bit = fChain->GetBranch("hlt2_bit"); 
   b_hlt_path_names_HLT2 = fChain->GetBranch("hlt_path_names_HLT2"); 
   b_hlt_n = fChain->GetBranch("hlt_n"); 
   b_hlt_p4 = fChain->GetBranch("hlt_p4"); 
   b_hlt_candpath = fChain->GetBranch("hlt_candpath"); 
   b_ht_25 = fChain->GetBranch("ht_25"); 
   b_ht_35 = fChain->GetBranch("ht_35"); 
   b_ht_50 = fChain->GetBranch("ht_50"); 
   b_rho_algo1 = fChain->GetBranch("rho_algo1"); 
   b_rho_algo2 = fChain->GetBranch("rho_algo2"); 
   b_rho_algo3 = fChain->GetBranch("rho_algo3"); 
   b_ht_nomet25 = fChain->GetBranch("ht_nomet25"); 
   b_ht_nomet35 = fChain->GetBranch("ht_nomet35"); 
   b_ht_nomet50 = fChain->GetBranch("ht_nomet50"); 
   b_ht_trk = fChain->GetBranch("ht_trk"); 
   b_ht_trkvec = fChain->GetBranch("ht_trkvec"); 
   b_ht_2lpt_n = fChain->GetBranch("ht_2lpt_n"); 
   b_ht_2lpt_inds = fChain->GetBranch("ht_2lpt_inds"); 
   b_ht_4lpt_n = fChain->GetBranch("ht_4lpt_n"); 
   b_ht_4lpt_inds = fChain->GetBranch("ht_4lpt_inds"); 
   b_ht_2lpt25 = fChain->GetBranch("ht_2lpt25"); 
   b_ht_2lpt35 = fChain->GetBranch("ht_2lpt35"); 
   b_ht_2lpt50 = fChain->GetBranch("ht_2lpt50"); 
   b_ht_4lpt25 = fChain->GetBranch("ht_4lpt25"); 
   b_ht_4lpt35 = fChain->GetBranch("ht_4lpt35"); 
   b_ht_4lpt50 = fChain->GetBranch("ht_4lpt50"); 
   b_jet_algo1_n = fChain->GetBranch("jet_algo1_n"); 
   b_jet_algo1_p4 = fChain->GetBranch("jet_algo1_p4"); 
   b_jet_algo1_emfrac = fChain->GetBranch("jet_algo1_emfrac"); 
   b_jet_algo1_hadfrac = fChain->GetBranch("jet_algo1_hadfrac"); 
   b_jet_algo1_ntk = fChain->GetBranch("jet_algo1_ntk"); 
   b_jet_algo1_tkind = fChain->GetBranch("jet_algo1_tkind"); 
   b_jet_algo1_calotwind = fChain->GetBranch("jet_algo1_calotwind"); 
   b_jet_algo2_n = fChain->GetBranch("jet_algo2_n"); 
   b_jet_algo2_p4 = fChain->GetBranch("jet_algo2_p4"); 
   b_jet_algo2_emfrac = fChain->GetBranch("jet_algo2_emfrac"); 
   b_jet_algo2_hadfrac = fChain->GetBranch("jet_algo2_hadfrac"); 
   b_jet_algo2_ntk = fChain->GetBranch("jet_algo2_ntk"); 
   b_jet_algo2_tkind = fChain->GetBranch("jet_algo2_tkind"); 
   b_jet_algo2_calotwind = fChain->GetBranch("jet_algo2_calotwind"); 
   b_jet_algo3_n = fChain->GetBranch("jet_algo3_n"); 
   b_jet_algo3_p4 = fChain->GetBranch("jet_algo3_p4"); 
   b_jet_algo3_emfrac = fChain->GetBranch("jet_algo3_emfrac"); 
   b_jet_algo3_hadfrac = fChain->GetBranch("jet_algo3_hadfrac"); 
   b_jet_algo3_ntk = fChain->GetBranch("jet_algo3_ntk"); 
   b_jet_algo3_tkind = fChain->GetBranch("jet_algo3_tkind"); 
   b_jet_algo3_calotwind = fChain->GetBranch("jet_algo3_calotwind"); 
   b_jet_algoPF1_n = fChain->GetBranch("jet_algoPF1_n"); 
   b_jet_algoPF1_p4 = fChain->GetBranch("jet_algoPF1_p4"); 
   b_jet_algoPF1_emfrac = fChain->GetBranch("jet_algoPF1_emfrac"); 
   b_jet_algoPF1_hadfrac = fChain->GetBranch("jet_algoPF1_hadfrac"); 
   b_jet_algoPF1_erescale = fChain->GetBranch("jet_algoPF1_erescale"); 
   b_jet_algoPF1_ntk = fChain->GetBranch("jet_algoPF1_ntk"); 
   b_jet_algoPF1_tkind = fChain->GetBranch("jet_algoPF1_tkind"); 
   b_jet_algoPF1_calotwind = fChain->GetBranch("jet_algoPF1_calotwind");
   b_jet_algoPF1_beta = fChain->GetBranch("jet_algoPF1_beta");
   b_jet_algoPF1_betaStar = fChain->GetBranch("jet_algoPF1_betaStar");
   b_jet_algoPF1_betaStarClassic = fChain->GetBranch("jet_algoPF1_betaStarClassic");
   b_jet_algoPF1_dR2Mean = fChain->GetBranch("jet_algoPF1_dR2Mean");
   b_jet_algoPF1_dRMean = fChain->GetBranch("jet_algoPF1_dRMean");
   b_jet_algoPF1_dZ = fChain->GetBranch("jet_algoPF1_dZ");
   b_jet_algoPF1_frac01 = fChain->GetBranch("jet_algoPF1_frac01");
   b_jet_algoPF1_frac02 = fChain->GetBranch("jet_algoPF1_frac02");
   b_jet_algoPF1_frac03 = fChain->GetBranch("jet_algoPF1_frac03");
   b_jet_algoPF1_frac04 = fChain->GetBranch("jet_algoPF1_frac04");
   b_jet_algoPF1_frac05 = fChain->GetBranch("jet_algoPF1_frac05");
   b_jet_algoPF1_full_mva = fChain->GetBranch("jet_algoPF1_full_mva");
   b_jet_algoPF1_simple_mva = fChain->GetBranch("jet_algoPF1_simple_mva");
   b_jet_algoPF1_nCharged = fChain->GetBranch("jet_algoPF1_nCharged");
   b_jet_algoPF1_nNeutrals = fChain->GetBranch("jet_algoPF1_nNeutrals");
   b_jet_algoPF1_full_wp_level = fChain->GetBranch("jet_algoPF1_full_wp_level");
   b_jet_algoPF1_simple_wp_level = fChain->GetBranch("jet_algoPF1_simple_wp_level");
   b_jet_algoPF2_n = fChain->GetBranch("jet_algoPF2_n"); 
   b_jet_algoPF2_p4 = fChain->GetBranch("jet_algoPF2_p4"); 
   b_jet_algoPF2_emfrac = fChain->GetBranch("jet_algoPF2_emfrac"); 
   b_jet_algoPF2_hadfrac = fChain->GetBranch("jet_algoPF2_hadfrac"); 
   b_jet_algoPF2_ntk = fChain->GetBranch("jet_algoPF2_ntk"); 
   b_jet_algoPF2_tkind = fChain->GetBranch("jet_algoPF2_tkind"); 
   b_jet_algoPF2_calotwind = fChain->GetBranch("jet_algoPF2_calotwind"); 
   b_jet_algoPF3_n = fChain->GetBranch("jet_algoPF3_n"); 
   b_jet_algoPF3_p4 = fChain->GetBranch("jet_algoPF3_p4"); 
   b_jet_algoPF3_emfrac = fChain->GetBranch("jet_algoPF3_emfrac"); 
   b_jet_algoPF3_hadfrac = fChain->GetBranch("jet_algoPF3_hadfrac"); 
   b_jet_algoPF3_ntk = fChain->GetBranch("jet_algoPF3_ntk"); 
   b_jet_algoPF3_tkind = fChain->GetBranch("jet_algoPF3_tkind"); 
   b_jet_algoPF3_calotwind = fChain->GetBranch("jet_algoPF3_calotwind"); 
   b_l1emiso_n = fChain->GetBranch("l1emiso_n"); 
   b_l1emiso_eta = fChain->GetBranch("l1emiso_eta"); 
   b_l1emiso_et = fChain->GetBranch("l1emiso_et"); 
   b_l1emiso_phi = fChain->GetBranch("l1emiso_phi"); 
   b_l1emnoniso_n = fChain->GetBranch("l1emnoniso_n"); 
   b_l1emnoniso_et = fChain->GetBranch("l1emnoniso_et"); 
   b_l1emnoniso_eta = fChain->GetBranch("l1emnoniso_eta"); 
   b_l1emnoniso_phi = fChain->GetBranch("l1emnoniso_phi"); 
   b_l1cenjet_n = fChain->GetBranch("l1cenjet_n"); 
   b_l1cenjet_et = fChain->GetBranch("l1cenjet_et"); 
   b_l1cenjet_eta = fChain->GetBranch("l1cenjet_eta"); 
   b_l1cenjet_phi = fChain->GetBranch("l1cenjet_phi"); 
   b_l1forjet_n = fChain->GetBranch("l1forjet_n"); 
   b_l1forjet_et = fChain->GetBranch("l1forjet_et"); 
   b_l1forjet_eta = fChain->GetBranch("l1forjet_eta"); 
   b_l1forjet_phi = fChain->GetBranch("l1forjet_phi"); 
   b_l1taujet_n = fChain->GetBranch("l1taujet_n"); 
   b_l1taujet_et = fChain->GetBranch("l1taujet_et"); 
   b_l1taujet_eta = fChain->GetBranch("l1taujet_eta"); 
   b_l1taujet_phi = fChain->GetBranch("l1taujet_phi"); 
   b_l1met_et = fChain->GetBranch("l1met_et"); 
   b_l1met_phi = fChain->GetBranch("l1met_phi"); 
   b_l1mu_n = fChain->GetBranch("l1mu_n"); 
   b_l1mu_et = fChain->GetBranch("l1mu_et"); 
   b_l1mu_eta = fChain->GetBranch("l1mu_eta"); 
   b_l1mu_phi = fChain->GetBranch("l1mu_phi"); 
   b_l1_labels = fChain->GetBranch("l1_labels"); 
   b_l1bits_phy = fChain->GetBranch("l1bits_phy"); 
   b_l1bits_tec = fChain->GetBranch("l1bits_tec"); 
   b_lpt_n = fChain->GetBranch("lpt_n"); 
   b_lpt_emu_n = fChain->GetBranch("lpt_emu_n"); 
   b_lpt_mu_n = fChain->GetBranch("lpt_mu_n"); 
   b_lpt_el_n = fChain->GetBranch("lpt_el_n"); 
   b_lpt_pho_n = fChain->GetBranch("lpt_pho_n"); 
   b_lpt_pdgid = fChain->GetBranch("lpt_pdgid"); 
   b_lpt_ind = fChain->GetBranch("lpt_ind"); 
   b_lpt_duplicate = fChain->GetBranch("lpt_duplicate"); 
   b_lpt_p4 = fChain->GetBranch("lpt_p4"); 
   b_lpt_indgen = fChain->GetBranch("lpt_indgen"); 
   b_lpt_drmatch = fChain->GetBranch("lpt_drmatch"); 
   b_met_met = fChain->GetBranch("met_met"); 
   b_met_phi = fChain->GetBranch("met_phi"); 
   b_met_met_nocalo = fChain->GetBranch("met_met_nocalo"); 
   b_met_phi_nocalo = fChain->GetBranch("met_phi_nocalo"); 
   b_met_met_crossed = fChain->GetBranch("met_met_crossed"); 
   b_met_phi_crossed = fChain->GetBranch("met_phi_crossed"); 
   b_met_met_s9 = fChain->GetBranch("met_met_s9"); 
   b_met_phi_s9 = fChain->GetBranch("met_phi_s9"); 
   b_met_met_mip = fChain->GetBranch("met_met_mip"); 
   b_met_phi_mip = fChain->GetBranch("met_phi_mip"); 
   b_met_met_jet = fChain->GetBranch("met_met_jet"); 
   b_met_phi_jet = fChain->GetBranch("met_phi_jet"); 
   b_met_tcmet = fChain->GetBranch("met_tcmet"); 
   b_met_phi_tcmet = fChain->GetBranch("met_phi_tcmet"); 
   b_met_pfmet = fChain->GetBranch("met_pfmet"); 
   b_met_phi_pfmet = fChain->GetBranch("met_phi_pfmet"); 
   b_mu_glo_n = fChain->GetBranch("mu_glo_n"); 
   b_mu_glo_p4 = fChain->GetBranch("mu_glo_p4"); 
   b_mu_glo_momvtx = fChain->GetBranch("mu_glo_momvtx"); 
   b_mu_glo_posvtx = fChain->GetBranch("mu_glo_posvtx"); 
   b_mu_glo_posecal = fChain->GetBranch("mu_glo_posecal"); 
   b_mu_glo_poshcal = fChain->GetBranch("mu_glo_poshcal"); 
   b_mu_glo_nmatches = fChain->GetBranch("mu_glo_nmatches"); 
   b_mu_glo_em = fChain->GetBranch("mu_glo_em"); 
   b_mu_glo_had = fChain->GetBranch("mu_glo_had"); 
   b_mu_glo_ho = fChain->GetBranch("mu_glo_ho"); 
   b_mu_glo_emS9 = fChain->GetBranch("mu_glo_emS9"); 
   b_mu_glo_hadS9 = fChain->GetBranch("mu_glo_hadS9"); 
   b_mu_glo_hoS9 = fChain->GetBranch("mu_glo_hoS9"); 
   b_mu_glo_chi2 = fChain->GetBranch("mu_glo_chi2"); 
   b_mu_glo_dof = fChain->GetBranch("mu_glo_dof"); 
   b_mu_glo_tkind = fChain->GetBranch("mu_glo_tkind"); 
   b_mu_glo_staind = fChain->GetBranch("mu_glo_staind"); 
   b_mu_glo_z0 = fChain->GetBranch("mu_glo_z0"); 
   b_mu_glo_d0 = fChain->GetBranch("mu_glo_d0"); 
   b_mu_glo_D0Vtx = fChain->GetBranch("mu_glo_D0Vtx"); 
   b_mu_glo_DZVtx = fChain->GetBranch("mu_glo_DZVtx"); 
   b_mu_glo_z0err = fChain->GetBranch("mu_glo_z0err"); 
   b_mu_glo_d0err = fChain->GetBranch("mu_glo_d0err"); 
   b_mu_glo_charge = fChain->GetBranch("mu_glo_charge"); 
   b_mu_glo_losthits = fChain->GetBranch("mu_glo_losthits"); 
   b_mu_glo_validhits = fChain->GetBranch("mu_glo_validhits"); 
   b_mu_glo_innerhits = fChain->GetBranch("mu_glo_innerhits"); 
   b_mu_glo_type = fChain->GetBranch("mu_glo_type"); 
   b_mu_glo_iso = fChain->GetBranch("mu_glo_iso"); 
   b_mu_glo_pixelhits = fChain->GetBranch("mu_glo_pixelhits"); 
   b_mu_glo_validChmbhits = fChain->GetBranch("mu_glo_validChmbhits"); 
   b_mu_glo_ecaliso03 = fChain->GetBranch("mu_glo_tkpterr"); 
   b_mu_glo_ecaliso03 = fChain->GetBranch("mu_glo_ecaliso03"); 
   b_mu_glo_hcaliso03 = fChain->GetBranch("mu_glo_hcaliso03"); 
   b_mu_glo_tkiso03 = fChain->GetBranch("mu_glo_tkiso03"); 
   b_mu_glo_dz = fChain->GetBranch("mu_glo_dz"); 
   b_pho_n = fChain->GetBranch("pho_n"); 
   b_pho_feta = fChain->GetBranch("pho_feta"); 
   b_pho_crackcorr = fChain->GetBranch("pho_crackcorr"); 
   b_pho_localcorr = fChain->GetBranch("pho_localcorr"); 
   b_pho_isEB = fChain->GetBranch("pho_isEB"); 
   b_pho_isEE = fChain->GetBranch("pho_isEE"); 
   b_pho_isEBGap = fChain->GetBranch("pho_isEBGap"); 
   b_pho_isEEGap = fChain->GetBranch("pho_isEEGap"); 
   b_pho_isEBEEGap = fChain->GetBranch("pho_isEBEEGap"); 
   b_pho_see = fChain->GetBranch("pho_see"); 
   b_pho_sieie = fChain->GetBranch("pho_sieie"); 
   b_pho_e1x5 = fChain->GetBranch("pho_e1x5"); 
   b_pho_e2x5 = fChain->GetBranch("pho_e2x5"); 
   b_pho_e3x3 = fChain->GetBranch("pho_e3x3"); 
   b_pho_e5x5 = fChain->GetBranch("pho_e5x5"); 
   b_pho_emaxxtal = fChain->GetBranch("pho_emaxxtal"); 
   b_pho_hoe = fChain->GetBranch("pho_hoe"); 
   b_pho_h1oe = fChain->GetBranch("pho_h1oe"); 
   b_pho_h2oe = fChain->GetBranch("pho_h2oe"); 
   b_pho_r1x5 = fChain->GetBranch("pho_r1x5"); 
   b_pho_r2x5 = fChain->GetBranch("pho_r2x5"); 
   b_pho_r9 = fChain->GetBranch("pho_r9"); 
   b_pho_ecalsumetconedr04 = fChain->GetBranch("pho_ecalsumetconedr04"); 
   b_pho_hcalsumetconedr04 = fChain->GetBranch("pho_hcalsumetconedr04"); 
   b_pho_hcal1sumetconedr04 = fChain->GetBranch("pho_hcal1sumetconedr04"); 
   b_pho_hcal2sumetconedr04 = fChain->GetBranch("pho_hcal2sumetconedr04"); 
   b_pho_trksumptsolidconedr04 = fChain->GetBranch("pho_trksumptsolidconedr04"); 
   b_pho_trksumpthollowconedr04 = fChain->GetBranch("pho_trksumpthollowconedr04"); 
   b_pho_ntrksolidconedr04 = fChain->GetBranch("pho_ntrksolidconedr04"); 
   b_pho_ntrkhollowconedr04 = fChain->GetBranch("pho_ntrkhollowconedr04"); 
   b_pho_ecalsumetconedr03 = fChain->GetBranch("pho_ecalsumetconedr03"); 
   b_pho_hcalsumetconedr03 = fChain->GetBranch("pho_hcalsumetconedr03"); 
   b_pho_hcal1sumetconedr03 = fChain->GetBranch("pho_hcal1sumetconedr03"); 
   b_pho_hcal2sumetconedr03 = fChain->GetBranch("pho_hcal2sumetconedr03"); 
   b_pho_trksumptsolidconedr03 = fChain->GetBranch("pho_trksumptsolidconedr03"); 
   b_pho_trksumpthollowconedr03 = fChain->GetBranch("pho_trksumpthollowconedr03"); 
   b_pho_ntrksolidconedr03 = fChain->GetBranch("pho_ntrksolidconedr03"); 
   b_pho_ntrkhollowconedr03 = fChain->GetBranch("pho_ntrkhollowconedr03"); 
   b_pho_p4 = fChain->GetBranch("pho_p4"); 
   b_pho_calopos = fChain->GetBranch("pho_calopos"); 
   b_pho_barrel = fChain->GetBranch("pho_barrel"); 
   b_pho_scind = fChain->GetBranch("pho_scind"); 
   b_pho_haspixseed = fChain->GetBranch("pho_haspixseed"); 
   b_pho_hasconvtks = fChain->GetBranch("pho_hasconvtks"); 
   b_pho_nconv = fChain->GetBranch("pho_nconv"); 
   b_pho_isconv = fChain->GetBranch("pho_isconv"); 
   b_pho_conv_ntracks = fChain->GetBranch("pho_conv_ntracks"); 
   b_pho_conv_pairinvmass = fChain->GetBranch("pho_conv_pairinvmass"); 
   b_pho_conv_paircotthetasep = fChain->GetBranch("pho_conv_paircotthetasep"); 
   b_pho_conv_eoverp = fChain->GetBranch("pho_conv_eoverp"); 
   b_pho_conv_zofprimvtxfromtrks = fChain->GetBranch("pho_conv_zofprimvtxfromtrks"); 
   b_pho_conv_distofminapproach = fChain->GetBranch("pho_conv_distofminapproach"); 
   b_pho_conv_dphitrksatvtx = fChain->GetBranch("pho_conv_dphitrksatvtx"); 
   b_pho_conv_dphitrksatecal = fChain->GetBranch("pho_conv_dphitrksatecal"); 
   b_pho_conv_detatrksatecal = fChain->GetBranch("pho_conv_detatrksatecal"); 
   b_pho_conv_tk1_d0 = fChain->GetBranch("pho_conv_tk1_d0"); 
   b_pho_conv_tk1_pout = fChain->GetBranch("pho_conv_tk1_pout"); 
   b_pho_conv_tk1_pin = fChain->GetBranch("pho_conv_tk1_pin"); 
   b_pho_conv_tk2_d0 = fChain->GetBranch("pho_conv_tk2_d0"); 
   b_pho_conv_tk2_pout = fChain->GetBranch("pho_conv_tk2_pout"); 
   b_pho_conv_tk2_pin = fChain->GetBranch("pho_conv_tk2_pin"); 
   b_pho_conv_tk1_dz = fChain->GetBranch("pho_conv_tk1_dz"); 
   b_pho_conv_tk2_dz = fChain->GetBranch("pho_conv_tk2_dz"); 
   b_pho_conv_tk1_dzerr = fChain->GetBranch("pho_conv_tk1_dzerr"); 
   b_pho_conv_tk2_dzerr = fChain->GetBranch("pho_conv_tk2_dzerr"); 
   b_pho_conv_tk1_nh = fChain->GetBranch("pho_conv_tk1_nh"); 
   b_pho_conv_tk2_nh = fChain->GetBranch("pho_conv_tk2_nh"); 
   b_pho_conv_chi2 = fChain->GetBranch("pho_conv_chi2"); 
   b_pho_conv_chi2_probability = fChain->GetBranch("pho_conv_chi2_probability"); 
   b_pho_conv_ch1ch2 = fChain->GetBranch("pho_conv_ch1ch2"); 
   b_pho_conv_validvtx = fChain->GetBranch("pho_conv_validvtx"); 
   b_pho_conv_MVALikelihood = fChain->GetBranch("pho_conv_MVALikelihood"); 
   b_pho_sipip = fChain->GetBranch("pho_sipip"); 
   b_pho_sieip = fChain->GetBranch("pho_sieip"); 
   b_pho_zernike20 = fChain->GetBranch("pho_zernike20"); 
   b_pho_zernike42 = fChain->GetBranch("pho_zernike42"); 
   b_pho_e2nd = fChain->GetBranch("pho_e2nd"); 
   b_pho_e2x5right = fChain->GetBranch("pho_e2x5right"); 
   b_pho_e2x5left = fChain->GetBranch("pho_e2x5left"); 
   b_pho_e2x5Top = fChain->GetBranch("pho_e2x5Top"); 
   b_pho_e2x5bottom = fChain->GetBranch("pho_e2x5bottom"); 
   b_pho_eright = fChain->GetBranch("pho_eright"); 
   b_pho_eleft = fChain->GetBranch("pho_eleft"); 
   b_pho_etop = fChain->GetBranch("pho_etop"); 
   b_pho_ebottom = fChain->GetBranch("pho_ebottom"); 
   b_pho_e2overe9 = fChain->GetBranch("pho_e2overe9"); 
   b_pho_seed_severity = fChain->GetBranch("pho_seed_severity"); 
   b_pho_seed_time = fChain->GetBranch("pho_seed_time"); 
   b_pho_seed_outoftimechi2 = fChain->GetBranch("pho_seed_outoftimechi2"); 
   b_pho_seed_chi2 = fChain->GetBranch("pho_seed_chi2"); 
   b_pho_seed_recoflag = fChain->GetBranch("pho_seed_recoflag"); 
   b_pho_conv_vtx = fChain->GetBranch("pho_conv_vtx"); 
   b_pho_conv_pair_momentum = fChain->GetBranch("pho_conv_pair_momentum"); 
   b_pho_conv_refitted_momentum = fChain->GetBranch("pho_conv_refitted_momentum"); 
   b_pho_conv_vertexcorrected_p4 = fChain->GetBranch("pho_conv_vertexcorrected_p4"); 
   b_pho_residCorrEnergy = fChain->GetBranch("pho_residCorrEnergy"); 
   b_pho_residCorrResn = fChain->GetBranch("pho_residCorrResn"); 
   b_pho_regr_energy = fChain->GetBranch("pho_regr_energy");
   b_pho_regr_energyerr = fChain->GetBranch("pho_regr_energyerr");
   b_conv_n = fChain->GetBranch("conv_n");
   b_conv_p4 = fChain->GetBranch("conv_p4");
   b_conv_ntracks = fChain->GetBranch("conv_ntracks");
   b_conv_pairinvmass = fChain->GetBranch("conv_pairinvmass");
   b_conv_paircotthetasep = fChain->GetBranch("conv_paircotthetasep");
   b_conv_eoverp = fChain->GetBranch("conv_eoverp");
   b_conv_distofminapproach = fChain->GetBranch("conv_distofminapproach");
   b_conv_dphitrksatvtx = fChain->GetBranch("conv_dphitrksatvtx");
   b_conv_dphitrksatecal = fChain->GetBranch("conv_dphitrksatecal");
   b_conv_detatrksatecal = fChain->GetBranch("conv_detatrksatecal");
   b_conv_dxy = fChain->GetBranch("conv_dxy");
   b_conv_dz = fChain->GetBranch("conv_dz");
   b_conv_lxy = fChain->GetBranch("conv_lxy");
   b_conv_lz = fChain->GetBranch("conv_lz");
   b_conv_zofprimvtxfromtrks = fChain->GetBranch("conv_zofprimvtxfromtrks");
   b_conv_nHitsBeforeVtx = fChain->GetBranch("conv_nHitsBeforeVtx");
   b_conv_nSharedHits = fChain->GetBranch("conv_nSharedHits");
   b_conv_validvtx = fChain->GetBranch("conv_validvtx");
   b_conv_MVALikelihood = fChain->GetBranch("conv_MVALikelihood");
   b_conv_chi2 = fChain->GetBranch("conv_chi2");
   b_conv_chi2_probability = fChain->GetBranch("conv_chi2_probability");
   b_conv_vtx_xErr = fChain->GetBranch("conv_vtx_xErr");
   b_conv_vtx_yErr = fChain->GetBranch("conv_vtx_yErr");
   b_conv_vtx_zErr = fChain->GetBranch("conv_vtx_zErr");
   b_conv_tk1_dz = fChain->GetBranch("conv_tk1_dz");
   b_conv_tk2_dz = fChain->GetBranch("conv_tk2_dz");
   b_conv_tk1_dzerr = fChain->GetBranch("conv_tk1_dzerr");
   b_conv_tk2_dzerr = fChain->GetBranch("conv_tk2_dzerr");
   b_conv_tk1_nh = fChain->GetBranch("conv_tk1_nh");
   b_conv_tk2_nh = fChain->GetBranch("conv_tk2_nh");
   b_conv_ch1ch2 = fChain->GetBranch("conv_ch1ch2");
   b_conv_tk1_d0 = fChain->GetBranch("conv_tk1_d0");
   b_conv_tk1_pout = fChain->GetBranch("conv_tk1_pout");
   b_conv_tk1_pin = fChain->GetBranch("conv_tk1_pin");
   b_conv_tk2_d0 = fChain->GetBranch("conv_tk2_d0");
   b_conv_tk2_pout = fChain->GetBranch("conv_tk2_pout");
   b_conv_tk2_pin = fChain->GetBranch("conv_tk2_pin");
   b_conv_vtx = fChain->GetBranch("conv_vtx");
   b_conv_tk1_pterr = fChain->GetBranch("conv_tk1_pterr");
   b_conv_tk2_pterr = fChain->GetBranch("conv_tk2_pterr");
   b_conv_tk1_etaerr = fChain->GetBranch("conv_tk1_etaerr");
   b_conv_tk2_etaerr = fChain->GetBranch("conv_tk2_etaerr");
   b_conv_tk1_thetaerr = fChain->GetBranch("conv_tk1_thetaerr");
   b_conv_tk2_thetaerr = fChain->GetBranch("conv_tk2_thetaerr");
   b_conv_tk1_phierr = fChain->GetBranch("conv_tk1_phierr");
   b_conv_tk2_phierr = fChain->GetBranch("conv_tk2_phierr");
   b_conv_tk1_lambdaerr = fChain->GetBranch("conv_tk1_lambdaerr");
   b_conv_tk2_lambdaerr = fChain->GetBranch("conv_tk2_lambdaerr");
   b_conv_pair_momentum = fChain->GetBranch("conv_pair_momentum");
   b_conv_refitted_momentum = fChain->GetBranch("conv_refitted_momentum");
   b_conv_singleleg_momentum = fChain->GetBranch("conv_singleleg_momentum");
   b_lptgeninfo_n = fChain->GetBranch("lptgeninfo_n"); 
   b_lptgen_n = fChain->GetBranch("lptgen_n"); 
   b_lptgeninfo_p4 = fChain->GetBranch("lptgeninfo_p4"); 
   b_lptgen_p4 = fChain->GetBranch("lptgen_p4"); 
   b_lptgen_befrad_p4 = fChain->GetBranch("lptgen_befrad_p4"); 
   b_lptgeninfo_status = fChain->GetBranch("lptgeninfo_status"); 
   b_lptgeninfo_pdgid = fChain->GetBranch("lptgeninfo_pdgid"); 
   b_lptgeninfo_mother = fChain->GetBranch("lptgeninfo_mother"); 
   b_lptgen_status = fChain->GetBranch("lptgen_status"); 
   b_lptgen_pdgid = fChain->GetBranch("lptgen_pdgid"); 
   b_lptgen_mother = fChain->GetBranch("lptgen_mother"); 
   b_lptgen_motherpdgid = fChain->GetBranch("lptgen_motherpdgid"); 
   b_lptgen_indrec = fChain->GetBranch("lptgen_indrec"); 
   b_lptgen_indrecel = fChain->GetBranch("lptgen_indrecel"); 
   b_lptgen_indrecph = fChain->GetBranch("lptgen_indrecph"); 
   b_lptgen_indrecmu = fChain->GetBranch("lptgen_indrecmu"); 
   b_lptgen_drmatch = fChain->GetBranch("lptgen_drmatch"); 
   b_lptgen_drmatchel = fChain->GetBranch("lptgen_drmatchel"); 
   b_lptgen_drmatchmu = fChain->GetBranch("lptgen_drmatchmu"); 
   b_lptgen_drmatchph = fChain->GetBranch("lptgen_drmatchph"); 
   b_lptgen_indinfo = fChain->GetBranch("lptgen_indinfo"); 
   b_lptgen_historycode = fChain->GetBranch("lptgen_historycode"); 
   b_simtrk_n = fChain->GetBranch("simtrk_n"); 
   b_simtrk_p4 = fChain->GetBranch("simtrk_p4"); 
   b_simtrk_vtx = fChain->GetBranch("simtrk_vtx"); 
   b_simtrk_pdgid = fChain->GetBranch("simtrk_pdgid"); 
   b_simtrk_trkid = fChain->GetBranch("simtrk_trkid"); 
   b_simtrk_mothertrkid = fChain->GetBranch("simtrk_mothertrkid"); 
   b_simvtx = fChain->GetBranch("simvtx"); 
   b_simhit_n = fChain->GetBranch("simhit_n"); 
   b_simhit_xyz = fChain->GetBranch("simhit_xyz"); 
   b_simhit_pabs = fChain->GetBranch("simhit_pabs"); 
   b_simhit_eloss = fChain->GetBranch("simhit_eloss"); 
   b_simhit_subdet = fChain->GetBranch("simhit_subdet"); 
   b_simhit_pdgid = fChain->GetBranch("simhit_pdgid"); 
   b_simhit_trkid = fChain->GetBranch("simhit_trkid"); 
   b_simhit_simtrkind = fChain->GetBranch("simhit_simtrkind"); 
   b_tk_n = fChain->GetBranch("tk_n"); 
   b_tk_p4 = fChain->GetBranch("tk_p4"); 
   b_tk_vtx_pos = fChain->GetBranch("tk_vtx_pos"); 
   b_tk_nhits = fChain->GetBranch("tk_nhits"); 
   b_tk_charge = fChain->GetBranch("tk_charge"); 
   b_tk_nlosthit = fChain->GetBranch("tk_nlosthit"); 
   b_tk_tpind = fChain->GetBranch("tk_tpind"); 
   b_tk_chi2 = fChain->GetBranch("tk_chi2"); 
   b_tk_dof = fChain->GetBranch("tk_dof"); 
   b_tk_d0 = fChain->GetBranch("tk_d0"); 
   b_tk_dz = fChain->GetBranch("tk_dz"); 
   b_tk_qoverperr = fChain->GetBranch("tk_qoverperr"); 
   b_tk_pterr = fChain->GetBranch("tk_pterr"); 
   b_tk_etaerr = fChain->GetBranch("tk_etaerr"); 
   b_tk_phierr = fChain->GetBranch("tk_phierr"); 
   b_tk_d0err = fChain->GetBranch("tk_d0err"); 
   b_tk_dzerr = fChain->GetBranch("tk_dzerr"); 
   b_tk_hp_nvalid = fChain->GetBranch("tk_hp_nvalid"); 
   b_tk_hp_nlost = fChain->GetBranch("tk_hp_nlost"); 
   b_tk_hp_nvalidpix = fChain->GetBranch("tk_hp_nvalidpix"); 
   b_tk_hp_expin = fChain->GetBranch("tk_hp_expin"); 
   b_tk_hp_expout = fChain->GetBranch("tk_hp_expout"); 
   b_tk_quality = fChain->GetBranch("tk_quality"); 
   b_tk_algo = fChain->GetBranch("tk_algo"); 
   b_tp_n = fChain->GetBranch("tp_n"); 
   b_tp_p4 = fChain->GetBranch("tp_p4"); 
   b_tp_vtx = fChain->GetBranch("tp_vtx"); 
   b_tv_xyz = fChain->GetBranch("tv_xyz"); 
   b_tp_pdgid = fChain->GetBranch("tp_pdgid"); 
   b_tp_motherid = fChain->GetBranch("tp_motherid"); 
   b_tp_charge = fChain->GetBranch("tp_charge"); 
   b_tp_tkind = fChain->GetBranch("tp_tkind"); 
   b_tp_genind = fChain->GetBranch("tp_genind"); 
   b_tp_d0 = fChain->GetBranch("tp_d0"); 
   b_tp_dz = fChain->GetBranch("tp_dz"); 
   b_bs_xyz = fChain->GetBranch("bs_xyz"); 
   b_bs_sigmaZ = fChain->GetBranch("bs_sigmaZ"); 
   b_bs_x0Error = fChain->GetBranch("bs_x0Error"); 
   b_bs_y0Error = fChain->GetBranch("bs_y0Error"); 
   b_bs_z0Error = fChain->GetBranch("bs_z0Error"); 
   b_bs_sigmaZ0Error = fChain->GetBranch("bs_sigmaZ0Error"); 
   b_vtx_n = fChain->GetBranch("vtx_n"); 
   b_vtx_xyz = fChain->GetBranch("vtx_xyz"); 
   b_vtx_dxdydz = fChain->GetBranch("vtx_dxdydz"); 
   b_vtx_vectorp3 = fChain->GetBranch("vtx_vectorp3"); 
   b_vtx_x2dof = fChain->GetBranch("vtx_x2dof"); 
   b_vtx_ndof = fChain->GetBranch("vtx_ndof"); 
   b_vtx_scalarpt = fChain->GetBranch("vtx_scalarpt"); 
   b_vtx_ntks = fChain->GetBranch("vtx_ntks"); 
   b_vtx_tkind = fChain->GetBranch("vtx_tkind"); 
   b_vtx_tkweight = fChain->GetBranch("vtx_tkweight"); 
   b_bs_std_xyz = fChain->GetBranch("bs_std_xyz"); 
   b_bs_std_sigmaZ = fChain->GetBranch("bs_std_sigmaZ"); 
   b_bs_std_x0Error = fChain->GetBranch("bs_std_x0Error"); 
   b_bs_std_y0Error = fChain->GetBranch("bs_std_y0Error"); 
   b_bs_std_z0Error = fChain->GetBranch("bs_std_z0Error"); 
   b_bs_std_sigmaZ0Error = fChain->GetBranch("bs_std_sigmaZ0Error"); 
   b_vtx_std_n = fChain->GetBranch("vtx_std_n"); 
   b_vtx_std_xyz = fChain->GetBranch("vtx_std_xyz"); 
   b_vtx_std_dxdydz = fChain->GetBranch("vtx_std_dxdydz"); 
   b_vtx_std_vectorp3 = fChain->GetBranch("vtx_std_vectorp3"); 
   b_vtx_std_x2dof = fChain->GetBranch("vtx_std_x2dof"); 
   b_vtx_std_ndof = fChain->GetBranch("vtx_std_ndof"); 
   b_vtx_std_scalarpt = fChain->GetBranch("vtx_std_scalarpt"); 
   b_vtx_std_ntks = fChain->GetBranch("vtx_std_ntks"); 
   b_vtx_std_tkind = fChain->GetBranch("vtx_std_tkind"); 
   b_vtx_std_tkweight = fChain->GetBranch("vtx_std_tkweight"); 
   b_rho = fChain->GetBranch("rho");
   b_rho_algo1 = fChain->GetBranch("rho_algo1");
   b_rho_algo2 = fChain->GetBranch("rho_algo2");
   b_rho_algo3 = fChain->GetBranch("rho_algo3");
   b_pho_r19 = fChain->GetBranch("pho_r19");
   b_pho_maxoraw = fChain->GetBranch("pho_maxoraw");
   b_pho_cep = fChain->GetBranch("pho_cep");
   b_pho_lambdaratio = fChain->GetBranch("pho_lambdaratio");
   b_pho_lambdadivcov = fChain->GetBranch("pho_lambdadivcov");
   b_pho_etawidth = fChain->GetBranch("pho_etawidth");
   b_pho_brem = fChain->GetBranch("pho_brem");
   b_pho_smaj = fChain->GetBranch("pho_smaj");
   b_pho_e2x2 = fChain->GetBranch("pho_e2x2");
 b_pho_pfiso_myneutral01 = fChain->GetBranch("pho_pfiso_myneutral01");
b_pho_pfiso_myneutral02 = fChain->GetBranch("pho_pfiso_myneutral02");
b_pho_pfiso_myneutral03 = fChain->GetBranch("pho_pfiso_myneutral03");
b_pho_pfiso_myneutral04 = fChain->GetBranch("pho_pfiso_myneutral04");
b_pho_pfiso_myneutral05 = fChain->GetBranch("pho_pfiso_myneutral05");
b_pho_pfiso_myneutral06 = fChain->GetBranch("pho_pfiso_myneutral06");
b_pho_pfiso_myphoton01 = fChain->GetBranch("pho_pfiso_myphoton01");
b_pho_pfiso_myphoton02 = fChain->GetBranch("pho_pfiso_myphoton02");
b_pho_pfiso_myphoton03 = fChain->GetBranch("pho_pfiso_myphoton03");
b_pho_pfiso_myphoton04 = fChain->GetBranch("pho_pfiso_myphoton04");
b_pho_pfiso_myphoton05 = fChain->GetBranch("pho_pfiso_myphoton05");
b_pho_pfiso_myphoton06 = fChain->GetBranch("pho_pfiso_myphoton06");
b_pho_pfiso_mycharged01 = fChain->GetBranch("pho_pfiso_mycharged01");
b_pho_pfiso_mycharged02 = fChain->GetBranch("pho_pfiso_mycharged02");
b_pho_pfiso_mycharged03 = fChain->GetBranch("pho_pfiso_mycharged03");
b_pho_pfiso_mycharged04 = fChain->GetBranch("pho_pfiso_mycharged04");
b_pho_pfiso_mycharged05 = fChain->GetBranch("pho_pfiso_mycharged05");
b_pho_pfiso_mycharged06 = fChain->GetBranch("pho_pfiso_mycharged06");
b_pho_isPFPhoton = fChain->GetBranch("pho_isPFPhoton");
b_pho_isPFElectron = fChain->GetBranch("pho_isPFElectron");
b_pho_must = fChain->GetBranch("pho_must");
b_pho_mustnc = fChain->GetBranch("pho_mustnc");
b_pho_pfpresh1 = fChain->GetBranch("pho_pfpresh1");
b_pho_pfpresh2 = fChain->GetBranch("pho_pfpresh2");
b_pho_mustenergy = fChain->GetBranch("pho_mustenergy");
b_pho_mustenergyout = fChain->GetBranch("pho_mustenergyout");
b_pho_pflowE = fChain->GetBranch("pho_pflowE");
b_pho_pfdeta = fChain->GetBranch("pho_pfdeta");
b_pho_pfdphi = fChain->GetBranch("pho_pfdphi");
b_pho_pfclusrms = fChain->GetBranch("pho_pfclusrms");
b_pho_pfclusrmsmust = fChain->GetBranch("pho_pfclusrmsmust");
b_pho_eseffsixix = fChain->GetBranch("pho_eseffsixix");
b_pho_eseffsiyiy = fChain->GetBranch("pho_eseffsiyiy");

