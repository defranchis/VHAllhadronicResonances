// $Id: CycleCreators.py 344 2012-12-13 13:10:53Z krasznaa $

// Local include(s):
#include "../include/WHAnalysis.h"

// External include(s):
#include "../../GoodRunsLists/include/TGoodRunsListReader.h"

#include <TMath.h>

ClassImp( WHAnalysis );

// define cut names

const std::string WHAnalysis::kCutName[ WHAnalysis::kNumCuts ] = {
  "BeforeCuts",            // C0
  "JSON",                  // C1
  "Trigger",               // C2
  "MetFilters",            // C3
  "TwoFatJets",            // C4
  "FatJetsDeltaEta",       // C5
  "DijetMass",             // C6
  "Tau21HP",               // C7
  "VWindow",               // C8
  "HiggsWindow",           // C9
  "SubjetSingleTag",       // C10
  "SubjetDoubleTag"        // C11
};

WHAnalysis::WHAnalysis()
   : SCycleBase()
   , m_jetAK4( this )
   , m_jetAK8( this )
   , m_eventInfo( this )
   , m_electron( this )
   , m_muon( this )
   , m_missingEt( this )
   , m_genParticle( this )
   , m_pileupReweightingTool( this )
{

   m_logger << INFO << "Hello!" << SLogger::endmsg;
   SetLogName( GetName() );
   
   // read configuration details from XML file
   // (defaults agree with July 2010 acceptance challenge)
   DeclareProperty( "RecoTreeName",             m_recoTreeName             = "physics" );
   DeclareProperty( "OutputTreeName",           m_outputTreeName           = "analysis" );
   DeclareProperty( "JetAK4Name",               m_jetAK4Name               = "jetAK4" );
   DeclareProperty( "JetAK8Name",               m_jetAK8Name               = "jetAK8" );
   DeclareProperty( "ElectronName",             m_electronName             = "el" );
   DeclareProperty( "MuonName",                 m_muonName                 = "mu" );
   DeclareProperty( "MissingEtName",            m_missingEtName            = "MET" );
   DeclareProperty( "GenParticleName",          m_genParticleName          = "genParticle" );
   
   DeclareProperty( "IsData",                   m_isData                   = false );
   DeclareProperty( "IsSignal",                 m_isSignal                 = true );
   DeclareProperty( "ApplyMETFilters",          m_applyMETFilters          = true );
   
   DeclareProperty( "JetPtCut",                 m_jetPtCut           = 200. );
   DeclareProperty( "JetEtaCut",                m_jetEtaCut          = 2.4  );
   DeclareProperty( "MjjCut",                   m_mjjCut             = 1000 );
   DeclareProperty( "JetDeltaEtaCut",           m_jetDeltaEtaCut     = 1.3  );

   DeclareProperty( "Tau21HPCut",                m_tau21HPCut             = 0.45 );
   DeclareProperty( "Tau21LPCut",                m_tau21LPCut             = 0.75 );
   DeclareProperty( "MWLowerCut",                m_mWLowerCut             = 65 );
   DeclareProperty( "MWUpperCut",                m_mWUpperCut             = 85 );
   DeclareProperty( "MZLowerCut",                m_mZLowerCut             = 85 );
   DeclareProperty( "MZUpperCut",                m_mZUpperCut             = 105 );
   DeclareProperty( "MHLowerCut",                m_mHLowerCut             = 105 );
   DeclareProperty( "MHUpperCut",                m_mHUpperCut             = 135 );
   
   DeclareProperty( "CSVMin",                m_csvMin             = 0.3 );
   
   DeclareProperty( "ElectronPtCut",                 m_electronPtCut              = 200. );
   DeclareProperty( "ElectronEtaCut",                m_electronEtaCut             = 2.4 );
   
   DeclareProperty( "MuonPtCut",                 m_muonPtCut              = 200. );
   DeclareProperty( "MuonEtaCut",                m_muonEtaCut             = 2.4 );
   
   DeclareProperty( "METCut",                m_metCut             = 2.4 );
   
   DeclareProperty( "JSONName",                 m_jsonName             = std::string (std::getenv("SFRAME_DIR")) + "/../GoodRunsLists/JSON/Cert_246908-260627_13TeV_PromptReco_Collisions15_25ns_JSON_Silver_v2.txt" );
   
   
}

WHAnalysis::~WHAnalysis() {

  m_logger << INFO << "Tschoe!" << SLogger::endmsg;

}

