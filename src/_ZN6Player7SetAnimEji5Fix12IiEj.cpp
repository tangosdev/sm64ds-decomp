//cpp
#include "types.h"
#define LAUNDER16(p) ((u16 *)(int)(((long long)(int)(p))))
extern "C" {
extern u8 data_0209f2d8;
extern void *data_ov002_020ff480[];
extern void *data_ov002_020ff2f0[];
void _ZN13SharedFilePtr7ReleaseEv(void *thiz);
void _ZN9Animation8LoadFileER13SharedFilePtr(void *arg);
int _ZNK6Player14GetBodyModelIDEjb(void *thiz, u32 a, int b);
void _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(void *thiz, void *bcaFile, int b, Fix12i c, u32 d);
void _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(void *thiz, void *btpFile, int b, Fix12i c, u32 d);
#pragma opt_propagation off
void _ZN6Player7SetAnimEji5Fix12IiEj(char *self, u32 a, int b, Fix12i c, u16 d)
{
    u32 charId;
    void *sfp;
    u32 sl;
    int field8;
    u32 newVal;
    int id;
    int flag;
    int param2;
    void *modelPtr;

    flag = (data_0209f2d8 == 1);
    if (flag) {
        if ((int)a < 0x11 || (int)a >= 0x72) {
            u16 *f = LAUNDER16(self + 0x6ce);
            *f = (u16)(*f | 0x80);
            return;
        }
    }

    newVal = a << 2;
    if (newVal != *(u32 *)(self + 0x63c)) {
        _ZN13SharedFilePtr7ReleaseEv(data_ov002_020ff480[*(u32 *)(self + 0x63c) + (*(int *)(self + 8) & 3)]);
        _ZN9Animation8LoadFileER13SharedFilePtr(data_ov002_020ff480[newVal + (*(int *)(self + 8) & 3)]);

        id = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
        modelPtr = ((void **)(self + 0xdc))[id];
        *(int *)((char *)modelPtr + 0x60) = 0;

        if ((*(u16 *)(self + 0x73c) & 7) == 4) {
            charId = *(u8 *)(self + 0x6dd);
            if (charId == *(u32 *)(self + 8))
                charId = *(u8 *)(self + 0x6dc);

            _ZN13SharedFilePtr7ReleaseEv(data_ov002_020ff480[*(u32 *)(self + 0x63c) + charId]);
            sfp = data_ov002_020ff480[newVal + charId];
            _ZN9Animation8LoadFileER13SharedFilePtr(sfp);

            id = _ZNK6Player14GetBodyModelIDEjb(self, charId & 0xff, 0);
            _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(((void **)(self + 0xdc))[id],
                                                        *(void **)((char *)sfp + 4), b, c, d);
            {
                u16 *g = LAUNDER16(self + 0x73e);
                *g = (u16)(*g + 1);
            }
        }
    }

    field8 = *(int *)(self + 8);
    sl = a * 4;
    id = _ZNK6Player14GetBodyModelIDEjb(self, (u32)(field8 & 0xff), 0);
    _ZN9ModelAnim7SetAnimEP8BCA_Filei5Fix12IiEj(
        ((void **)(self + 0xdc))[id],
        *(void **)((char *)data_ov002_020ff480[(field8 & 3) + (a * 4)] + 4), b, c, d);

    field8 = *(int *)(self + 8);
    if ((int)sl >= 0x60) sl = 0x60;
    sl += (field8 & 3);
    param2 = 0;
    if (((int)sl >> 2) == 1) param2 = 0x40000000;
    _ZN15TextureSequence7SetFileER8BTP_Filei5Fix12IiEj(
        self + 0x1dc + field8 * 0x14,
        *(void **)((char *)data_ov002_020ff2f0[sl] + 4),
        param2, 0x1000, 0);

    *(u32 *)(self + 0x63c) = newVal;
}
}
