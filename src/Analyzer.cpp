#define Analyzer_cxx
#include "Analyzer.h"
#include <TStyle.h>
#include <TCanvas.h>
#include <TH1.h>
#include<TH1F.h>
#include<TH2F.h>
#include<TF1.h>
#include <vector>
#include<TLorentzVector.h>
#include<TLegend.h>
#include<THStack.h>
#include<TGraph.h>
#include<TGraphPainter.h>
#include<TMath.h>
#include "TSystem.h"
#include "cConstants.h"
#include <TFile.h>
#include <cassert>
#include <cmath>
#include <iostream>
#include<TLine.h>
#include<TLatex.h>
#include <TStyle.h>
cConstantSpline::cConstantSpline(const TString& filename) : filename_(filename), spline_(nullptr) {}

void cConstantSpline::initspline(bool isDbkg) {
  if (!spline_) {
    TFile* f_ =TFile::Open(filename_);
    spline_.reset((TSpline3*)(f_->Get("sp_gr_varReco_Constant_Smooth")->Clone()));
    f_->Close();
  }
  assert(spline_.get());
}

double cConstantSpline::eval(double ZZMass, bool isDbkg) {
  initspline(isDbkg);
  return spline_->Eval(ZZMass);
}

namespace {
 
  cConstantSpline DVBF2jetsSpline("SmoothKDConstant_m4l_DjjVBF13TeV.root");
  cConstantSpline DVBF1jetSpline("SmoothKDConstant_m4l_DjVBF13TeV.root");
  cConstantSpline DZHhSpline("SmoothKDConstant_m4l_DjjZH13TeV.root");
  cConstantSpline DWHhSpline("SmoothKDConstant_m4l_DjjWH13TeV.root");
}

extern "C" float getDVBF2jetsConstant(float ZZMass){
  return DVBF2jetsSpline.eval(ZZMass, false);
}

extern "C" float getDVBF1jetConstant(float ZZMass){
  return DVBF1jetSpline.eval(ZZMass, false);
}

extern "C" float getDWHhConstant(float ZZMass){
  return DWHhSpline.eval(ZZMass, false);
}
extern "C" float getDZHhConstant(float ZZMass){
  return DZHhSpline.eval(ZZMass, false);
}

