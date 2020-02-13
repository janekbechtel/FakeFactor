#include "ViennaTool/interface/TNtupleAnalyzer.h"

#include <iostream>
#include <sstream>

using namespace std;

TNtupleAnalyzer::TNtupleAnalyzer()
{
  cout << "Instance of TNtupleAnalyzer created" << endl;
}

TNtupleAnalyzer::~TNtupleAnalyzer()
{
}

void TNtupleAnalyzer::loadFile(TString filename, TString chain)
{
  
  tchain = new TChain(chain);
  tchain->Add(filename);

  if       (filename==datafile) this->curr_sample="data";
  else if  (filename==DYfile || filename==DY_NJfile) this->curr_sample="DY";
  else if  (filename==TTfile) this->curr_sample="TT";
  else if  (filename==Wjetsfile) this->curr_sample="Wjets";
  else if  (filename==QCDfile) this->curr_sample="QCD";
  else if  (filename==VVfile) this->curr_sample="VV";
  else this->curr_sample="unknown";
  std::cout << "Loading file " << filename << "..." << endl;
  int nev=0;
  if (chain==_NtupleTreeName || chain==_NtupleTreeName_KIT)
  {  
    event = new NtupleClass(tchain); 
    if (DEBUG) {
      nev=event->fChain->GetEntries(); 
      std::cout<<"File: "<<filename<<" loaded \n" << "#events: "<< nev << std::endl;
    }
  }
  else{
    std::cout << "unknown chain!" << std::endl;
  }
}

