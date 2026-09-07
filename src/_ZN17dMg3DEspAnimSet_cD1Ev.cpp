//cpp
// @symbol _ZN17dMg3DEspAnimSet_cD1Ev
#include "dMg3DEspAnimSet_c.h"
#include "SharedFilePtr.h"
extern void* data_ov006_02141e94;
extern void* data_ov006_02141e6c;

dMg3DEspAnimSet_c::~dMg3DEspAnimSet_c()
{
  ((SharedFilePtr *)(&data_ov006_02141e94))->Release();
  ((SharedFilePtr *)(&data_ov006_02141e6c))->Release();
}