void WHAnalysis::BeginCycle() throw( SError ) {

  m_logger << INFO << "Hello to you!" << SLogger::endmsg;
  
  //
  // Load the GRL:
  //
  if (m_isData) {
    m_logger << INFO << "Loading GoodRunsList from file: " << m_jsonName << SLogger::endmsg;
    Root::TGoodRunsListReader reader( TString( m_jsonName.c_str() ) );
    if( ! reader.Interpret() ) {
      m_logger << FATAL << "Couldn't read in the GRL!" << SLogger::endmsg;
      throw SError( ( "Couldn't read in file: " + m_jsonName ).c_str(), SError::SkipCycle );
    }
    m_grl = reader.GetMergedGoodRunsList();
    m_grl.Summary();
    m_grl.SetName( "MyGoodRunsList" );
  
    //
    // Add it as a configuration object, so that the worker nodes will receive it:
    //
    AddConfigObject( &m_grl );
		
    // m_logger << INFO << "Loading RunEventFilter from file: " << m_runEventFilterName << SLogger::endmsg;
    // m_runEventFilterReader.SetTextFile( TString( m_runEventFilterName.c_str() ) );
    // if( ! m_runEventFilterReader.Interpret() ) {
    //   m_logger << FATAL << "Couldn't read in the RunEventFilter file!" << SLogger::endmsg;
    //   throw SError( ( "Couldn't read in file: " + m_runEventFilterName ).c_str(), SError::SkipCycle );
    // }
		
  }
  
  m_triggerNames.clear();
	
  //Dijet triggers
  m_triggerNames.push_back("AK8PFJet360_TrimMass30") ;
  m_triggerNames.push_back("AK8PFHT700_TrimR0p1PT0p03Mass50") ;
  // trignames.push_back("AK8DiPFJet280_200_TrimMass30_BTagCSV0p45") ;
  m_triggerNames.push_back("PFHT650_WideJetMJJ950DEtaJJ1p5") ;
  m_triggerNames.push_back("PFHT650_WideJetMJJ900DEtaJJ1p5") ;
  m_triggerNames.push_back("PFHT800_v") ;
  
   return;

}

void WHAnalysis::EndCycle() throw( SError ) {

   return;

}

