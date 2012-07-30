import FWCore.ParameterSet.Config as cms
import copy

#DATA TYPE
flagData = 'OFF'
flagMC = 'OFF'
flagFastSim = 'OFF'
flagPG = 'OFF'

#SKIM TYPE
flagSkimDiphoton = 'OFF'
flagSkimPJet = 'OFF'
flagVLPreSelection = 'OFF'
flagMyPreSelection = 'OFF'
flagNoSkim = 'OFF'
flagMMgSkim = 'OFF'
flagSkimworz = 'OFF'
flagSkim1El = 'OFF'
flagAddPdfWeight = 'OFF'

#ADDITIONAL OPTIONS
flagAOD = 'ON'
jobMaker = 'jobmaker unknown'

if (not((flagNoSkim is 'ON') ^ (flagSkimDiphoton is 'ON') ^ (flagMMgSkim is 'ON') ^ (flagVLPreSelection is 'ON') ^ (flagSkim1El is 'ON') ^ (flagSkimworz is 'ON') ^ (flagMyPreSelection is 'ON') ^ (flagSkimPJet is 'ON'))):
  print "You must skim or not skim... these are your options"
  exit(-1)

process = cms.Process("Globe") 
process.load("Configuration.StandardSequences.GeometryDB_cff") 
process.load("HiggsAnalysis.HiggsTo2photons.h2ganalyzer_53X_cfi")
#pi0 disc
process.load("RecoEcal.EgammaClusterProducers.preshowerClusterShape_cfi")
process.load("EgammaAnalysis.PhotonIDProducers.piZeroDiscriminators_cfi")
#FIXME
process.piZeroDiscriminators.preshClusterShapeProducer = cms.string('multi5x5PreshowerClusterShape')
process.piZeroDiscriminators.preshClusterShapeCollectionX = cms.string('multi5x5PreshowerXClustersShape')    
process.piZeroDiscriminators.preshClusterShapeCollectionY = cms.string('multi5x5PreshowerYClustersShape')

if flagAOD is 'OFF':
  #rerun ConvId
  process.load("RecoEgamma.EgammaPhotonProducers.conversionTrackSequence_cff")
  # NOTICE: You need the following two python files to rerun the conversion tracking with ECAL association
  process.load("RecoEgamma.EgammaPhotonProducers.conversionTrackCandidates_cfi")
  process.load("RecoEgamma.EgammaPhotonProducers.ckfOutInTracksFromConversions_cfi")
  ###############

process.load("Configuration.StandardSequences.FrontierConditions_GlobalTag_cff")
process.load("PhysicsTools/PatAlgos/patSequences_cff")
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.StandardSequences.Mixing')
process.load('Configuration.StandardSequences.MagneticField_38T_cff')
process.load('Configuration.StandardSequences.RawToDigi_cff')
process.load('Configuration.StandardSequences.L1Reco_cff')
process.load('Configuration.StandardSequences.Reconstruction_cff')
process.load('HiggsAnalysis.HiggsTo2photons.ZMuSkim_cff')
process.load('HiggsAnalysis.HiggsTo2photons.photonReRecoForMMG_cfi')
process.load('HiggsAnalysis.HiggsTo2photons.cicFilter_cfi')

if flagSkimDiphoton == 'ON':
  process.load('HLTrigger.HLTfilters.hltHighLevel_cfi')
  process.DiPhotonHltFilter = copy.deepcopy(process.hltHighLevel)
  process.DiPhotonHltFilter.throw = cms.bool(False)
  process.DiPhotonHltFilter.HLTPaths = ["HLT_Photon*_CaloId*_Iso*_Photon*_CaloId*_Iso*_*","HLT_Photon*_CaloId*_Iso*_Photon*_R9Id*_*","HLT_Photon*_R9Id*_Photon*_CaloId*_Iso*_*","HLT_Photon*_R9Id*_Photon*_R9Id*_*","HLT_Photon*_R9Id*_OR_CaloId*_Iso*_Photon*_R9Id*_OR_CaloId*_Iso*_*","HLT_Photon*_R9Id*_OR_CaloId*_Iso*_Photon*_*"]
  #process.load('Configuration.Skimming.PDWG_DiPhoton_SD_cff')

if flagSkimPJet == 'ON':
  process.load('HLTrigger.HLTfilters.hltHighLevel_cfi')
  process.PhotonHltFilter = copy.deepcopy(process.hltHighLevel)
  process.PhotonHltFilter.throw = cms.bool(False)
  process.PhotonHltFilter.HLTPaths = ["HLT_Photon*_CaloIdVL_IsoL_v*"]

