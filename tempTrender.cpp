#include "tempTrender.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
using namespace std;

// ROOT library obejcts
#include <TF1.h> // 1d function class
#include <TH1.h> // 1d histogram classes
#include <TH2.h> // 2d histogram classes
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
  TCanvas* c1 = new TCanvas("c1", "Hottest and coldest day canvas", 900, 600);
  c1->Divide(1,1);
  c1->cd(1);
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

  // Save the canvas as a picture
  c1->SaveAs("hotCold.png");  
}


void tempTrender::tempPerMonth() {
  // create two histograms
  TH2F* histPerMonth = new TH2F("histPerMonth", "Hist; Month of the year; Temperature", 12, 1, 13, 40, -10, 30);
  TH2F* histPerMonthHottest = new TH2F("histPerMonthHottest", "Hist; Month of the year; Temperature", 12, 1, 13, 40, -10, 30);
  TH2F* histPerMonthColdest = new TH2F("histPerMonthColdest", "Hist; Month of the year; Temperature", 12, 1, 13, 40, -10, 30);

  // read the temperature and date from the data set of Lund
  ifstream file(getFilePath());
  vector<Double_t> tempOfMonth;
  Int_t year, month, day, helpInt;
  Double_t temp;
  Char_t dash,comma,colon;
  string helpString;
  Int_t yearF = 1961;
  Int_t monthF = 01;
  Int_t nMonth = 1;

    while(file >> year >> dash >> month >> dash >> day >> comma >> helpInt >> colon >> helpInt >> colon >> helpInt >> comma >> temp) {
      if (year!=yearF){
        nMonth=month;
        yearF = year;
      }

      if (month!=monthF){

        Double_t sum = std::accumulate(tempOfMonth.begin(), tempOfMonth.end(), 0.0);
        Double_t mean = sum / tempOfMonth.size();
        Double_t hottest = *max_element(tempOfMonth.begin(),tempOfMonth.end());
        Double_t coldest = *min_element(tempOfMonth.begin(),tempOfMonth.end());

        cout << "In the year " << yearF << ", month "<< nMonth << " the average " << mean << " hottest " << hottest << endl;
        histPerMonth->Fill(nMonth, mean);
        histPerMonthHottest->Fill(nMonth, hottest);
        histPerMonthColdest->Fill(nMonth, coldest);

        nMonth++;
        monthF = month;
        tempOfMonth.clear();
      }

      tempOfMonth.push_back(temp);
    }
  file.close();
  // create canvas
  TCanvas* c3 = new TCanvas("c3", "Temperature per month canvas", 900, 600);
  c3->Divide(1,1);
  c3->cd(1);
  histPerMonth->SetMarkerStyle(8);
  histPerMonth->SetMarkerSize(0.6);
  histPerMonth->Draw();

  histPerMonthHottest->SetMarkerStyle(3);
  histPerMonthHottest->SetMarkerColor(kRed);
  histPerMonthHottest->Draw("SAME");

  histPerMonthColdest->SetMarkerStyle(4);
  histPerMonthColdest->SetMarkerColor(kBlue);
  histPerMonthColdest->Draw("SAME");

  TLegend *leg = new TLegend(0.5, 0.75, 0.15, 0.92, "", "NDC");
  leg->SetFillStyle(0);
  leg->SetBorderSize(0);
  leg->AddEntry(histPerMonth, "Average temp", "f");
  leg->AddEntry(histPerMonthHottest, "Maximum temp", "f");
  leg->AddEntry(histPerMonthColdest, "Minimum temp", "f");
  histPerMonth->SetFillColor( kBlack);
  histPerMonthHottest->SetFillColor( kRed);
  histPerMonthColdest->SetFillColor( kBlue);
  leg->Draw();

  // Save the canvas as a picture
  c3->SaveAs("tempPerMonth.png");
}


