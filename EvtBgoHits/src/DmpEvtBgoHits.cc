/*   $Id: DmpEvtBgoHits.cc, 2014-10-05 15:47:37+08:00 DAMPE $
 *--------------------------------------------------------
 *  Author(s):
 *
 *--------------------------------------------------------
*/

#include "DmpEvtBgoHits.h"

ClassImp(DmpEvtBgoHits)

DmpEvtBgoHits::DmpEvtBgoHits()
{
}

DmpEvtBgoHits::~DmpEvtBgoHits()
{
}

void DmpEvtBgoHits::Reset()
{
fGlobalBarID.clear();
fHits.clear();
fHits_S0.clear();
fHits_S1.clear();
fPosition.clear();
}