void WHAnalysis::BeginInputData( const SInputData& id ) throw( SError ) {

  m_logger << INFO << "RecoTreeName:         " <<             m_recoTreeName << SLogger::endmsg;
  m_logger << INFO << "OutputTreeName:       " <<             m_outputTreeName << SLogger::endmsg;
  m_logger << INFO << "JetAK4Name:           " <<             m_jetAK4Name << SLogger::endmsg;
  m_logger << INFO << "JetAK8Name:           " <<             m_jetAK8Name << SLogger::endmsg;
  m_logger << INFO << "ElectronName:         " <<             m_electronName << SLogger::endmsg;
  m_logger << INFO << "MuonName:             " <<             m_muonName << SLogger::endmsg;
  m_logger << INFO << "GenParticleName:      " <<             m_genParticleName << SLogger::endmsg;
  
  m_logger << INFO << "IsData:           " <<                   (m_isData ? "TRUE" : "FALSE") << SLogger::endmsg;
  m_logger << INFO << "IsSignal:           " <<                 (m_isSignal ? "TRUE" : "FALSE") << SLogger::endmsg;
  m_logger << INFO << "ApplyMETFilters:           " <<          (m_applyMETFilters ? "TRUE" : "FALSE") << SLogger::endmsg;
  
  m_logger << INFO << "JetPtCut:           " <<                 m_jetPtCut << SLogger::endmsg;
  m_logger << INFO << "JetEtaCut:           " <<                m_jetEtaCut << SLogger::endmsg;
  m_logger << INFO << "MjjCut:           " <<                m_mjjCut << SLogger::endmsg;
  m_logger << INFO << "JetDeltaEtaCut:           " <<                m_jetDeltaEtaCut << SLogger::endmsg;

  m_logger << INFO << "Tau21HPCut:           " <<                m_tau21HPCut << SLogger::endmsg;
  m_logger << INFO << "Tau21LPCut:           " <<                m_tau21LPCut << SLogger::endmsg;
  m_logger << INFO << "MWLowerCut:           " <<                m_mWLowerCut << SLogger::endmsg;
  m_logger << INFO << "MWUpperCut:           " <<                m_mWUpperCut << SLogger::endmsg;
  m_logger << INFO << "MZLowerCut:           " <<                m_mZLowerCut << SLogger::endmsg;
  m_logger << INFO << "MZUpperCut:           " <<                m_mZUpperCut << SLogger::endmsg;
  m_logger << INFO << "MHLowerCut:           " <<                m_mHLowerCut << SLogger::endmsg;
  m_logger << INFO << "MHUpperCut:           " <<                m_mHUpperCut << SLogger::endmsg;
  
  m_logger << INFO << "CSVMin:           " <<                m_csvMin << SLogger::endmsg;
  
  m_logger << INFO << "ElectronPtCut:           " <<                 m_electronPtCut << SLogger::endmsg;
  m_logger << INFO << "ElectronEtaCut:           " <<                m_electronEtaCut << SLogger::endmsg;
  
  m_logger << INFO << "MuonPtCut:           " <<                 m_muonPtCut << SLogger::endmsg;
  m_logger << INFO << "MuonEtaCut:           " <<                m_muonEtaCut << SLogger::endmsg;
  
  m_logger << INFO << "METCut:           " <<                m_metCut << SLogger::endmsg;
  
  m_logger << INFO << "JSONName:           " <<                 m_jsonName << SLogger::endmsg;
  
  if (!m_isData) m_pileupReweightingTool.BeginInputData( id );
  
  if (m_isData) {
    TObject* grl;
    if( ! ( grl = GetConfigObject( "MyGoodRunsList" ) ) ) {
      m_logger << FATAL << "Can't access the GRL!" << SLogger::endmsg;
      throw SError( "Can't access the GRL!", SError::SkipCycle );
    }
    m_grl = *( dynamic_cast< Root::TGoodRunsList* >( grl ) );
  }
  
  //
  // output branches
  //
  DeclareVariable(b_weight              , "weight"                 );
  DeclareVariable(b_weightGen           , "weightGen"              );
  DeclareVariable(b_weightPU            , "weightPU"               );
  
  DeclareVariable(b_weightPU            , "weightPU"               );
  
  DeclareVariable( b_ak8jets_pt,           "b_ak8jets_pt"          );
  DeclareVariable( b_ak8jets_phi,          "b_ak8jets_phi"         );
  DeclareVariable( b_ak8jets_eta,          "b_ak8jets_eta"         );
  DeclareVariable( b_ak8jets_e,            "b_ak8jets_e"           );
  DeclareVariable( b_ak8jets_tau21,        "b_ak8jets_tau21"       );
  DeclareVariable( b_ak8jets_m,            "b_ak8jets_m"           );
  DeclareVariable( b_ak8jets_mpruned,      "b_ak8jets_mpruned"     );
  DeclareVariable( b_ak8jets_csv,          "b_ak8jets_csv"         );
  DeclareVariable( b_ak8jets_subjets_dr,   "b_ak8jets_subjets_dr"  );
  DeclareVariable( b_ak8jets_subjets_deta, "b_ak8jets_subjets_deta");
  DeclareVariable( b_ak8jets_subjets_dphi, "b_ak8jets_subjets_dphi");
  DeclareVariable( b_ak8jets_subjet1_pt,   "b_ak8jets_subjet1_pt"  );
  DeclareVariable( b_ak8jets_subjet2_pt,   "b_ak8jets_subjet2_pt"  );
  DeclareVariable( b_ak8jets_subjet1_csv,  "b_ak8jets_subjet1_csv" );
  DeclareVariable( b_ak8jets_subjet2_csv,  "b_ak8jets_subjet2_csv" );
  
  DeclareVariable(b_selection_bits            , "selection_bits"   );
  DeclareVariable(b_selection_lastcut         , "selection_lastcut");
  
  //
  // Declare the output histograms:
  //
  Book( TH1F( "cutflow", "cutflow", 20, 0.5, 20.5 ));
  Book( TH1F( "METFilters", "METFilters", 20, 0.5, 20.5 ));
  
   return;

}