void TNtupleAnalyzer::GetWeights(const TString preselectionFile) {
  /*
  Below the weight is defined with applying SFs, Tigger SFs
  */
    
  weight=1.;
  if( !preselectionFile.Contains("preselection_data")){
    if( !preselectionFile.Contains("preselection_EMB")){
      float trgWeight = 1.;
      if( CHAN == kMU ) {   
        if ((event->pt_1 < 25)) {
          trgWeight *= (event->crossTriggerDataEfficiencyWeight_1/event->crossTriggerMCEfficiencyWeight_1)*(event->crossTriggerDataEfficiencyWeight_tight_DeepTau_2/event->crossTriggerMCEfficiencyWeight_tight_DeepTau_2);
        }
        else {
          trgWeight *= (event->singleTriggerDataEfficiencyWeightKIT_1/event->singleTriggerMCEfficiencyWeightKIT_1);
        }  
      }
      else if( CHAN == kEL ) {
        if (event->pt_1 < 33) {
          trgWeight *= (event->crossTriggerMCWeight_1*(event->crossTriggerDataEfficiencyWeight_tight_DeepTau_2/event->crossTriggerMCEfficiencyWeight_tight_DeepTau_2));
        }  
        else {
          trgWeight *= (event->trigger_32_35_Weight_1);

        }  
      }
      else if (CHAN == kTAU)
      {
        trgWeight *= ((((event->byTightDeepTau2017v2p1VSjet_1<0.5 && event->byVLooseDeepTau2017v2p1VSjet_1>0.5)*event->crossTriggerDataEfficiencyWeight_vloose_DeepTau_1 + (event->byTightDeepTau2017v2p1VSjet_1>0.5)*event->crossTriggerDataEfficiencyWeight_tight_DeepTau_1)*((event->byTightDeepTau2017v2p1VSjet_2<0.5 && event->byVLooseDeepTau2017v2p1VSjet_2>0.5)*event->crossTriggerDataEfficiencyWeight_vloose_DeepTau_2 + (event->byTightDeepTau2017v2p1VSjet_2>0.5)*event->crossTriggerDataEfficiencyWeight_tight_DeepTau_2))/(((event->byTightDeepTau2017v2p1VSjet_1<0.5 && event->byVLooseDeepTau2017v2p1VSjet_1>0.5)*event->crossTriggerMCEfficiencyWeight_vloose_DeepTau_1 + (event->byTightDeepTau2017v2p1VSjet_1>0.5)*event->crossTriggerMCEfficiencyWeight_tight_DeepTau_1)*((event->byTightDeepTau2017v2p1VSjet_2<0.5 && event->byVLooseDeepTau2017v2p1VSjet_2>0.5)*event->crossTriggerMCEfficiencyWeight_vloose_DeepTau_2 + (event->byTightDeepTau2017v2p1VSjet_2>0.5)*event->crossTriggerMCEfficiencyWeight_tight_DeepTau_2)));
      }
      if (trgWeight>2.0) {
        trgWeight = 1.0;  
      }
     
      weight *= 1000.0*luminosity * event->puweight * event->muTauFakeRateWeight * event->eleTauFakeRateWeight * event->idWeight_1  *  event->isoWeight_1  * event->trackWeight_1 * trgWeight;


      // if (CHAN == kTAU) weight *= 1.;//event->sf_DoubleTauTight;
      // else              weight *= event->sf_SingleOrCrossTrigger;
      if( preselectionFile.Contains("preselection_TT") ) weight *= 0.85 * event->topPtReweightWeightRun2;
      if( preselectionFile.Contains("preselection_DY") || preselectionFile.Contains("preselection_Wjets") ) weight *= event->zPtReweightWeight;

      float stitchingWeight = 1.;
      if( preselectionFile.Contains("preselection_DY") ) {
        stitchingWeight = ((event->genbosonmass >= 50.0)*0.0000606542*((event->npartons == 0 || event->npartons >= 5)*1.0 + (event->npartons == 1)*0.194267667208 + (event->npartons == 2)*0.21727746547 + (event->npartons == 3)*0.26760465744 + (event->npartons == 4)*0.294078683662) + (event->genbosonmass < 50.0)*event->numberGeneratedEventsWeight*event->crossSectionPerEventWeight);
        if (stitchingWeight < 1e-12) {
          std::cout << "Stitching weight of zero!" << std::endl;
          exit(1);
        }
        weight *= stitchingWeight;
      }      
     else if( preselectionFile.Contains("preselection_Wjets") ) {
        stitchingWeight = ((0.0008662455*((event->npartons <= 0 || event->npartons >= 5)*1.0 + (event->npartons == 1)*0.174101755934 + (event->npartons == 2)*0.136212630745 + (event->npartons == 3)*0.0815667415121 + (event->npartons == 4)*0.06721295702670023)) * (event->genbosonmass>=0.0) + event->numberGeneratedEventsWeight * event->crossSectionPerEventWeight * (event->genbosonmass<0.0));
        if (stitchingWeight < 1e-12) {
          std::cout << "Stitching weight of zero!" << std::endl;
          exit(1);
        }
        weight *= stitchingWeight;       
     }
      else{
        weight *= event->numberGeneratedEventsWeight * event->crossSectionPerEventWeight;
      }

    }else{
      float trgWeight = 1.0;
      if( CHAN == kMU ) {   
        if ((event->pt_1 < 25)) {
          trgWeight *= (event->crossTriggerEmbeddedWeight_1)*(event->crossTriggerDataEfficiencyWeight_tight_DeepTau_2/event->crossTriggerEMBEfficiencyWeight_tight_DeepTau_2);
        }
        else {
          trgWeight *= (event->trigger_24_27_Weight_1);
        }  
      }
      else if( CHAN == kEL ) {
        if (event->pt_1 < 33) {
          if (event->crossTriggerEMBEfficiencyWeight_tight_DeepTau_2>0.01) {
          trgWeight *= (event->crossTriggerEmbeddedWeight_1*(event->crossTriggerDataEfficiencyWeight_tight_DeepTau_2/event->crossTriggerEMBEfficiencyWeight_tight_DeepTau_2));
          }
      //  std::cout << "crosstrg weight: " << trgWeight << std::endl;
        }
        else {
          trgWeight *= (event->trigger_32_35_Weight_1);
        // std::cout << "trg weight: " << trgWeight << std::endl;
        }
      }
      else if (CHAN == kTAU)
      {
        trgWeight *= ((((event->byTightDeepTau2017v2p1VSjet_1<0.5 && event->byVLooseDeepTau2017v2p1VSjet_1>0.5)*event->crossTriggerDataEfficiencyWeight_vloose_DeepTau_1 + (event->byTightDeepTau2017v2p1VSjet_1>0.5)*event->crossTriggerDataEfficiencyWeight_tight_DeepTau_1)*((event->byTightDeepTau2017v2p1VSjet_2<0.5 && event->byVLooseDeepTau2017v2p1VSjet_2>0.5)*event->crossTriggerDataEfficiencyWeight_vloose_DeepTau_2 + (event->byTightDeepTau2017v2p1VSjet_2>0.5)*event->crossTriggerDataEfficiencyWeight_tight_DeepTau_2))/(((event->byTightDeepTau2017v2p1VSjet_1<0.5 && event->byVLooseDeepTau2017v2p1VSjet_1>0.5)*event->crossTriggerEMBEfficiencyWeight_vloose_DeepTau_1 + (event->byTightDeepTau2017v2p1VSjet_1>0.5)*event->crossTriggerEMBEfficiencyWeight_tight_DeepTau_1)*((event->byTightDeepTau2017v2p1VSjet_2<0.5 && event->byVLooseDeepTau2017v2p1VSjet_2>0.5)*event->crossTriggerEMBEfficiencyWeight_vloose_DeepTau_2 + (event->byTightDeepTau2017v2p1VSjet_2>0.5)*event->crossTriggerEMBEfficiencyWeight_tight_DeepTau_2)));
      }
      if (trgWeight>10.0) {
        trgWeight = 1.0;  
      }
      if (event->generatorWeight<=1.0) {
      weight *= event->generatorWeight * event->muonEffTrgWeight * event->embeddedDecayModeWeight * event->muonEffIDWeight_1 * event->muonEffIDWeight_2 * event->idWeight_1 * event->isoWeight_1 * trgWeight;
      }
      else {
         weight = 0.0; // Check with new embedded ntuples if this still occurs
      }
    }


    if( CHAN == kTAU ){ // CHANGE IF TAU WP CHANGES! https://twiki.cern.ch/twiki/bin/view/CMS/TauIDRecommendation13TeV#Tau_ID_SF_for_CMSSW_9_4_X_or_hig
          if(event->gen_match_1 == 5 && event->byTightDeepTau2017v2p1VSjet_1) weight *= event->tauIDScaleFactorWeight_tight_DeepTau2017v2p1VSjet_1;
          else if(event->gen_match_1 == 5 && event->byVLooseDeepTau2017v2p1VSjet_1 ) weight *= event->tauIDScaleFactorWeight_vloose_DeepTau2017v2p1VSjet_1;
          if(event->gen_match_2 == 5 && event->byTightDeepTau2017v2p1VSjet_2) weight *= event->tauIDScaleFactorWeight_tight_DeepTau2017v2p1VSjet_1;
          else if(event->gen_match_2 == 5 && event->byVLooseDeepTau2017v2p1VSjet_2 ) weight *= event->tauIDScaleFactorWeight_vloose_DeepTau2017v2p1VSjet_1;
    }
    if( CHAN != kTAU ){
        if(event->gen_match_2 == 5 && event->byTightDeepTau2017v2p1VSjet_2) weight *= event->tauIDScaleFactorWeight_tight_DeepTau2017v2p1VSjet_1;
        else if(event->gen_match_2 == 5 && event->byVLooseDeepTau2017v2p1VSjet_2 ) weight *= event->tauIDScaleFactorWeight_vloose_DeepTau2017v2p1VSjet_1;
    }
  }
  weight_sf=weight;
  if(CHAN==kTAU && !COINFLIP){
    weight=weight*0.5;
    weight_sf=weight_sf*0.5;
  }
  if(DEBUG)
  {
    std::cout << "event has the following weight: "    << weight    << std::endl;
    std::cout << "event has the following weight_sf: " << weight_sf << std::endl;\
  }
  //////////////////////////////////////////////////////////////////////////
}

void TNtupleAnalyzer::select(const TString preselectionFile, const Int_t mode)
{
  TString preselFile = preselectionFile;
  TFile * fout_file = new TFile(preselFile,"RECREATE");
  TTree * t_Events  = new TTree("Events","Events");
  this->initOutfileTree(t_Events);
  //Start loop over tree
  Double_t nentries = Double_t(event->fChain->GetEntries());
  Int_t pc=0; // count the number of events that passed the selection

  cout<<"Producing " << preselFile << " , processing "<<Int_t(nentries)<<" events."<< endl;
  
  nnn=0;
  for (Int_t jentry=0; jentry<nentries;jentry++){
    if (DEBUG) {
      if (jentry > 100) {
        std::cout << jentry << " events are enough" << std::endl;
        break;
      }
    } 
    else{
      if (jentry%50000 == 0) {
      cout << "Event " << jentry << " is processed: " << jentry / nentries * 100 << "% of total" << endl;
      }
    }

    
    event->GetEntry(jentry);
    Int_t ntau=setTreeValues(preselFile, mode); //preselection happens in there now


    if (ntau>=1){ 
      if (DEBUG) { std::cout << "Event ID " << jentry << " passed preselection and is written to root file" << std::endl;}
      this->GetWeights(preselectionFile);
      if (weight>0.0) { t_Events->Fill(); } 
      pc++; 
    }
    if(CHAN==kTAU && !COINFLIP){
      ntau=0;
      ntau=setTreeValues(preselFile, mode, 2);
      if(ntau>=1 && weight>0.0){ t_Events->Fill(); pc++; }
    }
  }// End loop over all events
  if (DEBUG) {
  cout<<pc<<" events passed preselection."<< endl;
  cout<<nnn<<" events got skipped"<< endl;
  }
  t_Events->Write();
  fout_file->Close();

  
}

