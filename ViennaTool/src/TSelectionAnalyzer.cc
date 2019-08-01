#include "ViennaTool/interface/TSelectionAnalyzer.h"

#include <iostream>
#include <sstream>

#include <chrono>
#include <ctime>    


using namespace std;

TSelectionAnalyzer::TSelectionAnalyzer()
{
}

TSelectionAnalyzer::~TSelectionAnalyzer()
{
}

void TSelectionAnalyzer::calcBgEstSim(const TString preselection,const Int_t mode,const Int_t categoryMode,const TString output, const Int_t cuts)
{
  TH1D *tightSR,*looseSR,*tightSR_alt,*allSR;
  // TH1D *tightSR_TEST,*looseSR_TEST,*tightSR_alt_TEST,*allSR_TEST;
  TString suff=""; if (mode & _SS ) suff=" (SS) ";
  if (mode & MT) {
    tightSR = new TH1D("hh_t_mt","Tight events in SR"+suff,nbins_mt,hist_min_mt,hist_max_mt);
    looseSR = new TH1D("hh_l_mt","Loose events in SR"+suff,nbins_mt,hist_min_mt,hist_max_mt);
    tightSR_alt = new TH1D("hh_t_alt_mt","Tight events in alt SR"+suff,nbins_mt,hist_min_mt,hist_max_mt);
    allSR   = new TH1D("hh_a_mt",  "All events in SR"+suff,nbins_mt,hist_min_mt,hist_max_mt);
  }
  else if (mode & MVIS && mode & _AI) {
    tightSR = new TH1D("hh_t_mvis","Tight events in SR"+suff,w_mvis_n,w_mvis_v);
    looseSR = new TH1D("hh_l_mvis","Loose events in SR"+suff,w_mvis_n,w_mvis_v);
    tightSR_alt = new TH1D("hh_t_alt_mvis","Tight events in alt SR"+suff,w_mvis_n,w_mvis_v);
    allSR   = new TH1D("hh_a_mvis",  "All events in SR"+suff,w_mvis_n,w_mvis_v);
  }
  else if (mode & MVIS) {
    tightSR = new TH1D("hh_t_mvis","Tight events in SR"+suff,nbins_mvis,hist_min_mvis,hist_max_mvis);
    looseSR = new TH1D("hh_l_mvis","Loose events in SR"+suff,nbins_mvis,hist_min_mvis,hist_max_mvis);
    tightSR_alt = new TH1D("hh_t_alt_mvis","Tight events in alt SR"+suff,nbins_mvis,hist_min_mvis,hist_max_mvis);
    allSR   = new TH1D("hh_a_mvis",  "All events in SR"+suff,nbins_mvis,hist_min_mvis,hist_max_mvis);
  }
  else if (mode & PT) {
    tightSR = new TH1D("hh_t_pt","Tight events in SR"+suff,nbins_pt,hist_min_pt,hist_max_pt);
    looseSR = new TH1D("hh_l_pt","Loose events in SR"+suff,nbins_pt,hist_min_pt,hist_max_pt);
    tightSR_alt = new TH1D("hh_t_alt_pt","Tight events in alt SR"+suff,nbins_pt,hist_min_pt,hist_max_pt);
    allSR   = new TH1D("hh_a_pt",  "All events in SR"+suff,nbins_pt,hist_min_pt,hist_max_pt);
  }
  else if (mode & SVFIT) {                                      
    tightSR = new TH1D("hh_t_svfit","Tight events in SR"+suff,nbins_svfit,hist_min_svfit,hist_max_svfit);
    looseSR = new TH1D("hh_l_svfit","Loose events in SR"+suff,nbins_svfit,hist_min_svfit,hist_max_svfit);
    tightSR_alt = new TH1D("hh_t_alt_svfit","Tight events in alt SR"+suff,nbins_svfit,hist_min_svfit,hist_max_svfit);
    allSR   = new TH1D("hh_a_svfit",  "All events in SR"+suff,nbins_svfit,hist_min_svfit,hist_max_svfit);
  }
  else if (mode & LEPPT) {                                      
    tightSR = new TH1D("hh_t_lepPt","Tight events in SR"+suff,nbins_lepPt,hist_min_lepPt,hist_max_lepPt);
    looseSR = new TH1D("hh_l_lepPt","Loose events in SR"+suff,nbins_lepPt,hist_min_lepPt,hist_max_lepPt);
    tightSR_alt = new TH1D("hh_t_alt_lepPt","Tight events in alt SR"+suff,nbins_lepPt,hist_min_lepPt,hist_max_lepPt);
    allSR   = new TH1D("hh_a_lepPt",  "All events in SR"+suff,nbins_lepPt,hist_min_lepPt,hist_max_lepPt);
  }
  
  else {
    std::cout<<"ERROR: no valid mode given. Exiting TNtupleAnalyzer::calcBgEstSim..."<<std::endl;
    return;
  }
  loadFile(preselection,"Events");

  
  TString cutstring = this->getSRCutString(mode, categoryMode);
  
  if ( DEBUG ) std::cout << "SR CUT STRING: " << cutstring << endl;

  TString s_selval, s_histoname = "";
  if (mode & MVIS) {      s_selval = "alltau_mvis[tau_iso_ind]"; s_histoname = "mvis";  }
  else if (mode & PT)   { s_selval = "alltau_pt[tau_iso_ind]";   s_histoname = "pt";    }
  else if (mode & MT) {   s_selval = "alltau_mt[tau_iso_ind]";   s_histoname = "mt";    }
  else if (mode & LEPPT) {s_selval = "lep_pt";         s_histoname = "lepPt"; }
  else if (mode & SVFIT) {s_selval = "alltau_svfit[tau_iso_ind]"; s_histoname = "svfit"; }

  auto start = std::chrono::system_clock::now();
  event_s->fChain->Draw(s_selval + ">>hh_a_"+s_histoname+"", "weight_sf * "+cutstring, "goff");
   if ( DEBUG ) cout << "hh_a_"<<s_histoname << " DONE " << endl;
  event_s->fChain->Draw(s_selval + ">>hh_t_"+s_histoname+"", "weight_sf * "+cutstring + "*" + this->getWPCutString("tight"), "goff");
   if ( DEBUG ) cout << "hh_t_"<<s_histoname << " DONE " << endl;
  event_s->fChain->Draw(s_selval + ">>hh_t_alt_"+s_histoname+"", "weight_sf * "+cutstring + "*" + this->getWPCutString("tight_alt"), "goff");
   if ( DEBUG ) cout << "hh_t_alt_"<<s_histoname << " DONE " << endl;
  event_s->fChain->Draw(s_selval + ">>hh_l_"+s_histoname+"", "weight_sf * "+cutstring + "*" + this->getWPCutString("loose"), "goff");
   if ( DEBUG ) cout << "hh_l_"<<s_histoname << " DONE " << endl;
  auto end  = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end-start;
  cout << "Elapsed time (NEW): " << elapsed_seconds.count() << endl;

  
  // start = std::chrono::system_clock::now();
  // Int_t nentries = Int_t(event_s->fChain->GetEntries());
  // std::cout<<"calcBgEstSim: Input " << preselection << ", output " << output << ". Processing "<<nentries<<" events."<<flush;
  // Double_t fillVal=0;
  // Int_t tau_ind=0;
  // for (Int_t jentry=0; jentry<nentries;jentry++) {
  //   event_s->GetEntry(jentry);
  //   tau_ind=event_s->tau_iso_ind;
  //   if ( tau_ind<0 ) continue;

  //   if ( !fulfillCategory(categoryMode) ) continue;
  //   if ( isInSR(mode,tau_ind) && ( !cuts || this->passesCuts(cuts,tau_ind) )  ){
  //     fillVal=this->selVal(mode,tau_ind);

  //     Double_t DMscaling=1;
  //     Double_t XSscaling=1;
  //     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //     //tau id scaling
  //     if( preselection.Contains("DY_TT") || preselection.Contains("TT_T") || preselection.Contains("VV_T") ){
  //       if(event_s->alltau_decay->at(0)<5) DMscaling=1.;
  //       else DMscaling=1.;
  //     }
  //     ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //     //tau XS scaling
  //     /*if( preselection.Contains("DY_TT") || preselection.Contains("DY_L") || preselection.Contains("DY_J") ){
  //       XSscaling=0.95;
        
  //       }*/ // this is now already done in the preprocessing
  //     allSR->Fill(fillVal,event_s->weight_sf*DMscaling*XSscaling);
  //     if ( this->isTight(0,tau_ind) ) tightSR->Fill(fillVal,event_s->weight_sf*DMscaling*XSscaling);
  //     if ( this->isLoose(0,tau_ind) ) looseSR->Fill(fillVal,event_s->weight_sf*DMscaling*XSscaling);
  //     if ( this->isTight_alt(0,tau_ind) ) tightSR_alt->Fill(fillVal,event_s->weight_sf*DMscaling*XSscaling);
  //   }
  // }
  // std::cout << " in tightSR: " << tightSR->Integral(-1,-1) << std::endl;
  // end  = std::chrono::system_clock::now();
  // elapsed_seconds = end-start;
  // cout << "Elapsed time (OLD): " << elapsed_seconds.count() << endl;

  // for( int i = 0; i < tightSR->GetNbinsX(); i++){
  //   if( abs(allSR->GetBinContent(i) - allSR_TEST->GetBinContent(i)) > 0 ){
  //     cout << "WARNING: all: " << allSR->GetBinContent(i) << " " << allSR_TEST->GetBinContent(i) << endl;
  //   }
  //   if( abs(tightSR->GetBinContent(i) - tightSR_TEST->GetBinContent(i)) > 0 ){
  //     cout << "WARNING: tight: " << tightSR->GetBinContent(i) << " " << tightSR_TEST->GetBinContent(i) << endl;
  //   }
  //   if( abs(looseSR->GetBinContent(i) - looseSR_TEST->GetBinContent(i)) > 0 ){
  //     cout << "WARNING: loose: " << looseSR->GetBinContent(i) << " " << looseSR_TEST->GetBinContent(i) << endl;
  //   }
  //   if( abs(tightSR_alt->GetBinContent(i) - tightSR_alt_TEST->GetBinContent(i)) > 0 ){
  //     cout << "WARNING: tight_alt: " << tightSR_alt->GetBinContent(i) << " " << tightSR_alt_TEST->GetBinContent(i) << endl;
  //   }
    
  // }

  // TFile f2(output+"test.root","recreate");
  // tightSR_TEST->Write();
  // looseSR_TEST->Write();
  // allSR_TEST->Write();
  // f2.Close();

  TFile f(output,"recreate");

  std::cout << "***************** HISTO STATS ********************" << std::endl;
  std::cout << tightSR->GetEntries() << std::endl;
  std::cout << looseSR->GetEntries() << std::endl;
  std::cout << tightSR_alt->GetEntries() << std::endl;
  std::cout << allSR->GetEntries() << std::endl;

  tightSR->Write();
  looseSR->Write();
  tightSR_alt->Write();
  allSR->Write();
  f.Close();

  delete tightSR;delete looseSR;delete tightSR_alt;delete allSR;
  delete tchain;delete event_s;
}