void WHAnalysis::EndInputData( const SInputData& ) throw( SError ) {

  //
  // Final analysis of cut flow
  //
  
  m_logger << INFO << "cut flow:" << SLogger::endmsg;
  m_logger << INFO << Form( "Cut\t%25.25s\tEvents\tRelEff\tAbsEff", "Name" ) << SLogger::endmsg;
  
  Double_t ntot = Hist( "cutflow" )->GetBinContent( 1 );
  m_logger << INFO << Form( "\t%25.25s\t%6.0f", "start", ntot ) << SLogger::endmsg;
  for( Int_t ibin = 2; ibin <= kNumCuts; ++ibin ) {
    Int_t    icut    = ibin - 1;
    Double_t nevents = Hist( "cutflow" )->GetBinContent( ibin );
    Double_t releff  = 100. * nevents / Hist( "cutflow" )->GetBinContent( ibin-1 );
    Double_t abseff  = 100. * nevents / ntot;
    m_logger << INFO  << Form( "C%d\t%25.25s\t%6.0f\t%6.2f\t%6.2f", icut-1, kCutName[icut].c_str(), nevents, releff, abseff ) << SLogger::endmsg;
  }
   
   return;

}

void WHAnalysis::BeginInputFile( const SInputData& ) throw( SError ) {

  m_logger << INFO << "Connecting input variables" << SLogger::endmsg;
  if (m_isData) {
    // m_jetAK4.ConnectVariables(       m_recoTreeName.c_str(), D3PD::JetBasic|D3PD::JetAnalysis, (m_jetAK4Name + "_").c_str() );
    m_jetAK8.ConnectVariables(       m_recoTreeName.c_str(), D3PD::JetBasic|D3PD::JetAnalysis|D3PD::JetSubstructure|D3PD::JetPrunedSubjets, (m_jetAK8Name + "_").c_str() );
    m_eventInfo.ConnectVariables(    m_recoTreeName.c_str(), D3PD::EventInfoBasic|D3PD::EventInfoTrigger|D3PD::EventInfoMETFilters, "" );
  }
  else {
    // m_jetAK4.ConnectVariables(       m_recoTreeName.c_str(), D3PD::JetBasic|D3PD::JetAnalysis|D3PD::JetTruth, (m_jetAK4Name + "_").c_str() );
    m_jetAK8.ConnectVariables(       m_recoTreeName.c_str(), D3PD::JetBasic|D3PD::JetAnalysis|D3PD::JetSubstructure|D3PD::JetTruth|D3PD::JetPrunedSubjets|D3PD::JetPrunedSubjetsTruth, (m_jetAK8Name + "_").c_str() );
    m_eventInfo.ConnectVariables(    m_recoTreeName.c_str(), D3PD::EventInfoBasic|D3PD::EventInfoTrigger|D3PD::EventInfoMETFilters|D3PD::EventInfoTruth, "" );
  }
  // m_electron.ConnectVariables(     m_recoTreeName.c_str(), D3PD::ElectronBasic|D3PD::ElectronID, (m_electronName + "_").c_str() );
  // m_muon.ConnectVariables(         m_recoTreeName.c_str(), D3PD::MuonBasic|D3PD::MuonID|D3PD::MuonIsolation, (m_muonName + "_").c_str() );
  // m_missingEt.ConnectVariables(    m_recoTreeName.c_str(), D3PD::MissingEtBasic, (m_missingEtName + "_").c_str() );
  m_genParticle.ConnectVariables(  m_recoTreeName.c_str(), D3PD::GenParticleBasic, (m_genParticleName + "_").c_str() );
  m_logger << INFO << "Connecting input variables completed" << SLogger::endmsg;

   return;

}

