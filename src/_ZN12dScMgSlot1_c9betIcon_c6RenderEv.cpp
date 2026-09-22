//cpp
// @symbol _ZN12dScMgSlot1_c9betIcon_c6RenderEv
#include "dScMgSlot1_c.h"

extern "C" {
void _ZN3OAM9RenderSubEP7OamAttriiii(void* a, int b, int c, int d, int e);
s32 GetGameLanguage(void);
void func_ov004_020af948(void* a, int b, int c, void* m);
extern void* data_ov006_0213e528[];
extern void** data_ov006_0213e5ec[];
}

void dScMgSlot1_c::betIcon_c::Render()
{
    _ZN3OAM9RenderSubEP7OamAttriiii(
        data_ov006_0213e528[unk_010], unk_004, unk_006, -1, 1);
    int i;
    int sb = 0xb0;
    int z = 0;
    for (i = 0; i < unk_01c; i++) {
        func_ov004_020af948(data_ov006_0213e5ec[GetGameLanguage()][2], 0xb0 + i*0x10, sb, (void*)z);
    }
}
