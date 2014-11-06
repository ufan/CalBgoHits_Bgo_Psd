/*   $Id: DmpEvtBgoHits.h, 2014-10-05 15:47:37+08:00 DAMPE $
 *--------------------------------------------------------
 *  Author(s):
 *
 *--------------------------------------------------------
*/

#ifndef DmpEvtBgoHits_H
#define DmpEvtBgoHits_H

#include "TObject.h"

class DmpEvtBgoHits : public TObject{
/*
 *  DmpEvtBgoHits
 *
 */
public:
  DmpEvtBgoHits();
  ~DmpEvtBgoHits();

  void Reset();

public:

/*For Eventclass, if changed, add to Reset() */
std::vector<short> fGlobalBarID;
std::vector<double> fHits;
std::vector<double> fHits_S0;
std::vector<double> fHits_S1;
std::vector<double> fPosition;
 
  
  ClassDef(DmpEvtBgoHits,1)

};

#endif