void Analyzer::Loop()
{
 
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntriesFast();

   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      cout<<LepPt->at(0);
      // if (Cut(ientry) < 0) continue;
   }
}
void Analyzer::Fill_Histogram(TString s)
{
  TFile *f;  
   
      f = new TFile(s);
      TDirectory * dir = (TDirectory*)f->Get(s+":/ZZTree");
      dir->GetObject("candTree",tree);

   double w;
   Init(tree);
   h1 = (TH1F*)f->Get("ZZTree/Counters"); //dodano (radi citanja iz Countersa)
   if (fChain == 0) return;
   Long64_t nentries = fChain->GetEntriesFast();
   Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
	  w=137000*xsec*overallEventWeight/h1->GetBinContent(40);
      if(s.Contains("ggH125"))
	  {
          float c_Mela2j = getDVBF2jetsConstant(ZZMass);
          float D_VBF2j=1./(1.+ c_Mela2j*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal);
	       float c_Mela1j = getDVBF1jetConstant(ZZMass);
          float D_VBF1j = 1./(1.+ c_Mela1j*p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal));
	       float c_MelaWH = getDWHhConstant(ZZMass);
	       float D_WHh = 1./(1.+ c_MelaWH*(p_HadWH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadWH_mavjj_JECNominal*p_HadWH_SIG_ghw1_1_JHUGen_JECNominal));
	       float c_MelaZH = getDZHhConstant(ZZMass);
          float D_ZHh = 1./(1.+ c_MelaZH*(p_HadZH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadZH_mavjj_JECNominal*p_HadZH_SIG_ghz1_1_JHUGen_JECNominal));
		    if(nCleanedJetsPt30>=2)
          {
             Histo_D_VBF2j_ggH125->Fill(D_VBF2j,w);
             Histo_D_WH_ggH125->Fill(D_WHh,w);
             Histo_D_ZH_ggH125->Fill(D_ZHh,w);
          }
          if(nCleanedJetsPt30==1)
          Histo_D_VBF1j_ggH125->Fill(D_VBF1j,w);
          Histo_nCleanedJetsPt30_ggH125->Fill(nCleanedJetsPt30,w);
          Histo_nCleanedJetsPt30BTagged_ggH125->Fill(nCleanedJetsPt30BTagged,w);
          Histo_nExtraLep_ggH125->Fill(nExtraLep,w);
          Histo_nExtraZ_ggH125->Fill(nExtraZ,w);
          Histo_ZZPt_ggH125->Fill(ZZPt,w);
          Histo_PFMET_ggH125->Fill(PFMET,w);
		    if(DiJetMass>0)
          Histo_DiJetMass_ggH125->Fill(DiJetMass,w);
          Histo_ZZEta_ggH125->Fill(ZZEta,w);
          Histo_ZZPhi_ggH125->Fill(ZZPhi,w);
          Histo_DiJetDEta_ggH125->Fill(DiJetDEta,w);
          Histo_ZZjjPt_ggH125->Fill(ZZjjPt,w);
          Histo_DiJetFisher_ggH125->Fill(DiJetFisher,w);	  
	  }	  
      if(s.Contains("VBFH125"))
	  {
          float c_Mela2j = getDVBF2jetsConstant(ZZMass);
          float D_VBF2j=1./(1.+ c_Mela2j*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal);
	       float c_Mela1j = getDVBF1jetConstant(ZZMass);
          float D_VBF1j = 1./(1.+ c_Mela1j*p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal));
	       float c_MelaWH = getDWHhConstant(ZZMass);
	       float D_WHh = 1./(1.+ c_MelaWH*(p_HadWH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadWH_mavjj_JECNominal*p_HadWH_SIG_ghw1_1_JHUGen_JECNominal));
	       float c_MelaZH = getDZHhConstant(ZZMass);
          float D_ZHh = 1./(1.+ c_MelaZH*(p_HadZH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadZH_mavjj_JECNominal*p_HadZH_SIG_ghz1_1_JHUGen_JECNominal));
		    if(nCleanedJetsPt30>=2)
          Histo_D_VBF2j_VBFH125->Fill(D_VBF2j,w);
          if(nCleanedJetsPt30==1)
          Histo_D_VBF1j_VBFH125->Fill(D_VBF1j,w);
          Histo_nCleanedJetsPt30_VBFH125->Fill(nCleanedJetsPt30,w);	
          Histo_nCleanedJetsPt30BTagged_VBFH125->Fill(nCleanedJetsPt30BTagged,w);
          Histo_nExtraLep_VBFH125->Fill(nExtraLep,w);
          Histo_nExtraZ_VBFH125->Fill(nExtraZ,w);
		    Histo_ZZPt_VBFH125->Fill(ZZPt,w);
          if(DiJetMass>0)
          Histo_DiJetMass_VBFH125->Fill(DiJetMass,w);
          Histo_PFMET_VBFH125->Fill(PFMET,w);
          Histo_ZZjjPt_VBFH125->Fill(ZZjjPt,w);
          Histo_ZZEta_VBFH125->Fill(ZZEta,w);
          Histo_ZZPhi_VBFH125->Fill(ZZPhi,w);
          Histo_DiJetDEta_VBFH125->Fill(DiJetDEta,w);
          Histo_DiJetFisher_VBFH125->Fill(DiJetFisher,w);		  
      }
	  if(s.Contains("ttH125"))
	  {
         Histo_nCleanedJetsPt30_ttH125->Fill(nCleanedJetsPt30,w);
         Histo_nCleanedJetsPt30BTagged_ttH125->Fill(nCleanedJetsPt30BTagged,w);
         Histo_nExtraLep_ttH125->Fill(nExtraLep,w);
         Histo_nExtraZ_ttH125->Fill(nExtraZ,w);
         Histo_ZZPt_ttH125->Fill(ZZPt,w);
		   if(DiJetMass>0)
         Histo_DiJetMass_ttH125->Fill(DiJetMass,w);
         Histo_PFMET_ttH125->Fill(PFMET,w);
         Histo_ZZEta_ttH125->Fill(ZZEta,w);
         Histo_ZZPhi_ttH125->Fill(ZZPhi,w);
         Histo_DiJetDEta_ttH125->Fill(DiJetDEta,w);
         Histo_ZZjjPt_ttH125->Fill(ZZjjPt,w);
         Histo_DiJetFisher_ttH125->Fill(DiJetFisher,w);		  
      }
      if(s.Contains("WplusH")||s.Contains("WminusH"))
	   {
		   float c_Mela2j = getDVBF2jetsConstant(ZZMass);
         float D_VBF2j=1./(1.+ c_Mela2j*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal);
	      float c_Mela1j = getDVBF1jetConstant(ZZMass);
         float D_VBF1j = 1./(1.+ c_Mela1j*p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal));
	      float c_MelaWH = getDWHhConstant(ZZMass);
	      float D_WHh = 1./(1.+ c_MelaWH*(p_HadWH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadWH_mavjj_JECNominal*p_HadWH_SIG_ghw1_1_JHUGen_JECNominal));
	      float c_MelaZH = getDZHhConstant(ZZMass);
         float D_ZHh = 1./(1.+ c_MelaZH*(p_HadZH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadZH_mavjj_JECNominal*p_HadZH_SIG_ghz1_1_JHUGen_JECNominal));
         if(nCleanedJetsPt30>=2)
         Histo_D_WH_WH125->Fill(D_WHh,w);
         Histo_nCleanedJetsPt30_WH125->Fill(nCleanedJetsPt30,w);
         Histo_nCleanedJetsPt30BTagged_WH125->Fill(nCleanedJetsPt30BTagged,w);
         Histo_nExtraLep_WH125->Fill(nExtraLep,w);
         Histo_nExtraZ_WH125->Fill(nExtraZ,w);
         Histo_ZZPt_WH125->Fill(ZZPt,w);
		   if(DiJetMass>0)
         Histo_DiJetMass_WH125->Fill(DiJetMass,w);
         Histo_PFMET_WH125->Fill(PFMET,w);
         Histo_ZZEta_WH125->Fill(ZZEta,w);
         Histo_ZZPhi_WH125->Fill(ZZPhi,w);
         Histo_DiJetDEta_WH125->Fill(DiJetDEta,w);
         Histo_ZZjjPt_WH125->Fill(ZZjjPt,w);
         Histo_DiJetFisher_WH125->Fill(DiJetFisher,w);	  
      }
      if(s.Contains("ZH"))
	   {
		   float c_Mela2j = getDVBF2jetsConstant(ZZMass);
         float D_VBF2j=1./(1.+ c_Mela2j*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal);
	      float c_Mela1j = getDVBF1jetConstant(ZZMass);
         float D_VBF1j = 1./(1.+ c_Mela1j*p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal));
	      float c_MelaWH = getDWHhConstant(ZZMass);
	      float D_WHh = 1./(1.+ c_MelaWH*(p_HadWH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadWH_mavjj_JECNominal*p_HadWH_SIG_ghw1_1_JHUGen_JECNominal));
	      float c_MelaZH = getDZHhConstant(ZZMass);
         float D_ZHh = 1./(1.+ c_MelaZH*(p_HadZH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadZH_mavjj_JECNominal*p_HadZH_SIG_ghz1_1_JHUGen_JECNominal));
         if(nCleanedJetsPt30>=2)
         Histo_D_ZH_ZH125->Fill(D_ZHh,w);	
         Histo_nCleanedJetsPt30_ZH125->Fill(nCleanedJetsPt30,w);
         Histo_nCleanedJetsPt30BTagged_ZH125->Fill(nCleanedJetsPt30BTagged,w);
         Histo_nExtraLep_ZH125->Fill(nExtraLep,w);
         Histo_nExtraZ_ZH125->Fill(nExtraZ,w);
         Histo_ZZPt_ZH125->Fill(ZZPt,w);
		   if(DiJetMass>0)
         Histo_DiJetMass_ZH125->Fill(DiJetMass,w);
         Histo_PFMET_ZH125->Fill(PFMET,w);
         Histo_ZZEta_ZH125->Fill(ZZEta,w);
         Histo_ZZPhi_ZH125->Fill(ZZPhi,w);
         Histo_DiJetDEta_ZH125->Fill(DiJetDEta,w);
         Histo_ZZjjPt_ZH125->Fill(ZZjjPt,w);
         Histo_DiJetFisher_ZH125->Fill(DiJetFisher,w);  
      }
	  }
	   
}
void Analyzer :: Plot_Histogram()
{
	gStyle->SetOptStat(0);
   
   TCanvas *basic_Canvas=new TCanvas("","",1600,1200);
	basic_Canvas->Divide(2,2);
   //nCleanedJetsPt30
	//TCanvas * nCleanedJetsPt30_Canvas=new TCanvas("nCleanedJetsPt30","nCleanedJetsPt30",1600,800);
	basic_Canvas->cd(1);
	TLegend*  nCleanedJetsPt30_Legend=new TLegend(0.75,0.65,0.9,0.9);
	Histo_nCleanedJetsPt30_ggH125->Scale(1/Histo_nCleanedJetsPt30_ggH125->Integral());
	Histo_nCleanedJetsPt30_VBFH125->Scale(1/Histo_nCleanedJetsPt30_VBFH125->Integral());
	Histo_nCleanedJetsPt30_ttH125->Scale(1/Histo_nCleanedJetsPt30_ttH125->Integral());
   Histo_nCleanedJetsPt30_WH125->Scale(1/Histo_nCleanedJetsPt30_WH125->Integral());
   Histo_nCleanedJetsPt30_ZH125->Scale(1/Histo_nCleanedJetsPt30_ZH125->Integral());
	nCleanedJetsPt30_Legend->AddEntry(Histo_nCleanedJetsPt30_ggH125,"ggH125","f");
	nCleanedJetsPt30_Legend->AddEntry(Histo_nCleanedJetsPt30_VBFH125,"VBFH125","f");
	nCleanedJetsPt30_Legend->AddEntry(Histo_nCleanedJetsPt30_ttH125,"ttH125","f");
	nCleanedJetsPt30_Legend->AddEntry(Histo_nCleanedJetsPt30_WH125,"WH125","f");
   nCleanedJetsPt30_Legend->AddEntry(Histo_nCleanedJetsPt30_ZH125,"ZH125","f");
	Histo_nCleanedJetsPt30_ggH125->SetLineColor(kBlue);
	Histo_nCleanedJetsPt30_VBFH125->SetLineColor(kGreen);
	Histo_nCleanedJetsPt30_ttH125->SetLineColor(kMagenta);
   Histo_nCleanedJetsPt30_WH125->SetLineColor(kRed+2);
   Histo_nCleanedJetsPt30_ZH125->SetLineColor(kPink);
	Histo_nCleanedJetsPt30_ggH125->SetTitle("");
	Histo_nCleanedJetsPt30_ggH125->GetXaxis()->SetTitle("selected jets");
	Histo_nCleanedJetsPt30_ggH125->GetYaxis()->SetTitle("normalized to 1");
	Histo_nCleanedJetsPt30_ggH125->Draw("HIST SAME");
   Histo_nCleanedJetsPt30_VBFH125->Draw("HIST SAME");
	Histo_nCleanedJetsPt30_ttH125->Draw("HIST SAME");
	Histo_nCleanedJetsPt30_WH125->Draw("HIST SAME");
   Histo_nCleanedJetsPt30_ZH125->Draw("HIST SAME");
	nCleanedJetsPt30_Legend->Draw();
	//nCleanedJetsPt30_Canvas->SaveAs("nCleanedJetsPt30.pdf");
	
	//nCleanedJetsPt30BTagged
	//TCanvas * nCleanedJetsPt30BTagged_Canvas=new TCanvas("nCleanedJetsPt30_BTagged","nCleanedJetsPt30_BTagged",1600,800);
	basic_Canvas->cd(2);
	TLegend*  nCleanedJetsPt30BTagged_Legend=new TLegend(0.75,0.65,0.9,0.9);
	Histo_nCleanedJetsPt30BTagged_ggH125->Scale(1/Histo_nCleanedJetsPt30BTagged_ggH125->Integral());
	Histo_nCleanedJetsPt30BTagged_VBFH125->Scale(1/Histo_nCleanedJetsPt30BTagged_VBFH125->Integral());
	Histo_nCleanedJetsPt30BTagged_ttH125->Scale(1/Histo_nCleanedJetsPt30BTagged_ttH125->Integral());
   Histo_nCleanedJetsPt30BTagged_WH125->Scale(1/Histo_nCleanedJetsPt30BTagged_WH125->Integral());
   Histo_nCleanedJetsPt30BTagged_ZH125->Scale(1/Histo_nCleanedJetsPt30BTagged_ZH125->Integral());
	nCleanedJetsPt30BTagged_Legend->AddEntry(Histo_nCleanedJetsPt30BTagged_ggH125,"ggH125","f");
	nCleanedJetsPt30BTagged_Legend->AddEntry(Histo_nCleanedJetsPt30BTagged_VBFH125,"VBFH125","f");
	nCleanedJetsPt30BTagged_Legend->AddEntry(Histo_nCleanedJetsPt30BTagged_ttH125,"ttH125","f");
	nCleanedJetsPt30BTagged_Legend->AddEntry(Histo_nCleanedJetsPt30BTagged_WH125,"WH125","f");
   nCleanedJetsPt30BTagged_Legend->AddEntry(Histo_nCleanedJetsPt30BTagged_ZH125,"ZH125","f");
	Histo_nCleanedJetsPt30BTagged_ggH125->SetLineColor(kBlue);
	Histo_nCleanedJetsPt30BTagged_VBFH125->SetLineColor(kGreen);
	Histo_nCleanedJetsPt30BTagged_ttH125->SetLineColor(kMagenta);
   Histo_nCleanedJetsPt30BTagged_WH125->SetLineColor(kRed+2);
   Histo_nCleanedJetsPt30BTagged_ZH125->SetLineColor(kPink);
	Histo_nCleanedJetsPt30BTagged_ggH125->SetTitle("");
	Histo_nCleanedJetsPt30BTagged_ggH125->GetXaxis()->SetTitle("b-tagged jets");
	Histo_nCleanedJetsPt30BTagged_ggH125->GetYaxis()->SetTitle("normalized to 1");
	Histo_nCleanedJetsPt30BTagged_ggH125->Draw("HIST SAME");
   Histo_nCleanedJetsPt30BTagged_VBFH125->Draw("HIST SAME");
	Histo_nCleanedJetsPt30BTagged_ttH125->Draw("HIST SAME");
	Histo_nCleanedJetsPt30BTagged_WH125->Draw("HIST SAME");
   Histo_nCleanedJetsPt30BTagged_ZH125->Draw("HIST SAME");
	nCleanedJetsPt30BTagged_Legend->Draw();
	//nCleanedJetsPt30BTagged_Canvas->SaveAs("nCleanedJetsPt30BTagged.pdf");
	
	

    //nExtraLep
	 //TCanvas * nExtraLep_Canvas=new TCanvas("nExtraLep","nExtraLep",1600,800);
	 basic_Canvas->cd(3);
	 TLegend*  nExtraLep_Legend=new TLegend(0.75,0.7,0.9,0.9);
	 Histo_nExtraLep_ggH125->Scale(1/Histo_nExtraLep_ggH125->Integral());
	 Histo_nExtraLep_VBFH125->Scale(1/Histo_nExtraLep_VBFH125->Integral());
	 Histo_nExtraLep_ttH125->Scale(1/Histo_nExtraLep_ttH125->Integral());
    Histo_nExtraLep_WH125->Scale(1/Histo_nExtraLep_WH125->Integral());
    Histo_nExtraLep_ZH125->Scale(1/Histo_nExtraLep_ZH125->Integral());
	 //nExtraLep_Legend->AddEntry(Histo_nExtraLep_ggH125,"ggH125","f");
	 nExtraLep_Legend->AddEntry(Histo_nExtraLep_VBFH125,"VBFH125","f");
	 nExtraLep_Legend->AddEntry(Histo_nExtraLep_ttH125,"ttH125","f");
	 nExtraLep_Legend->AddEntry(Histo_nExtraLep_WH125,"WH125","f");
    nExtraLep_Legend->AddEntry(Histo_nExtraLep_ZH125,"ZH125","f");
	 Histo_nExtraLep_ggH125->SetLineColor(kBlue);
	 Histo_nExtraLep_VBFH125->SetLineColor(kGreen);
	 Histo_nExtraLep_ttH125->SetLineColor(kMagenta);
    Histo_nExtraLep_WH125->SetLineColor(kRed+2);
    Histo_nExtraLep_ZH125->SetLineColor(kPink);
	 Histo_nExtraLep_ggH125->SetTitle("");
	 Histo_nExtraLep_ggH125->GetXaxis()->SetTitle("extra leptons");
	 Histo_nExtraLep_ggH125->GetYaxis()->SetTitle("normalized to 1");
	 Histo_nExtraLep_ggH125->Draw("HIST SAME");
    Histo_nExtraLep_VBFH125->Draw("HIST SAME");
	 Histo_nExtraLep_ttH125->Draw("HIST SAME");
	 Histo_nExtraLep_WH125->Draw("HIST SAME");
    Histo_nExtraLep_ZH125->Draw("HIST SAME");
	 nExtraLep_Legend->Draw();
	 //nExtraLep_Canvas->SaveAs("nExtraLep.pdf");

    //nExtraZ
	 //TCanvas * nExtraZ_Canvas=new TCanvas("nExtraZ","nExtraZ",1600,800);
	 basic_Canvas->cd(4);
	 TLegend*  nExtraZ_Legend=new TLegend(0.75,0.75,0.9,0.9);
	 Histo_nExtraZ_ggH125->Scale(1/Histo_nExtraZ_ggH125->Integral());
	 Histo_nExtraZ_VBFH125->Scale(1/Histo_nExtraZ_VBFH125->Integral());
	 Histo_nExtraZ_ttH125->Scale(1/Histo_nExtraZ_ttH125->Integral());
    Histo_nExtraZ_WH125->Scale(1/Histo_nExtraZ_WH125->Integral());
    Histo_nExtraZ_ZH125->Scale(1/Histo_nExtraZ_ZH125->Integral());
	 //nExtraZ_Legend->AddEntry(Histo_nExtraZ_ggH125,"ggH125","f");
	 //nExtraZ_Legend->AddEntry(Histo_nExtraZ_VBFH125,"VBFH125","f");
	 nExtraZ_Legend->AddEntry(Histo_nExtraZ_ttH125,"ttH125","f");
	 nExtraZ_Legend->AddEntry(Histo_nExtraZ_WH125,"WH125","f");
    nExtraZ_Legend->AddEntry(Histo_nExtraZ_ZH125,"ZH125","f");
	 Histo_nExtraZ_ggH125->SetLineColor(kBlue);
	 Histo_nExtraZ_VBFH125->SetLineColor(kGreen);
	 Histo_nExtraZ_ttH125->SetLineColor(kMagenta);
    Histo_nExtraZ_WH125->SetLineColor(kRed+2);
    Histo_nExtraZ_ZH125->SetLineColor(kPink);
	 Histo_nExtraZ_ggH125->SetTitle("");
	 Histo_nExtraZ_ggH125->GetXaxis()->SetTitle("extra Z bosons");
	 Histo_nExtraZ_ggH125->GetYaxis()->SetTitle("normalized to 1");
	 Histo_nExtraZ_ggH125->Draw("HIST SAME");
    Histo_nExtraZ_VBFH125->Draw("HIST SAME");
	 Histo_nExtraZ_ttH125->Draw("HIST SAME");
	 Histo_nExtraZ_WH125->Draw("HIST SAME");
    Histo_nExtraZ_ZH125->Draw("HIST SAME");
	 nExtraZ_Legend->Draw();
	 //nExtraZ_Canvas->SaveAs("nExtraZ.pdf");

    basic_Canvas->SaveAs("basic.pdf");
	
   TCanvas *D_Canvas=new TCanvas("","",1600,1200);
   D_Canvas->Divide(2,2);
   //D_VBF2j
   //TCanvas *D_VBF2j_Canvas=new TCanvas("D_VBF2j","D_VBF2j",1600,800);
	D_Canvas->cd(1);
	TLegend* D_VBF2j_Legend=new TLegend(0.75,0.75,0.9,0.9);
	Histo_D_VBF2j_ggH125->Scale(1/Histo_D_VBF2j_ggH125->Integral());
	Histo_D_VBF2j_VBFH125->Scale(1/Histo_D_VBF2j_VBFH125->Integral());
   D_VBF2j_Legend->AddEntry(Histo_D_VBF2j_ggH125,"ggH125","f");
	D_VBF2j_Legend->AddEntry(Histo_D_VBF2j_VBFH125,"VBFH125","f");
   Histo_D_VBF2j_ggH125->SetLineColor(kBlue);
	Histo_D_VBF2j_VBFH125->SetLineColor(kGreen);
   Histo_D_VBF2j_ggH125->SetTitle("");
   Histo_D_VBF2j_ggH125->GetXaxis()->SetTitleSize(0.04);
   Histo_D_VBF2j_ggH125->GetXaxis()->SetTitle("D^{ME}_{VBF-2j}");
	Histo_D_VBF2j_ggH125->GetYaxis()->SetTitle("normalized to 1");
   Histo_D_VBF2j_ggH125->Draw("HIST SAME");
	Histo_D_VBF2j_VBFH125->Draw("HIST SAME");
	D_VBF2j_Legend->Draw();
	//D_VBF2j_Canvas->SaveAs("D_VBF2j.pdf"); 

   //D_VBF1j 
   //TCanvas *D_VBF1j_Canvas=new TCanvas("D_VBF1j","D_VBF1j",1600,800);
	D_Canvas->cd(2);
	TLegend* D_VBF1j_Legend=new TLegend(0.75,0.75,0.9,0.9);
	Histo_D_VBF1j_ggH125->Scale(1/Histo_D_VBF1j_ggH125->Integral());
	Histo_D_VBF1j_VBFH125->Scale(1/Histo_D_VBF1j_VBFH125->Integral());
   D_VBF1j_Legend->AddEntry(Histo_D_VBF1j_ggH125,"ggH125","f");
	D_VBF1j_Legend->AddEntry(Histo_D_VBF1j_VBFH125,"VBFH125","f");
   Histo_D_VBF1j_ggH125->SetLineColor(kBlue);
	Histo_D_VBF1j_VBFH125->SetLineColor(kGreen);
   Histo_D_VBF1j_ggH125->SetTitle("");
   Histo_D_VBF1j_ggH125->GetXaxis()->SetTitleSize(0.04);
   Histo_D_VBF1j_ggH125->GetXaxis()->SetTitle("D^{ME}_{VBF-1j}");
	Histo_D_VBF1j_ggH125->GetYaxis()->SetTitle("normalized to 1");
   Histo_D_VBF1j_ggH125->Draw("HIST SAME");
	Histo_D_VBF1j_VBFH125->Draw("HIST SAME");
	D_VBF1j_Legend->Draw();
	//D_VBF1j_Canvas->SaveAs("D_VBF1j.pdf");

   //D_WH 
   //TCanvas *D_WH_Canvas=new TCanvas("D_WH","D_WH",1600,800);
	D_Canvas->cd(3);
	TLegend* D_WH_Legend=new TLegend(0.65,0.75,0.8,0.9);
	Histo_D_WH_ggH125->Scale(1/Histo_D_WH_ggH125->Integral());
	Histo_D_WH_WH125->Scale(1/Histo_D_WH_WH125->Integral());
   D_WH_Legend->AddEntry(Histo_D_WH_ggH125,"ggH125","f");
	D_WH_Legend->AddEntry(Histo_D_WH_WH125,"WH125","f");
   Histo_D_WH_ggH125->SetLineColor(kBlue);
	Histo_D_WH_WH125->SetLineColor(kRed+2);
   Histo_D_WH_WH125->GetXaxis()->SetTitleSize(0.04);
   Histo_D_WH_WH125->SetTitle("");
   Histo_D_WH_WH125->GetXaxis()->SetTitle("D^{ME}_{WH-hadr}");
	Histo_D_WH_WH125->GetYaxis()->SetTitle("normalized to 1");
   Histo_D_WH_WH125->Draw("HIST SAME");
   Histo_D_WH_ggH125->Draw("HIST SAME");
	D_WH_Legend->Draw();
	//D_WH_Canvas->SaveAs("D_WH.pdf"); 

   //D_ZH 
   //TCanvas *D_ZH_Canvas=new TCanvas("D_ZH","D_ZH",1600,800);
	D_Canvas->cd(4);
	TLegend* D_ZH_Legend=new TLegend(0.65,0.75,0.8,0.9);
	Histo_D_ZH_ggH125->Scale(1/Histo_D_ZH_ggH125->Integral());
	Histo_D_ZH_ZH125->Scale(1/Histo_D_ZH_ZH125->Integral());
   D_ZH_Legend->AddEntry(Histo_D_ZH_ggH125,"ggH125","f");
	D_ZH_Legend->AddEntry(Histo_D_ZH_ZH125,"ZH125","f");
   Histo_D_ZH_ggH125->SetLineColor(kBlue);
	Histo_D_ZH_ZH125->SetLineColor(kPink);
   Histo_D_ZH_ggH125->GetXaxis()->SetTitleSize(0.04);
   Histo_D_ZH_ggH125->SetTitle("");
   Histo_D_ZH_ggH125->GetXaxis()->SetTitle("D^{ME}_{ZH-hadr}");
	Histo_D_ZH_ggH125->GetYaxis()->SetTitle("normalized to 1");
   Histo_D_ZH_ggH125->Draw("HIST SAME");
	Histo_D_ZH_ZH125->Draw("HIST SAME");
	D_ZH_Legend->Draw();
	//D_ZH_Canvas->SaveAs("D_ZH.pdf"); 

   D_Canvas->SaveAs("Dis.pdf"); 

   TCanvas *additional1_Canvas=new TCanvas("","",1600,1200);
   additional1_Canvas->Divide(2,2);
   //PFMET
	//TCanvas *PFMET_Canvas=new TCanvas("PFMET","PFMET",1600,800);
   additional1_Canvas->cd(1);
	TLegend* PFMET_Legend=new TLegend(0.75,0.65,0.9,0.9);
	Histo_PFMET_ggH125->Scale(1/Histo_PFMET_ggH125->Integral());
	Histo_PFMET_VBFH125->Scale(1/Histo_PFMET_VBFH125->Integral());
	Histo_PFMET_ttH125->Scale(1/Histo_PFMET_ttH125->Integral());
   Histo_PFMET_WH125->Scale(1/Histo_PFMET_WH125->Integral());
   Histo_PFMET_ZH125->Scale(1/Histo_PFMET_ZH125->Integral());
	PFMET_Legend->AddEntry(Histo_PFMET_ggH125,"ggH125","f");
	PFMET_Legend->AddEntry(Histo_PFMET_VBFH125,"VBFH125","f");
	PFMET_Legend->AddEntry(Histo_PFMET_ttH125,"ttH125","f");
	PFMET_Legend->AddEntry(Histo_PFMET_WH125,"WH125","f");
   PFMET_Legend->AddEntry(Histo_PFMET_ZH125,"ZH125","f");
	Histo_PFMET_ggH125->SetLineColor(kBlue);
	Histo_PFMET_VBFH125->SetLineColor(kGreen);
	Histo_PFMET_ttH125->SetLineColor(kMagenta);
   Histo_PFMET_WH125->SetLineColor(kRed+2);
   Histo_PFMET_ZH125->SetLineColor(kPink);
	Histo_PFMET_ggH125->SetTitle("");
	Histo_PFMET_ggH125->GetXaxis()->SetTitle("missing transverse momentum");
	Histo_PFMET_ggH125->GetYaxis()->SetTitle("normalized to 1");
	Histo_PFMET_ggH125->Draw("HIST SAME");
	Histo_PFMET_VBFH125->Draw("HIST SAME");
	Histo_PFMET_ttH125->Draw("HIST SAME");
   Histo_PFMET_WH125->Draw("HIST SAME");
   Histo_PFMET_ZH125->Draw("HIST SAME");
	PFMET_Legend->Draw();
	//PFMET_Canvas->SaveAs("PFMET.pdf"); 

   //ZZPt
	//TCanvas *ZZPt_Canvas=new TCanvas("ZZPt","ZZPt",1600,800);
	additional1_Canvas->cd(2);
   TLegend* ZZPt_Legend=new TLegend(0.75,0.65,0.9,0.9);
	Histo_ZZPt_ggH125->Scale(1/Histo_ZZPt_ggH125->Integral());
	Histo_ZZPt_VBFH125->Scale(1/Histo_ZZPt_VBFH125->Integral());
	Histo_ZZPt_ttH125->Scale(1/Histo_ZZPt_ttH125->Integral());
   Histo_ZZPt_WH125->Scale(1/Histo_ZZPt_WH125->Integral());
   Histo_ZZPt_ZH125->Scale(1/Histo_ZZPt_ZH125->Integral());
	ZZPt_Legend->AddEntry(Histo_ZZPt_ggH125,"ggH125","f");
	ZZPt_Legend->AddEntry(Histo_ZZPt_VBFH125,"VBFH125","f");
	ZZPt_Legend->AddEntry(Histo_ZZPt_ttH125,"ttH125","f");
	ZZPt_Legend->AddEntry(Histo_ZZPt_WH125,"WH125","f");
   ZZPt_Legend->AddEntry(Histo_ZZPt_ZH125,"ZH125","f");
	Histo_ZZPt_ggH125->SetLineColor(kBlue);
	Histo_ZZPt_VBFH125->SetLineColor(kGreen);
	Histo_ZZPt_ttH125->SetLineColor(kMagenta);
   Histo_ZZPt_WH125->SetLineColor(kRed+2);
   Histo_ZZPt_ZH125->SetLineColor(kPink);
	Histo_ZZPt_ggH125->SetTitle("");
	Histo_ZZPt_ggH125->GetXaxis()->SetTitle("ZZ pair transverse momentum");
	Histo_ZZPt_ggH125->GetYaxis()->SetTitle("normalized to 1");
	Histo_ZZPt_ggH125->Draw("HIST SAME");
	Histo_ZZPt_VBFH125->Draw("HIST SAME");
	Histo_ZZPt_ttH125->Draw("HIST SAME");
   Histo_ZZPt_WH125->Draw("HIST SAME");
   Histo_ZZPt_ZH125->Draw("HIST SAME");
	ZZPt_Legend->Draw();
	//ZZPt_Canvas->SaveAs("ZZPt.pdf");

   //DiJetMass
	//TCanvas *DiJetMass_Canvas=new TCanvas("DiJetMass","DiJetMass",1600,800);
	additional1_Canvas->cd(3);
   TLegend* DiJetMass_Legend=new TLegend(0.75,0.65,0.9,0.9);
	Histo_DiJetMass_ggH125->Scale(1/Histo_DiJetMass_ggH125->Integral());
	Histo_DiJetMass_VBFH125->Scale(1/Histo_DiJetMass_VBFH125->Integral());
	Histo_DiJetMass_ttH125->Scale(1/Histo_DiJetMass_ttH125->Integral());
   Histo_DiJetMass_WH125->Scale(1/Histo_DiJetMass_WH125->Integral());
   Histo_DiJetMass_ZH125->Scale(1/Histo_DiJetMass_ZH125->Integral());
	DiJetMass_Legend->AddEntry(Histo_DiJetMass_ggH125,"ggH125","f");
	DiJetMass_Legend->AddEntry(Histo_DiJetMass_VBFH125,"VBFH125","f");
	DiJetMass_Legend->AddEntry(Histo_DiJetMass_ttH125,"ttH125","f");
	DiJetMass_Legend->AddEntry(Histo_DiJetMass_WH125,"WH125","f");
   DiJetMass_Legend->AddEntry(Histo_DiJetMass_ZH125,"ZH125","f");
	Histo_DiJetMass_ggH125->SetLineColor(kBlue);
	Histo_DiJetMass_VBFH125->SetLineColor(kGreen);
	Histo_DiJetMass_ttH125->SetLineColor(kMagenta);
   Histo_DiJetMass_WH125->SetLineColor(kRed+2);
   Histo_DiJetMass_ZH125->SetLineColor(kPink);
	Histo_DiJetMass_WH125->SetTitle("");
	Histo_DiJetMass_WH125->GetXaxis()->SetTitle("dijet mass");
	Histo_DiJetMass_WH125->GetYaxis()->SetTitle("normalized to 1");
	Histo_DiJetMass_WH125->Draw("HIST SAME");
   Histo_DiJetMass_ggH125->Draw("HIST SAME");
	Histo_DiJetMass_VBFH125->Draw("HIST SAME");
	Histo_DiJetMass_ttH125->Draw("HIST SAME");
   Histo_DiJetMass_ZH125->Draw("HIST SAME");
	DiJetMass_Legend->Draw();
	//DiJetMass_Canvas->SaveAs("DiJetMass.pdf");

   //DiJetDEta
   //TCanvas *DiJetDEta_Canvas=new TCanvas("DiJetDEta","DiJetDEta",1600,800);
   additional1_Canvas->cd(4);
	TLegend* DiJetDEta_Legend=new TLegend(0.75,0.65,0.9,0.9);
	Histo_DiJetDEta_ggH125->Scale(1/Histo_DiJetDEta_ggH125->Integral());
	Histo_DiJetDEta_VBFH125->Scale(1/Histo_DiJetDEta_VBFH125->Integral());
	Histo_DiJetDEta_ttH125->Scale(1/Histo_DiJetDEta_ttH125->Integral());
   Histo_DiJetDEta_WH125->Scale(1/Histo_DiJetDEta_WH125->Integral());
   Histo_DiJetDEta_ZH125->Scale(1/Histo_DiJetDEta_ZH125->Integral());
	DiJetDEta_Legend->AddEntry(Histo_DiJetDEta_ggH125,"ggH125","f");
	DiJetDEta_Legend->AddEntry(Histo_DiJetDEta_VBFH125,"VBFH125","f");
	DiJetDEta_Legend->AddEntry(Histo_DiJetDEta_ttH125,"ttH125","f");
	DiJetDEta_Legend->AddEntry(Histo_DiJetDEta_WH125,"WH125","f");
   DiJetDEta_Legend->AddEntry(Histo_DiJetDEta_ZH125,"ZH125","f");
	Histo_DiJetDEta_ggH125->SetLineColor(kBlue);
	Histo_DiJetDEta_VBFH125->SetLineColor(kGreen);
	Histo_DiJetDEta_ttH125->SetLineColor(kMagenta);
   Histo_DiJetDEta_WH125->SetLineColor(kRed+2);
   Histo_DiJetDEta_ZH125->SetLineColor(kPink);
	Histo_DiJetDEta_WH125->SetTitle("");
	Histo_DiJetDEta_WH125->GetXaxis()->SetTitle("dijet pseudorapidity");
	Histo_DiJetDEta_WH125->GetYaxis()->SetTitle("normalized to 1");
	Histo_DiJetDEta_WH125->Draw("HIST SAME");
   Histo_DiJetDEta_ZH125->Draw("HIST SAME");
   Histo_DiJetDEta_ggH125->Draw("HIST SAME");
	Histo_DiJetDEta_VBFH125->Draw("HIST SAME");
	Histo_DiJetDEta_ttH125->Draw("HIST SAME");
	DiJetDEta_Legend->Draw();
	//DiJetDEta_Canvas->SaveAs("DijetDEta.pdf");

   additional1_Canvas->SaveAs("additional1.pdf");

   TCanvas *additional2_Canvas=new TCanvas("","",1600,1200);
   additional2_Canvas->Divide(2,2);

   //ZZjjPt
	//TCanvas *ZZjjPt_Canvas=new TCanvas("ZZjjPt","ZZjjPt",1600,800);
	additional2_Canvas->cd(1);
   TLegend* ZZjjPt_Legend=new TLegend(0.75,0.65,0.9,0.9);
	Histo_ZZjjPt_ggH125->Scale(1/Histo_ZZjjPt_ggH125->Integral());
	Histo_ZZjjPt_VBFH125->Scale(1/Histo_ZZjjPt_VBFH125->Integral());
	Histo_ZZjjPt_ttH125->Scale(1/Histo_ZZjjPt_ttH125->Integral());
   Histo_ZZjjPt_WH125->Scale(1/Histo_ZZjjPt_WH125->Integral());
   Histo_ZZjjPt_ZH125->Scale(1/Histo_ZZjjPt_ZH125->Integral());
	ZZjjPt_Legend->AddEntry(Histo_ZZjjPt_ggH125,"ggH125","f");
	ZZjjPt_Legend->AddEntry(Histo_ZZjjPt_VBFH125,"VBFH125","f");
	ZZjjPt_Legend->AddEntry(Histo_ZZjjPt_ttH125,"ttH125","f");
	ZZjjPt_Legend->AddEntry(Histo_ZZjjPt_WH125,"WH125","f");
   ZZjjPt_Legend->AddEntry(Histo_ZZjjPt_ZH125,"ZH125","f");
	Histo_ZZjjPt_ggH125->SetLineColor(kBlue);
	Histo_ZZjjPt_VBFH125->SetLineColor(kGreen);
	Histo_ZZjjPt_ttH125->SetLineColor(kMagenta);
   Histo_ZZjjPt_WH125->SetLineColor(kRed+2);
   Histo_ZZjjPt_ZH125->SetLineColor(kPink);
	Histo_ZZjjPt_VBFH125->SetTitle("");
	Histo_ZZjjPt_VBFH125->GetXaxis()->SetTitle("ZZjj transverse momentum");
	Histo_ZZjjPt_VBFH125->GetYaxis()->SetTitle("normalized to 1");
	Histo_ZZjjPt_VBFH125->Draw("HIST SAME");
   Histo_ZZjjPt_ggH125->Draw("HIST SAME");
	Histo_ZZjjPt_ttH125->Draw("HIST SAME");
   Histo_ZZjjPt_WH125->Draw("HIST SAME");
   Histo_ZZjjPt_ZH125->Draw("HIST SAME");
	ZZjjPt_Legend->Draw();
	//ZZjjPt_Canvas->SaveAs("ZZjjPt.pdf");

   //ZZEta
	//TCanvas *ZZEta_Canvas=new TCanvas("ZZEta","ZZEta",1600,800);
	additional2_Canvas->cd(2);
   TLegend* ZZEta_Legend=new TLegend(0.75,0.65,0.9,0.9);
	Histo_ZZEta_ggH125->Scale(1/Histo_ZZEta_ggH125->Integral());
	Histo_ZZEta_VBFH125->Scale(1/Histo_ZZEta_VBFH125->Integral());
	Histo_ZZEta_ttH125->Scale(1/Histo_ZZEta_ttH125->Integral());
   Histo_ZZEta_WH125->Scale(1/Histo_ZZEta_WH125->Integral());
   Histo_ZZEta_ZH125->Scale(1/Histo_ZZEta_ZH125->Integral());
	ZZEta_Legend->AddEntry(Histo_ZZEta_ggH125,"ggH125","f");
	ZZEta_Legend->AddEntry(Histo_ZZEta_VBFH125,"VBFH125","f");
	ZZEta_Legend->AddEntry(Histo_ZZEta_ttH125,"ttH125","f");
	ZZEta_Legend->AddEntry(Histo_ZZEta_WH125,"WH125","f");
   ZZEta_Legend->AddEntry(Histo_ZZEta_ZH125,"ZH125","f");
	Histo_ZZEta_ggH125->SetLineColor(kBlue);
	Histo_ZZEta_VBFH125->SetLineColor(kGreen);
	Histo_ZZEta_ttH125->SetLineColor(kMagenta);
   Histo_ZZEta_WH125->SetLineColor(kRed+2);
   Histo_ZZEta_ZH125->SetLineColor(kPink);
	Histo_ZZEta_ttH125->SetTitle("");
	Histo_ZZEta_ttH125->GetXaxis()->SetTitle("ZZ pair pseudorapidity");
	Histo_ZZEta_ttH125->GetYaxis()->SetTitle("normalized to 1");
	Histo_ZZEta_ttH125->Draw("HIST SAME");
   Histo_ZZEta_WH125->Draw("HIST SAME");
   Histo_ZZEta_ZH125->Draw("HIST SAME");
   Histo_ZZEta_ggH125->Draw("HIST SAME");
	Histo_ZZEta_VBFH125->Draw("HIST SAME");
	ZZEta_Legend->Draw();
	//ZZEta_Canvas->SaveAs("ZZEta.pdf");

   //ZZPhi
	//TCanvas *ZZPhi_Canvas=new TCanvas("ZZPhi","ZZPhi",1600,800);
	additional2_Canvas->cd(3);
   TLegend* ZZPhi_Legend=new TLegend(0.75,0.65,0.9,0.9);
	Histo_ZZPhi_ggH125->Scale(1/Histo_ZZPhi_ggH125->Integral());
	Histo_ZZPhi_VBFH125->Scale(1/Histo_ZZPhi_VBFH125->Integral());
	Histo_ZZPhi_ttH125->Scale(1/Histo_ZZPhi_ttH125->Integral());
   Histo_ZZPhi_WH125->Scale(1/Histo_ZZPhi_WH125->Integral());
   Histo_ZZPhi_ZH125->Scale(1/Histo_ZZPhi_ZH125->Integral());
	ZZPhi_Legend->AddEntry(Histo_ZZPhi_ggH125,"ggH125","f");
	ZZPhi_Legend->AddEntry(Histo_ZZPhi_VBFH125,"VBFH125","f");
	ZZPhi_Legend->AddEntry(Histo_ZZPhi_ttH125,"ttH125","f");
	ZZPhi_Legend->AddEntry(Histo_ZZPhi_WH125,"WH125","f");
   ZZPhi_Legend->AddEntry(Histo_ZZPhi_ZH125,"ZH125","f");
	Histo_ZZPhi_ggH125->SetLineColor(kBlue);
	Histo_ZZPhi_VBFH125->SetLineColor(kGreen);
	Histo_ZZPhi_ttH125->SetLineColor(kMagenta);
   Histo_ZZPhi_WH125->SetLineColor(kRed+2);
   Histo_ZZPhi_ZH125->SetLineColor(kPink);
	Histo_ZZPhi_ZH125->SetTitle("");
	Histo_ZZPhi_ZH125->GetXaxis()->SetTitle("ZZ pair azimuthal angle");
	Histo_ZZPhi_ZH125->GetYaxis()->SetTitle("normalized to 1");
	Histo_ZZPhi_ZH125->Draw("HIST SAME");
   Histo_ZZPhi_ggH125->Draw("HIST SAME");
	Histo_ZZPhi_VBFH125->Draw("HIST SAME");
	Histo_ZZPhi_ttH125->Draw("HIST SAME");
   Histo_ZZPhi_WH125->Draw("HIST SAME");
	ZZPhi_Legend->Draw();
	//ZZPhi_Canvas->SaveAs("ZZPhi.pdf");

   //DiJetFisher
	//TCanvas *DiJetFisher_Canvas=new TCanvas("DiJetFisher","DiJetFisher",1600,800);
	additional2_Canvas->cd(4);
   TLegend* DiJetFisher_Legend=new TLegend(0.75,0.65,0.9,0.9);
	Histo_DiJetFisher_ggH125->Scale(1/Histo_DiJetFisher_ggH125->Integral());
	Histo_DiJetFisher_VBFH125->Scale(1/Histo_DiJetFisher_VBFH125->Integral());
	Histo_DiJetFisher_ttH125->Scale(1/Histo_DiJetFisher_ttH125->Integral());
   Histo_DiJetFisher_WH125->Scale(1/Histo_DiJetFisher_WH125->Integral());
   Histo_DiJetFisher_ZH125->Scale(1/Histo_DiJetFisher_ZH125->Integral());
	DiJetFisher_Legend->AddEntry(Histo_DiJetFisher_ggH125,"ggH125","f");
	DiJetFisher_Legend->AddEntry(Histo_DiJetFisher_VBFH125,"VBFH125","f");
	DiJetFisher_Legend->AddEntry(Histo_DiJetFisher_ttH125,"ttH125","f");
	DiJetFisher_Legend->AddEntry(Histo_DiJetFisher_WH125,"WH125","f");
   DiJetFisher_Legend->AddEntry(Histo_DiJetFisher_ZH125,"ZH125","f");
	Histo_DiJetFisher_ggH125->SetLineColor(kBlue);
	Histo_DiJetFisher_VBFH125->SetLineColor(kGreen);
	Histo_DiJetFisher_ttH125->SetLineColor(kMagenta);
   Histo_DiJetFisher_WH125->SetLineColor(kRed+2);
   Histo_DiJetFisher_ZH125->SetLineColor(kPink);
	Histo_DiJetFisher_ZH125->SetTitle("");
	Histo_DiJetFisher_ZH125->GetXaxis()->SetTitle("dijet Fisher constant");
	Histo_DiJetFisher_ZH125->GetYaxis()->SetTitle("normalized to 1");
	Histo_DiJetFisher_ZH125->Draw("HIST SAME");
   Histo_DiJetFisher_ggH125->Draw("HIST SAME");
	Histo_DiJetFisher_VBFH125->Draw("HIST SAME");
	Histo_DiJetFisher_ttH125->Draw("HIST SAME");
   Histo_DiJetFisher_WH125->Draw("HIST SAME");
   Histo_DiJetFisher_ZH125->Draw("HIST SAME");
	DiJetFisher_Legend->Draw();
	//DiJetFisher_Canvas->SaveAs("DiJetFisher.pdf"); 

   additional2_Canvas->SaveAs("additional2.pdf");
	 
}

