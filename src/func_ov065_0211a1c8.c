// @symbol func_ov065_0211a1c8
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_common.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjCtMecha03_c.h"
// @emits daObjCtMecha03_c_Behavior
/* recovered: renamed to Class_Method */
/* daObjCtMecha03_c::Behavior - recovered from vtable slot identity */
typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;

extern u16 DecIfAbove0_Short(u16* p);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void* v);
extern int RandomIntInternal(int* seed);
extern int _ZN8Platform13IsClsnInRangeE5Fix12IiES1_(void* self, int a, int b);
extern void _ZN8Platform19UpdateClsnPosAndRotEv(void* self);

extern u8 data_0209f2c0;
extern int data_0209e650;

#define I16(off) (*(short*)(((int)c + (off)) & 0xFFFFFFFFFFFFFFFFLL))

int daObjCtMecha03_c_Behavior(char* c)
{
    struct daObjCtMecha03_c *self = (struct daObjCtMecha03_c *)(void *)c;
    if (data_0209f2c0 != 3) {
        if (*(unsigned short*)(c + 0x300 + 0x26) != 0) {
            if (DecIfAbove0_Short((u16*)(c + 0x326)) == 0) {
                _ZN5Sound9PlayBank3EjRK7Vector3(0x38, c + 0x74);
            }
        }
        if (DecIfAbove0_Short((u16*)(c + 0x328)) == 0) {
            {
                short vx = *(short*)(c + 0x300 + 0x1e);
                short vy = *(short*)(c + 0x300 + 0x22);
                short* accelP = (short*)(((int)c + 0x324) & 0xFFFFFFFFFFFFFFFFLL);
                if (vy * vx > 0) {
                    vx = -vx;
                    *(short*)(c + 0x300 + 0x1e) = vx;
                }
                {
                    short spd = *(short*)(c + 0x300 + 0x20);
                    short pos = *(short*)(c + 0x300 + 0x1e);
                    short accel = *accelP;
                    *accelP = (short)(spd * pos + accel);
                }
            }
            if (data_0209f2c0 == 2 && *(short*)(c + 0x300 + 0x24) == 0) {
                int r0 = RandomIntInternal(&data_0209e650);
                if ((unsigned)r0 % 3 != 0)
                    *(short*)(c + 0x300 + 0x20) = 0xd;
                else
                    *(short*)(c + 0x300 + 0x20) = 0x2a;
                if ((r0 & 1) == 0) {
                    *(short*)(c + 0x300 + 0x28) = (short)((unsigned)r0 >> 0x1b) + 3;
                }
            }
            if (*(short*)(c + 0x300 + 0x24) == 0) {
                *(short*)(c + 0x300 + 0x26) = *(unsigned short*)(c + 0x300 + 0x28) + 0xf;
            }
            I16(0x322) = I16(0x322) + *(short*)(c + 0x300 + 0x24);
        }
        self->unk_090 = *(short*)(c + 0x300 + 0x22);
    }

    func_ov065_0211a114(c);
    func_ov065_02119fe8(c);
    if (_ZN8Platform13IsClsnInRangeE5Fix12IiES1_(c, 0x300000, -0x200000) != 0)
        _ZN8Platform19UpdateClsnPosAndRotEv(c);
    return 1;
}
