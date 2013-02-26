#!/bin/csh

root -b -l -q 'BDT_Zee_bdtin.C(0,0)'
root -b -l -q BDT_Zee_bdtin.C
root -b -l -q bdtout_syst_interp.C
root -b -l -q idmva_interp.C
root -b -l -q idmvaInputs_EB.C
root -b -l -q idmvaInputs_EE.C
root -b -l -q sigmaE_interp.C
root -b -l -q 'BDT_Zee_bdtin_cat.C(0)'
root -b -l -q BDT_Zee_bdtin_cat.C
root -b -l -q ptRescale.C
root -b -l -q rhoVsNvtx.C
root -b -l -q 'zee_mass.C(0)'
root -b -l -q zee_mass.C
root -b -l -q zee_mass_basecat2x2.C
root -b -l -q 'zee_mass_passCiC.C(0)'
root -b -l -q zee_mass_passCiC.C
