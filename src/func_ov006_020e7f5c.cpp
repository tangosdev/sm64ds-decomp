//cpp
#include "dMg3DEspModel_c.h"

// Keep the scalar-speed boundary used by the existing SetAnim definition.
// The measured native Fix12-by-value call grows this function by eight bytes.
extern "C" void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(ModelAnim*, BCA_File*, int, int, u16);

extern int data_ov006_02141e7c[];
extern dMg3DEspModel_c::State data_ov006_0213c76c;

extern "C" void func_ov006_020e7f5c(dMg3DEspModel_c* model)
{
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(&model->mModelAnim,
        (BCA_File*)data_ov006_02141e7c[1], 0, 0x800, 0);
    model->mState = data_ov006_0213c76c;
}
