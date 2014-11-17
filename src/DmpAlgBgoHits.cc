/* for BGO Hits                           */
/* yong zhzhy@mail.ustc.edu.cn 08-10-2014 */

#include "DmpAlgBgoHits.h"
#include "DmpDataBuffer.h"
#include "TMath.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "DmpBgoBase.h"
#include "DmpTimeConvertor.h"
#include "DmpPsdBase.h"
#include "DmpCore.h"
#include <iostream>
#include <fstream>
//-------------------------------------------------------------------
DmpAlgBgoHits::DmpAlgBgoHits()
 :DmpVAlg("BgoHits"),
	fEvtHeader(0),
	fBgoRaw(0),
	fBgoDyCoe(0),
	fBgoMips(0),
  //      fBgoAtt(0),
	fBgoHits(0),
        fSaveEvtHeader(false),
        fPsdRaw(0)
{
        
}

//-------------------------------------------------------------------
bool DmpAlgBgoHits::GetDyCoePar(){
  TFile *fDyPar=new TFile("./DyCoe/DyCoePar.root");
  if(fDyPar==0){
    std::cout<<"Can not open DyCoe Par root file!"<<std::endl;
    return false;
  }
  TTree *Dytree=(TTree*)fDyPar->Get("Calibration/Bgo");
  TBranch *b_fBgoDyCoe;
  Dytree->SetBranchAddress("DyCoe",&fBgoDyCoe,&b_fBgoDyCoe);
  Dytree->GetEntry(0);
  //prepare parameters
  short gid=0,l=0,b=0,s=0;
  short nPmts=(short)fBgoDyCoe->GlobalPMTID.size();
  for(short i=0;i<nPmts; ++i){
    gid=fBgoDyCoe->GlobalPMTID[i];
    l=DmpBgoBase::GetLayerID(gid);
    b=DmpBgoBase::GetBarID(gid);
    s=DmpBgoBase::GetSideID(gid);
    DyCoePar_58[l][b][s][0]=fBgoDyCoe->Slp_Dy8vsDy5[i];
    DyCoePar_58[l][b][s][1]=fBgoDyCoe->Inc_Dy8vsDy5[i];
    DyCoePar_25[l][b][s][0]=fBgoDyCoe->Slp_Dy5vsDy2[i];
    DyCoePar_25[l][b][s][1]=fBgoDyCoe->Inc_Dy5vsDy2[i];
  //  std::cout<<"Layer:"<<l<<" Bar:"<<b<<" Side:"<<s<<std::endl;
  //  std::cout<<"***"<<DyCoePar_58[l][b][s][0]<<std::endl;
  }
  long nentries=Dytree->GetEntries();
  if(nentries==2){
  Dytree->GetEntry(1);
  short nPmts=(short)fBgoDyCoe->GlobalPMTID.size();
  for(short i=0;i<nPmts; ++i){
    gid=fBgoDyCoe->GlobalPMTID[i];
    l=DmpBgoBase::GetLayerID(gid);
    b=DmpBgoBase::GetBarID(gid);
    s=DmpBgoBase::GetSideID(gid);
    if(DyCoePar_58[l][b][s][0]==0){
    DyCoePar_58[l][b][s][0]=fBgoDyCoe->Slp_Dy8vsDy5[i];
    DyCoePar_58[l][b][s][1]=fBgoDyCoe->Inc_Dy8vsDy5[i];
    }
 //   DyCoePar_58[l][b][s][0]=fBgoDyCoe->Slp_Dy8vsDy5[i];
 //   DyCoePar_58[l][b][s][1]=fBgoDyCoe->Inc_Dy8vsDy5[i];
    DyCoePar_25[l][b][s][0]=fBgoDyCoe->Slp_Dy5vsDy2[i];
    DyCoePar_25[l][b][s][1]=fBgoDyCoe->Inc_Dy5vsDy2[i];
  //  std::cout<<"Layer:"<<l<<" Bar:"<<b<<" Side:"<<s<<std::endl;
  //  std::cout<<"***"<<DyCoePar_58[l][b][s][0]<<std::endl;
  }


  }
  delete Dytree;
  delete fDyPar;
  //usage: QdcCoe[fGidOrder[gid]];//Quadratic Coefficients
  //       Slope[...],Cst[...] are same.
  return true;
}
//-------------------------------------------------------------------
bool DmpAlgBgoHits::GetMipsPar(){
  TFile *fMipPar=new TFile("./MIPs/MIPsPar.root");
  if(fMipPar==0){
    std::cout<<"Can not open MIPs Par root file!"<<std::endl;
    exit(0);
  }
  TTree *Miptree=(TTree*)fMipPar->Get("Calibration/Bgo");
  TBranch *b_fBgoMip;
  Miptree->SetBranchAddress("Mips",&fBgoMips,&b_fBgoMip);
  Miptree->GetEntry(0);
  //prepare parameters
  short gid=0,l=0,b=0,s=0;
  short nBars=(short)fBgoMips->GlobalBarID.size();
  for(short i=0;i<nBars;++i){
    gid=fBgoMips->GlobalBarID[i];
    l=DmpBgoBase::GetLayerID(gid);
    b=DmpBgoBase::GetBarID(gid);
    s=fBgoMips->BgoSide[i];//s=0,1,2
    MipsPar[l][b][s][0]=fBgoMips->MPV[i];//using the maximum
    MipsPar[l][b][s][1]=fBgoMips->Gsigma[i];
    MipsPar[l][b][s][2]=fBgoMips->Lwidth[i];
  }
  delete Miptree;
  delete fMipPar;
  //usage: QdcCoe[fGidOrder[gid]];//Quadratic Coefficients
  //       Slope[...],Cst[...] are same.
  return true;
} 
//-------------------------------------------------------------------
bool DmpAlgBgoHits::GetAttPar(){
  ifstream Apar;
  Apar.open("./Attenuation/AttPar");
   if(!Apar.good()){
    std::cout<<"Can not open Att Par file!"<<std::endl;
    return false;
  } 
  
  short l=0,b=0;
  double attpar;
  short nGbar=14*22;
   for(short i=0; i<nGbar;i++){
     l=(short)(i/22);
     b=i%22;
     for(int j=0 ;j<2;j++){ 
      Apar>>attpar;
      AttPar[l][b][j]=attpar;
//      std::cout<<AttPar[i][j]<<"\t";
    }
  //  std::cout<<std::endl;
  }
  Apar.close();
  return true;
}   
//-------------------------------------------------------------------
bool DmpAlgBgoHits::GetPsdPed(){
  ifstream PPpar;
  PPpar.open("./PsdPar/PsdPed");
  if(!PPpar.good()){
  std::cout<<"Can not open Psd Par file!"<<std::endl;
  return false;
  }
  for(int dy=0;dy<2;++dy){
    for(int la=0;la<2;++la){
      for(int ba=0;ba<41;++ba){
        for(int si=0;si<2;++si){
        PPpar>>PsdPedMean[la][ba][si][dy];
        PPpar>>PsdPedSigma[la][ba][si][dy];
        }
      }
    }
  }
  PPpar.close();
  return true;
}
//-------------------------------------------------------------------
bool DmpAlgBgoHits::GetPsdMips(){
  ifstream PMpar;
  PMpar.open("./PsdPar/PsdMips");
  if(!PMpar.good()){
  std::cout<<"Can not open Psd Mips file!"<<std::endl;
  return false;
  }
  for(int l=0;l<2;++l){
    for(int b=0;b<41;++b){
      for(int s=0;s<2;++s){
        for(int p=0;p<3;++p){
        PMpar>>PsdMips[l][b][s][p];
 
        }
      }
    }
  }
  PMpar.close();
  return true;
}
//-------------------------------------------------------------------
DmpAlgBgoHits::~DmpAlgBgoHits(){
}