void TNtupleAnalyzer::closeFile()
{
  delete event;
  delete tchain;  
}


void TNtupleAnalyzer::SetNewEventInfo() {
  
  bpt_1=event->bpt_1;
  bpt_2=event->bpt_2;
  nbtag=event->nbtag;
  njets=event->njets;
  mjj=event->mjj;
  met=event->met;
  jdeta=event->jdeta;
  njetingap20=event->njetingap20;
  mu2_iso=-999;  
  m_leplep=-999;  
  lep_dR=-999;
  mt_leplep=-999;
  n_iso_lep=0;
  n_iso_otherLep=0;

  lep_pt=event->pt_1;
  lep_eta=event->eta_1;
  lep_phi=event->phi_1;
  lep_iso=event->iso_1;
  lep_q=event->q_1;

  otherLep_pt=-999;
  otherLep_eta=-999;
  otherLep_iso=-999;
  otherLep_q=-999;

  m_otherLep->resize(0);
  m_otherLep_pt->resize(0);
  m_otherLep_eta->resize(0);
  m_otherLep_phi->resize(0);
  m_otherLep_m->resize(0);
  m_otherLep_iso->resize(0);
  m_otherLep_q->resize(0);

  
  m_lep->resize(0);
  m_lep_pt->resize(0);
  m_lep_eta->resize(0);
  m_lep_phi->resize(0);
  m_lep_m->resize(0);
  m_lep_iso->resize(0);
  m_lep_q->resize(0);
 
}

void TNtupleAnalyzer::ResizeVectors() {
  alltau_pt->resize(0);
  alltau_eta->resize(0);
  alltau_phi->resize(0);
  alltau_q->resize(0);
  alltau_decay->resize(0);
  alltau_beta->resize(0);
  alltau_mediumBeta->resize(0);
  
  alltau_vvvlooseDNN->resize(0);
  alltau_vvlooseDNN->resize(0);
  alltau_vlooseDNN->resize(0);
  alltau_looseDNN->resize(0);
  alltau_mediumDNN->resize(0);
  alltau_tightDNN->resize(0);
  alltau_vtightDNN->resize(0);
  alltau_vvtightDNN->resize(0);
  
  alltau_lepVeto->resize(0);
  alltau_gen_match->resize(0);
  alltau_dRToLep->resize(0);
  alltau_dRToOtherLep->resize(0);
  alltau_dRToB->resize(0);
  alltau_mvis->resize(0);
  alltau_mt->resize(0);
  alltau_mt2->resize(0);
  alltau_svfit->resize(0);
  alltau_Zpt->resize(0);

}