void Analyzer :: Categorize(TString s1)
{
	TFile *f;  
 
      f = new TFile(s1);
      TDirectory * dir = (TDirectory*)f->Get(s1+":/ZZTree");
      dir->GetObject("candTree",tree);
   Init(tree);
   h1 = (TH1F*)f->Get("ZZTree/Counters");  
   if (fChain == 0) return;
   Long64_t nentries = fChain->GetEntriesFast();
   double w;
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
	  w=137000*xsec*overallEventWeight/h1->GetBinContent(40);
	  float c_Mela2j = getDVBF2jetsConstant(ZZMass);
      float D_VBF2j=1./(1.+ c_Mela2j*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal);
	  float c_Mela1j = getDVBF1jetConstant(ZZMass);
      float D_VBF1j = 1./(1.+ c_Mela1j*p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal));
	  float c_MelaWH = getDWHhConstant(ZZMass);
	  float D_WHh = 1./(1.+ c_MelaWH*(p_HadWH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadWH_mavjj_JECNominal*p_HadWH_SIG_ghw1_1_JHUGen_JECNominal));
	  float c_MelaZH = getDZHhConstant(ZZMass);
      float D_ZHh = 1./(1.+ c_MelaZH*(p_HadZH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadZH_mavjj_JECNominal*p_HadZH_SIG_ghz1_1_JHUGen_JECNominal));      	  
	  if(s1.Contains("ggH125"))
	  {
	 
	if( nExtraLep==0 && (((nCleanedJetsPt30==2||nCleanedJetsPt30==3)&&nCleanedJetsPt30BTagged_bTagSF<=1)||(nCleanedJetsPt30>=4&&nCleanedJetsPt30BTagged_bTagSF==0)) && D_VBF2j>0.5 )
    Histo_ggH125->Fill(4.5,w);

    else if( nExtraLep==0 && (nCleanedJetsPt30==2||nCleanedJetsPt30==3||(nCleanedJetsPt30>=4&&nCleanedJetsPt30BTagged_bTagSF==0)) && (D_WHh>0.5||D_ZHh>0.5) )
    Histo_ggH125->Fill(3.5,w); 

    else if( ( nCleanedJetsPt30<=3 && nCleanedJetsPt30BTagged_bTagSF==0 && (nExtraLep==1||nExtraZ>=1) )|| ( nCleanedJetsPt30==0 && nExtraLep>=1 ) )
    Histo_ggH125->Fill(2.5,w); 

    else if( nCleanedJetsPt30>=4 && nCleanedJetsPt30BTagged_bTagSF>=1 && nExtraLep ==0)
    Histo_ggH125->Fill(1.5,w); 
  
    else if( nExtraLep>=1 )
    Histo_ggH125->Fill(0.5,w);
	
    else if( nExtraLep==0 && nCleanedJetsPt30==1 && D_VBF1j>0.5 )
    Histo_ggH125->Fill(5.5,w);
 
    else
    Histo_ggH125->Fill(6.5,w);
	 
    }
	
	  if(s1.Contains("VBFH125"))
	  {
		   
	if( nExtraLep==0 && (((nCleanedJetsPt30==2||nCleanedJetsPt30==3)&&nCleanedJetsPt30BTagged_bTagSF<=1)||(nCleanedJetsPt30>=4&&nCleanedJetsPt30BTagged_bTagSF==0)) && D_VBF2j>0.5 )
    Histo_VBFH125->Fill(4.5,w);

    else if( nExtraLep==0 && (nCleanedJetsPt30==2||nCleanedJetsPt30==3||(nCleanedJetsPt30>=4&&nCleanedJetsPt30BTagged_bTagSF==0)) && (D_WHh>0.5||D_ZHh>0.5) )
    Histo_VBFH125->Fill(3.5,w); 

    else if( ( nCleanedJetsPt30<=3 && nCleanedJetsPt30BTagged_bTagSF==0 && (nExtraLep==1||nExtraZ>=1) )|| ( nCleanedJetsPt30==0 && nExtraLep>=1 ) )
    Histo_VBFH125->Fill(2.5,w); 

    else if( nCleanedJetsPt30>=4 && nCleanedJetsPt30BTagged_bTagSF>=1 && nExtraLep ==0)
    Histo_VBFH125->Fill(1.5,w); 
  
    else if( nExtraLep>=1 )
    Histo_VBFH125->Fill(0.5,w);
	
    else if( nExtraLep==0 && nCleanedJetsPt30==1 && D_VBF1j>0.5 )
    Histo_VBFH125->Fill(5.5,w);
 
    else
    Histo_VBFH125->Fill(6.5,w); 
		   

	  }
	  if(s1.Contains("ttH125"))
	  {
	 
	if( nExtraLep==0 && (((nCleanedJetsPt30==2||nCleanedJetsPt30==3)&&nCleanedJetsPt30BTagged_bTagSF<=1)||(nCleanedJetsPt30>=4&&nCleanedJetsPt30BTagged_bTagSF==0)) && D_VBF2j>0.5 )
    Histo_ttH125->Fill(4.5,w);

    else if( nExtraLep==0 && (nCleanedJetsPt30==2||nCleanedJetsPt30==3||(nCleanedJetsPt30>=4&&nCleanedJetsPt30BTagged_bTagSF==0)) && (D_WHh>0.5||D_ZHh>0.5) )
    Histo_ttH125->Fill(3.5,w); 

    else if( ( nCleanedJetsPt30<=3 && nCleanedJetsPt30BTagged_bTagSF==0 && (nExtraLep==1||nExtraZ>=1) )|| ( nCleanedJetsPt30==0 && nExtraLep>=1 ) )
    Histo_ttH125->Fill(2.5,w); 

    else if( nCleanedJetsPt30>=4 && nCleanedJetsPt30BTagged_bTagSF>=1 && nExtraLep ==0)
    Histo_ttH125->Fill(1.5,w); 
  
    else if( nExtraLep>=1 )
    Histo_ttH125->Fill(0.5,w);
	
    else if( nExtraLep==0 && nCleanedJetsPt30==1 && D_VBF1j>0.5 )
    Histo_ttH125->Fill(5.5,w);
 
    else
    Histo_ttH125->Fill(6.5,w); 
	 
	  }
	  
	  if(s1.Contains("WminusH125")||s1.Contains("WplusH125")||s1.Contains("ZH125"))
	  {

    
	if( nExtraLep==0 && (((nCleanedJetsPt30==2||nCleanedJetsPt30==3)&&nCleanedJetsPt30BTagged_bTagSF<=1)||(nCleanedJetsPt30>=4&&nCleanedJetsPt30BTagged_bTagSF==0)) && D_VBF2j>0.5 )
    Histo_VH125->Fill(4.5,w);

    else if( nExtraLep==0 && (nCleanedJetsPt30==2||nCleanedJetsPt30==3||(nCleanedJetsPt30>=4&&nCleanedJetsPt30BTagged_bTagSF==0)) && (D_WHh>0.5||D_ZHh>0.5) )
    Histo_VH125->Fill(3.5,w); 

    else if( ( nCleanedJetsPt30<=3 && nCleanedJetsPt30BTagged_bTagSF==0 && (nExtraLep==1||nExtraZ>=1) )|| ( nCleanedJetsPt30==0 && nExtraLep>=1 ) )
    Histo_VH125->Fill(2.5,w); 

    else if( nCleanedJetsPt30>=4 && nCleanedJetsPt30BTagged_bTagSF>=1 && nExtraLep ==0)
    Histo_VH125->Fill(1.5,w); 
  
    else if( nExtraLep>=1 )
    Histo_VH125->Fill(0.5,w);
	
    else if( nExtraLep==0 && nCleanedJetsPt30==1 && D_VBF1j>0.5 )
    Histo_VH125->Fill(5.5,w);
 
    else
    Histo_VH125->Fill(6.5,w);
	 
	  }
  }
}