//-------------------------------------------------------------------
bool DmpAlgBgoHits::Initialize(){
 // gRootIOSvc->Set("Output/FileName","./"+gRootIOSvc->GetInputFileName()+"_Hits.root");
  // read input data
  fEvtHeader =new DmpEvtHeader();
  fEvtHeader = dynamic_cast<DmpEvtHeader*>(gDataBuffer->ReadObject("Event/Cutped/EventHeader"));
  fBgoRaw = dynamic_cast<DmpEvtBgoRaw*>(gDataBuffer->ReadObject("Event/Cutped/Bgo"));
  fBgoHits = new DmpEvtBgoHits();
  gDataBuffer->RegisterObject("Event/Cal/Hits",fBgoHits,"DmpEvtBgoHits");
  fPsdRaw=new DmpEvtBgoRaw();      
  gDataBuffer->LinkRootFile("Event/Rdc/Psd",fPsdRaw);
  fPsdHits = new DmpEvtBgoHits();
  gDataBuffer->RegisterObject("Event/Cal/PsdHits",fPsdHits,"DmpEvtBgoHits");
  
  if(fSaveEvtHeader){
    gDataBuffer->RegisterObject("Event/Cal/EventHeader",fEvtHeader,"DmpEvtHeader");
  }

        bool prepareDyCoePar=GetDyCoePar();
	if(!prepareDyCoePar){
	  std::cout<<"Error:Can not read DyCoe Par!"<<std::endl;
	  return false;
	}
         
        bool prepareMipPar=GetMipsPar();
	if(!prepareMipPar){
	  std::cout<<"Error:Can not read Mips Par!"<<std::endl;
	  return false;
	}
        bool prepareAttPar=GetAttPar();
	if(!prepareAttPar){
	  std::cout<<"Error:Can not read Att Par!"<<std::endl;
	  return false;
	}
        bool preparePsdPedPar=GetPsdPed();
	if(!preparePsdPedPar){
	  std::cout<<"Error:Can not read Psd Ped Par!"<<std::endl;
	  return false;
        }
        bool preparePsdMipsPar=GetPsdMips();
	if(!preparePsdMipsPar){
	  std::cout<<"Error:Can not read Psd Mips Par!"<<std::endl;
	  return false;
        }
  
  //Get First Trigger
  gRootIOSvc->PrepareEvent(0);
  short firstT = fEvtHeader->GetTrigger();
  std::cout<<"  INFO    []:First Trigger: "<<firstT<<std::endl;
  //Cut Last Package
  int Tstarttime=fEvtHeader->GetSecond();
  int Tlasttime=Tstarttime;
  int Tnowtime=0;
  int Ttimegap=0;
  timecut=Tstarttime;
 // int nEvents=gCore->GetMaxEventNumber();
  //std::cout<<"INFO    []:event number:"<<nEvents<<std::endl;
  //if(nEvents>600){
    for(int ievent=0;ievent<800;ievent++){
      gRootIOSvc->PrepareEvent(ievent);
      Tnowtime=fEvtHeader->GetSecond();
      Ttimegap=Tnowtime -Tlasttime;
      if(Ttimegap>60){
        timecut=Tnowtime;
       std::cout<<"  INFO    []:"<<"Cut last package event!(Event:"<<ievent<<")"<<std::endl;
        break;
      }  
      Tlasttime=Tnowtime;
    }
 // }
  
  //std::string t = DmpTimeConvertor::Second2Date(timecut);
  //gCore->Set("StartTime",t);
  //std::cout<<t<<": "<<timecut<<" "<<DmpTimeConvertor::Date2Second(t)<<std::endl;
  return true;
}
//-------------------------------------------------------------------
bool DmpAlgBgoHits::Reset(){
Position.SetXYZ(0.,0.,0.);
  for(short layer=0;layer<14;++layer ){
    for(short bar=0;bar<24;++bar){ 
      for(short side=0;side<2;++side){
	adc_dy2[layer][bar][side]=0;
	adc_dy5[layer][bar][side]=0; 
	adc_dy8[layer][bar][side]=0; 
      }
    }
  }

}
//-------------------------------------------------------------------
bool DmpAlgBgoHits::ProcessThisEvent(){

  if (fEvtHeader->InjectedExternalTrigger()==false){return false;}
    //check run mode
 int timenow=fEvtHeader->GetSecond();
if(timenow<timecut){return false;}
//if(timenow<58920200){return false;}
 //Reset event class
 fBgoHits->Reset();
 

 Reset();
 //

 //choose dynodes: 2,5 or 8
  double HitsBuffer[14][22][2];
  short  tag[14][22][2];
  memset(tag,0,sizeof(tag));
  memset(HitsBuffer,0,sizeof(HitsBuffer));
  short nSignal=fBgoRaw->fGlobalDynodeID.size();
  short gid=0,l=0,b=0,s=0,d=0;
  double adc=0.;
 //check the high rate
  for(short i=0;i<nSignal;++i){
    gid = fBgoRaw->fGlobalDynodeID[i];
    adc = fBgoRaw->fADC[i];
    DmpBgoBase::LoadLBSDID(gid,l,b,s,d);
    if(d == 2){  
      adc_dy2[l][b][s] = adc;
    }
    else if(d == 5){ 
      adc_dy5[l][b][s] = adc;
    }
    else if(d == 8){
      adc_dy8[l][b][s] = adc;
    }  
  }


  for(short i=0;i<nSignal;++i){ 
    gid=fBgoRaw->fGlobalDynodeID[i];
    adc=fBgoRaw->fADC[i];
    l=DmpBgoBase::GetLayerID(gid);
    b=DmpBgoBase::GetBarID(gid);
    s=DmpBgoBase::GetSideID(gid);
    d=DmpBgoBase::GetDynodeID(gid);
    if(b>=22){continue;}//spare channels
  bool usingDy8=true;
  bool usingDy5=true;
  
  double computedDy8=adc_dy5[l][b][s]*DyCoePar_58[l][b][s][0]+DyCoePar_58[l][b][s][1];
  double computedDy5=adc_dy2[l][b][s]*DyCoePar_25[l][b][s][0]+DyCoePar_25[l][b][s][1];
  if(DyCoePar_58[l][b][s][0]!=0){
    if(TMath::Abs(computedDy8-adc_dy8[l][b][s])>1200&&computedDy8!=0&&adc_dy5[l][b][s]>50){usingDy8=false;} 
  } 
  if(DyCoePar_25[l][b][s][0]!=0){
    if(TMath::Abs(computedDy5-adc_dy5[l][b][s])>1200&&computedDy5!=0&&adc_dy2[l][b][s]>50){usingDy5=false;} 
  }
    if(d==8&&adc<14000&&usingDy8==true){
      HitsBuffer[l][b][s]=adc/MipsPar[l][b][s][0];
      tag[l][b][s]=d;
    }  
    else if(d==5&&adc<14000&&tag[l][b][s]!=8&&usingDy5==true){
      double adc_8=adc*DyCoePar_58[l][b][s][0]+DyCoePar_58[l][b][s][1];
      HitsBuffer[l][b][s]=adc_8/MipsPar[l][b][s][0];
      tag[l][b][s]=d;
    }
    else if(d==2&&tag[l][b ][s]!=5&&tag[l][b][s]!=8&&usingDy8==false&&usingDy5==false){
      if(DyCoePar_25[l][b][s][0]<=0){
      std::cout<<"Using dy2,Dy parameters: Slope "<<DyCoePar_25[l][b][s][0]<<",intercept "<<DyCoePar_25[l][b][s][1]<<std::endl;
      double adc_8=adc_dy5[l][b][s]*DyCoePar_58[l][b][s][0]+DyCoePar_58[l][b][s][1];
      HitsBuffer[l][b][s]=adc_8/MipsPar[l][b][s][0];
      }
      else{
 //     std::cout<<"Using dy2,MIPs parameters: MPV "<<MipsPar[l][b][s][0]<<std::endl;
      double adc_5=adc*DyCoePar_25[l][b][s][0]+DyCoePar_25[l][b][s][1];
      double adc_8=adc_5*DyCoePar_58[l][b][s][0]+DyCoePar_58[l][b][s][1];
      HitsBuffer[l][b][s]=adc_8/MipsPar[l][b][s][0];
      tag[l][b][s]=d;
      }
    }
  }
  //fill Hits event class
  for(short il=0;il<14;++il){ 
    for(short ib=0;ib<22;++ib) {
      //s0*s1 !=0;
      if(HitsBuffer[il][ib][0]!=0&&HitsBuffer[il][ib][1]!=0){

  //    std::cout<<"il"<<il<<"ib"<<ib<<std::endl;
      short gid_bar=DmpBgoBase::ConstructGlobalBarID(il,ib);
      fBgoHits->fGlobalBarID.push_back(gid_bar);
      fBgoHits->fES0.push_back(HitsBuffer[il][ib][0]*23.6*1.077);
      fBgoHits->fES1.push_back(HitsBuffer[il][ib][1]*23.6*1.077);
      //  if(TMath::Abs(Hi tsBuffer[il][ib][0]/HitsBuffer[il][ib][1]-1)<0.4){//1-exp(-600/lambda)=0.36; (lambda=1350mm)
          if(HitsBuffer[il][ib][0]*HitsBuffer[il][ib][1]*MipsPar[il][ib][0][0]*MipsPar[il][ib][1][0]<0){
	DmpLogError<<"Hits0= "<<HitsBuffer[il][ib][0]<<" Hits1= "<<HitsBuffer[il][ib][1]<<" Mip_MPV[0]="<<MipsPar[il][ib][0][0]<<" Mip_MPV[1]"<<MipsPar[il][ib][1][0]<<DmpLogEndl;
	  }
          double combinedhits=TMath::Sqrt(HitsBuffer[il][ib][0]*HitsBuffer[il][ib][1]*MipsPar[il][ib][0][0]*MipsPar[il][ib][1][0])/MipsPar[il][ib][2][0]; 
        //DmpLogWarning<<"Layer="<<il<<" Bar="<<ib<<" Event="<<gCore->GetCurrentEventID()<<" combinedhits:"<<combinedhits<<" ADC"<<" MipsPar:"<<MipsPar[il][ib][2][0]<<DmpLogEndl;
          fBgoHits->fEnergy.push_back(combinedhits*23.6*1.077);
	  double pos=(TMath::Log(HitsBuffer[il][ib][0]/HitsBuffer[il][ib][1])-AttPar[il][ib][1])/AttPar[il][ib][0]*10-300;
	  if(il%2==0){ 
            Position.SetX(pos);
	    double yy=DmpBgoBase::Parameter()->BarCenter(gid_bar).y();
              Position.SetY(yy);
	  } 
	  else{ 
	    Position.SetY(pos);
	    double xx=DmpBgoBase::Parameter()->BarCenter(gid_bar).x();
              Position.SetX(xx);
	  }
	  double zz=DmpBgoBase::Parameter()->LayerCenter(gid_bar).z();
          Position.SetZ(zz);
           
	   fBgoHits->fPosition.push_back(Position);//along the BGO bar (mm)
      //  }
       // else  {
        //  DmpLogWarning<<"Layer="<<il<<" Bar="<<ib<<" Event="<<gCore->GetCurrentEventID()<<" Energies from two BGO sides UNmatch!"<<DmpLogEndl;
        //std::cout<<"Side 0:"<<HitsBuffer[il][ib][0]*22.5<<" MeV;\n"<<"Side 1:"<<HitsBuffer[il][ib][1]*22.5<<" MeV"<<std::endl;          
        //  fBgoHits->fEnergy.push_back(HitsBuffer[il][ib][0]*22.5);
        //  fBgoHits->fPosition.push_back(Position);//along the BGO bar (mm)
      //  }
      } 
      //s0!=0 or s1 !=0;
  //    else if(HitsBuffer[il][ib][0]!=0||HitsBuffer[il][ib][1]!=0){
    //    if((HitsBuffer[il][ib][0]+HitsBuffer[il][ib][1])<0.5){//set noise threshold: 0.5MIPs
    //    std::cout<<"Noise hits removed (<0.5Mips on a single BGO side (0 on the other side))!"<<std::endl;
    //    }
   //     else {std::cout<<"Warning: Energies unmatch! (>0.5 MIPs one Side, 0 MIPs on the other side!);"<<std::endl;
   //     } 
   //   }
    }
  }
  ProcessPsdHits();
  return true;
}

