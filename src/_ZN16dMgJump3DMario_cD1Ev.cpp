//cpp
// @symbol _ZN16dMgJump3DMario_cD1Ev
#include "dMgJump3DMario_c.h"
#include "SharedFilePtr.h"

extern SharedFilePtr data_ov006_02140450;
extern SharedFilePtr data_ov006_02140460;
extern SharedFilePtr data_ov006_02140468;
extern SharedFilePtr data_ov006_02140458;
extern SharedFilePtr data_ov006_02140438;
extern SharedFilePtr data_ov006_02140440;
extern SharedFilePtr data_ov006_02140448;
extern int data_ov006_02140430;
extern int data_ov006_0214040c;
extern int data_ov006_02140424;
extern int data_ov006_02140408;
extern int data_ov006_0214042c;

dMgJump3DMario_c::~dMgJump3DMario_c()
{
    data_ov006_02140450.Release();
    data_ov006_02140460.Release();
    data_ov006_02140468.Release();
    data_ov006_02140458.Release();
    data_ov006_02140438.Release();
    data_ov006_02140440.Release();
    data_ov006_02140448.Release();
    data_ov006_02140430 = 0;
    data_ov006_0214040c = 0;
    data_ov006_02140424 = 0;
    data_ov006_02140408 = 0;
    data_ov006_0214042c = 0;
}