Int_t TNtupleAnalyzer::setTreeValues(const TString preselectionFile, const Int_t mode, Int_t whichTau) // whichTau is default set to 1
{
  Int_t evt_ID = event->entry;
  if (DEBUG) {std::cout << "event " << evt_ID << " entered preselection" << std::endl;}
  /*
    Trigger selection + flagMETFilter + kinematic pt_2 cut 
    are applied in the following lines
  */
  if(CHAN==kMU &&  ((event->flagMETFilter <0.5) || (event->pt_2<30) || !(((event->trg_singlemuon_27 == 1) || (event->trg_singlemuon_24 == 1)) || (event->pt_1 < 25 && (event->trg_crossmuon_mu20tau27_hps == 1 || event->trg_crossmuon_mu20tau27 == 1))))) return 0; 
  if(CHAN==kTAU && ((event->flagMETFilter <0.5) || !((((!(event->isMC||event->isEmbedded) && event->run>=317509) || (event->isMC||event->isEmbedded)) && (event->trg_doubletau_35_mediso_hps == 1)) || (!(event->isMC||event->isEmbedded) && (event->run<317509) && ((event->trg_doubletau_35_tightiso_tightid == 1) || (event->trg_doubletau_40_mediso_tightid == 1) || (event->trg_doubletau_40_tightiso == 1)))))) return 0;
  if(CHAN==kEL &&  ((event->flagMETFilter <0.5) || !(event->pt_2>30 && event->pt_1 > 25 && (((event->trg_singleelectron_35 == 1) || (event->trg_singleelectron_32 == 1) || (event->pt_1>25 && event->pt_1<33 && event->pt_2>35 && (event->trg_crossele_ele24tau30_hps == 1 || event->trg_crossele_ele24tau30 == 1)))))))  return 0;
  if (DEBUG) {std::cout << "event " << evt_ID << " passed trigger selection, MET filter and kinematics" << std::endl;}
  /*  
    Lepton vetos
  */
  if(CHAN==kMU) passesTauLepVetos = ((event->byTightDeepTau2017v2p1VSmu_2>0.5) && (event->byVLooseDeepTau2017v2p1VSe_2>0.5));
  if(CHAN==kEL) passesTauLepVetos = ((event->byLooseDeepTau2017v2p1VSmu_2>0.5) && (event->byTightDeepTau2017v2p1VSe_2>0.5));
  if(CHAN==kTAU)passesTauLepVetos = ((event->byVLooseDeepTau2017v2p1VSe_1>0.5)&&(event->byVLooseDeepTau2017v2p1VSe_2>0.5)&&(event->byLooseDeepTau2017v2p1VSmu_1>0.5)&&(event->byLooseDeepTau2017v2p1VSmu_2>0.5));
  passes3LVeto= ((event->extramuon_veto < 0.5) && (event->extraelec_veto < 0.5) && (event->dilepton_veto<0.5));
  if ( CHAN == kMU  ) passesDLVeto= !(event->dilepton_veto);
  if ( CHAN == kEL  ) passesDLVeto= !(event->dilepton_veto);
  if ( CHAN == kTAU ) passesDLVeto= passesTauLepVetos;
  if (DEBUG) {
    std::cout << "event passes Tau lepton vetos: "    << passesTauLepVetos    << std::endl;
    std::cout << "event passes Tau 2-lepton vetos: "  << passesDLVeto       << std::endl;
    std::cout << "event passes Tau 3-lepton vetos: "    << passes3LVeto    << std::endl;
  }
  //////////////////////////////////////////////////////////////////////////


  float m_tau_pt_cut=TAU_PT_CUT;
  float m_tau_eta_cut=TAU_ETA_CUT;
  if ( CHAN == kTAU ) { 
    m_tau_pt_cut=TAU_PT_CUT_TT; 
    m_tau_eta_cut=TAU_ETA_CUT_TT; 
  }
  


  float decay=event->decayMode_2;
  int newDMs=event->decayModeFindingNewDMs_2;
  if ( !( (passesTauLepVetos) && (decay != 5 && decay != 6 ) && (newDMs==1) && (event->pt_2 > m_tau_pt_cut ) && (fabs(event->eta_2) < m_tau_eta_cut) ) ) {
    if (CHAN != kTAU) {
      nnn++;
      return 0;
    }
    else {
      decay=event->decayMode_1;
      newDMs=event->decayModeFindingNewDMs_1;
      if ( !((passesTauLepVetos) && (decay != 5 && decay != 6 ) &&  (newDMs==1) && (event->pt_1 > m_tau_pt_cut ) && ( fabs(event->eta_1) < m_tau_eta_cut )) ){
        nnn++;
        return 0;
      }
    }
  }
  

  this->SetNewEventInfo(); // Fills kinematic and other variables for each event new. Vectors get emptied. These variables are private to the TNtupleAnalyzer class -> see the .h file
  
  
  int nLep;
  int nOtherLep;

  nLep = 0;
  nOtherLep = 0;

  std::vector<TLorentzVector> v_otherLep; 
  double m_iso=1e6;
  
  for (int i=0; i<nOtherLep; i++){
    if (   m_otherLep_iso->at(i) < LEP_ISO_CUT  &&  m_otherLep->at(i).Pt() > LEP_PT_CUT  &&  fabs(m_otherLep->at(i).Eta()) < (TAU_ETA_CUT+0.1)    ){  //TAU eta+0.1 since we want to check overlap with taus!
      v_otherLep.push_back( TLorentzVector(m_otherLep->at(i) ));
    }

    if ( ! ( m_otherLep->at(i).Pt() > LEP_PT_CUT && fabs(m_otherLep->at(i).Eta()) < LEP_ETA_CUT ) ) continue;
    if (   m_otherLep_iso->at(i)<LEP_ISO_CUT  )  n_iso_otherLep++;
    
    if ( m_otherLep_iso->at(i) < m_iso ){
      otherLep_pt  = m_otherLep->at(i).Pt(); //TODO TLorentzVector -> gets written in TBranch at the end
      otherLep_eta = m_otherLep->at(i).Eta();
      otherLep_phi = m_otherLep->at(i).Phi();
      otherLep_iso = m_otherLep_iso->at(i);
      otherLep_q   = m_otherLep_q->at(i);
      m_iso=otherLep_iso;
    }
  }

  

  //for dR calculations
  std::vector<double> v_lep_eta_iso;  
  std::vector<double> v_lep_phi_iso;  // TODO: TLorentzVector lep_iso??
  if ( event->iso_1 < LEP_ISO_CUT && event->pt_1 > LEP_PT_CUT && fabs( event->eta_1 ) < (TAU_ETA_CUT+0.1) && CHAN != kTAU ){
    v_lep_eta_iso.push_back( event->eta_1 );
    v_lep_phi_iso.push_back( event->phi_1 );
  }

  //to select m(ll)~m(Z) events
  std::vector<TLorentzVector> v_lep;
  std::vector<double> v_lep_q;  
  if ( event->pt_1 > LEP_PT_CUT  && fabs( event->eta_1 ) < LEP_ETA_CUT && CHAN != kTAU ){
	  v_lep.push_back  ( 	 TLorentzVector(event->pt_1, event->eta_1, event->phi_1, event->m_1) );
    v_lep_q.push_back(   event->q_1 );		
    if ( event->iso_1 < LEP_ISO_CUT ) n_iso_lep++;
  }
  
  for (int i=0; i<nLep; i++){

    //for dR calculations
    if (   m_lep_iso->at(i)<LEP_ISO_CUT  &&  m_lep->at(i).Pt() > LEP_PT_CUT  &&  fabs(m_lep->at(i).Eta()) < (TAU_ETA_CUT+0.1)    ){  //TAU eta+0.1 since we want to check overlap with taus!
      v_lep_eta_iso.push_back( m_lep->at(i).Eta() );
      v_lep_phi_iso.push_back( m_lep->at(i).Phi() );
    }

    //to select m(ll)~m(Z) events
    if (   m_lep->at(i).Pt()>LEP_PT_CUT  &&  fabs(m_lep->at(i).Eta()) < LEP_ETA_CUT  ){ 
	    v_lep.push_back( TLorentzVector(m_lep->at(i)) );
      v_lep_q.push_back(   m_lep_q->at(i) );
      if ( m_lep_iso->at(i) < LEP_ISO_CUT ) n_iso_lep++;
    }

  }
  
  
  
  //select mZ candidate
  TLorentzVector vec1, vec2, vec; // used for Z candidate selection
  Double_t lep1_eta=-999, lep1_phi=-999, lep2_eta=-999, lep2_phi=-999;
  for (unsigned i=0; i<v_lep.size(); i++){
    for (unsigned j=i+1; j<v_lep.size(); j++){
      if ( ( v_lep_q.at(i) * v_lep_q.at(j) ) >= 0 ) continue;

      double m_dR=calcDR( v_lep.at(i).Eta() , v_lep.at(i).Phi() , v_lep.at(j).Eta() , v_lep.at(j).Phi() );
      if ( m_dR < DR_LEP_CUT ) continue;

      vec1 = TLorentzVector( v_lep.at(i) ); 
      vec2 = TLorentzVector( v_lep.at(j) );

      vec=vec1+vec2;

      if ( fabs(vec.M()-MZ) < fabs(m_leplep-MZ) ){
        m_leplep=vec.M();
        lep_dR=m_dR;
        lep1_eta=v_lep.at(i).Eta();		//TODO: is never used -> DELETE
        lep2_eta=v_lep.at(j).Eta();		//TODO: is never used -> DELETE
        lep1_phi=v_lep.at(i).Phi();
        lep2_phi=v_lep.at(j).Phi();

        Double_t deltaPhi=TVector2::Phi_mpi_pi( lep1_phi - lep2_phi );
        mt_leplep=sqrt(2*v_lep.at(i).Pt() *v_lep.at(j).Pt() *(1-TMath::Cos(deltaPhi))); //transverse mass of the ll candidate closest to the Z-mass
      }
    }
  }

  this->ResizeVectors(); // set vector size to zero, i.e. empty them

  

  // some variable definitions
  double dR; int m; // m == tau decay mode
  Double_t dR1, dR2;
  int loop_end;
  int pdgID;

  dR=this->calcDR( event->eta_1, event->phi_1, event->eta_2, event->phi_2 ); //should always be >0.5 ... but to be safe
  tau_iso_ind=-1; // this is useless I think - it is a private variable inside class and later used and filled?

  //for tt: insert both _1 and _2, but in random order (i.e. not iso- or pt-ordered!!); other channels: simply insert _2
  Int_t TT_AS_LEP=1;
  // std::cout << "COINFLIP: " << COINFLIP;
  if ( CHAN == kTAU ){
    if(COINFLIP) {
      if ( r3.Uniform(2)>1.0 ) TT_AS_LEP=2; //if uniform random number in (0;2) is >1, then use it as 2nd tau as lep; otherwise 1st. For et/mt, TT_AS_LEP is always =1
    }
    else if(!COINFLIP){
      if(whichTau==2) TT_AS_LEP=2;
    }
  }
  


  decay=event->decayMode_2;
  newDMs=event->decayModeFindingNewDMs_2;
  // TODO: there is no passestaulepvetos in embedded samples!!!

  // For DeepTauID ask for newDMs and veto DMs 5 and 6
  if ( (passesTauLepVetos) && (decay != 5 && decay != 6 ) &&  (newDMs==1) && (dR>0.5) && (event->pt_2 > m_tau_pt_cut ) && (fabs(event->eta_2) < m_tau_eta_cut) && (TT_AS_LEP==1) ){
    
    Int_t tpos=0;
    tau_iso_ind=tpos;
    alltau_pt->insert(alltau_pt->begin()+tpos,event->pt_2);
    alltau_eta->insert(alltau_eta->begin()+tpos,event->eta_2);
    alltau_phi->insert(alltau_phi->begin()+tpos,event->phi_2);
    
    alltau_q->insert(alltau_q->begin()+tpos,event->q_2/abs(event->q_2));
    alltau_decay->insert(alltau_decay->begin()+tpos,decay);

    alltau_vvvlooseDNN->insert(alltau_vvvlooseDNN->begin()+tpos, event->byVVVLooseDeepTau2017v2p1VSjet_2 );
    alltau_vvlooseDNN->insert(alltau_vvlooseDNN->begin()+tpos, event->byVVLooseDeepTau2017v2p1VSjet_2 );
    alltau_vlooseDNN->insert(alltau_vlooseDNN->begin()+tpos, event->byVLooseDeepTau2017v2p1VSjet_2 );
    alltau_looseDNN->insert(alltau_looseDNN->begin()+tpos, event->byLooseDeepTau2017v2p1VSjet_2 );
    alltau_mediumDNN->insert(alltau_mediumDNN->begin()+tpos, event->byMediumDeepTau2017v2p1VSjet_2 );
    alltau_tightDNN->insert(alltau_tightDNN->begin()+tpos, event->byTightDeepTau2017v2p1VSjet_2 );
    alltau_vtightDNN->insert(alltau_vtightDNN->begin()+tpos, event->byVTightDeepTau2017v2p1VSjet_2 );
    alltau_vvtightDNN->insert(alltau_vvtightDNN->begin()+tpos, event->byVVTightDeepTau2017v2p1VSjet_2 );
  

    alltau_lepVeto->insert(alltau_lepVeto->begin()+tpos,passesTauLepVetos);
    alltau_gen_match->insert(alltau_gen_match->begin()+tpos,event->gen_match_2);
    alltau_mvis->insert(alltau_mvis->begin()+tpos,event->m_vis);
    alltau_mt->insert(alltau_mt->begin()+tpos,event->mt_1);
    alltau_mt2->insert(alltau_mt2->begin()+tpos,event->mt_2);
    
    if(use_svfit)alltau_svfit->insert(alltau_svfit->begin()+tpos,event->m_sv);
    else alltau_svfit->insert(alltau_svfit->begin()+tpos,0.);
    
    TLorentzVector leg2; leg2.SetPtEtaPhiM(event->pt_2,event->eta_2,event->phi_2,event->m_2);
    TLorentzVector leg1; leg1.SetPtEtaPhiM(event->pt_1,event->eta_1,event->phi_1,event->m_1);
    TLorentzVector Emiss;
    Emiss.SetPtEtaPhiM(event->met,0,event->metphi,0);
    alltau_Zpt->insert( alltau_Zpt->begin()+tpos,(leg1+leg2+Emiss).Pt() );
      
    dR1=1e6;
    double m_dR1=dR1;
    for (unsigned iL=0; iL<v_lep_eta_iso.size(); iL++){
      m_dR1=calcDR( event->eta_2,event->phi_2,v_lep_eta_iso.at(iL),v_lep_phi_iso.at(iL) );
      if ( dR1>m_dR1 ) dR1=m_dR1;
    }
    alltau_dRToLep->insert(alltau_dRToLep->begin()+tpos , dR1 );

    dR1=1e6;
    m_dR1=dR1;
    for (unsigned iL=0; iL<v_otherLep.size(); iL++){
      m_dR1=calcDR( event->eta_2,event->phi_2,v_otherLep.at(iL).Eta(),v_otherLep.at(iL).Phi() );
      if ( dR1>m_dR1 ) dR1=m_dR1;
    }
    alltau_dRToOtherLep->insert(alltau_dRToOtherLep->begin()+tpos, dR1 );
    
    dR1=calcDR(event->eta_2,event->phi_2,event->beta_1,event->bphi_1);
    dR2=calcDR(event->eta_2,event->phi_2,event->beta_2,event->bphi_2);
    alltau_dRToB->insert(alltau_dRToB->begin()+tpos, min(dR1,dR2) );
  } else if ( CHAN!=kTAU ){ //if most iso tau is not ok, then do not use event if this flag is set
    std::cout << "\033[1;31m most isolated tau (leg2) is not ok - go to next event \033[0m" << std::endl;
    nnn++;
    return 0;
  }
 
  //now, for _1, the same as above for the tt chan. FIXME very ugly to duplicate code, should be moved to a dedicated method
  if ( TT_AS_LEP == 2 ){ //only possible for kTAU

    float decay=event->decayMode_1;
    int newDMs=event->decayModeFindingNewDMs_1;

    // For DeepTauID ask for newDMs and veto DMs 5 and 6  
    if ( (passesTauLepVetos) && (decay != 5 && decay != 6 ) &&  (newDMs==1) && (dR>0.5) && (event->pt_1 > m_tau_pt_cut ) && ( fabs(event->eta_1) < m_tau_eta_cut ) ){
      Int_t tpos=0;
      tau_iso_ind=tpos;    
      alltau_pt->insert(alltau_pt->begin()+tpos,event->pt_1);
      alltau_eta->insert(alltau_eta->begin()+tpos,event->eta_1);
      alltau_phi->insert(alltau_phi->begin()+tpos,event->phi_1);
      alltau_q->insert(alltau_q->begin()+tpos,event->q_1);
      alltau_decay->insert(alltau_decay->begin()+tpos,decay);
  
      // deepTauIDv2
      alltau_vvvlooseDNN->insert(alltau_vvvlooseDNN->begin()+tpos, event->byVVVLooseDeepTau2017v2p1VSjet_1 );
      alltau_vvlooseDNN->insert(alltau_vvlooseDNN->begin()+tpos, event->byVVLooseDeepTau2017v2p1VSjet_1 );
      alltau_vlooseDNN->insert(alltau_vlooseDNN->begin()+tpos, event->byVLooseDeepTau2017v2p1VSjet_1 );
      alltau_looseDNN->insert(alltau_looseDNN->begin()+tpos, event->byLooseDeepTau2017v2p1VSjet_1 );
      alltau_mediumDNN->insert(alltau_mediumDNN->begin()+tpos, event->byMediumDeepTau2017v2p1VSjet_1 );
      alltau_tightDNN->insert(alltau_tightDNN->begin()+tpos, event->byTightDeepTau2017v2p1VSjet_1 );
      alltau_vtightDNN->insert(alltau_vtightDNN->begin()+tpos, event->byVTightDeepTau2017v2p1VSjet_1 );
      alltau_vvtightDNN->insert(alltau_vvtightDNN->begin()+tpos, event->byVVTightDeepTau2017v2p1VSjet_1 );

      alltau_lepVeto->insert(alltau_lepVeto->begin()+tpos,passesTauLepVetos);
      alltau_gen_match->insert(alltau_gen_match->begin()+tpos,event->gen_match_1);
      alltau_mvis->insert(alltau_mvis->begin()+tpos,event->m_vis);
      alltau_mt->insert(alltau_mt->begin()+tpos,event->mt_2);
      alltau_mt2->insert(alltau_mt2->begin()+tpos,event->mt_1);
      if(use_svfit)alltau_svfit->insert(alltau_svfit->begin()+tpos,event->m_sv);
      else alltau_svfit->insert(alltau_svfit->begin()+tpos,0.);
      TLorentzVector leg2; leg2.SetPtEtaPhiM(event->pt_2,event->eta_2,event->phi_2,event->m_2);
      TLorentzVector leg1; leg1.SetPtEtaPhiM(event->pt_1,event->eta_1,event->phi_1,event->m_1);
      TLorentzVector Emiss;
      Emiss.SetPtEtaPhiM(event->met,0,event->metphi,0);
      alltau_Zpt->insert( alltau_Zpt->begin()+tpos,(leg1+leg2+Emiss).Pt() );
      
      dR1=1e6;
      double m_dR1=dR1;
      for (unsigned iL=0; iL<v_lep_eta_iso.size(); iL++){
        m_dR1=calcDR( event->eta_1,event->phi_1,v_lep_eta_iso.at(iL),v_lep_phi_iso.at(iL) );
        if ( dR1>m_dR1 ) dR1=m_dR1;
      }
      alltau_dRToLep->insert(alltau_dRToLep->begin()+tpos , dR1 );
      
      dR1=1e6;
      m_dR1=dR1;
      for (unsigned iL=0; iL<v_otherLep.size(); iL++){
        m_dR1=calcDR( event->eta_1,event->phi_1,v_otherLep.at(iL).Eta(),v_otherLep.at(iL).Phi() );
        if ( dR1>m_dR1 ) dR1=m_dR1;
      }
      alltau_dRToOtherLep->insert(alltau_dRToOtherLep->begin()+tpos, dR1 );
      
      dR1=this->calcDR(event->eta_1,event->phi_1,event->beta_1,event->bphi_1);
      dR2=calcDR(event->eta_1,event->phi_1,event->beta_2,event->bphi_2);
      alltau_dRToB->insert(alltau_dRToB->begin()+tpos, min(dR1,dR2) );
    } else{
      return 0;
    }
  }
  

  if ( CHAN == kTAU ){
    //now: take "other tau" (as determined by the random number above) as "lep"
    if ( TT_AS_LEP == 2 ){
      lep_dR=-99; //needed for mZ in ee/mumu CR; not needed for tautau
      lep_pt=event->pt_2;
      lep_eta=event->eta_2;
      lep_phi=event->phi_2;
      lep_q=event->q_2;
      //lep_iso = ( (calcVTightFF==1 && event->byVTightIsolationMVArun2017v2DBoldDMwLT2017_2==1) || (calcVTightFF==0 && event->byTightIsolationMVArun2017v2DBoldDMwLT2017_2==1) )  ? 10 : 0;
      // lep_iso = (event->byVTightIsolationMVArun2017v2DBoldDMwLT2017_2==0 && event->byTightIsolationMVArun2017v2DBoldDMwLT2017_2==1) ? 10 : 0;  //CHANGE IF TAU WP CHANGES! tight & !vtight
      // lep_iso = event->byVTightIsolationMVArun2017v2DBoldDMwLT2017_2==1 ? 10 : 0;  //CHANGE IF TAU WP CHANGES! vtight
      
      // 
      lep_iso = event->byTightDeepTau2017v2p1VSjet_2==1 ? 10 : 0;  //CHANGE IF TAU WP CHANGES! tight
      // lep_vloose = ( event->byVLooseIsolationMVArun2017v2DBoldDMwLT2017_2 == 1 ) ? 1 : 0;
      // lep_loose = ( event->byLooseIsolationMVArun2017v2DBoldDMwLT2017_2 == 1 ) ? 1 : 0;
      // lep_medium = ( event->byMediumIsolationMVArun2017v2DBoldDMwLT2017_2 == 1 ) ? 1 : 0;

    } else{
      lep_dR=-99; //needed for mZ in ee/mumu CR; not needed for tautau
      lep_pt=event->pt_1;
      lep_eta=event->eta_1;
      lep_phi=event->phi_1;
      lep_q=event->q_1;
      // lep_iso = ( (calcVTightFF==1 && event->byVTightIsolationMVArun2017v2DBoldDMwLT2017_1==1) || (calcVTightFF==0 && event->byTightIsolationMVArun2017v2DBoldDMwLT2017_1==1) )  ? 10 : 0;
      // lep_iso = (event->byVTightIsolationMVArun2017v2DBoldDMwLT2017_1==0 && event->byTightIsolationMVArun2017v2DBoldDMwLT2017_1==1) ? 10 : 0;  //CHANGE IF TAU WP CHANGES! tight & !vtight
      // lep_iso = event->byVTightIsolationMVArun2017v2DBoldDMwLT2017_1==1 ? 10 : 0;  //CHANGE IF TAU WP CHANGES! vtight
      lep_iso = event->byTightDeepTau2017v2p1VSjet_1==1 ? 10 : 0;  //CHANGE IF TAU WP CHANGES! tight
      // lep_vloose = ( event->byVLooseIsolationMVArun2017v2DBoldDMwLT2017_1 == 1 ) ? 1 : 0;
      // lep_loose = ( event->byLooseIsolationMVArun2017v2DBoldDMwLT2017_1 == 1 ) ? 1 : 0;
      // lep_medium = ( event->byMediumIsolationMVArun2017v2DBoldDMwLT2017_1 == 1 ) ? 1 : 0;

    

    }
  }

  if ( !this->fitsGenCategory(mode) ) return 0;

  //FIXME : what is this good for?
  lep_vvvloose = 0;
  lep_vvloose  = 0;
  lep_vloose   = 0;
  lep_loose    = 0;
  lep_medium   = 0; 

  return alltau_pt->size(); //if 0: no tau that passes lep veto and DMF!
}

