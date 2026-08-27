#include "types.h"
extern int Player_ScaleByCharFactor(void* c, int a);
extern void func_02022b04(int x, int y, int z);
extern void _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(int x, int y, int z);

extern int data_0208e430;
extern u8 data_020a0e40;
extern s16 data_0209f4a0[];
extern u8 data_0209f4ac[];
extern u16 data_0209f49c[];

void func_ov002_020d45c0(char* self)
{
    if (*(u8*)(self + 0x707) != 0) return;
    if (data_0208e430 == 0x2e) return;

    if (*(int*)(self + 0x98) < Player_ScaleByCharFactor(self, 0x24000)) {
        if (*(s16*)((char*)data_0209f4a0 + data_020a0e40 * 0x18) >= 0xe00) {
            if (*(u8*)((char*)data_0209f4ac + data_020a0e40 * 0x18) != 0
                || (*(u16*)((char*)data_0209f49c + data_020a0e40 * 0x18) & 0x800)) {
                if (*(u8*)(self + 0x703) != 0)
                    func_02022b04(*(int*)(self + 0x5c), *(int*)(self + 0x60), *(int*)(self + 0x64));
                else
                    _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int*)(self + 0x5c), *(int*)(self + 0x60), *(int*)(self + 0x64));
            }
        }
    }

    if (*(u8*)((char*)data_0209f4ac + data_020a0e40 * 0x18) == 0) return;

    if (*(u8*)(self + 0x70c) != 0) {
        if (*(u16*)(self + 0x6a6) != 0) {
            if (*(u8*)(self + 0x703) != 0)
                func_02022b04(*(int*)(self + 0x5c), *(int*)(self + 0x60), *(int*)(self + 0x64));
            else
                _ZN8Particle20RunningSlidingDustAtE5Fix12IiES1_S1_(*(int*)(self + 0x5c), *(int*)(self + 0x60), *(int*)(self + 0x64));
        }
        if (*(int*)(self + 0x98) < Player_ScaleByCharFactor(self, 0x1c000))
            *(u8*)(self + 0x70c) = 0;
    } else {
        if (*(int*)(self + 0x98) > Player_ScaleByCharFactor(self, 0x26000)) {
            *(u16*)(self + 0x6a6) = 0x10;
            *(u8*)(self + 0x70c) = 1;
        }
    }
}