void Analyzer :: Categorize_Display()
{
	int i;
	gStyle->SetOptStat(0);
	const char *categories[7]={"ttH-leptonic tagged ","ttH-hadronic tagged ","VH-leptonic tagged ","VH-hadronic tagged ","VBF-2jet tagged "," VBF-1jet tagged"," Untagged"};
	Histo_ggH125->SetTitle("ggH");
	Histo_VBFH125->SetTitle("VBF");
	Histo_ttH125->SetTitle("ttH");
	Histo_VH125->SetTitle("VH");
	for (i=1;i<=7;i++) 
	{
		Histo_ggH125->GetXaxis()->SetBinLabel(i,categories[i-1]);
		Histo_VBFH125->GetXaxis()->SetBinLabel(i,categories[i-1]);
		Histo_ttH125->GetXaxis()->SetBinLabel(i,categories[i-1]);
		Histo_VH125->GetXaxis()->SetBinLabel(i,categories[i-1]);
	}
	Histo_ggH125->SetFillColor(kBlue);
	Histo_VBFH125->SetFillColor(kGreen+2);
	Histo_ttH125->SetFillColor(kMagenta);
	Histo_VH125->SetFillColor(kRed);
	Histo_ggH125->GetYaxis()->SetTitle("Expected events");
	Histo_VBFH125->GetYaxis()->SetTitle("Expected events");
	Histo_ttH125->GetYaxis()->SetTitle("Expected events");
	Histo_VH125->GetYaxis()->SetTitle("Expected events");
	
	TCanvas *canvas_categories_1=new TCanvas("ggH125","ggH125",1400,800);
	canvas_categories_1->Divide(2,2);
	
	Histo_ggH125->GetYaxis()->SetTitleSize(0.05);
	Histo_ggH125->GetXaxis()->SetLabelSize(0.08);
    Histo_ggH125->GetYaxis()->SetLabelSize(0.04);
    Histo_ggH125->SetBarOffset(0.1);
	Histo_ggH125->SetBarWidth(0.8);
	canvas_categories_1->cd(1);
	gPad->SetLeftMargin(0.35);
	Histo_ggH125->Draw("hbar");
	
	Histo_VBFH125->GetYaxis()->SetTitleSize(0.05);
	Histo_VBFH125->GetXaxis()->SetLabelSize(0.08);
	Histo_VBFH125->GetYaxis()->SetLabelSize(0.04);
	Histo_VBFH125->SetBarOffset(0.1);
	Histo_VBFH125->SetBarWidth(0.8);
	canvas_categories_1->cd(2);
	gPad->SetLeftMargin(0.25);
	Histo_VBFH125->Draw("hbar");
	
	Histo_ttH125->GetYaxis()->SetTitleSize(0.05);
	Histo_ttH125->GetXaxis()->SetLabelSize(0.08);
	Histo_ttH125->GetYaxis()->SetLabelSize(0.04);
	Histo_ttH125->SetBarOffset(0.1);
	Histo_ttH125->SetBarWidth(0.8);
	canvas_categories_1->cd(3);
	gPad->SetLeftMargin(0.35);
	Histo_ttH125->Draw("hbar");
	
	Histo_VH125->GetYaxis()->SetTitleSize(0.05);
	Histo_VH125->GetXaxis()->SetLabelSize(0.08);
	Histo_VH125->GetYaxis()->SetLabelSize(0.04);
	Histo_VH125->SetBarOffset(0.1);
	Histo_VH125->SetBarWidth(0.8);
	canvas_categories_1->cd(4);
	gPad->SetLeftMargin(0.25);
	Histo_VH125->GetYaxis()->SetRangeUser(0., 5.);
	Histo_VH125->Draw("hbar");
	
	//canvas_categories_1->SaveAs("Categorization.png");
	
	double VBF2j_exp_events=Histo_ggH125->Integral(5,5)+Histo_VBFH125->Integral(5,5)+Histo_ttH125->Integral(5,5)+Histo_VH125->Integral(5,5);
	double VHh_exp_events=Histo_ggH125->Integral(4,4)+Histo_VBFH125->Integral(4,4)+Histo_ttH125->Integral(4,4)+Histo_VH125->Integral(4,4);
	double VHl_exp_events=Histo_ggH125->Integral(3,3)+Histo_VBFH125->Integral(3,3)+Histo_ttH125->Integral(3,3)+Histo_VH125->Integral(3,3);
	double ttHh_exp_events=Histo_ggH125->Integral(2,2)+Histo_VBFH125->Integral(2,2)+Histo_ttH125->Integral(2,2)+Histo_VH125->Integral(2,2);
	double ttHl_exp_events=Histo_ggH125->Integral(1,1)+Histo_VBFH125->Integral(1,1)+Histo_ttH125->Integral(1,1)+Histo_VH125->Integral(1,1);
	double VBF1j_exp_events=Histo_ggH125->Integral(6,6)+Histo_VBFH125->Integral(6,6)+Histo_ttH125->Integral(6,6)+Histo_VH125->Integral(6,6);
	double Untagged_exp_events=Histo_ggH125->Integral(7,7)+Histo_VBFH125->Integral(7,7)+Histo_ttH125->Integral(7,7)+Histo_VH125->Integral(7,7);
	
	
	double exp_events[7]={ttHl_exp_events,ttHh_exp_events,VHl_exp_events,VHh_exp_events,VBF2j_exp_events,VBF1j_exp_events,Untagged_exp_events};
	
   cout<<"ttH:"<<endl;
   cout<<(Histo_ggH125->Integral(1,1)+Histo_ggH125->Integral(2,2))/(exp_events[0]+exp_events[1])<<endl;
   cout<<(Histo_VBFH125->Integral(1,1)+Histo_VBFH125->Integral(2,2))/(exp_events[0]+exp_events[1])<<endl;
   cout<<(Histo_ttH125->Integral(1,1)+Histo_ttH125->Integral(2,2))/(exp_events[0]+exp_events[1])<<" "<<Histo_ttH125->Integral(1,1)+Histo_ttH125->Integral(2,2)<<endl;
   cout<<(Histo_VH125->Integral(1,1)+Histo_VH125->Integral(2,2))/(exp_events[0]+exp_events[1])<<endl;
   cout<<"VH:"<<endl;
   cout<<(Histo_ggH125->Integral(3,3)+Histo_ggH125->Integral(4,4))/(exp_events[2]+exp_events[3])<<endl;
   cout<<(Histo_VBFH125->Integral(3,3)+Histo_VBFH125->Integral(4,4))/(exp_events[2]+exp_events[3])<<endl;
   cout<<(Histo_ttH125->Integral(3,3)+Histo_ttH125->Integral(4,4))/(exp_events[2]+exp_events[3])<<endl;
   cout<<(Histo_VH125->Integral(3,3)+Histo_VH125->Integral(4,4))/(exp_events[2]+exp_events[3])<<" "<<Histo_VH125->Integral(3,3)+Histo_VH125->Integral(4,4)<<endl;
   cout<<"VBFH:"<<endl;
   cout<<(Histo_ggH125->Integral(5,5)+Histo_ggH125->Integral(6,6))/(exp_events[4]+exp_events[5])<<endl;
   cout<<(Histo_VBFH125->Integral(5,5)+Histo_VBFH125->Integral(6,6))/(exp_events[4]+exp_events[5])<<" "<<Histo_VBFH125->Integral(5,5)+Histo_VBFH125->Integral(6,6)<<endl;
   cout<<(Histo_ttH125->Integral(5,5)+Histo_ttH125->Integral(6,6))/(exp_events[4]+exp_events[5])<<endl;
   cout<<(Histo_VH125->Integral(5,5)+Histo_VH125->Integral(6,6))/(exp_events[4]+exp_events[5])<<endl;
   cout<<"Untagged:"<<endl;
   cout<<(Histo_ggH125->Integral(7,7) )/(exp_events[6] )<<" "<<Histo_ggH125->Integral(7,7)<<endl;
   cout<<(Histo_VBFH125->Integral(7,7) )/(exp_events[6] )<<endl;
   cout<<(Histo_ttH125->Integral(7,7) )/(exp_events[6] )<<endl;
   cout<<(Histo_VH125->Integral(7,7) )/(exp_events[6] )<<endl;

   cout<<endl<<ttHl_exp_events+ttHh_exp_events<<" "<<VHl_exp_events+VHh_exp_events<<" "<<VBF1j_exp_events+VBF2j_exp_events<<" "<<Untagged_exp_events<<endl;
	
   for(i=1;i<=7;i++)
	{
	  Histo_ggH125->SetBinContent(i,Histo_ggH125->Integral(i,i)/exp_events[i-1]);	
     Histo_VBFH125->SetBinContent(i,Histo_VBFH125->Integral(i,i)/exp_events[i-1]); 
     Histo_ttH125->SetBinContent(i,Histo_ttH125->Integral(i,i)/exp_events[i-1]);
     Histo_VH125->SetBinContent(i,Histo_VH125->Integral(i,i)/exp_events[i-1]);
	}
	THStack *stack = new THStack("stack","Categorization (simulation) ; ;fraction");
	TCanvas *master_canvas=new TCanvas("master","master",1400,800);
	TLegend* legend=new TLegend(0.9,0.7,0.98,0.9);
	legend->AddEntry(Histo_ggH125,"ggH","f");
	legend->AddEntry(Histo_VBFH125,"VBFH","f");
	legend->AddEntry(Histo_ttH125,"ttH","f");
	legend->AddEntry(Histo_VH125,"VH","f");
	gPad->SetLeftMargin(0.2);
	TLatex *t1=new TLatex(0.3,0.35,"0.66 exp. events");
	TLatex *t2=new TLatex(0.3,1.35,"1.17 exp. events");
	TLatex *t3=new TLatex(0.3,2.35,"1.53 exp. events");
	TLatex *t4=new TLatex(0.3,3.35,"11.59 exp. events");
	TLatex *t5=new TLatex(0.3,4.35,"12.09 exp. events");
	TLatex *t6=new TLatex(0.3,5.35,"23.80 exp. events");
	TLatex *t7=new TLatex(0.3,6.35,"168.57 exp. events");
	t1->SetTextColor(kWhite);
	t2->SetTextColor(kWhite);
	t1->SetTextColor(kWhite);
	t3->SetTextColor(kWhite);
	t4->SetTextColor(kWhite);
	t5->SetTextColor(kWhite);
	t6->SetTextColor(kWhite);
	t7->SetTextColor(kWhite);
	stack->Add(Histo_ggH125);
	stack->Add(Histo_VBFH125);
	stack->Add(Histo_ttH125);
	stack->Add(Histo_VH125);
	stack->Draw("hbar");
	legend->Draw();
	t1->Draw();
	t2->Draw();
	t3->Draw();
	t4->Draw();
	t5->Draw();
	t6->Draw();
	t7->Draw();
	//master_canvas->SaveAs("Categorization_stack.pdf");
   //stack
}
 