void tempTrender::DayTemp(int monthToCalculate, int dayToCalculate){

  //Histogram for the temperatures of a given day of the year each year since 1961.
  TH1D* tempday = new TH1D("Day Temp","Temperature ; Temperature; Entries",300,-20,40);

  ifstream file("smhi-opendata_Lund.csv");
  Int_t helpint;
  Double_t temp;  
  Int_t day, month, year;
  Char_t helpChar;
  Int_t nY = 0;
  Double_t tempvec[55];


  //assigning names to everything in data
  while(file >> year >> helpChar >> month >> helpChar >> day >> helpChar >> helpint >> helpChar >> helpint >> helpChar >> helpint >> helpChar >> temp){
    if(month == monthToCalculate){
      if(day == dayToCalculate){
        Double_t sum_temps = 0;
        Int_t n=0;
        while(day == dayToCalculate){
          sum_temps+=temp;
          n++;
          file >> year >> helpChar >> month >> helpChar >> day >> helpChar >> helpint >> helpChar >> helpint >> helpChar >> helpint >> helpChar >> temp;
        }

        tempvec[nY]=sum_temps/n;
        tempday->Fill(tempvec[nY]);
        nY++;
      }

    }

  }
  

  double mean = tempday->GetMean(); //The mean of the distribution
  double stdev = tempday->GetRMS();

  TCanvas* c4 = new TCanvas("c4", "Canvas for DayTemp", 900, 600);
  c4->Divide(1,1);
  c4->cd(1);
  tempday->SetFillColor(kRed);
  tempday->SetMarkerColor(kRed);
  tempday->SetMinimum(0);

  tempday->Draw();
  cout << "Mean temperature is " << mean << endl;
  cout << "The standard deviation from the mean value is " << stdev << endl;

  // Save the canvas as a picture
  c4->SaveAs("DayTemp.png");
  
  /*TF1* dayGaus= new TF1("dayGaus","gaus",-20,40);
  dayGaus->SetParameters(0.1,mean,50);
  tempday->Fit(dayGaus, "Q");*/
}


void tempTrender::tempPerDay() {
  // create two histograms
  TH2F* histPerDay = new TH2F("histPerDay", "Hist; Day of the year; Temperature", 366, 1, 366, 40, -10, 30);

  // read the temperature and date from the data set of Lund
  ifstream file(getFilePath());
  vector<Double_t> tempOfDay;
  Int_t year, month, day, helpInt;
  Double_t temp;
  Char_t dash,comma,colon;
  string helpString;
  Int_t yearF = 1961;
  Int_t dayF = 01;
  Int_t nDay = 1;

    while(file >> year >> dash >> month >> dash >> day >> comma >> helpInt >> colon >> helpInt >> colon >> helpInt >> comma >> temp) {
      if (year!=yearF){
        nDay=day;
        yearF = year;
      }

      if (day!=dayF){

        Double_t sum = std::accumulate(tempOfDay.begin(), tempOfDay.end(), 0.0);
        Double_t mean = sum / tempOfDay.size();
        std::vector<Double_t> diff(tempOfDay.size());
        std::transform(tempOfDay.begin(), tempOfDay.end(), diff.begin(), [mean](Double_t x) { return x - mean; });
        Double_t sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        Double_t stdev = std::sqrt(sq_sum / tempOfDay.size());
        cout << "In the year " << yearF << ", day "<< nDay << " the average " << mean << " the standard deviation " << stdev << endl;
        histPerDay->Fill(nDay, mean);

        nDay++;
        dayF = day;
        tempOfDay.clear();
      }

      tempOfDay.push_back(temp);
    }
  file.close();
  // create canvas
  TCanvas* c2 = new TCanvas("c2", "Temperature per day canvas", 900, 600);
  c2->Divide(1,1);
  c2->cd(1);
  histPerDay->SetMarkerStyle(8);
  histPerDay->SetMarkerSize(0.6);
  histPerDay->Draw();

  // Save the canvas as a picture
  c2->SaveAs("tempPerDay.png");
}