void WHAnalysis::ExecuteEvent( const SInputData&, Double_t ) throw( SError ) {

  m_logger << VERBOSE << "ExecuteEvent" << SLogger::endmsg;
  
  clearBranches();
  TBits selectionBits(kNumCuts);
  selectionBits.SetBitNumber( kBeforeCuts );
  bool moveOn = true;
  
  // Cut 1: check for data if run/lumiblock in JSON
  if (m_isData) {
    if (isGoodEvent(m_eventInfo.runNumber, m_eventInfo.lumiBlock)) {
      selectionBits.SetBitNumber( kJSON );
    }
  }
  else {
    selectionBits.SetBitNumber( kJSON );
  }
  
  // Cut 2: pass trigger
  if (passTrigger()) {
    m_logger << VERBOSE << "Trigger pass" << SLogger::endmsg;
    selectionBits.SetBitNumber( kTrigger );
  }
  
  // Cut 3: pass MET filters
  if (passMETFilters()) {
    m_logger << VERBOSE << "passMETFilters" << SLogger::endmsg;
    selectionBits.SetBitNumber( kMetFilters );
  }
  
  // Cut 4: select two fat jets
  std::vector<DESY::Jet> goodFatJets;
  for ( int i = 0; i < (m_jetAK8.N); ++i ) {
    DESY::Jet myjet( &m_jetAK8, i );
    if (myjet.pt() > m_jetPtCut) {
      if (fabs(myjet.eta()) < m_jetEtaCut) {
        if (myjet.IDTight()) {
          goodFatJets.push_back(myjet);
        }
      }
    }
  }
  if (goodFatJets.size() >= 2) {
    selectionBits.SetBitNumber( kTwoFatJets );
  }
  else {
    // can only continue with at least two selected fat jets
    moveOn = false;
  }
  
  int goodFatJet1Index = -1;
  int goodFatJet2Index = -1;
  
  if (moveOn) {
    // Cut 5: find two selected jets that are close in DeltaEta
  
    for (unsigned int i = 0; i < goodFatJets.size()-1; ++i) {
      for (unsigned int j = i+1; j < goodFatJets.size(); ++j) {
        if (fabs(goodFatJets[i].eta() - goodFatJets[j].eta()) < m_jetDeltaEtaCut) {
          selectionBits.SetBitNumber( kFatJetsDeltaEta );
          goodFatJet1Index = i;
          goodFatJet2Index = j;
        }
        if (goodFatJet1Index != -1) break;
      }
      if (goodFatJet1Index != -1) break;
    }
    if (!(selectionBits.TestBitNumber( kFatJetsDeltaEta ))) {
      moveOn = false;
    }
  }
  
  if (moveOn) {
  
    // Cut 6: require dijet system to pass mass threshold
    TLorentzVector dijet = goodFatJets[goodFatJet1Index].tlv() + goodFatJets[goodFatJet2Index].tlv();
    if (dijet.M() > m_mjjCut) {
      selectionBits.SetBitNumber( kDijetMass );
    }
  
    // Cut 7: pass tau21 for both jets
    bool jet1PassTau21 = false;
    bool jet2PassTau21 = false;
  
    if (goodFatJets[goodFatJet1Index].tau1() != 0) {
      if ((goodFatJets[goodFatJet1Index].tau2() / goodFatJets[goodFatJet1Index].tau1()) < m_tau21HPCut) {
        jet1PassTau21 = true;
      }
    }
    if (goodFatJets[goodFatJet2Index].tau1() != 0) {
      if ((goodFatJets[goodFatJet2Index].tau2() / goodFatJets[goodFatJet2Index].tau1()) < m_tau21HPCut) {
        jet2PassTau21 = true;
      }
    }
    if (jet1PassTau21 && jet2PassTau21) {
      selectionBits.SetBitNumber( kTau21HP );
    }
  
    // Cut 8: require one of the jets to be in the V-boson mass window
    // Should later use V/Z categorisation
    // make other selected jet the Higgs jet
    DESY::Jet vectorJet;
    DESY::Jet higgsJet;
    if ((goodFatJets[goodFatJet1Index].pruned_massCorr() > m_mWLowerCut) && (goodFatJets[goodFatJet1Index].pruned_massCorr() <= m_mZUpperCut)) {
      vectorJet = goodFatJets[goodFatJet1Index];
      higgsJet = goodFatJets[goodFatJet2Index];
      selectionBits.SetBitNumber( kVWindow );
    }
    else if ((goodFatJets[goodFatJet2Index].pruned_massCorr() > m_mWLowerCut) && (goodFatJets[goodFatJet2Index].pruned_massCorr() <= m_mZUpperCut)) {
      vectorJet = goodFatJets[goodFatJet2Index];
      higgsJet = goodFatJets[goodFatJet1Index];
      selectionBits.SetBitNumber( kVWindow );
    }

    // Cut 9: check if Higgs candidate jet is in Higgs mass window
    if ((higgsJet.pruned_massCorr() > m_mHLowerCut) && (higgsJet.pruned_massCorr() <= m_mHUpperCut)) {
      selectionBits.SetBitNumber( kHiggsWindow );
    }

    // Cut 10: require at least one of the subjets from the Higgs jet to be b-tagged
    // count number of b-tagged subjets
    int nTaggedSubjets = 0;
    for (int i = 0; i < higgsJet.subjet_pruned_N(); ++i) {
      if (higgsJet.subjet_pruned_csv()[i] > m_csvMin) {
        ++nTaggedSubjets;
      }
    }
    if (nTaggedSubjets >= 1) {
      selectionBits.SetBitNumber( kSubjetSingleTag );
    }
  
    // Cut 11: require two subjets from the Higgs jet to be b-tagged
    if (nTaggedSubjets >= 2) {
      selectionBits.SetBitNumber( kSubjetDoubleTag );
    }
  
    if (!m_isData) {
      b_weight = getEventWeight();
    }
  
    // std::vector<DESY::Jet> goodJetsAK4;
    // for ( int i = 0; i < (m_jetAK4.N); ++i ) {
    //   DESY::Jet myjet( &m_jetAK4, i );
    //   if (fabs(myjet.eta()) < m_jetEtaCut) {
    //     if (myjet.pt() > m_jetPtCut) {
    //       goodJetsAK4.push_back(myjet);
    //       // m_logger << INFO << myjet.pt() << SLogger::endmsg;
    //       Book( TH1F( "Jet_pt", "Jet p_{T} [GeV]", 100, 0.0, 1000 ), "AK4_low" )->Fill( myjet.pt() );
    //       Book( TH1F( "Jet_eta", "Jet #eta", 50, -2.5, 2.5 ), "AK4_low" )->Fill( myjet.eta() );
    //       Book( TH1F( "Jet_m", "Jet m [GeV]", 50, 0, 250 ), "AK4_low" )->Fill( myjet.m() );
    //       Book( TH1F( "Jet_phi", "Jet #phi", 50, -TMath::Pi(), TMath::Pi() ), "AK4_low" )->Fill( myjet.phi() );
    //       Book( TH1F( "Jet_e", "Jet e [GeV]", 100, 0.0, 1000 ), "AK4_low" )->Fill( myjet.e() );
    //
    //       Book( TH1F( "Jet_muf", "Jet muf", 50, 0.0, 1.0 ), "AK4_low" )->Fill( myjet.muf() );
    //       Book( TH1F( "Jet_phf", "Jet phf", 50, 0.0, 1.0 ), "AK4_low" )->Fill( myjet.phf() );
    //       Book( TH1F( "Jet_emf", "Jet emf", 50, 0.0, 1.0 ), "AK4_low" )->Fill( myjet.emf() );
    //       Book( TH1F( "Jet_nhf", "Jet nhf", 50, 0.0, 1.0 ), "AK4_low" )->Fill( myjet.nhf() );
    //       Book( TH1F( "Jet_chf", "Jet chf", 50, 0.0, 1.0 ), "AK4_low" )->Fill( myjet.chf() );
    //       Book( TH1F( "Jet_area", "Jet area", 40, 0.0, 4.0 ), "AK4_low" )->Fill( myjet.area() );
    //       Book( TH1F( "Jet_cm", "Jet cm", 50, 0.0, 50.0 ), "AK4_low" )->Fill( myjet.cm() );
    //       Book( TH1F( "Jet_nm", "Jet nm", 50, 0.0, 50.0 ), "AK4_low" )->Fill( myjet.nm() );
    //     }
    //   }
    // }
    //
    //
    // double mW_min = 60;
    // double mW_max = 100;
    //
    // std::vector<DESY::Jet> goodJetsAK8;
    // for ( int i = 0; i < (m_jetAK8.N); ++i ) {
    //   DESY::Jet myjet( &m_jetAK8, i );
    //   if (fabs(myjet.eta()) < m_jetEtaCut) {
    //     if (myjet.pt() > m_jetPtCut) {
    //       goodJetsAK8.push_back(myjet);
    //       // m_logger << INFO << myjet.pt() << SLogger::endmsg;
    //       Book( TH1F( "Jet_pt", "Jet p_{T} [GeV]", 100, 0.0, 1000 ), "AK8_low" )->Fill( myjet.pt() );
    //       Book( TH1F( "Jet_eta", "Jet #eta", 50, -2.5, 2.5 ), "AK8_low" )->Fill( myjet.eta() );
    //       Book( TH1F( "Jet_m", "Jet m [GeV]", 50, 0, 250 ), "AK8_low" )->Fill( myjet.m() );
    //       Book( TH1F( "Jet_phi", "Jet #phi", 50, -TMath::Pi(), TMath::Pi() ), "AK8_low" )->Fill( myjet.phi() );
    //       Book( TH1F( "Jet_e", "Jet e [GeV]", 100, 0.0, 1000 ), "AK8_low" )->Fill( myjet.e() );
    //
    //       Book( TH1F( "Jet_muf", "Jet muf", 50, 0.0, 1.0 ), "AK8_low" )->Fill( myjet.muf() );
    //       Book( TH1F( "Jet_phf", "Jet phf", 50, 0.0, 1.0 ), "AK8_low" )->Fill( myjet.phf() );
    //       Book( TH1F( "Jet_emf", "Jet emf", 50, 0.0, 1.0 ), "AK8_low" )->Fill( myjet.emf() );
    //       Book( TH1F( "Jet_nhf", "Jet nhf", 50, 0.0, 1.0 ), "AK8_low" )->Fill( myjet.nhf() );
    //       Book( TH1F( "Jet_chf", "Jet chf", 50, 0.0, 1.0 ), "AK8_low" )->Fill( myjet.chf() );
    //       Book( TH1F( "Jet_area", "Jet area", 40, 0.0, 4.0 ), "AK8_low" )->Fill( myjet.area() );
    //       Book( TH1F( "Jet_cm", "Jet cm", 50, 0.0, 50.0 ), "AK8_low" )->Fill( myjet.cm() );
    //       Book( TH1F( "Jet_nm", "Jet nm", 50, 0.0, 50.0 ), "AK8_low" )->Fill( myjet.nm() );
    //
    //       Book( TH1F( "Jet_tau1", "Jet tau1", 50, 0, 1.0 ), "AK8_low" )->Fill( myjet.tau1() );
    //       Book( TH1F( "Jet_tau2", "Jet tau2", 50, 0, 1.0 ), "AK8_low" )->Fill( myjet.tau2() );
    //       Book( TH1F( "Jet_tau3", "Jet tau3", 50, 0, 1.0 ), "AK8_low" )->Fill( myjet.tau3() );
    //
    //       Book( TH1F( "Jet_tau21", "Jet tau21", 50, 0, 1.0 ), "AK8_low" )->Fill( myjet.tau2()/myjet.tau1() );
    //       Book( TH1F( "Jet_tau32", "Jet tau32", 50, 0, 1.0 ), "AK8_low" )->Fill( myjet.tau3()/myjet.tau2() );
    //       Book( TH1F( "Jet_tau31", "Jet tau31", 50, 0, 1.0 ), "AK8_low" )->Fill( myjet.tau3()/myjet.tau1() );
    //
    //       Book( TH1F( "Jet_pruned_m", "Jet pruned m [GeV]", 50, 0, 250 ), "AK8_low" )->Fill( myjet.pruned_mass() );
    //       Book( TH1F( "Jet_softdrop_m", "Jet softdrop m [GeV]", 50, 0, 250 ), "AK8_low" )->Fill( myjet.softdrop_mass() );
    //
    //       if ((myjet.pruned_mass() > mW_min) && (myjet.pruned_mass() < mW_max))
    //         Book( TH1F( "Jet_pruned_Wwindow_m", "Jet pruned W window m [GeV]", 50, mW_min, mW_max ), "AK8_low" )->Fill( myjet.pruned_mass() );
    //       if ((myjet.softdrop_mass() > mW_min) && (myjet.softdrop_mass() < mW_max))
    //         Book( TH1F( "Jet_softdrop_Wwindow_m", "Jet softdrop W window m [GeV]", 50, mW_min, mW_max ), "AK8_low" )->Fill( myjet.softdrop_mass() );
    //
    //     }
    //   }
    // }
  
    // fill final analysis histograms and cut info for ntuple
    bool passed_all = true;
    for( UInt_t icut = 0; icut < kNumCuts; ++icut ) {
      if( selectionBits.TestBitNumber( icut ) != kTRUE ){
        passed_all = false;
      }
      else{
        if (icut) b_selection_bits|=1<<icut; 
        if (icut-1==(unsigned)b_selection_lastcut)
          b_selection_lastcut++;
      }
    }//cut loop
  
  } // two fat jets
  
  fillCutflow("cutflow", selectionBits, b_weight);
  
   return;

}