void Analyzer :: TMVAMultiClass()
{
	 
    TMVA::Tools::Instance();
 
   TString outfileName = "TMVAMulticlass.root";
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );
 
   TMVA::Factory *factory = new TMVA::Factory( "TMVAMulticlass", outputFile,
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=multiclass" );
   TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset");  
 
   //dataloader->AddVariable( "D_VBF2j:=1./(1.+ p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal)", 'F' );
   //dataloader->AddVariable("D_VBF1j := 1./(1.+p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal))",'F');
   //dataloader->AddVariable("D_WHh := 1./(1.+ (p_HadWH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadWH_mavjj_JECNominal*p_HadWH_SIG_ghw1_1_JHUGen_JECNominal))",'F');
   //dataloader->AddVariable("D_ZHh := 1./(1.+ (p_HadZH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadZH_mavjj_JECNominal*p_HadZH_SIG_ghz1_1_JHUGen_JECNominal))",'F');
   //dataloader->AddVariable("1.0/(1+(1*p_QQB_BKG_MCFM)/p_GG_SIG_ghg2_1_ghz1_1_JHUGen)",'F');  // separator za qqZZ 

    dataloader->AddVariable("p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal",'F');
    dataloader->AddVariable("p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal",'F');
    dataloader->AddVariable("p_JQCD_SIG_ghg2_1_JHUGen_JECNominal",'F');
    dataloader->AddVariable("p_JVBF_SIG_ghv1_1_JHUGen_JECNominal",'F');
    dataloader->AddVariable("pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal",'F');
     
     
     	
	
   dataloader->AddVariable("ZZMass",'F');
   dataloader->AddVariable( "nCleanedJetsPt30BTagged", 'F' );
   dataloader->AddVariable( "nExtraLep", 'F' );
   dataloader->AddVariable( "nCleanedJetsPt30", 'F' );
   
   
   TFile *input1(0);
   TString fname1 = "/home/public/data/2018_MC/ggH125/ZZ4lAnalysis.root";
   if (!gSystem->AccessPathName( fname1 )) {
      input1 = TFile::Open( fname1 ); // check if file in local directory exists
   }
   if (!input1) {
      std::cout << "ERROR: could not open data file" << std::endl;
      exit(1);
   }
   TFile *input2(0);
   TString fname2 = "/home/public/data/2018_MC/VBFH125/ZZ4lAnalysis.root";
   if (!gSystem->AccessPathName( fname2 )) {
      input2 = TFile::Open( fname2 );  
   }
   
   TFile *input3(0);
   TString fname3 = "/home/public/data/2018_MC/ttH125/ZZ4lAnalysis.root";
   if (!gSystem->AccessPathName( fname3 )) {
      input3 = TFile::Open( fname3 ); // check if file in local directory exists
   }
   TFile *input4(0);
   TString fname4 = "/home/public/data/2018_MC/ZZTo4lext1/ZZ4lAnalysis.root";
   if (!gSystem->AccessPathName( fname4 )) {
      input4 = TFile::Open( fname4 ); // check if file in local directory exists
   }
    
   TTree *Tree_ggH  = (TTree*)input1->Get("ZZTree/candTree");
   TTree *Tree_VBFH = (TTree*)input2->Get("ZZTree/candTree");
   TTree *Tree_ttH = (TTree*)input3->Get("ZZTree/candTree");
   TTree *Tree_qqZZ = (TTree*)input4->Get("ZZTree/candTree");
       
   
   gROOT->cd( outfileName+TString(":/") );
   dataloader->AddTree(Tree_ggH,"ggH");
   dataloader->AddTree(Tree_VBFH,"VBFH");
   dataloader->AddTree(Tree_ttH,"ttH");
   //dataloader->AddTree(Tree_qqZZ,"qqZZ");
   
   dataloader->SetWeightExpression ("137000*xsec*overallEventWeight/28744188","ggH");
   dataloader->SetWeightExpression ("137000*xsec*overallEventWeight/1819984.75","VBFH");
   dataloader->SetWeightExpression ("137000*xsec*overallEventWeight/257544.9375","ttH");
   //dataloader->SetWeightExpression ("137000/8398762*xsec*overallEventWeight","qqZZ");
   
    TCut cut="ZZMass>118 && ZZMass<130";
    dataloader->PrepareTrainingAndTestTree(cut, "SplitMode=Random:NormMode=NumEvents:!V" );
   //dataloader->PrepareTrainingAndTestTree(cut, "nTrain_ggH=1000:nTrain_VBFH=1000:nTrain_ttH=1000:SplitMode=Random:NormMode=NumEvents:!V" );
 
   factory->BookMethod( dataloader,  TMVA::Types::kBDT, "BDTG", "!H:!V:NTrees=1000:BoostType=Grad:Shrinkage=0.10:UseBaggedBoost:BaggedSampleFraction=0.50:nCuts=20:MaxDepth=2:IgnoreNegWeightsInTraining");
 
   factory->TrainAllMethods();
 
   factory->TestAllMethods();
 
   factory->EvaluateAllMethods();
 
   outputFile->Close();
 
   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAMulticlass is done!" << std::endl;
   
   
   TCanvas *canvas1;
   canvas1= new TCanvas("","",1600,900);
   TFile *f1 = new TFile("TMVAMulticlass.root");  
   TH1F* a = (TH1F*)f1->Get("dataset/Method_BDT/BDTG/MVA_BDTG_Test_ggH_prob_for_ggH");
   TH1F* b = (TH1F*)f1->Get("dataset/Method_BDT/BDTG/MVA_BDTG_Test_VBFH_prob_for_ggH");
   TH1F* c = (TH1F*)f1->Get("dataset/Method_BDT/BDTG/MVA_BDTG_Test_ttH_prob_for_ggH");
   TH1F* d = (TH1F*)f1->Get("dataset/Method_BDT/BDTG/MVA_BDTG_Test_ggH_prob_for_VBFH");
   TH1F* e = (TH1F*)f1->Get("dataset/Method_BDT/BDTG/MVA_BDTG_Test_VBFH_prob_for_VBFH");
   TH1F* f = (TH1F*)f1->Get("dataset/Method_BDT/BDTG/MVA_BDTG_Test_ttH_prob_for_VBFH");
   TH1F* g = (TH1F*)f1->Get("dataset/Method_BDT/BDTG/MVA_BDTG_Test_ggH_prob_for_ttH");
   TH1F* h = (TH1F*)f1->Get("dataset/Method_BDT/BDTG/MVA_BDTG_Test_VBFH_prob_for_ttH");
   TH1F* i = (TH1F*)f1->Get("dataset/Method_BDT/BDTG/MVA_BDTG_Test_ttH_prob_for_ttH");
   a->GetYaxis()->SetRangeUser(0,5);
   c->GetYaxis()->SetRangeUser(0,38);
   d->GetYaxis()->SetRangeUser(0,5);
   i->GetYaxis()->SetRangeUser(0,6);
   canvas1->Divide(3,3);
   canvas1->cd(1);
   a->SetFillColor(kBlue);
   b->SetFillColor(kBlue);
   c->SetFillColor(kBlue);
   d->SetFillColor(kBlue);
   e->SetFillColor(kBlue);
   f->SetFillColor(kBlue);
   g->SetFillColor(kBlue);
   h->SetFillColor(kBlue);
   i->SetFillColor(kBlue);
   a->Draw("BAR");
   canvas1->cd(2);
   b->Draw("BAR");
   canvas1->cd(3);
   c->Draw("BAR");
   canvas1->cd(4);
   d->Draw("BAR");
   canvas1->cd(5);
   e->Draw("BAR");
   canvas1->cd(6);
   f->Draw("BAR");
   canvas1->cd(7);
   g->Draw("BAR");
   canvas1->cd(8);
   h->Draw("BAR");
   canvas1->cd(9);
   i->Draw("BAR");
   canvas1->SaveAs("BDT_distributions.pdf"); 
   delete factory;
   delete dataloader;
   
   //TTreeReader reader("dataset/TrainTree",  TFile::Open("TMVAMulticlass.root"));
   //TTreeReaderValue<Float_t> reader_bdtg(reader, "BDTG");
   //while (reader.Next()) {std::cout << *reader_bdtg.Get() << std::endl;} 
}

