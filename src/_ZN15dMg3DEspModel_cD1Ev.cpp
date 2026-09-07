//cpp
// @symbol _ZN15dMg3DEspModel_cD1Ev
#include "dMg3DEspModel_c.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov006_02141e54;
extern SharedFilePtr data_ov006_02141e5c;
extern SharedFilePtr data_ov006_02141e7c;
extern SharedFilePtr data_ov006_02141e8c;
extern SharedFilePtr data_ov006_02141e84;
extern SharedFilePtr data_ov006_02141e64;

dMg3DEspModel_c::~dMg3DEspModel_c()
{
    data_ov006_02141e54.Release();
    data_ov006_02141e5c.Release();
    data_ov006_02141e7c.Release();
    data_ov006_02141e8c.Release();
    data_ov006_02141e84.Release();
    data_ov006_02141e64.Release();
}