bool WHAnalysis::isGoodEvent(int runNumber, int lumiSection) {
  
  bool isGood = true;
  if (m_isData) {
    isGood = m_grl.HasRunLumiBlock( runNumber, lumiSection );
    if( !isGood ) {
      m_logger << WARNING << "Bad event! Run: " << runNumber <<  " - Lumi Section: " << lumiSection << SLogger::endmsg;
      // throw SError( SError::SkipEvent );
    }
    else m_logger << INFO << "Good event! Run: " << runNumber <<  " - Lumi Section: " << lumiSection << SLogger::endmsg;
  }
  return isGood;
  
}


bool WHAnalysis::passTrigger() {
  
  bool passTrigger = false;
  
  for (std::map<std::string,bool>::iterator it = (m_eventInfo.trigDecision)->begin(); it != (m_eventInfo.trigDecision)->end(); ++it){
      for (unsigned int t = 0; t < m_triggerNames.size(); ++t ){
        if ((it->first).find(m_triggerNames[t]) != std::string::npos) {
          if (it->second == true) {
            m_logger << VERBOSE << "Trigger pass: " << (it->first) << SLogger::endmsg;
            passTrigger = true;
            return passTrigger;
          }
        }
      }
    }
  
  return passTrigger;
  
}


bool WHAnalysis::passMETFilters() {
  
  bool passMetFilters = true;
  
  // using only what's recommended in https://twiki.cern.ch/twiki/bin/viewauth/CMS/MissingETOptionalFiltersRun2
    
  if( !(m_eventInfo.PV_filter) ) {
    passMetFilters = false;
    m_logger << VERBOSE << "PV_filter" << SLogger::endmsg;
    Hist( "METFilters" )->Fill(1);
  }
  if( !(m_eventInfo.passFilter_CSCHalo) ) {
    passMetFilters = false;
    m_logger << VERBOSE << "passFilter_CSCHalo" << SLogger::endmsg;
    Hist( "METFilters" )->Fill(2);
  }
  if( !(m_eventInfo.passFilter_HBHELoose) ) {
    passMetFilters = false;
    m_logger << VERBOSE << "passFilter_HBHELoose" << SLogger::endmsg;
    Hist( "METFilters" )->Fill(3);
  }
  if( !(m_eventInfo.passFilter_HBHEIso) ) {
    passMetFilters = false;
    m_logger << VERBOSE << "passFilter_HBHEIso" << SLogger::endmsg;
    Hist( "METFilters" )->Fill(4);
  }
  if( !(m_eventInfo.passFilter_EEBadSc) ) {
    passMetFilters = false;
    m_logger << VERBOSE << "passFilter_EEBadSc" << SLogger::endmsg;
    Hist( "METFilters" )->Fill(5);
  }
  
  return passMetFilters;
  
}