void Analyzer :: TMVAMultiClassApplication()
{
	
   TMVA::Tools::Instance();
   
   std::map<std::string,int> Use;
   Use["BDTG"]            = 1;
  
   TMVA::Reader *reader = new TMVA::Reader( "!Color:!Silent" );
    
   Float_t  D_VBF2j, D_VBF1j, ZZMass,p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal, p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal,nCleanedJetsPt30BTagged, nExtraLep, nCleanedJetsPt30,
   p_JQCD_SIG_ghg2_1_JHUGen_JECNominal,p_JVBF_SIG_ghv1_1_JHUGen_JECNominal,pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal,ZZPt,xsec, overallEventWeight;
   
   Short_t a,b,c;
   
   //reader->AddVariable( "D_VBF2j:=1./(1.+ p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal)", &D_VBF2j );
   //reader->AddVariable( "D_VBF1j:= 1./(1.+p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal))", &D_VBF1j );
   reader->AddVariable( "p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal", &p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal );
   reader->AddVariable( "p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal", &p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal );
   reader->AddVariable( "p_JQCD_SIG_ghg2_1_JHUGen_JECNominal", &p_JQCD_SIG_ghg2_1_JHUGen_JECNominal );
   reader->AddVariable( "p_JVBF_SIG_ghv1_1_JHUGen_JECNominal", &p_JVBF_SIG_ghv1_1_JHUGen_JECNominal );
   reader->AddVariable( "pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal", &pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal );
   reader->AddVariable( "ZZMass", &ZZMass );
   reader->AddVariable( "nCleanedJetsPt30BTagged", &nCleanedJetsPt30BTagged );
   reader->AddVariable( "nExtraLep", &nExtraLep );
   reader->AddVariable( "nCleanedJetsPt30", &nCleanedJetsPt30);
  
   TString dir    = "dataset/weights/";
   TString prefix = "TMVAMulticlass";
   for (std::map<std::string,int>::iterator it = Use.begin(); it != Use.end(); it++) {
      if (it->second) {
        TString methodName = TString(it->first) + TString(" method");
        TString weightfile = dir + prefix + TString("_") + TString(it->first) + TString(".weights.xml");
        reader->BookMVA( methodName, weightfile );
      }
   }
  
   UInt_t nbin = 100;
   TH1F  *histBDTG_ttH(0);
   histBDTG_ttH  = new TH1F( "MVA_BDTG_ttH",   "MVA_BDTG_ttH",   nbin, 0., 1.1 );
   TFile *input(0);
   TString fname = "/home/public/data/2018_MC/ttH125/ZZ4lAnalysis.root";
   if (!gSystem->AccessPathName( fname )) {
      input = TFile::Open( fname );  
   }
   if (!input) {
      std::cout << "ERROR: could not open data file, please generate example data first!" << std::endl;
      exit(1);
   }
   std::cout << "--- TMVAMulticlassApp : Using input file: " << input->GetName() << std::endl;
  
   TTree* theTree = (TTree*)input->Get("ZZTree/candTree");
   std::cout << "--- Select ttH sample" << std::endl;
   theTree->SetBranchAddress( "p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal", &p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal );
   theTree->SetBranchAddress( "p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal", &p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal );
   theTree->SetBranchAddress( "p_JQCD_SIG_ghg2_1_JHUGen_JECNominal", &p_JQCD_SIG_ghg2_1_JHUGen_JECNominal );
   theTree->SetBranchAddress( "p_JVBF_SIG_ghv1_1_JHUGen_JECNominal", &p_JVBF_SIG_ghv1_1_JHUGen_JECNominal );
   theTree->SetBranchAddress( "pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal", &pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal );
   theTree->SetBranchAddress( "ZZMass", &ZZMass );
   theTree->SetBranchAddress( "nCleanedJetsPt30BTagged", &a );
   theTree->SetBranchAddress( "nExtraLep", &b );
   theTree->SetBranchAddress( "nCleanedJetsPt30", &c );
   theTree->SetBranchAddress( "xsec", &xsec );
   theTree->SetBranchAddress( "overallEventWeight", &overallEventWeight );
   std::cout << "--- Processing: " << theTree->GetEntries() << " events" << std::endl;
   TStopwatch sw;
   sw.Start();
   for (Long64_t ievt=0; ievt<theTree->GetEntries();ievt++) {
      if (ievt%1000 == 0){
         std::cout << "--- ... Processing event: " << ievt << std::endl;
      }
      theTree->GetEntry(ievt);
	  //D_VBF2j=1./(1.+ p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal);
	  //D_VBF1j= 1./(1.+p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal)); 
      	nCleanedJetsPt30BTagged=a;	
		nExtraLep=b;
		nCleanedJetsPt30=c;
		double w=137000*xsec*overallEventWeight/257544.9375;
		if(ZZMass>118 && ZZMass<130)
        histBDTG_ttH->Fill((reader->EvaluateMulticlass( "BDTG method" ))[0],w);     		
   }
   
   sw.Stop();
   std::cout << "--- End of event loop: "; sw.Print();
   TFile *target  = new TFile( "TMVAMulticlassApp.root","RECREATE" );
    
   histBDTG_ttH->Write();
    
   target->Close();
   std::cout << "--- Created root file: \"TMVMulticlassApp.root\" containing the MVA output histograms" << std::endl;
   delete reader;
   std::cout << "==> TMVAMulticlassApp is done!" << std::endl << std::endl;
   
   /*TCanvas *canvas1;
   canvas1= new TCanvas("","",1600,900);
   TFile *f1 = new TFile("TMVAMulticlassApp.root");  
   TH1F* histo = (TH1F*)f1->Get("MVA_BDTG_ttH");
   histo->SetFillColor(kBlue);
   //histo->GetYaxis()->SetRangeUser(0,1.1);
   histo->Draw("BAR");
   canvas1->SaveAs("BDT_Application_ttH.pdf");
   double x=histo->Integral();
   cout<<x<<endl;*/
}

void Analyzer :: HiggsProduction_Training()
{
   
   TMVA::Tools::Instance();
   
   std::map<std::string,int> Use;
   TString outfileName( "TMVA.root" );
   TFile* outputFile = TFile::Open( outfileName, "RECREATE" );
   TMVA::Factory *factory = new TMVA::Factory( "TMVAClassification", outputFile,
                                               "!V:!Silent:Color:DrawProgressBar:Transformations=I;D;P;G,D:AnalysisType=Classification" );
   TMVA::DataLoader *dataloader=new TMVA::DataLoader("dataset_3");
   
   dataloader->AddVariable( "D_VBF2j:=1./(1.+ p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal)", 'F' );
   dataloader->AddVariable("D_VBF1j := 1./(1.+p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal))",'F');
   dataloader->AddVariable("D_WHh := 1./(1.+ (p_HadWH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadWH_mavjj_JECNominal*p_HadWH_SIG_ghw1_1_JHUGen_JECNominal))",'F');
   dataloader->AddVariable("D_ZHh := 1./(1.+ (p_HadZH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadZH_mavjj_JECNominal*p_HadZH_SIG_ghz1_1_JHUGen_JECNominal))",'F');
   
    
   /*dataloader->AddVariable( "p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal", 'F' );
   dataloader->AddVariable( "p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal", 'F' );
   dataloader->AddVariable( "p_JQCD_SIG_ghg2_1_JHUGen_JECNominal", 'F' );
   dataloader->AddVariable( "p_JVBF_SIG_ghv1_1_JHUGen_JECNominal", 'F' );
   dataloader->AddVariable( "pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal", 'F' );
   dataloader->AddVariable( "p_HadWH_mavjj_true_JECNominal", 'F' );
   dataloader->AddVariable( "p_HadWH_SIG_ghw1_1_JHUGen_JECNominal", 'F' );

   dataloader->AddVariable( "PFMET", 'F' );
   dataloader->AddVariable( "ZZPt", 'F' );
   dataloader->AddVariable( "DiJetMass", 'F' );
   dataloader->AddVariable( "DiJetDEta", 'F' );
   dataloader->AddVariable( "ZZEta", 'F' );
   dataloader->AddVariable( "ZZPhi", 'F' );
   dataloader->AddVariable( "ZZjjPt", 'F' );
   dataloader->AddVariable( "DiJetFisher", 'F' );*/
    
   dataloader->AddVariable( "nCleanedJetsPt30BTagged", 'F' );
   dataloader->AddVariable( "nExtraLep", 'F' );
   dataloader->AddVariable( "nCleanedJetsPt30", 'F' );
   
   TFile *input1 = new TFile("ZZ4lAnalysis_ggH.root","update");
   TFile *input2 = new TFile("ZZ4lAnalysis_VBFH.root","update");
   TFile *input3 = new TFile("ZZ4lAnalysis_ttH.root","update");
   TFile *input4 = new TFile("ZZ4lAnalysis_WminusH.root","update");
   TFile *input5 = new TFile("ZZ4lAnalysis_WplusH.root","update");
   TFile *input6 = new TFile("ZZ4lAnalysis_ZH.root","update");
   
   
   TTree *Tree_ggH  = (TTree*)input1->Get("ZZTree/candTree");
   TTree *Tree_VBFH = (TTree*)input2->Get("ZZTree/candTree");
   TTree *Tree_ttH = (TTree*)input3->Get("ZZTree/candTree");
   TTree *Tree_WminusH = (TTree*)input4->Get("ZZTree/candTree");
   TTree *Tree_WplusH = (TTree*)input5->Get("ZZTree/candTree");
   TTree *Tree_ZH = (TTree*)input6->Get("ZZTree/candTree");
  
   Float_t overallEventWeight,xsec,weight;
   TBranch *b1_weight = Tree_ggH->Branch("weight",&weight,"weight/F");
   Tree_ggH->SetBranchAddress("xsec",&xsec); 
   Tree_ggH->SetBranchAddress("overallEventWeight",&overallEventWeight);
  
   Long64_t nentries = Tree_ggH->GetEntries(); for (Long64_t i=0;i<nentries;i++) 
   { 
       Tree_ggH->GetEntry(i); 
	   weight=137000*xsec*overallEventWeight/28744188; 
	   b1_weight->Fill();
   }
 
   TBranch *b2_weight = Tree_VBFH->Branch("weight",&weight,"weight/F");
   Tree_VBFH->SetBranchAddress("xsec",&xsec);
   Tree_VBFH->SetBranchAddress("overallEventWeight",&overallEventWeight);
   nentries = Tree_VBFH->GetEntries(); 
   for (Long64_t i=0;i<nentries;i++) 
   { 
      Tree_VBFH->GetEntry(i);
	  weight=137000*xsec*overallEventWeight/1819984.75;
	  b2_weight->Fill();
   }
   
   TBranch *b3_weight = Tree_ttH->Branch("weight",&weight,"weight/F");
   Tree_ttH->SetBranchAddress("xsec",&xsec);
   Tree_ttH->SetBranchAddress("overallEventWeight",&overallEventWeight);
   nentries = Tree_ttH->GetEntries(); 
   for (Long64_t i=0;i<nentries;i++)
	{ 
      Tree_ttH->GetEntry(i); 
	  weight=137000*xsec*overallEventWeight/257544.9375;
	  b3_weight->Fill();
	} 
	
   TBranch *b4_weight = Tree_WminusH->Branch("weight",&weight,"weight/F");
   Tree_WminusH->SetBranchAddress("xsec",&xsec);
   Tree_WminusH->SetBranchAddress("overallEventWeight",&overallEventWeight);
   nentries = Tree_WminusH->GetEntries(); 
   for (Long64_t i=0;i<nentries;i++)
	{ 
      Tree_WminusH->GetEntry(i); 
	  weight=137000*xsec*overallEventWeight/103808;
	  b4_weight->Fill();
	}

   TBranch *b5_weight = Tree_WplusH->Branch("weight",&weight,"weight/F");
   Tree_WplusH->SetBranchAddress("xsec",&xsec);
   Tree_WplusH->SetBranchAddress("overallEventWeight",&overallEventWeight);
   nentries = Tree_WplusH->GetEntries(); 
   for (Long64_t i=0;i<nentries;i++)
	{ 
      Tree_WplusH->GetEntry(i); 
	  weight=137000*xsec*overallEventWeight/247439;
	  b5_weight->Fill();
	}
  	
   TBranch *b6_weight = Tree_ZH->Branch("weight",&weight,"weight/F");
   Tree_ZH->SetBranchAddress("xsec",&xsec);
   Tree_ZH->SetBranchAddress("overallEventWeight",&overallEventWeight);
   nentries = Tree_ZH->GetEntries(); 
   for (Long64_t i=0;i<nentries;i++)
	{ 
      Tree_ZH->GetEntry(i); 
	  weight=137000*xsec*overallEventWeight/351375;
	  b6_weight->Fill();
	} 
	
   dataloader->SetSignalWeightExpression("weight");   
   dataloader->SetBackgroundWeightExpression("weight");
   
   //dataloader->AddSignalTree(Tree_ttH);
   dataloader->AddBackgroundTree( Tree_ggH );
   dataloader->AddSignalTree( Tree_VBFH );
   //dataloader->AddSignalTree( Tree_WminusH );
   //dataloader->AddSignalTree( Tree_WplusH );
   //dataloader->AddSignalTree( Tree_ZH );
   
   TCut mycuts = "ZZMass>118 && ZZMass<130 && pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal!=0";  
   TCut mycutb = "ZZMass>118 && ZZMass<130 && pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal!=0";  
   //dataloader->PrepareTrainingAndTestTree( mycuts, mycutb, "nTrain_Signal=1000:nTrain_Background=1000:SplitMode=Random:NormMode=NumEvents:!V" );
   dataloader->PrepareTrainingAndTestTree( mycuts, mycutb, "SplitMode=Random:NormMode=None:!V" );

   factory->BookMethod( dataloader, TMVA::Types::kBDT, "BDT",
    "!H:!V:NTrees=1000:MinNodeSize=2.5%:MaxDepth=3:BoostType=AdaBoost:AdaBoostBeta=0.5:UseBaggedBoost:BaggedSampleFraction=0.5:SeparationType=GiniIndex:nCuts=20" );
    	
   factory->TrainAllMethods();

   factory->TestAllMethods();

   factory->EvaluateAllMethods();

   outputFile->Close();
   std::cout << "==> Wrote root file: " << outputFile->GetName() << std::endl;
   std::cout << "==> TMVAClassification is done!" << std::endl;
   
   delete factory;
   delete dataloader;
   
   //TFile *f = new TFile("TMVA.root"); 
   //TH1F* signal = (TH1F*)f->Get("dataset/Method_BDT/BDT/MVA_BDT_Train_S");
   //TH1F* bkg = (TH1F*)f->Get("dataset/Method_BDT/BDT/MVA_BDT_Train_B");
   
   gStyle->SetOptStat(0);
   TH1F* signal = new TH1F("signal", " ", 160, -0.3, 0.4);
   TH1F* bkg = new TH1F("bkg", "bkg", 160, -0.3, 0.4);
   TLine *line=new TLine(0.0349,0,0.0349,0.64);
   TLatex *t=new TLatex(0.05,0.3,"#scale[0.7]{cut=0.0349}");
   line->SetLineStyle(kDashed);
   signal->SetLineColor(kBlue);
   bkg->SetLineColor(kRed);
   Float_t x,y; 
   TTreeReader reader("dataset_3/TrainTree",  TFile::Open("TMVA.root"));
   TTreeReaderValue<Int_t> reader_classID(reader, "classID");
   TTreeReaderValue<Float_t> reader_bdt(reader, "BDT");
   TTreeReaderValue<Float_t> reader_weight(reader, "weight");
   while (reader.Next()) 
   {
	   //cout << *reader_bdt.Get() <<" "<< *reader_weight.Get() <<endl;
	   if(*reader_classID.Get()==0)
		 signal->Fill(*reader_bdt.Get(),*reader_weight.Get());
       else
         bkg->Fill(*reader_bdt.Get(),*reader_weight.Get());		   
   } 
   signal->Scale(1./signal->Integral());
   bkg->Scale(1./bkg->Integral());
	TCanvas *canvas= new TCanvas("","",1600,900);
   auto legend = new TLegend(0.8,0.8,0.9,0.9);
   legend->AddEntry(signal,"VBFH","f");
   legend->AddEntry(bkg,"ggH","f");
   bkg->GetXaxis()->SetTitle("BDT_{ggH}^{VBFH}");
   bkg->GetYaxis()->SetTitle("normalized to 1");
   bkg->Draw("hist");
   signal->Draw("hist same");
   legend->Draw();
   line->Draw();
   t->Draw();
   canvas->SaveAs("VBFH_BDT_basic.pdf");
   
}

