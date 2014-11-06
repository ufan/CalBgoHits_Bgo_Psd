/* for BGO Hits                           */
/* yong zhzhy@mail.ustc.edu.cn 08-10-2014 */
#ifndef DmpAlgBgoHits_H
#define DmpAlgBgoHits_H

#include "DmpVAlg.h"
#include "DmpEvtHeader.h"
#include "DmpEvtBgoRaw.h"
#include "DmpEvtBgoDyCoe.h"
#include "DmpEvtBgoMips.h"
#include "DmpEvtBgoHits.h"
#include "TVector3.h"
class DmpEvtBgoRaw;
class DmpEvtBgoDyCoe;
class DmpEvtBgoMips;
class DmpEvtBgoHits;
class DmpEvtHeader;

class DmpAlgBgoHits : public DmpVAlg{
/*
 *  DmpAlgBgoHits
 *
 */
public:
  DmpAlgBgoHits();
  ~DmpAlgBgoHits();

  //void Set(const std::string &type,const std::string &value);
  // if you need to set some options for your algorithm at run time. Overload Set()
  bool Initialize();
  bool GetDyCoePar();
  bool GetMipsPar();
  bool GetAttPar();
  bool ProcessThisEvent();    // only for algorithm
  bool ProcessPsdHits();    // only for algorithm
  bool Finalize();
  bool Reset(); 
  bool GetPsdPed();
  bool GetPsdMips();
  bool GetPsdDyCoe(); 
private:
DmpEvtHeader *fEvtHeader;
DmpEvtBgoRaw *fBgoRaw;
DmpEvtBgoDyCoe *fBgoDyCoe;
DmpEvtBgoMips  *fBgoMips;
DmpEvtBgoHits *fBgoHits;

DmpEvtBgoRaw *fPsdRaw;
DmpEvtBgoHits *fPsdHits;

double DyCoePar_58[14][22][2][2];//layer,bar,side, 0:Slope and 1:Intercept
double DyCoePar_25[14][22][2][2];//layer,bar,side, 0:Slope and 1:Intercept
double MipsPar[14][22][3][3];//layer,bar,side :2 mean combined value,0:MPV 1:Gsigma 2:Lwidth
double AttPar[14][22][2];//layer,bar,0:Slope 1:Intercept
TVector3 Position;

double PsdPedMean[2][41][2][2];//layer,bar,side, dy
double PsdPedSigma[2][41][2][2];//
double PsdMips[2][41][2][3];//layer,bar,side, 0:MPV 1:Gsigma 2:Lwidth

//time cut
  int timecut;

};

#endif
