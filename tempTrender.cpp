#include "tempTrender.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// ROOT library obejcts
#include <TF1.h> // 1d function class
#include <TH1.h> // 1d histogram classes
#include <TStyle.h>  // style object
#include <TMath.h>   // math functions
#include <TCanvas.h> // canvas object
#include <TLegend.h> // legend object

tempTrender::tempTrender(string filePath) {
	cout << "The user supplied " << filePath << " as the path to the data file." << endl;
  setFilePath(filePath);
}

double tempTrender::Gaussian(double* x, double* par) {
  return par[0]*exp(-0.5*(x[0]*x[0] - 2*x[0]*par[1] + par[1]*par[1])/(par[2]*par[2]));}

void tempTrender::hotCold() {
  // create two histograms for hottest day and coldest day
  TH1D* histHot = new TH1D("histHot", "hist Hot; Day of the year; Entries", 366, 1, 366);
  TH1D* histCold = new TH1D("histCold", "hist Cold; Day of the year; Entries", 366, 1, 366);

  // read the temperature and date from the data set of Lund
  ifstream file(getFilePath());
  vector<Double_t> tempOfYear;
  vector<Int_t> dayOfYear;
  Int_t nYears = 54;
  Double_t maxTemp[nYears];
  Double_t minTemp[nYears];
  Double_t maxDay[nYears];
  Double_t minDay[nYears];
  Int_t maxIndex, minIndex;
  Int_t year, month, day, helpInt;
  Double_t temp;
  Char_t dash,comma,colon;
  string helpString;
  Int_t yearF = 1961;
  Int_t dayF = 01;
  Int_t nDay = 1;

  Int_t nY=-1;
    while(file >> year >> dash >> month >> dash >> day >> comma >> helpInt >> colon >> helpInt >> colon >> helpInt >> comma >> temp) {
      if (year!=yearF){
        nY++;
        maxTemp[nY] = *max_element(tempOfYear.begin(),tempOfYear.end());
        maxIndex = distance(tempOfYear.begin(), max_element(tempOfYear.begin(),tempOfYear.end()));
        maxDay[nY] = dayOfYear[maxIndex];
        minTemp[nY] = *min_element(tempOfYear.begin(),tempOfYear.end());
        minIndex = distance(tempOfYear.begin(), min_element(tempOfYear.begin(),tempOfYear.end()));
        minDay[nY] = dayOfYear[minIndex];
        cout << "In the year " << yearF << ", the hottest day is "<< maxDay[nY] << " with temperature " << maxTemp[nY] << ", and the coldest day is " << minDay[nY] << " with temperature " << minTemp[nY] << endl;
        histHot->Fill(maxDay[nY]);
        histCold->Fill(minDay[nY]);
        nDay=1;
        yearF = year;
      }

      tempOfYear.push_back(temp);
      if (day!=dayF){nDay++;}
      dayOfYear.push_back(nDay);
      //      cout << nDay << " : " << temp << endl;
      dayF = day;
    }
  file.close();

  // create canvas
  //  TCanvas* c1 = new TCanvas("c1", "Hottest and coldest day canvas", 900, 600);
  TLegend *leg = new TLegend(0.65, 0.75, 0.92, 0.92, "", "NDC");
  leg->SetFillStyle(0); //Hollow fill (transparent)
  leg->SetBorderSize(0); //Get rid of the border
  leg->AddEntry(histHot, "Hottest day", "f");
  leg->AddEntry(histCold, "Coldest day", "f");
  histHot->SetMinimum(0);
  histCold->SetMinimum(0);
  histHot->SetFillColor( kRed);
  histHot->Draw();
  histCold->SetFillColor( kBlue);
  histCold->Draw("SAME");
  leg->Draw();

  // fitting histHot and histCold
  TF1* funcHot = new TF1("Gaussian", "gaus", 1, 366);
  funcHot->SetParameters(5, 200, 50); //Starting values for fitting
  //  histHot->Fit(funcHot, "Q0R");
  histHot->Fit(funcHot);
  cout << "The mean is " << funcHot->GetParameter(1) << endl;
  cout << "Its uncertainty is " << funcHot->GetParError(1) << endl;

  /*
  TF1* funcCold = new TF1("Gaussian", "gaus", 1, 366, 3);
  funcCold->SetParameters(1, 1, 1); //Starting values for fitting
  //  histCold->Fit(funcCold, "Q0R");
  histCold->Fit(funcCold);
  cout << "The mean is " << funcCold->GetParameter(1) << endl;
  cout << "Its uncertainty is " << funcCold->GetParError(1) << endl;
  */
}