void Analyzer :: HiggsProduction_Application(TString s1)
{
	TMVA::Tools::Instance();
   std::map<std::string,int> Use;
   Use["BDT"]            = 1;
   TMVA::Reader *reader_1 = new TMVA::Reader( "!Color:!Silent" );
   TMVA::Reader *reader_2 = new TMVA::Reader( "!Color:!Silent" );
   TMVA::Reader *reader_3 = new TMVA::Reader( "!Color:!Silent" );
   Float_t  D_VBF2j, D_VBF1j,D_WHh,D_ZHh, ZZMass,p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal, p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal,nCleanedJetsPt30BTagged, nExtraLep, nCleanedJetsPt30,
   p_JQCD_SIG_ghg2_1_JHUGen_JECNominal,p_JVBF_SIG_ghv1_1_JHUGen_JECNominal,pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal,p_HadWH_mavjj_true_JECNominal,p_HadWH_mavjj_JECNominal,
   p_HadWH_SIG_ghw1_1_JHUGen_JECNominal,p_HadZH_mavjj_true_JECNominal,p_HadZH_mavjj_JECNominal,p_HadZH_SIG_ghz1_1_JHUGen_JECNominal,xsec, overallEventWeight,
   PFMET, ZZPt, DiJetMass, DiJetDEta, DiJetFisher, ZZEta, ZZPhi, ZZjjPt;
   Short_t a,b,c;
   //Float_t ttH_cut=0.12, VH_cut=0.12, VBF_cut=0.04;
   //Float_t ttH_cut=0.122, VH_cut=0.108, VBF_cut=0.03645;
   Float_t ttH_cut=0.122, VH_cut=0.08, VBF_cut=0.023;
   // ttH vs ggH+VBF+VH
   reader_1->AddVariable( "D_VBF2j:=1./(1.+ p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal)", &D_VBF2j );
   reader_1->AddVariable( "D_VBF1j:= 1./(1.+p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal))", &D_VBF1j );
   reader_1->AddVariable("D_WHh := 1./(1.+ (p_HadWH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadWH_mavjj_JECNominal*p_HadWH_SIG_ghw1_1_JHUGen_JECNominal))", &D_WHh);
   reader_1->AddVariable("D_ZHh := 1./(1.+ (p_HadZH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadZH_mavjj_JECNominal*p_HadZH_SIG_ghz1_1_JHUGen_JECNominal))", &D_ZHh);
   reader_1->AddVariable( "p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal", &p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal );
   reader_1->AddVariable( "p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal", &p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal );
   reader_1->AddVariable( "p_JQCD_SIG_ghg2_1_JHUGen_JECNominal", &p_JQCD_SIG_ghg2_1_JHUGen_JECNominal );
   reader_1->AddVariable( "p_JVBF_SIG_ghv1_1_JHUGen_JECNominal", &p_JVBF_SIG_ghv1_1_JHUGen_JECNominal );
   reader_1->AddVariable( "pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal", &pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal );
   reader_1->AddVariable( "p_HadWH_mavjj_true_JECNominal", &p_HadWH_mavjj_true_JECNominal );
   reader_1->AddVariable( "p_HadWH_SIG_ghw1_1_JHUGen_JECNominal", &p_HadWH_SIG_ghw1_1_JHUGen_JECNominal );
   
   reader_1->AddVariable( "PFMET", &PFMET );
   reader_1->AddVariable( "ZZPt",  &ZZPt );
   reader_1->AddVariable( "DiJetMass", &DiJetMass );
   reader_1->AddVariable( "DiJetDEta", &DiJetDEta );
   reader_1->AddVariable( "ZZEta", &ZZEta );
   reader_1->AddVariable( "ZZPhi", &ZZPhi );
   reader_1->AddVariable( "ZZjjPt", &ZZjjPt );
   reader_1->AddVariable( "DiJetFisher", &DiJetFisher );
   reader_1->AddVariable( "nCleanedJetsPt30BTagged", &nCleanedJetsPt30BTagged );
   reader_1->AddVariable( "nExtraLep", &nExtraLep );
   reader_1->AddVariable( "nCleanedJetsPt30", &nCleanedJetsPt30);
   
   // VH vs ggh+VBF
   reader_2->AddVariable( "D_VBF2j:=1./(1.+ p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal)", &D_VBF2j );
   reader_2->AddVariable( "D_VBF1j:= 1./(1.+p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal))", &D_VBF1j );
   reader_2->AddVariable("D_WHh := 1./(1.+ (p_HadWH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadWH_mavjj_JECNominal*p_HadWH_SIG_ghw1_1_JHUGen_JECNominal))", &D_WHh);
   reader_2->AddVariable("D_ZHh := 1./(1.+ (p_HadZH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadZH_mavjj_JECNominal*p_HadZH_SIG_ghz1_1_JHUGen_JECNominal))", &D_ZHh);
   reader_2->AddVariable( "p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal", &p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal );
   reader_2->AddVariable( "p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal", &p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal );
   reader_2->AddVariable( "p_JQCD_SIG_ghg2_1_JHUGen_JECNominal", &p_JQCD_SIG_ghg2_1_JHUGen_JECNominal );
   reader_2->AddVariable( "p_JVBF_SIG_ghv1_1_JHUGen_JECNominal", &p_JVBF_SIG_ghv1_1_JHUGen_JECNominal );
   reader_2->AddVariable( "pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal", &pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal );
   reader_2->AddVariable( "p_HadWH_mavjj_true_JECNominal", &p_HadWH_mavjj_true_JECNominal );
   reader_2->AddVariable( "p_HadWH_SIG_ghw1_1_JHUGen_JECNominal", &p_HadWH_SIG_ghw1_1_JHUGen_JECNominal );
   
   reader_2->AddVariable( "PFMET", &PFMET );
   reader_2->AddVariable( "ZZPt", &ZZPt );
   reader_2->AddVariable( "DiJetMass", &DiJetMass );
   reader_2->AddVariable( "DiJetDEta", &DiJetDEta );
   reader_2->AddVariable( "ZZEta", &ZZEta );
   reader_2->AddVariable( "ZZPhi", &ZZPhi );
   reader_2->AddVariable( "ZZjjPt", &ZZjjPt );
   reader_2->AddVariable( "DiJetFisher", &DiJetFisher );
   reader_2->AddVariable( "nCleanedJetsPt30BTagged", &nCleanedJetsPt30BTagged );
   reader_2->AddVariable( "nExtraLep", &nExtraLep );
   reader_2->AddVariable( "nCleanedJetsPt30", &nCleanedJetsPt30);
   
   // ggH vs VBF
   reader_3->AddVariable( "D_VBF2j:=1./(1.+ p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal)", &D_VBF2j );
   reader_3->AddVariable( "D_VBF1j:= 1./(1.+p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal))", &D_VBF1j );
   reader_3->AddVariable("D_WHh := 1./(1.+ (p_HadWH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadWH_mavjj_JECNominal*p_HadWH_SIG_ghw1_1_JHUGen_JECNominal))", &D_WHh);
   reader_3->AddVariable("D_ZHh := 1./(1.+ (p_HadZH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadZH_mavjj_JECNominal*p_HadZH_SIG_ghz1_1_JHUGen_JECNominal))", &D_ZHh);
   reader_3->AddVariable( "p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal", &p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal );
   reader_3->AddVariable( "p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal", &p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal );
   reader_3->AddVariable( "p_JQCD_SIG_ghg2_1_JHUGen_JECNominal", &p_JQCD_SIG_ghg2_1_JHUGen_JECNominal );
   reader_3->AddVariable( "p_JVBF_SIG_ghv1_1_JHUGen_JECNominal", &p_JVBF_SIG_ghv1_1_JHUGen_JECNominal );
   reader_3->AddVariable( "pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal", &pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal );
   reader_3->AddVariable( "p_HadWH_mavjj_true_JECNominal", &p_HadWH_mavjj_true_JECNominal );
   reader_3->AddVariable( "p_HadWH_SIG_ghw1_1_JHUGen_JECNominal", &p_HadWH_SIG_ghw1_1_JHUGen_JECNominal );
   
   reader_3->AddVariable( "PFMET", &PFMET );
   reader_3->AddVariable( "ZZPt", &ZZPt );
   reader_3->AddVariable( "DiJetMass", &DiJetMass );
   reader_3->AddVariable( "DiJetDEta", &DiJetDEta );
   reader_3->AddVariable( "ZZEta", &ZZEta );
   reader_3->AddVariable( "ZZPhi", &ZZPhi );
   reader_3->AddVariable( "ZZjjPt", &ZZjjPt );
   reader_3->AddVariable( "DiJetFisher", &DiJetFisher );
   reader_3->AddVariable( "nCleanedJetsPt30BTagged", &nCleanedJetsPt30BTagged );
   reader_3->AddVariable( "nExtraLep", &nExtraLep );
   reader_3->AddVariable( "nCleanedJetsPt30", &nCleanedJetsPt30);
   
   reader_1->BookMVA("BDT method","dataset_7/weights/TMVAClassification_BDT.weights.xml");
   reader_2->BookMVA("BDT method","dataset_8/weights/TMVAClassification_BDT.weights.xml");
   reader_3->BookMVA("BDT method","dataset_9/weights/TMVAClassification_BDT.weights.xml");
   
   TFile *input = new TFile(s1);
   
    
   TTree* theTree = (TTree*)input->Get("ZZTree/candTree");
    
   theTree->SetBranchAddress( "p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal", &p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal );
   theTree->SetBranchAddress( "p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal", &p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal );
   theTree->SetBranchAddress( "p_JQCD_SIG_ghg2_1_JHUGen_JECNominal", &p_JQCD_SIG_ghg2_1_JHUGen_JECNominal );
   theTree->SetBranchAddress( "p_JVBF_SIG_ghv1_1_JHUGen_JECNominal", &p_JVBF_SIG_ghv1_1_JHUGen_JECNominal );
   theTree->SetBranchAddress( "pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal", &pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal );
   theTree->SetBranchAddress( "p_HadWH_mavjj_true_JECNominal", &p_HadWH_mavjj_true_JECNominal );
   theTree->SetBranchAddress( "p_HadWH_mavjj_JECNominal", &p_HadWH_mavjj_JECNominal );
   theTree->SetBranchAddress( "p_HadWH_SIG_ghw1_1_JHUGen_JECNominal", &p_HadWH_SIG_ghw1_1_JHUGen_JECNominal );
   theTree->SetBranchAddress( "p_HadZH_mavjj_true_JECNominal", &p_HadZH_mavjj_true_JECNominal );
   theTree->SetBranchAddress( "p_HadZH_mavjj_JECNominal", &p_HadZH_mavjj_JECNominal );
   theTree->SetBranchAddress( "p_HadZH_SIG_ghz1_1_JHUGen_JECNominal", &p_HadZH_SIG_ghz1_1_JHUGen_JECNominal );
   theTree->SetBranchAddress( "ZZMass", &ZZMass );
   theTree->SetBranchAddress( "nCleanedJetsPt30BTagged", &a );
   theTree->SetBranchAddress( "nExtraLep", &b );
   theTree->SetBranchAddress( "nCleanedJetsPt30", &c );
   theTree->SetBranchAddress( "xsec", &xsec );
   theTree->SetBranchAddress( "overallEventWeight", &overallEventWeight );
   theTree->SetBranchAddress( "PFMET", &PFMET );
   theTree->SetBranchAddress( "ZZPt", &ZZPt );
   theTree->SetBranchAddress( "DiJetMass", &DiJetMass );
   theTree->SetBranchAddress( "DiJetDEta", &DiJetDEta );
   theTree->SetBranchAddress( "ZZEta", &ZZEta );
   theTree->SetBranchAddress( "ZZPhi", &ZZPhi );
   theTree->SetBranchAddress( "ZZjjPt", &ZZjjPt );
   theTree->SetBranchAddress( "DiJetFisher", &DiJetFisher );
   
   h1 = (TH1F*)input->Get("ZZTree/Counters");
   std::cout << "--- Processing: " << theTree->GetEntries() << " events" << std::endl;
   Float_t ttH_events=0,VH_events=0,VBF_events=0,Untagged_events=0;
   for (Long64_t ievt=0; ievt<theTree->GetEntries();ievt++) {
      theTree->GetEntry(ievt);
	  D_VBF2j=1./(1.+ p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal/p_JJVBF_SIG_ghv1_1_JHUGen_JECNominal);
	  D_VBF1j= 1./(1.+p_JQCD_SIG_ghg2_1_JHUGen_JECNominal/(p_JVBF_SIG_ghv1_1_JHUGen_JECNominal*pAux_JVBF_SIG_ghv1_1_JHUGen_JECNominal));
	  D_WHh = 1./(1.+ (p_HadWH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadWH_mavjj_JECNominal*p_HadWH_SIG_ghw1_1_JHUGen_JECNominal));
	  D_ZHh = 1./(1.+ (p_HadZH_mavjj_true_JECNominal*p_JJQCD_SIG_ghg2_1_JHUGen_JECNominal)/(p_HadZH_mavjj_JECNominal*p_HadZH_SIG_ghz1_1_JHUGen_JECNominal));
      	nCleanedJetsPt30BTagged=a;	
		nExtraLep=b;
		nCleanedJetsPt30=c;
        double w=137000*xsec*overallEventWeight/h1->GetBinContent(40);
        if(s1.Contains("ggH125"))
	{			
		 
			 if((reader_1->EvaluateMVA( "BDT method" ))>ttH_cut)
			{
            if(nExtraLep>=1 || nCleanedJetsPt30<=3)
            Histo_ggH125_BDT->Fill(0.5,w); 
            //ttHl+=w;
            else
            Histo_ggH125_BDT->Fill(1.5,w); 
            //ttHh+=w;
         }
             else if((reader_2->EvaluateMVA( "BDT method" ))>VH_cut)
			{
            if(nExtraLep>=1)
            Histo_ggH125_BDT->Fill(2.5,w); 
            //VHl+=w;
            else
            Histo_ggH125_BDT->Fill(3.5,w); 
            //VHh+=w;
         }
            else if((reader_3->EvaluateMVA( "BDT method" ))>VBF_cut)
			{
            if(nCleanedJetsPt30<=1)
            Histo_ggH125_BDT->Fill(4.5,w);
            //VBF1j+=w;
            else
            Histo_ggH125_BDT->Fill(5.5,w);
            //VBF2j+=w;
         }
            else
            Histo_ggH125_BDT->Fill(6.5,w);			
         
	}
        if(s1.Contains("WplusH125")||s1.Contains("WminusH125")||s1.Contains("ZH125"))
	{			
		 
			 if((reader_1->EvaluateMVA( "BDT method" ))>ttH_cut)
			{ 
            if(nExtraLep>=1 || nCleanedJetsPt30<=3)
            Histo_VH125_BDT->Fill(0.5,w);
            //ttHl+=w;
            else
            Histo_VH125_BDT->Fill(1.5,w);
            //ttHh+=w;
         }
             else if((reader_2->EvaluateMVA( "BDT method" ))>VH_cut)
			{
            if(nExtraLep>=1)
            Histo_VH125_BDT->Fill(2.5,w);
            //VHl+=w;
            else
            Histo_VH125_BDT->Fill(3.5,w);
            //VHh+=w;
         }
            else if((reader_3->EvaluateMVA( "BDT method" ))>VBF_cut)
			{
            if(nCleanedJetsPt30<=1)
            Histo_VH125_BDT->Fill(4.5,w);
            //VBF1j+=w;
            else
            Histo_VH125_BDT->Fill(5.5,w);
            //VBF2j+=w;
         }
            else
            Histo_VH125_BDT->Fill(6.5,w);			
         
	}
        if(s1.Contains("VBFH125"))
	{			
		 
			 if((reader_1->EvaluateMVA( "BDT method" ))>ttH_cut) 
			{ 
            if(nExtraLep>=1 || nCleanedJetsPt30<=3)
            Histo_VBFH125_BDT->Fill(0.5,w);
            //ttHl+=w;
            else
            Histo_VBFH125_BDT->Fill(1.5,w);
            ttHh+=w;
         }
             else if((reader_2->EvaluateMVA( "BDT method" ))>VH_cut)
			{
            if(nExtraLep>=1)
            Histo_VBFH125_BDT->Fill(2.5,w);
            //VHl+=w;
            else
            Histo_VBFH125_BDT->Fill(3.5,w);
            //VHh+=w;
         }
            else if((reader_3->EvaluateMVA( "BDT method" ))>VBF_cut)
			{
            if(nCleanedJetsPt30<=1)
            Histo_VBFH125_BDT->Fill(4.5,w);
            //VBF1j+=w;
            else
            Histo_VBFH125_BDT->Fill(5.5,w);
            //VBF2j+=w;
         }
            else
            Histo_VBFH125_BDT->Fill(6.5,w);			
         
	}
        if(s1.Contains("ttH125"))
	{			
		 
			 if((reader_1->EvaluateMVA( "BDT method" ))>ttH_cut)
			{ 
            if(nExtraLep>=1 || nCleanedJetsPt30<=3)
            Histo_ttH125_BDT->Fill(0.5,w);
            //ttHl+=w;
            else
            Histo_ttH125_BDT->Fill(1.5,w);
            //ttHh+=w;
         }
             else if((reader_2->EvaluateMVA( "BDT method" ))>VH_cut)
			{
            if(nExtraLep>=1)
            Histo_ttH125_BDT->Fill(2.5,w);
            //VHl+=w;
            else
            Histo_ttH125_BDT->Fill(3.5,w);
            //VHh+=w;
         }
            else if((reader_3->EvaluateMVA( "BDT method" ))>VBF_cut)
			{
            if(nCleanedJetsPt30<=1)
            Histo_ttH125_BDT->Fill(4.5,w);
            //VBF1j+=w;
            else
            Histo_ttH125_BDT->Fill(5.5,w);
            //VBF2j+=w;
         }
            else
            Histo_ttH125_BDT->Fill(6.5,w);			
         
	}   	
   }
   /*cout<<ttHl<<" "<<ttHh<<endl;
   cout<<VHl<<" "<<VHh<<endl;
   cout<<VBF1j<<" "<<VBF2j<<endl;*/
   //cout<<ttH_events<<" "<<VH_events<<" "<<VBF_events<<" "<<Untagged_events<<endl;
   
   TFile *target  = new TFile( "TMVAClassificationApplication.root","RECREATE" );
      
   target->Close();
   
   delete reader_1;
   delete reader_2;
   delete reader_3;
   
}

