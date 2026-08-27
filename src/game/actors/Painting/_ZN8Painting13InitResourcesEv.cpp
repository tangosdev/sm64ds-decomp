//cpp
#include "types.h"
// @symbol _ZN8Painting13InitResourcesEv
#include "Painting.h"
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daPicGate_c.h"
// recovered name: daPicGate_c_InitResources
/* recovered: renamed to Class_Method */
/* daPicGate_c::InitResources - recovered from vtable slot identity */
struct C;
typedef void (C::*PMF)();
struct Disp {
    PMF pmf;
    int pad[4];
};

extern "C" void _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(void* self, int a, int b, int c, int d);

extern Disp data_ov080_02128628[];
extern int data_0209caa0[];

s32 Painting::InitResources() {
    char* c = (char*)this;
    struct daPicGate_c *self = (struct daPicGate_c *)(void *)c;
    unsigned int m = (unsigned char)((*(unsigned int*)(c + 8) >> 0xd) & 3);

    if (m >= 2) {
        self->unk_1bb = 2;
        self->unk_1ba = self->unk_1bb;
        self->unk_1b8 = (u16)(self->unk_1ba * self->unk_1bb);
    } else {
        unsigned int idx = (unsigned char)(*(unsigned int*)(c + 8) & 0xf) + 1;
        switch (idx) {
        case 0:
        case 1:
        case 2:
            break;
        case 3:
            self->unk_1bb = data_ov080_02127714[0];
            self->unk_1ba = self->unk_1bb;
            break;
        case 4:
            self->unk_1bb = data_ov080_02127714[1];
            self->unk_1ba = self->unk_1bb;
            break;
        case 5:
            self->unk_1bb = data_ov080_02127714[2];
            self->unk_1ba = self->unk_1bb;
            break;
        case 6:
            self->unk_1bb = data_ov080_02127714[3];
            self->unk_1ba = self->unk_1bb;
            break;
        case 7:
            self->unk_1bb = data_ov080_02127714[4];
            self->unk_1ba = self->unk_1bb;
            break;
        case 8:
            self->unk_1bb = data_ov080_02127714[5];
            self->unk_1ba = self->unk_1bb;
            break;
        case 9:
        case 10:
        case 11:
        case 12:
        case 13:
        case 14:
        case 15:
            break;
        case 16:
            self->unk_1bb = data_ov080_02127714[6];
            self->unk_1ba = self->unk_1bb;
            break;
        }
        self->unk_1b8 = (u16)(self->unk_1ba * self->unk_1bb);
    }

    *(void**)(c + 0x1a0) = _ZN6Memory13operator_new2Ej((unsigned)(self->unk_1b8) * 0x18u);
    self->unk_1a8 = func_ov080_02125630(c, (int)((unsigned char)((*(unsigned int*)(c + 8) >> 8) & 0x1f)));
    *(void**)(c + 0x1ac) = &data_ov080_02127834;
    {
        unsigned int mi = (unsigned char)((*(unsigned int*)(c + 8) >> 0xd) & 3);
        *(Disp**)(c + 0x1a4) = &data_ov080_02128628[mi];
    }
    {
        Disp* d = *(Disp**)(c + 0x1a4);
        C* o = (C*)c;
        (o->*(d->pmf))();
    }

    {
        unsigned int f = *(unsigned int*)(c + 8);
        int lo = (int)((unsigned char)(f & 0xf) + 1);
        int hi = (int)((unsigned char)((f >> 4) & 0xf) + 1);
        int a = lo * 0x64000;
        int b = hi * 0x64000;
        _ZN8dActor_c9SetRangesE5Fix12IiES1_S1_S1_(c, b / 2, (a / 2) + 0xc8000, 0x1964000, 0x1964000);
    }
    {
        unsigned int f = *(unsigned int*)(c + 8);
        if ((unsigned char)((f >> 8) & 0x1f) == 4) {
            if ((unsigned char)((f >> 0xd) & 3) == 1) {
                int* pb0 = (int*)(((int)c + 0xb0));
                *pb0 = *pb0 & ~3;
            }
        }
    }
    {
        unsigned int f = *(unsigned int*)(c + 8);
        if ((unsigned char)((f >> 8) & 0x1f) == 7) {
            if ((unsigned char)((f >> 0xd) & 3) == 1) {
                self->unk_1b0 = self->mPosX;
                if (data_0209caa0[2] & 0x40000) {
                    int* p = (int*)(((int)c + 0x5c));
                    *p = *p + 0x802000;
                }
            }
        }
    }
    func_ov080_0212555c(c);
    return 1;
}