//assumes v_val is ordered in pt; otherwise it will not work!
Int_t TNtupleAnalyzer::findPos(const Double_t val, const vector<Double_t> *v_val)
{
  for (unsigned i=0; i<v_val->size(); i++){
    if ( val>v_val->at(i) ) return i; //beginning or in between
  } 
  return v_val->size(); //at the end, can also be 0
}

//checks preselection and (for DY) gen matches
Int_t TNtupleAnalyzer::fitsGenCategory(const Int_t mode)
{
  if ( ! alltau_gen_match->size() ) return 0;
  Int_t gm_1 = event->gen_match_1;
  Int_t gm_2 = event->gen_match_2;
  if(DEBUG) {
  std::cout << "Mode is " << mode << std::endl;
  std::cout << "First genmatching is " << gm_1 << std::endl;
  std::cout << "Second genmatching is " << gm_2 << std::endl;
  }
//  promptE   =1;
//  promptMu  =2;
//  tauE      =3;
//  tauMu     =4;
//  tauH      =5;
//  realJet   =6;
  Int_t leptonfromTau;
  Int_t leptonRealJet;
  if (CHAN == kEL) {
      leptonfromTau = 3;
      leptonRealJet = -1;
  }
  if (CHAN == kMU) {
      leptonfromTau = 4;
      leptonRealJet = -1;
  }
  if (CHAN == kTAU) {
      leptonfromTau = 5;
      leptonRealJet = 6;
  }
  if (EMB == 1){ //TODO FIXME Ugly code (don't copy paste)
    if (mode & (_DY) && mode & _TTAU) { //EMBEDDING 
      if ( gm_1 == leptonfromTau && gm_2 == 5) return 1;
    } else if (mode & _DY && mode & _JTAU) { // DY_J
      if (gm_1==leptonRealJet || gm_2==realJet) return 1;
    } else if (mode & _DY && mode & _LTAU) { // DY_L
      if ( !(gm_1 == leptonfromTau && gm_2 == 5)  && !(gm_1==leptonRealJet || gm_2==realJet)  ) return 1;
    }  else if (mode & _TT && mode & _JTAU) { // TT_J
      if (gm_1==leptonRealJet || gm_2==realJet) return 1;
    } else if (mode & _TT && mode & _LTAU) { // TT_L
      if ( !(gm_1 == leptonfromTau && gm_2 == 5)  && !(gm_1==leptonRealJet || gm_2==realJet)  ) return 1;
    } else if (mode & _VV && mode & _JTAU) { // VV_J
      if (gm_1==leptonRealJet || gm_2==realJet) return 1;
    } else if (mode & _VV && mode & _LTAU) { // VV_L
      if ( !(gm_1 == leptonfromTau && gm_2 == 5)  && !(gm_1==leptonRealJet || gm_2==realJet)  ) return 1;
    } else if (mode & _QCD) {
      return 1;
    } else {
      return 1;
    }
  }else{
    if (mode & _DY && mode & _TTAU) {         //DY_T
      if (gm_2==tauH) return 1;
    } else if (mode & _DY && mode & _JTAU) {  //DY_J
      if (gm_2==realJet) return 1;
    } else if (mode & _DY && mode & _LTAU) {  //DY_L
      if ( (gm_2!=tauH)     && (gm_2!=realJet)  ) return 1;
    } else if (mode & _TT && mode & _TTAU) {  //TT_T
      if (gm_2==tauH) return 1;
    } else if (mode & _TT && mode & _JTAU) {  //TT_J
      if (gm_2==realJet) return 1;
    } else if (mode & _TT && mode & _LTAU) {  //TT_L
      if ( (gm_2!=tauH)     && (gm_2!=realJet)  ) return 1;
    } else if (mode & _VV && mode &_TTAU) {   //VV_T
      if (gm_2==tauH) return 1;
    } else if (mode & _VV && mode & _JTAU) {  //VV_J
      if (gm_2==realJet) return 1;
    } else if (mode & _VV && mode & _LTAU) {  //VV_L
      if ( (gm_2!=tauH)     && (gm_2!=realJet)  ) return 1;
    } else if (mode & _QCD) {
      return 1;
    } else {
      return 1;
    }
  }
  return 0;
}