void Analyzer :: CategorizeBDT_Display()
{
	int i;
	gStyle->SetOptStat(0);
	const char *categories[7]={"ttH-leptonic tagged ","ttH-hadronic tagged ","VH-leptonic tagged ","VH-hadronic tagged ","VBF-2jet tagged "," VBF-1jet tagged"," Untagged"};
	Histo_ggH125_BDT->SetTitle("ggH");
	Histo_VBFH125_BDT->SetTitle("VBF");
	Histo_ttH125_BDT->SetTitle("ttH");
	Histo_VH125_BDT->SetTitle("VH");
	for (i=1;i<=7;i++) 
	{
		Histo_ggH125_BDT->GetXaxis()->SetBinLabel(i,categories[i-1]);
		Histo_VBFH125_BDT->GetXaxis()->SetBinLabel(i,categories[i-1]);
		Histo_ttH125_BDT->GetXaxis()->SetBinLabel(i,categories[i-1]);
		Histo_VH125_BDT->GetXaxis()->SetBinLabel(i,categories[i-1]);
	}
	Histo_ggH125_BDT->SetFillColor(kBlue);
	Histo_VBFH125_BDT->SetFillColor(kGreen+2);
	Histo_ttH125_BDT->SetFillColor(kMagenta);
	Histo_VH125_BDT->SetFillColor(kRed);
	Histo_ggH125_BDT->GetYaxis()->SetTitle("Expected events");
	Histo_VBFH125_BDT->GetYaxis()->SetTitle("Expected events");
	Histo_ttH125_BDT->GetYaxis()->SetTitle("Expected events");
	Histo_VH125_BDT->GetYaxis()->SetTitle("Expected events");
	
	TCanvas *canvas_categories_1=new TCanvas("ggH125","ggH125",1400,800);
	canvas_categories_1->Divide(2,2);
	
	Histo_ggH125_BDT->GetYaxis()->SetTitleSize(0.05);
	Histo_ggH125_BDT->GetXaxis()->SetLabelSize(0.08);
    Histo_ggH125_BDT->GetYaxis()->SetLabelSize(0.04);
    Histo_ggH125_BDT->SetBarOffset(0.1);
	Histo_ggH125_BDT->SetBarWidth(0.8);
	canvas_categories_1->cd(1);
	gPad->SetLeftMargin(0.35);
	Histo_ggH125_BDT->Draw("hbar");
	
	Histo_VBFH125_BDT->GetYaxis()->SetTitleSize(0.05);
	Histo_VBFH125_BDT->GetXaxis()->SetLabelSize(0.08);
	Histo_VBFH125_BDT->GetYaxis()->SetLabelSize(0.04);
	Histo_VBFH125_BDT->SetBarOffset(0.1);
	Histo_VBFH125_BDT->SetBarWidth(0.8);
	canvas_categories_1->cd(2);
	gPad->SetLeftMargin(0.25);
	Histo_VBFH125_BDT->Draw("hbar");
	
	Histo_ttH125_BDT->GetYaxis()->SetTitleSize(0.05);
	Histo_ttH125_BDT->GetXaxis()->SetLabelSize(0.08);
	Histo_ttH125_BDT->GetYaxis()->SetLabelSize(0.04);
	Histo_ttH125_BDT->SetBarOffset(0.1);
	Histo_ttH125_BDT->SetBarWidth(0.8);
	canvas_categories_1->cd(3);
	gPad->SetLeftMargin(0.35);
	Histo_ttH125_BDT->Draw("hbar");
	
	Histo_VH125_BDT->GetYaxis()->SetTitleSize(0.05);
	Histo_VH125_BDT->GetXaxis()->SetLabelSize(0.08);
	Histo_VH125_BDT->GetYaxis()->SetLabelSize(0.04);
	Histo_VH125_BDT->SetBarOffset(0.1);
	Histo_VH125_BDT->SetBarWidth(0.8);
	canvas_categories_1->cd(4);
	gPad->SetLeftMargin(0.25);
	Histo_VH125_BDT->GetYaxis()->SetRangeUser(0., 5.);
	Histo_VH125_BDT->Draw("hbar");
	
	//canvas_categories_1->SaveAs("Categorization_BDT.png");
	
	double VBF2j_exp_events=Histo_ggH125_BDT->Integral(5,5)+Histo_VBFH125_BDT->Integral(5,5)+Histo_ttH125_BDT->Integral(5,5)+Histo_VH125_BDT->Integral(5,5);
	double VHh_exp_events=Histo_ggH125_BDT->Integral(4,4)+Histo_VBFH125_BDT->Integral(4,4)+Histo_ttH125_BDT->Integral(4,4)+Histo_VH125_BDT->Integral(4,4);
	double VHl_exp_events=Histo_ggH125_BDT->Integral(3,3)+Histo_VBFH125_BDT->Integral(3,3)+Histo_ttH125_BDT->Integral(3,3)+Histo_VH125_BDT->Integral(3,3);
	double ttHh_exp_events=Histo_ggH125_BDT->Integral(2,2)+Histo_VBFH125_BDT->Integral(2,2)+Histo_ttH125_BDT->Integral(2,2)+Histo_VH125_BDT->Integral(2,2);
	double ttHl_exp_events=Histo_ggH125_BDT->Integral(1,1)+Histo_VBFH125_BDT->Integral(1,1)+Histo_ttH125_BDT->Integral(1,1)+Histo_VH125_BDT->Integral(1,1);
	double VBF1j_exp_events=Histo_ggH125_BDT->Integral(6,6)+Histo_VBFH125_BDT->Integral(6,6)+Histo_ttH125_BDT->Integral(6,6)+Histo_VH125_BDT->Integral(6,6);
	double Untagged_exp_events=Histo_ggH125_BDT->Integral(7,7)+Histo_VBFH125_BDT->Integral(7,7)+Histo_ttH125_BDT->Integral(7,7)+Histo_VH125_BDT->Integral(7,7);
	
	//cout<<ttH_exp_events<<" "<<VH_exp_events<<" "<<VBFH_exp_events<<" "<<Untagged_exp_events<<endl;
	
	double exp_events[7]={ttHl_exp_events,ttHh_exp_events,VHl_exp_events,VHh_exp_events,VBF2j_exp_events,VBF1j_exp_events,Untagged_exp_events};
	
   cout<<"ttH:"<<endl;
   cout<<(Histo_ggH125_BDT->Integral(1,1)+Histo_ggH125_BDT->Integral(2,2))/(exp_events[0]+exp_events[1])<<endl;
   cout<<(Histo_VBFH125_BDT->Integral(1,1)+Histo_VBFH125_BDT->Integral(2,2))/(exp_events[0]+exp_events[1])<<endl;
   cout<<(Histo_ttH125_BDT->Integral(1,1)+Histo_ttH125_BDT->Integral(2,2))/(exp_events[0]+exp_events[1])<<" "<<Histo_ttH125_BDT->Integral(1,1)+Histo_ttH125_BDT->Integral(2,2)<<endl;
   cout<<(Histo_VH125_BDT->Integral(1,1)+Histo_VH125_BDT->Integral(2,2))/(exp_events[0]+exp_events[1])<<endl;
   cout<<"VH:"<<endl;
   cout<<(Histo_ggH125_BDT->Integral(3,3)+Histo_ggH125_BDT->Integral(4,4))/(exp_events[2]+exp_events[3])<<endl;
   cout<<(Histo_VBFH125_BDT->Integral(3,3)+Histo_VBFH125_BDT->Integral(4,4))/(exp_events[2]+exp_events[3])<<endl;
   cout<<(Histo_ttH125_BDT->Integral(3,3)+Histo_ttH125_BDT->Integral(4,4))/(exp_events[2]+exp_events[3])<<endl;
   cout<<(Histo_VH125_BDT->Integral(3,3)+Histo_VH125_BDT->Integral(4,4))/(exp_events[2]+exp_events[3])<<" "<<Histo_VH125_BDT->Integral(3,3)+Histo_VH125_BDT->Integral(4,4)<<endl;
   cout<<"VBFH:"<<endl;
   cout<<(Histo_ggH125_BDT->Integral(5,5)+Histo_ggH125_BDT->Integral(6,6))/(exp_events[4]+exp_events[5])<<endl;
   cout<<(Histo_VBFH125_BDT->Integral(5,5)+Histo_VBFH125_BDT->Integral(6,6))/(exp_events[4]+exp_events[5])<<" "<<Histo_VBFH125_BDT->Integral(5,5)+Histo_VBFH125_BDT->Integral(6,6)<<endl;
   cout<<(Histo_ttH125_BDT->Integral(5,5)+Histo_ttH125_BDT->Integral(6,6))/(exp_events[4]+exp_events[5])<<endl;
   cout<<(Histo_VH125_BDT->Integral(5,5)+Histo_VH125_BDT->Integral(6,6))/(exp_events[4]+exp_events[5])<<endl;
   cout<<"Untagged:"<<endl;
   cout<<(Histo_ggH125_BDT->Integral(7,7) )/(exp_events[6] )<<" "<<Histo_ggH125_BDT->Integral(7,7)<<endl;
   cout<<(Histo_VBFH125_BDT->Integral(7,7) )/(exp_events[6] )<<endl;
   cout<<(Histo_ttH125_BDT->Integral(7,7) )/(exp_events[6] )<<endl;
   cout<<(Histo_VH125_BDT->Integral(7,7) )/(exp_events[6] )<<endl;

   cout<<endl<<ttHl_exp_events+ttHh_exp_events<<" "<<VHl_exp_events+VHh_exp_events<<" "<<VBF1j_exp_events+VBF2j_exp_events<<" "<<Untagged_exp_events<<endl;
	
	for(i=1;i<=7;i++)
	{
	  Histo_ggH125_BDT->SetBinContent(i,Histo_ggH125_BDT->Integral(i,i)/exp_events[i-1]);	
	  Histo_VBFH125_BDT->SetBinContent(i,Histo_VBFH125_BDT->Integral(i,i)/exp_events[i-1]);
	  Histo_ttH125_BDT->SetBinContent(i,Histo_ttH125_BDT->Integral(i,i)/exp_events[i-1]);
	  Histo_VH125_BDT->SetBinContent(i,Histo_VH125_BDT->Integral(i,i)/exp_events[i-1]);
	}
   //for(i=0;i<7;i++)
   //cout<<exp_events[i]<<endl;
	THStack *stack = new THStack("stack","BDT Categorization (simulation) ; ;fraction");
	TCanvas *master_canvas=new TCanvas("master","master",1400,800);
	TLegend* legend=new TLegend(0.9,0.7,0.98,0.9);
	legend->AddEntry(Histo_ggH125_BDT,"ggH","f");
	legend->AddEntry(Histo_VBFH125_BDT,"VBFH","f");
	legend->AddEntry(Histo_ttH125_BDT,"ttH","f");
	legend->AddEntry(Histo_VH125_BDT,"VH","f");
	gPad->SetLeftMargin(0.15);
	TLatex *t1=new TLatex(0.3,0.35,"0.73 exp. events");
	TLatex *t2=new TLatex(0.3,1.35,"0.88 exp. events");
	TLatex *t3=new TLatex(0.3,2.35,"1.51 exp. events");
	TLatex *t4=new TLatex(0.3,3.35,"6.42 exp. events");
	TLatex *t5=new TLatex(0.3,4.35,"15.19 exp. events");
	TLatex *t6=new TLatex(0.3,5.35,"14.68 exp. events");
	TLatex *t7=new TLatex(0.3,6.35,"180.01 exp. events");
	t1->SetTextColor(kWhite);
	t2->SetTextColor(kWhite);
	t1->SetTextColor(kWhite);
	t3->SetTextColor(kWhite);
	t4->SetTextColor(kWhite);
	t5->SetTextColor(kWhite);
	t6->SetTextColor(kWhite);
	t7->SetTextColor(kWhite);
	stack->Add(Histo_ggH125_BDT);
	stack->Add(Histo_VBFH125_BDT);
	stack->Add(Histo_ttH125_BDT);
	stack->Add(Histo_VH125_BDT);
	stack->Draw("hbar");
	legend->Draw();
	t1->Draw();
	t2->Draw();
	t3->Draw();
	t4->Draw();
   t5->Draw();
	t6->Draw();
	t7->Draw();
	//master_canvas->SaveAs("Categorization_BDT_stack.png");
   //stack
}