//-------------------------------------------------------------------
bool DmpAlgBgoHits::ProcessPsdHits(){
  fPsdHits->Reset();
  double HitsBuffer[2][41][2];
  memset(HitsBuffer,0,sizeof(HitsBuffer));

  short nS=fPsdRaw->fGlobalDynodeID.size();
// std::cout<<"nSignal: "<<nS<<std::endl;
  short gid=0,l=0,b=0,s=0,d=0;
  double adc=0.;
  for(int i=0;i<nS;i++){
    gid=fPsdRaw->fGlobalDynodeID[i];
    adc=fPsdRaw->fADC[i];
    l=DmpPsdBase::GetLayerID(gid);
    b=DmpPsdBase::GetStripID(gid);
    s=DmpPsdBase::GetSideID(gid);
    d=DmpPsdBase::GetDynodeID(gid);
    //std::cout<<"dynode:"<<d<<" layer:"<<l<<" bar:"<<b<<" side:"<<s<<std::endl;
    if(b>=41){continue;}
    if(d==8&&adc>(PsdPedMean[l][b][s][1]+3*PsdPedSigma[l][b][s][1])){
      adc=adc-PsdPedMean[l][b][s][1];
//    std::cout<<"PsdMips:"<<PsdMips[l][b][s][0]<<std::endl;
      HitsBuffer[l][b][s]=adc/PsdMips[l][b][s][0];
    }
  }
  for(short il=0;il<2;++il){ 
    for(short ib=0;ib<41;++ib) {
      if(HitsBuffer[il][ib][0]*HitsBuffer[il][ib][1]>0){
        short gid_bar=DmpBgoBase::ConstructGlobalBarID(il,ib);
        fPsdHits->fGlobalBarID.push_back(gid_bar);
        fPsdHits->fES0.push_back(HitsBuffer[il][ib][0]*2);
        fPsdHits->fES1.push_back(HitsBuffer[il][ib][1]*2);
        double combinedhits=TMath::Sqrt(HitsBuffer[il][ib][0]*HitsBuffer[il][ib][1]); 
          fPsdHits->fEnergy.push_back(combinedhits*2);

      }
    }
  
  }
  return true;
}
//-------------------------------------------------------------------
bool DmpAlgBgoHits::Finalize(){
  return true;
}