double WHAnalysis::getEventWeight() {
  
  double weight = 1.;
  for( unsigned int v = 0; v < (m_eventInfo.actualIntPerXing)->size(); ++v ){
    if ( (*m_eventInfo.bunchCrossing)[v] == 0 ) {
      b_weightPU = m_pileupReweightingTool.getPileUpweight( (*m_eventInfo.actualIntPerXing)[v] );
      m_logger << VERBOSE << "Weight: " << b_weightPU << " for true: " << (*m_eventInfo.actualIntPerXing)[v] << SLogger::endmsg;
      break;
    }
  }
  b_weightGen = (m_eventInfo.genEventWeight < 0) ? -1 : 1; 
  weight *= b_weightPU*b_weightGen;
  
  return weight;
  
}

void WHAnalysis::clearBranches() {
  
  b_weight = 1.;
  b_weightGen = 1.;
  b_weightPU = 1.;
  
  b_ak8jets_pt.clear();
  b_ak8jets_phi.clear();
  b_ak8jets_eta.clear();
  b_ak8jets_e.clear();
  b_ak8jets_tau21.clear();
  b_ak8jets_m.clear();
  b_ak8jets_mpruned.clear();
  b_ak8jets_csv.clear();
  b_ak8jets_subjets_dr.clear();
  b_ak8jets_subjets_deta.clear();
  b_ak8jets_subjets_dphi.clear();
  b_ak8jets_subjet1_pt.clear();
  b_ak8jets_subjet2_pt.clear();
  b_ak8jets_subjet1_csv.clear();
  b_ak8jets_subjet2_csv.clear();
  
  b_selection_bits = 0;
  b_selection_lastcut = 0;
  
}

void WHAnalysis::fillCutflow( const std::string histName, const TBits& cutmap, const Double_t weight ) {

  // sequential cut flow -> stop at first failed cut
  for( UInt_t i = 0; i < cutmap.GetNbits(); ++i ) {
    if( cutmap.TestBitNumber( i ) ) {
      Hist( "cutflow" )->Fill( i+1, weight );
    } else {
      break;
    }
  }
}

