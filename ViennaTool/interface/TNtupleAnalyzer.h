#ifndef __TNTUPLEANALYZER__
#define __TNTUPLEANALYZER__

#include "ViennaTool/NtupleClass.h"
#include "TLorentzVector.h"
#include "TChain.h"
#include "TTree.h"
#include "TRandom3.h"
#include <vector>

#include "ViennaTool/Globals.h"

class TNtupleAnalyzer{
 public:
  TNtupleAnalyzer();
  ~TNtupleAnalyzer();

  void loadFile(TString filename, TString chain);
  void closeFile();
  void select(const TString preselectionFile, const Int_t mode);
  Int_t setTreeValues(const TString preselectionFile, const Int_t mode, const Int_t whichTau=1);
  Int_t findPos(const Double_t val, const std::vector<Double_t> *v_val); //never used so far
  Int_t fitsGenCategory(const Int_t mode);
  Double_t calcDR(const Double_t eta1, const Double_t phi1, const Double_t eta2, const Double_t phi2);
  void initOutfileTree(TTree* tree);
  void SetNewEventInfo();
  void ResizeVectors();
  void GetWeights(const TString preselectionFile);

 private:
  TString curr_sample;

  NtupleClass *event;

  TChain* tchain;
  // fields variables of SignalClass
  //    double mtll,mMuMu, dRMu, bpt_1, weight, sf, weight_sf, mu2_iso;//,mu1_eta, mu1_phi, mu2_eta, mu2_phi;
  double mt_leplep,m_leplep, lep_dR, bpt_1, bpt_2, weight, sf, weight_sf, mu2_iso;//,mu1_eta, mu1_phi, mu2_eta, mu2_phi;
  int passes3LVeto, passesDLVeto, passesTauLepVetos, njets, nbtag;
  int nnn;
  double mjj,jdeta,njetingap20;
  double mvamet; double met;
  double otherLep_pt,otherLep_eta,otherLep_phi,otherLep_m,otherLep_iso;
  int    otherLep_q;
  double lep_pt,lep_eta,lep_phi,lep_m,lep_iso,jpt_1,jpt_2;
  double lep_vvvloose,lep_vvloose,lep_vloose,lep_loose,lep_medium; // only needed for tt channel
  int    lep_q, lep_gen_match;
  int n_iso_lep, n_iso_otherLep;
  int tau_iso_ind;


  std::vector<TLorentzVector> *m_otherLep;
  std::vector<Double_t> *m_otherLep_pt;
  std::vector<Double_t> *m_otherLep_eta;
  std::vector<Double_t> *m_otherLep_phi;
  std::vector<Double_t> *m_otherLep_m;
  std::vector<Double_t> *m_otherLep_iso;
  std::vector<Int_t>    *m_otherLep_q;

  std::vector<TLorentzVector> *m_lep;
  std::vector<Double_t> *m_lep_pt;
  std::vector<Double_t> *m_jpt_1;
  std::vector<Double_t> *m_jpt_2;

  std::vector<Double_t> *m_lep_eta;
  std::vector<Double_t> *m_lep_phi;
  std::vector<Double_t> *m_lep_m;
  std::vector<Double_t> *m_lep_iso;
  std::vector<Int_t>    *m_lep_q;

  std::vector<Double_t> *alltau_pt;
  std::vector<Double_t> *alltau_eta;
  std::vector<Double_t> *alltau_phi;
  std::vector<Double_t> *alltau_q;
  std::vector<Int_t> *alltau_decay;
  std::vector<Double_t> *alltau_beta;
  std::vector<Int_t> *alltau_mediumBeta;
  //    std::vector<Int_t> *alltau_tightBeta;
  // std::vector<Int_t> *alltau_vvvlooseMVA;
  // std::vector<Int_t> *alltau_vvlooseMVA;
  // std::vector<Int_t> *alltau_vlooseMVA;
  // std::vector<Int_t> *alltau_looseMVA;
  // std::vector<Int_t> *alltau_mediumMVA;
  // std::vector<Int_t> *alltau_tightMVA;
  // std::vector<Int_t> *alltau_vtightMVA;
  // std::vector<Int_t> *alltau_vvtightMVA;
  // DeepTau ID 
  std::vector<Int_t> *alltau_vvvlooseDNN;
  std::vector<Int_t> *alltau_vvlooseDNN;
  std::vector<Int_t> *alltau_vlooseDNN;
  std::vector<Int_t> *alltau_looseDNN;
  std::vector<Int_t> *alltau_mediumDNN;
  std::vector<Int_t> *alltau_tightDNN;
  std::vector<Int_t> *alltau_vtightDNN;
  std::vector<Int_t> *alltau_vvtightDNN;
  

  std::vector<Int_t> *alltau_lepVeto;
  std::vector<Int_t> *alltau_gen_match;
  std::vector<Double_t> *alltau_dRToB;
  std::vector<Double_t> *alltau_dRToLep;
  std::vector<Double_t> *alltau_dRToOtherLep;
  std::vector<Double_t> *alltau_mvis;
  std::vector<Double_t> *alltau_mt;
  std::vector<Double_t> *alltau_mt2;
  std::vector<Double_t> *alltau_svfit;
  std::vector<Double_t> *alltau_Zpt;
  
  // variables used in whole class
  Double_t* pt_cuts_Wjets,*pt_cuts_DY,*pt_cuts_TT,*pt_cuts_QCD,*pt_cuts_QCD_AI;
  Double_t* eta_cuts_Wjets,*eta_cuts_DY,*eta_cuts_TT,*eta_cuts_QCD;
  Int_t* decay_cuts_Wjets,*decay_cuts_DY,*decay_cuts_TT,*decay_cuts_QCD;
  Double_t* mt_cuts_Wjets,*mt_cuts_DY,*mt_cuts_TT,*mt_cuts_QCD;
  Int_t n_p_Wjets,n_p_DY,n_p_TT,n_p_QCD,n_p_QCD_AI;
  Int_t n_t_Wjets,n_t_DY,n_t_TT,n_t_QCD;
  Int_t n_e_Wjets,n_e_DY,n_e_TT,n_e_QCD;
  Int_t n_m_Wjets,n_m_DY,n_m_TT,n_m_QCD;

  //use for mtll correction
  Double_t corr_array[nFIT_BINS];
  //    Int_t corr_array_nbin;

  Double_t corr_d[NB_MTCORR], corr_k[NB_MTCORR];
  Double_t corr_d_err[NB_MTCORR], corr_k_err[NB_MTCORR], corr_SSxxOverN[NB_MTCORR], corr_avg_x[NB_MTCORR];
  Double_t sys_corr_d[NB_MTCORR], sys_corr_k[NB_MTCORR];

  TRandom3 r3;  
};



#endif
