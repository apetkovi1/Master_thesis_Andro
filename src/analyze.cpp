#include <iostream>
#include <string>
#include<fstream>
#include <vector>
#include "Analyzer.h"
#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TApplication.h"
#include "cConstants.h"
using namespace std;
int main()
{
    Analyzer* object1=new Analyzer();
    //object1->Fill_Histogram("/home/public/data/2018_MC/ggH125/ZZ4lAnalysis.root");
    //object1->Fill_Histogram("/home/public/data/2018_MC/VBFH125/ZZ4lAnalysis.root");
    //object1->Fill_Histogram("/home/public/data/2018_MC/ttH125/ZZ4lAnalysis.root");
	//object1->Fill_Histogram("/home/public/data/2018_MC/WplusH125/ZZ4lAnalysis.root");
	//object1->Fill_Histogram("/home/public/data/2018_MC/WminusH125/ZZ4lAnalysis.root");
	//object1->Fill_Histogram("/home/public/data/2018_MC/ZH125/ZZ4lAnalysis.root");
	//object1->Plot_Histogram();
	//object1->Categorize("/home/public/data/2018_MC/ggH125/ZZ4lAnalysis.root");
    //object1->Categorize("/home/public/data/2018_MC/VBFH125/ZZ4lAnalysis.root");
    //object1->Categorize("/home/public/data/2018_MC/ttH125/ZZ4lAnalysis.root");
	//object1->Categorize("/home/public/data/2018_MC/WminusH125/ZZ4lAnalysis.root");
	//object1->Categorize("/home/public/data/2018_MC/WplusH125/ZZ4lAnalysis.root");
	//object1->Categorize("/home/public/data/2018_MC/ZH125/ZZ4lAnalysis.root");
    //object1->Categorize_Display();
    //object1->TMVAMultiClass();
    //object1->TMVAMultiClassApplication();	
	object1->HiggsProduction_Training();
	//object1->HiggsProduction_Application("/home/public/data/2018_MC/ggH125/ZZ4lAnalysis.root");
	//object1->HiggsProduction_Application("/home/public/data/2018_MC/VBFH125/ZZ4lAnalysis.root");
	//object1->HiggsProduction_Application("/home/public/data/2018_MC/ttH125/ZZ4lAnalysis.root");
	//object1->HiggsProduction_Application("/home/public/data/2018_MC/WplusH125/ZZ4lAnalysis.root");
	//object1->HiggsProduction_Application("/home/public/data/2018_MC/WminusH125/ZZ4lAnalysis.root");
	//object1->HiggsProduction_Application("/home/public/data/2018_MC/ZH125/ZZ4lAnalysis.root");
	
	//object1->CategorizeBDT_Display();
}
