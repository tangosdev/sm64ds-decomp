//cpp
#include "dMg3DEspModel_c.h"
#include "Sound.h"

extern "C" void func_ov006_020e7f04(char* self)
{
    dMg3DEspModel_c* model = (dMg3DEspModel_c*)self;
    if (model->unk_20c)
        return;
    if (!model->mModelAnim.WillHitFrame(0)) {
        if (!model->mModelAnim.WillHitFrame(0x39))
            return;
    }
    Sound::PlayBank2_2D(0x18f);
}