void TSelectionAnalyzer::getCRHisto(TString preselection,Int_t mode,TString output)
{
  loadFile(preselection,"Events");

  std::cout << "In TSelectionanalyzer::getCRHisto , producing " << output << std::endl;
  const Int_t FI=1;
  TH1D* tightCR,*looseCR,*tightSR_alt,*allCR;
  // TH1D* tightCR_TEST,*looseCR_TEST,*tightSR_alt_TEST,*allCR_TEST;

  if (mode & MT) {
    tightCR = new TH1D("hh_t_mt","",FI*nbins_mt,hist_min_mt,hist_max_mt);
    looseCR = new TH1D("hh_l_mt","",FI*nbins_mt,hist_min_mt,hist_max_mt);
    tightSR_alt = new TH1D("hh_t_alt_mt","",FI*nbins_mt,hist_min_mt,hist_max_mt);
    allCR   = new TH1D("hh_a_mt","",FI*nbins_mt,hist_min_mt,hist_max_mt);
  }
  else if (mode & MVIS) {
    tightCR = new TH1D("hh_t_mvis","",w_mvis_n,w_mvis_v);
    looseCR = new TH1D("hh_l_mvis","",w_mvis_n,w_mvis_v);
    tightSR_alt = new TH1D("hh_t_alt_mvis","",w_mvis_n,w_mvis_v);
    allCR   = new TH1D("hh_a_mvis","",w_mvis_n,w_mvis_v);
  }
  else if (mode & PT) {
    tightCR = new TH1D("hh_t_pt","",FI*nbins_pt,hist_min_pt,hist_max_pt);
    looseCR = new TH1D("hh_l_pt","",FI*nbins_pt,hist_min_pt,hist_max_pt);
    tightSR_alt = new TH1D("hh_t_alt_pt","",FI*nbins_pt,hist_min_pt,hist_max_pt);
    allCR   = new TH1D("hh_a_pt","",FI*nbins_pt,hist_min_pt,hist_max_pt);
  }
  else if (mode & MUISO) {
    tightCR = new TH1D("hh_t_muiso","",w_muiso_n,w_muiso_v);
    looseCR = new TH1D("hh_l_muiso","",w_muiso_n,w_muiso_v);
    tightSR_alt = new TH1D("hh_t_alt_muiso","",w_muiso_n,w_muiso_v);
    allCR   = new TH1D("hh_a_muiso","",w_muiso_n,w_muiso_v);
  }
  else if (mode & LEPPT) {
    tightCR = new TH1D("hh_t_lepPt","",FI*nbins_lepPt,hist_min_lepPt,hist_max_lepPt);
    looseCR = new TH1D("hh_l_lepPt","",FI*nbins_lepPt,hist_min_lepPt,hist_max_lepPt);
    tightSR_alt = new TH1D("hh_t_alt_lepPt","",FI*nbins_lepPt,hist_min_lepPt,hist_max_lepPt);
    allCR   = new TH1D("hh_a_lepPt","",FI*nbins_lepPt,hist_min_lepPt,hist_max_lepPt);
  }
  else {
    std::cout<<"ERROR: no valid mode given. Exiting TNtupleAnalyzer::getCRHisto..."<<std::endl;
    return;
  }

  TString cutstring = this->getCRCutString(mode);
  
  if ( DEBUG ) std::cout << "CR CUT STRING: " << cutstring << endl;

  TString s_selval, s_histoname = "";
  if (mode & MVIS) {      s_selval = "alltau_mvis[0]"; s_histoname = "mvis";  }
  else if (mode & MUISO) {s_selval = "lep_iso";        s_histoname = "muiso"; }
  else if (mode & PT)   { s_selval = "alltau_pt[0]";   s_histoname = "pt";    }
  else if (mode & MT) {   s_selval = "alltau_mt[0]";   s_histoname = "mt";    }
  else if (mode & LEPPT) {s_selval = "lep_pt";         s_histoname = "lepPt"; }

  auto start = std::chrono::system_clock::now();
  auto end  = std::chrono::system_clock::now();
  std::chrono::duration<double> elapsed_seconds = end -start;
  // event_s->fChain->Draw(s_selval + ">>hh_a_"+s_histoname+"", "weight_sf * "+cutstring, "goff");
  //  if ( DEBUG ) cout << "hh_a_"<<s_histoname << " DONE " << endl;
  //  end  = std::chrono::system_clock::now();
  //  elapsed_seconds = end -start;
  //  cout << elapsed_seconds.count() << endl;
  // event_s->fChain->Draw(s_selval + ">>hh_t_"+s_histoname+"", "weight_sf * "+cutstring + "*" + this->getWPCutString("tight"), "goff");
  //  if ( DEBUG ) cout << "hh_t_"<<s_histoname << " DONE " << endl;
  //  end  = std::chrono::system_clock::now();
  //  elapsed_seconds = end -start;
  //  cout << elapsed_seconds.count() << endl;
  // event_s->fChain->Draw(s_selval + ">>hh_t_alt_"+s_histoname+"", "weight_sf * "+cutstring + "*" + this->getWPCutString("tight_alt"), "goff");
  //  if ( DEBUG ) cout << "hh_t_alt_"<<s_histoname << " DONE " << endl;
  //  end  = std::chrono::system_clock::now();
  //  elapsed_seconds = end -start;
  //  cout << elapsed_seconds.count() << endl;
  // event_s->fChain->Draw(s_selval + ">>hh_l_"+s_histoname+"", "weight_sf * "+cutstring + "*" + this->getWPCutString("loose"), "goff");
  //  if ( DEBUG ) cout << "hh_l_"<<s_histoname << " DONE " << endl;
  //  end  = std::chrono::system_clock::now();
  //  elapsed_seconds = end -start;
  //  cout << elapsed_seconds.count() << endl;
  
  
  // cout << "Elapsed time (NEW): " << elapsed_seconds.count() << endl;

  start = std::chrono::system_clock::now();
  Int_t ct=0,cl=0,ct_alt=0;
  Int_t nentries = Int_t(event_s->fChain->GetEntries());
  if (DEBUG) std::cout<<"The input chain contains: "<<nentries<<" events."<<std::endl;
  Double_t fillVal;
  for (Int_t jentry=0; jentry<nentries;jentry++) {
    if (DEBUG){ if (jentry%1000000==0 && jentry>0) std::cout<<jentry<<std::endl; }
    event_s->GetEntry(jentry);
    fillVal=this->selVal(mode);
    if (isInCR(mode)) {
      allCR->Fill(fillVal,event_s->weight_sf);
      if (this->isTight()) {
        ct++;
        tightCR->Fill(fillVal,event_s->weight_sf); 
      }
      else if (this->isLoose()) {
        cl++;
        looseCR->Fill(fillVal,event_s->weight_sf); 
      }
      if (this->isTight_alt()) {
        ct_alt++;
        tightSR_alt->Fill(fillVal,event_s->weight_sf); 
      }
    }
  }
  end  = std::chrono::system_clock::now();
  elapsed_seconds = end-start;
  cout << "Elapsed time (OLD): " << elapsed_seconds.count() << endl;


  if (DEBUG) std::cout<<cl<<" loose"<< std::endl;
  if (DEBUG) std::cout<<ct<<" tight"<< std::endl;
  if (DEBUG) std::cout<<ct_alt<<" tight alt"<<std::endl;

  // for( int i = 0; i < tightCR->GetNbinsX(); i++){
  //   if( abs(allCR->GetBinContent(i) - allCR_TEST->GetBinContent(i)) > 0 ){
  //     cout << "WARNING: all: " << allCR->GetBinContent(i) << " " << allCR_TEST->GetBinContent(i) << endl;
  //   }
  //   if( abs(tightCR->GetBinContent(i) - tightCR_TEST->GetBinContent(i)) > 0 ){
  //     cout << "WARNING: tight: " << tightCR->GetBinContent(i) << " " << tightCR_TEST->GetBinContent(i) << endl;
  //   }
  //   if( abs(looseCR->GetBinContent(i) - looseCR_TEST->GetBinContent(i)) > 0 ){
  //     cout << "WARNING: loose: " << looseCR->GetBinContent(i) << " " << looseCR_TEST->GetBinContent(i) << endl;
  //   }
  //   if( abs(tightSR_alt->GetBinContent(i) - tightSR_alt_TEST->GetBinContent(i)) > 0 ){
  //     cout << "WARNING: tight: " << tightSR_alt->GetBinContent(i) << " " << tightSR_alt_TEST->GetBinContent(i) << endl;
  //   }
    
  // }

  // TFile f2(output+"test.root","recreate");
  // tightCR_TEST->Write();
  // looseCR_TEST->Write();
  // allCR_TEST->Write();
  // f2.Close();

  TFile f1(output,"recreate");
  tightCR->Write();
  looseCR->Write();
  tightSR_alt->Write();
  allCR->Write();
  f1.Close();

  delete tightCR;delete looseCR;delete tightSR_alt;delete allCR;
  // delete tightCR_TEST;delete looseCR_TEST;delete tightSR_alt_TEST;delete allCR_TEST;
  delete event_s;delete tchain;
}