Double_t TNtupleAnalyzer::calcDR(const Double_t eta1, const Double_t phi1, const Double_t eta2, const Double_t phi2)
{
  Double_t dEta=eta1-eta2;
  Double_t dPhi=TVector2::Phi_mpi_pi(phi1-phi2);
  return(TMath::Sqrt(dEta*dEta+dPhi*dPhi));
}

 
void TNtupleAnalyzer::initOutfileTree(TTree* tree)
{
  tree->Branch("weight",&weight);
  tree->Branch("sf",&sf);
  tree->Branch("weight_sf",&weight_sf);
  tree->Branch("passes3LVeto",&passes3LVeto);
  tree->Branch("passesDLVeto",&passesDLVeto);
  tree->Branch("bpt_1",&bpt_1);
  tree->Branch("bpt_2",&bpt_2);
  tree->Branch("njets",&njets);
  tree->Branch("nbtag",&nbtag);
  tree->Branch("mjj",&mjj);
  tree->Branch("jdeta",&jdeta);
  tree->Branch("njetingap20", &njetingap20);
  tree->Branch("met",&met);
  tree->Branch("m_leplep",&m_leplep);
  tree->Branch("lep_dR",&lep_dR);
  tree->Branch("mt_leplep",&mt_leplep);
  tree->Branch("otherLep_pt" ,&otherLep_pt);
  tree->Branch("otherLep_eta",&otherLep_eta);  
  tree->Branch("otherLep_q"  ,&otherLep_q);
  tree->Branch("otherLep_iso",&otherLep_iso);
  tree->Branch("lep_pt" ,&lep_pt);	//TODO: TLorentzVector
  tree->Branch("lep_eta",&lep_eta);
  tree->Branch("lep_phi",&lep_phi);
  tree->Branch("lep_q"  ,&lep_q);
  tree->Branch("lep_iso",&lep_iso);
  tree->Branch("lep_vvvloose",&lep_vvvloose);
  tree->Branch("lep_vvloose",&lep_vvloose);
  tree->Branch("lep_vloose",&lep_vloose);
  tree->Branch("lep_loose",&lep_loose);
  tree->Branch("lep_medium",&lep_medium);
  tree->Branch("n_iso_lep",&n_iso_lep);
  tree->Branch("n_iso_otherLep",&n_iso_otherLep);
  tree->Branch("alltau_pt",&alltau_pt);
  tree->Branch("alltau_eta",&alltau_eta);
  tree->Branch("alltau_phi",&alltau_phi);
  tree->Branch("alltau_q",&alltau_q);
  tree->Branch("alltau_decay",&alltau_decay);
  tree->Branch("alltau_beta",&alltau_beta);
  

  // deepTauIDv2
  tree->Branch("alltau_vvvlooseDNN",&alltau_vvvlooseDNN);
  tree->Branch("alltau_vvlooseDNN",&alltau_vvlooseDNN);
  tree->Branch("alltau_vlooseDNN",&alltau_vlooseDNN);
  tree->Branch("alltau_looseDNN",&alltau_looseDNN);
  tree->Branch("alltau_mediumDNN",&alltau_mediumDNN);
  tree->Branch("alltau_tightDNN",&alltau_tightDNN);
  tree->Branch("alltau_vtightDNN",&alltau_vtightDNN);
  tree->Branch("alltau_vvtightDNN",&alltau_vvtightDNN);
  
  tree->Branch("alltau_lepVeto",&alltau_lepVeto);
  tree->Branch("alltau_gen_match",&alltau_gen_match);
  tree->Branch("alltau_dRToLep",&alltau_dRToLep);
  tree->Branch("alltau_dRToOtherLep",&alltau_dRToOtherLep);
  tree->Branch("alltau_dRToB",&alltau_dRToB);
  tree->Branch("alltau_mvis",&alltau_mvis);
  tree->Branch("alltau_mt",&alltau_mt);
  tree->Branch("alltau_mt2",&alltau_mt2);
  tree->Branch("alltau_svfit",&alltau_svfit);
  tree->Branch("alltau_Zpt", &alltau_Zpt);
  tree->Branch("tau_iso_ind",&tau_iso_ind);

  alltau_pt=new vector<Double_t>;
  alltau_eta=new vector<Double_t>;
  alltau_phi=new vector<Double_t>;
  alltau_q=new vector<Double_t>;
  alltau_decay=new vector<Int_t>;
  alltau_beta=new vector<Double_t>;
  alltau_mediumBeta=new vector<Int_t>;
  // alltau_vvvlooseMVA=new vector<Int_t>;
  // alltau_vvlooseMVA=new vector<Int_t>;
  // alltau_vlooseMVA=new vector<Int_t>;
  // alltau_looseMVA=new vector<Int_t>;
  // alltau_mediumMVA=new vector<Int_t>;
  // alltau_tightMVA=new vector<Int_t>;
  // alltau_vtightMVA=new vector<Int_t>;
  // alltau_vvtightMVA=new vector<Int_t>;


  alltau_vvvlooseDNN =new vector<Int_t>;;
  alltau_vvlooseDNN =new vector<Int_t>;;
  alltau_vlooseDNN =new vector<Int_t>;;
  alltau_looseDNN =new vector<Int_t>;;
  alltau_mediumDNN =new vector<Int_t>;;
  alltau_tightDNN =new vector<Int_t>;;
  alltau_vtightDNN =new vector<Int_t>;;
  alltau_vvtightDNN =new vector<Int_t>;;
  
  // alltau_vvvlooseDeepTauIDv2VSjet=new vector<Int_t>;
  // alltau_vvlooseDeepTauIDv2VSjet=new vector<Int_t>;
  // alltau_vlooseDeepTauIDv2VSjet=new vector<Int_t>;
  // alltau_looseDeepTauIDv2VSjet=new vector<Int_t>;
  // alltau_mediumDeepTauIDv2VSjet=new vector<Int_t>;
  // alltau_tightDeepTauIDv2VSjet=new vector<Int_t>;
  // alltau_vtightDeepTauIDv2VSjet=new vector<Int_t>;
  // alltau_vvtightDeepTauIDv2VSjet=new vector<Int_t>;

  alltau_lepVeto=new vector<Int_t>;
  alltau_gen_match=new vector<Int_t>;
  alltau_dRToLep=new vector<Double_t>;
  alltau_dRToOtherLep=new vector<Double_t>;
  alltau_dRToB=new vector<Double_t>;
  alltau_mvis=new vector<Double_t>;
  alltau_mt=new vector<Double_t>;
  alltau_mt2=new vector<Double_t>;
  alltau_svfit=new vector<Double_t>;
  alltau_Zpt=new vector<Double_t>;

  m_otherLep = new vector<TLorentzVector>;
  m_otherLep_pt = new vector<Double_t>;
  m_otherLep_eta  = new vector<Double_t>;;
  m_otherLep_phi = new vector<Double_t>;;
  m_otherLep_m = new vector<Double_t>;;
  m_otherLep_iso = new vector<Double_t>;;
  m_otherLep_q = new vector<Int_t>;

  m_lep = new vector<TLorentzVector>;
  m_lep_pt  = new vector<Double_t>;;
  m_lep_eta = new vector<Double_t>;;
  m_lep_phi = new vector<Double_t>;;
  m_lep_m = new vector<Double_t>;;
  m_lep_iso = new vector<Double_t>;;
  m_lep_q = new vector<Int_t>();

}