process.load("HiggsAnalysis.HiggsTo2photons.photonInvariantMassFilter_cfi")
process.load("HiggsAnalysis.HiggsTo2photons.CMSSW_RelValDUMMY_cfi")
#process.source.skipEvents = cms.untracked.uint32(3500)

hltLabel = "HLT"

process.options = cms.untracked.PSet(
    wantSummary = cms.untracked.bool(True)
)

process.load("FWCore.MessageLogger.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 500
process.maxEvents = cms.untracked.PSet(input = cms.untracked.int32(-1))

process.superClusterMerger =  cms.EDProducer("EgammaSuperClusterMerger",
                                             src = cms.VInputTag(cms.InputTag('correctedHybridSuperClusters'),
                                                                 cms.InputTag('correctedMulti5x5SuperClustersWithPreshower'))
                                             )


process.primaryVertexFilter = cms.EDFilter("GoodVertexFilter",
                                           vertexCollection = cms.InputTag('offlinePrimaryVertices'),
                                           minimumNDOF = cms.uint32(4) ,
                                           maxAbsZ = cms.double(15),
                                           maxd0 = cms.double(2)
                                           )

process.goodPhotonsLowPtCut = cms.EDFilter("PhotonSelector",
                                      src = cms.InputTag("photons"),
                                      cut = cms.string(
                                      "abs(superCluster.eta) < 2.5"
                                      " && superCluster.energy*sin(superCluster.position.theta) > 20."
                                      " && hadronicOverEm < 0.5 "
                                      " && trkSumPtHollowConeDR03 < 2.0*(3.5 + 0.001*superCluster.energy*sin(superCluster.position.theta))"
                                      " && ecalRecHitSumEtConeDR03 < 2.0*(4.2 + 0.006*superCluster.energy*sin(superCluster.position.theta))"
                                      " && hcalTowerSumEtConeDR03 < 2.0*(2.2 + 0.0025*superCluster.energy*sin(superCluster.position.theta))"
                                                      )
                                   )

process.TwoPhotonsLowPtCut = cms.EDFilter("CandViewCountFilter",
                                       src = cms.InputTag("goodPhotonsLowPtCut"),
                                       minNumber = cms.uint32(2)
                                     )

process.goodPhotonsHighPtCut = cms.EDFilter("PhotonSelector",
                                      src = cms.InputTag("photons"),
                                      cut = cms.string(
                                      "abs(superCluster.eta) < 2.5"
                                      " && superCluster.energy*sin(superCluster.position.theta) > 30."
                                      " && hadronicOverEm < 0.5 "
                                      " && trkSumPtHollowConeDR03 < 2.0*(3.5 + 0.001*superCluster.energy*sin(superCluster.position.theta))"
                                      " && ecalRecHitSumEtConeDR03 < 2.0*(4.2 + 0.006*superCluster.energy*sin(superCluster.position.theta))"
                                      " && hcalTowerSumEtConeDR03 < 2.0*(2.2 + 0.0025*superCluster.energy*sin(superCluster.position.theta))"
                                                      )
                                   )

process.OnePhotonsHighPtCut = cms.EDFilter("CandViewCountFilter",
                                       src = cms.InputTag("goodPhotonsHighPtCut"),
                                       minNumber = cms.uint32(1)
                                     )


process.dummySelector = cms.EDFilter("CandViewCountFilter",
                                     src = cms.InputTag("gsfElectrons"),
                                     minNumber = cms.uint32(0)
                                     )

process.goodElectronsOver5 = cms.EDFilter("GsfElectronSelector",
                                          filter = cms.bool(True),
                                          src = cms.InputTag("gsfElectrons"),
                                          cut = cms.string('superCluster().rawEnergy()*sin((2*atan(exp(superCluster().eta())))) > 5.')
                                          )

process.superClusterMerger =  cms.EDProducer("EgammaSuperClusterMerger",
                                             src = cms.VInputTag(cms.InputTag('correctedHybridSuperClusters'),
                                                                 cms.InputTag('correctedMulti5x5SuperClustersWithPreshower'))
                                             )

process.goodSCOver5 = cms.EDFilter("SuperClusterSelector",
                                   filter = cms.bool(True),
                                   src = cms.InputTag("superClusterMerger"),
                                   cut = cms.string('rawEnergy()*sin((2*atan(exp(eta())))) > 5.')
                                   )

process.pdfWeights = cms.EDProducer("PdfWeightProducer",
                                    #FixPOWHEG = cms.untracked.string("cteq66.LHgrid"),
                                    #GenTag = cms.untracked.InputTag("genParticles"),
                                    PdfInfoTag = cms.untracked.InputTag("generator"),
                                    PdfSetNames = cms.untracked.vstring("cteq66.LHgrid")
                                    )

#process.goodEvents = cms.Sequence(process.noScraping * process.primaryVertexFilter)
#process.pathToCheck = cms.Sequence(process.L10and34 *process.noScraping*process.primaryVertexFilter*process.HFCoincidence*process.L140or41)
#process.pathToCheck2 = cms.Sequence(process.L10and34 *process.noScraping*process.primaryVertexFilter*process.L140or41*process.HFCoincidence)

process.diMuonSelSeq.remove(process.ZMuHLTFilter)

if flagVLPreSelection == 'ON':
  process.eventFilter1 = cms.Sequence(process.superClusterMerger*process.goodPhotonsLowPtCut*process.TwoPhotonsLowPtCut+process.photonInvariantMassFilter) # for bkg
  process.eventFilter2 = cms.Sequence(process.superClusterMerger*process.goodPhotonsLowPtCut*process.TwoPhotonsLowPtCut+process.photonInvariantMassFilter) # for bkg
elif flagMyPreSelection == 'ON':
  process.eventFilter1 = cms.Sequence(process.cicFilterSequence) 
  process.eventFilter2 = cms.Sequence(process.cicFilterSequence)
elif flagSkimDiphoton == 'ON':
  process.eventFilter1 = cms.Sequence(process.DiPhotonHltFilter) # for some data
  process.eventFilter2 = cms.Sequence(process.DiPhotonHltFilter)      # for some data
  #process.eventFilter1 = cms.Sequence(process.CaloIdIsoPhotonPairsFilter) # for some data
  #process.eventFilter2 = cms.Sequence(process.R9IdPhotonPairsFilter)      # for some data
elif flagData == 'ON' and flagSkimPJet == 'ON':
  process.eventFilter1 = cms.Sequence(process.PhotonHltFilter)
  process.eventFilter2 = cms.Sequence(process.PhotonHltFilter)
elif flagMC == 'ON' and flagSkimPJet == 'ON':
  process.eventFilter1 = cms.Sequence(process.PhotonHltFilter*process.superClusterMerger*process.goodPhotonsHighPtCut*process.OnePhotonsHighPtCut) # for bkg
  process.eventFilter2 = cms.Sequence(process.PhotonHltFilter*process.superClusterMerger*process.goodPhotonsHighPtCut+process.OnePhotonsHighPtCut) # for bkg
elif flagNoSkim == 'ON':    
  process.eventFilter1 = cms.Sequence(process.dummySelector)   #for signal MC
  process.eventFilter2 = cms.Sequence(process.dummySelector)   #for signal MC
elif flagMMgSkim == 'ON':
  process.eventFilter1 = cms.Sequence(process.diMuonSelSeq*process.photonReReco)
  process.eventFilter2 = cms.Sequence(process.diMuonSelSeq*process.photonReReco)
elif flagSkimworz == 'ON':
  process.load('HiggsAnalysis.HiggsTo2photons.eidFilter_cfi')
  process.load('HiggsAnalysis.HiggsTo2photons.invariantMassFilter_cfi')
  process.eventFilter1 = cms.Sequence(process.goodElectronsOver5*process.superClusterMerger*process.goodSCOver5*process.invariantMassFilter*process.electronIdentificationFilter)
  process.eventFilter2= cms.Sequence(process.goodElectronsOver5*process.superClusterMerger*process.goodSCOver5*process.invariantMassFilter*process.electronIdentificationFilter)
elif flagSkim1El == 'ON':
  process.eventFilter1 = cms.Sequence(process.goodElectronsOver5)
  process.eventFilter2 = cms.Sequence(process.goodElectronsOver5)

process.h2ganalyzer.RootFileName = 'aod_mc_test.root'
process.h2ganalyzer.Debug_Level = 0

##-------------------- PFIsolation for Electrons ---------------------
from CommonTools.ParticleFlow.Tools.pfIsolation import setupPFElectronIso, setupPFPhotonIso
process.eleIsoSequence = setupPFElectronIso(process, 'gsfElectrons')
#process.phoIsoSequence = setupPFPhotonIso(process, 'photons')
##-------------------- PFNoPU for PF Isolation Electrons -------------
process.load("CommonTools.ParticleFlow.pfPileUp_cfi")
process.pfPileUp.PFCandidates = cms.InputTag("particleFlow")
##-------------------- Import the JEC services -----------------------
process.load('JetMETCorrections.Configuration.DefaultJEC_cff')
##-------------------- Import the Jet RECO modules -----------------------
#process.load('RecoJets.Configuration.RecoPFJets_cff')
#process.kt6PFJets = process.kt6PFJets.clone(rParam = 0.6, doRhoFastjet = True)
#process.ak5PFJets.doAreaFastjet = True
#process.kt6PFJetsForRhoCorrection = process.kt6PFJets.clone(rParam = 0.6, doRhoFastjet = True)
#process.kt6PFJetsForRhoCorrection.Rho_EtaMax = cms.double(2.5)

##-------------------- Filter to skip bugged events with non conserved energy -------
process.load("GeneratorInterface.GenFilters.TotalKinematicsFilter_cfi")

# event counters
process.processedEvents = cms.EDProducer("EventCountProducer")

process.eventCounters = cms.Sequence(process.processedEvents)

if (flagFastSim == 'OFF' and flagMC == 'ON' and flagPG == 'OFF'):
  process.eventCounters = cms.Sequence(process.totalKinematicsFilter * process.processedEvents)

if (flagAddPdfWeight == 'ON' and flagMC == 'ON'):
  process.eventCounters *= cms.Sequence(process.pdfWeights)

process.h2ganalyzer.globalCounters.extend(['processedEvents']) 

# PFIsolation photons
#process.load("HiggsAnalysis.HiggsTo2photons.pfIsolation_cff")

process.h2ganalyzerPath = cms.Sequence(process.h2ganalyzer)

#################################################
#       Charged Hadron Subtraction Jets         #
#################################################

from RecoJets.JetProducers.ak5PFJets_cfi import *
process.ak5PFchsJets = ak5PFJets.clone()
process.ak5PFchsJets.src = 'pfNoPileUp'

#And the sequence below re-runs the pfNoPu on the fly
process.load("CommonTools.ParticleFlow.pfNoPileUp_cff")
process.load("CommonTools.ParticleFlow.pfParticleSelection_cff")


# note pfPileUp modified according to JetMET's recommendations
process.pfPileUp.checkClosestZVertex = False
process.pfPileUp.Vertices = 'goodOfflinePrimaryVertices'

process.load("CommonTools.ParticleFlow.goodOfflinePrimaryVertices_cfi")
process.pfNoPileUpSequence.insert(0, process.goodOfflinePrimaryVertices)

process.ak5PFchsL1Fastjet = cms.ESProducer(
    'L1FastjetCorrectionESProducer',
    level       = cms.string('L1FastJet'),
    algorithm   = cms.string('AK5PFchs'),
    srcRho      = cms.InputTag('kt6PFJets','rho')
)

process.ak5PFchsL2Relative = process.ak5CaloL2Relative.clone( algorithm = 'AK5PFchs' )
process.ak5PFchsL3Absolute     = process.ak5CaloL3Absolute.clone( algorithm = 'AK5PFchs' )
process.ak5PFchsResidual  = process.ak5CaloResidual.clone( algorithm = 'AK5PFchs' )

process.ak5PFchsL1FastL2L3 = cms.ESProducer(
  'JetCorrectionESChain',
  correctors =
  cms.vstring('ak5PFchsL1Fastjet','ak5PFchsL2Relative',
              'ak5PFchsL3Absolute')
)

process.ak5PFchsL1FastL2L3Residual = cms.ESProducer(
  'JetCorrectionESChain',
  correctors =
  cms.vstring('ak5PFchsL1Fastjet','ak5PFchsL2Relative',
              'ak5PFchsL3Absolute','ak5PFchsResidual')
  )


#################################################
# Addition for Type1 pfMET corrections          #
#################################################

process.load("JetMETCorrections.Type1MET.pfMETCorrections_cff") 


#################################################
# Define path, first for AOD case then for RECO #
#################################################

process.p11 = cms.Path(process.eventCounters*process.eventFilter1* process.pfNoPileUpSequence * process.pfParticleSelectionSequence * process.eleIsoSequence*process.ak5PFchsJets*process.producePFMETCorrections  )
#process.p11 = cms.Path(process.eventCounters*process.eventFilter1* process.pfNoPileUpSequence * process.pfParticleSelectionSequence * process.eleIsoSequence*process.ak5PFchsJets*process.pfType1CorrectedMet  )


if (flagFastSim == 'OFF' or flagAOD == 'OFF'):
  process.p11 *= process.piZeroDiscriminators
    
#process.p11 *= (process.kt6PFJets* process.ak5PFJets* process.kt6PFJetsForRhoCorrection* process.h2ganalyzerPath)
process.p11 *= (process.h2ganalyzerPath)

process.p12 = copy.deepcopy(process.p11)
process.p12.replace(process.eventFilter1, process.eventFilter2)

if (flagAOD is 'OFF'):
  process.p11.insert(-1, (process.conversionTrackCandidates*process.ckfOutInTracksFromConversions*process.preshowerClusterShape*process.piZeroDiscriminators))

  process.p12.insert(-1, (process.conversionTrackCandidates*process.ckfOutInTracksFromConversions*process.preshowerClusterShape*process.piZeroDiscriminators))

#################################################
# End of Path definition                        #
#################################################

process.h2ganalyzer.JobMaker = jobMaker

if (flagAddPdfWeight == 'ON'):
  process.h2ganalyzer.doPdfWeight = True

if (flagFastSim is 'ON'):
  process.h2ganalyzer.doFastSim = True
  
if (flagMC is 'ON' and flagFastSim is 'ON'):
  process.h2ganalyzer.doGenJet_algo1 = False
  process.h2ganalyzer.doGenJet_algo2 = False
  process.h2ganalyzer.doGenJet_algo3 = False
  process.h2ganalyzer.doGenParticles = False
  process.h2ganalyzer.doGenMet = False
  process.h2ganalyzer.doGenVertices = False
elif ((flagMC is 'ON' and flagFastSim is 'OFF') or flagPG is 'ON'):
  process.h2ganalyzer.doGenJet_algo1 = True
  process.h2ganalyzer.doGenJet_algo2 = True
  process.h2ganalyzer.doGenJet_algo3 = True
  process.h2ganalyzer.doGenParticles = True
  process.h2ganalyzer.doGenMet = True
  process.h2ganalyzer.doGenVertices = True
elif flagData is 'ON':
  process.h2ganalyzer.doPileup = False
  process.h2ganalyzer.doGenJet_algo1 = False
  process.h2ganalyzer.doGenJet_algo2 = False
  process.h2ganalyzer.doGenJet_algo3 = False
  process.h2ganalyzer.doGenParticles = False
  process.h2ganalyzer.doGenVertices = False
  process.h2ganalyzer.doGenMet = False

if ((flagMC is 'ON' or flagPG is 'ON') and flagAOD is 'OFF'):
  process.h2ganalyzer.doSimTracks = True
  process.h2ganalyzer.doSimTrackPlusSimVertex = False

if flagAOD is 'ON':
  process.h2ganalyzer.doAodSim = True
  process.h2ganalyzer.doHcal = False
  process.h2ganalyzer.doHFHcal = False
  process.h2ganalyzer.doPreshowerHits = False
else:
  process.h2ganalyzer.doAodSim = False
  process.h2ganalyzer.doHcal = True
  process.h2ganalyzer.doHFHcal = True
  process.h2ganalyzer.doPreshowerHits = True
  process.h2ganalyzer.EcalHitEBColl = cms.InputTag("ecalRecHit","EcalRecHitsEB")
  process.h2ganalyzer.EcalHitEEColl = cms.InputTag("ecalRecHit","EcalRecHitsEE")
  process.h2ganalyzer.EcalHitESColl = cms.InputTag("ecalPreshowerRecHit","EcalRecHitsES")
  process.h2ganalyzer.HcalHitsBEColl = cms.InputTag("hbhereco")
  process.h2ganalyzer.HcalHitsFColl = cms.InputTag("hfreco")
  process.h2ganalyzer.HcalHitsHoColl = cms.InputTag("horeco")
  process.h2ganalyzer.BarrelBasicClusterColl = cms.InputTag("")
  process.h2ganalyzer.BarrelBasicClusterShapeColl = cms.InputTag("multi5x5BasicClusters","multi5x5BarrelShapeAssoc")
  process.h2ganalyzer.JetTrackAssociationColl_algo3 = cms.InputTag("kt4JetTracksAssociatorAtVertex")

if (flagPG is 'OFF'):
  process.h2ganalyzer.doL1 = True
  process.h2ganalyzer.doHLT = True
else:
  process.h2ganalyzer.doL1 = False
  process.h2ganalyzer.doHLT = False
  process.h2ganalyzer.doJet_algoPF3 = False
  process.h2ganalyzer.doParticleGun = True
  
process.GlobalTag.globaltag = "START53_V9::All"
process.h2ganalyzer.HLTParameters.PrimaryTriggerResultsTag = cms.InputTag("TriggerResults","", hltLabel)
process.h2ganalyzer.HLTParameters.useSecondaryTrigger = cms.bool(False)
process.h2ganalyzer.HLTParameters.TriggerResultsTag = cms.InputTag("hltTriggerSummaryAOD","", hltLabel)
