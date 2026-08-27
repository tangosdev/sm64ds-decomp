//cpp
// @symbol _ZN16daObjCtMecha03_c8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "daObjCtMecha03_c.h"

extern "C" {
extern u16 DecIfAbove0_Short(u16 *p);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, void *v);
extern int RandomIntInternal(int *seed);
extern int _ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(void *self, int a, int b);
extern void _ZN10dBgActor_c19UpdateClsnPosAndRotEv(void *self);
extern int func_ov065_0211a114(char *self);
extern void func_ov065_02119fe8(char *self);

extern u8 data_0209f2c0;
extern int data_0209e650;
}

/* I16 reads/writes 0x322 through a RAW combined offset -- not the (c+0x300)+0x22
   decomposition used everywhere else in this function. The mask is a no-op on a
   32-bit int; its only purpose is to stop the compiler from recognizing 0x322 as
   0x300+0x22 and reusing the r4+0x300 base already live in a register nearby.
   The ROM computes this one address via a literal-pool add instead -- verified
   by build_pin against the un-migrated func_ov065_0211a1c8. */
#define I16(off) (*(short *)(((int)c + (off)) & 0xFFFFFFFFFFFFFFFFLL))

int daObjCtMecha03_c::Behavior()
{
    char *c = (char *)this;

    if (data_0209f2c0 != 3) {
        if (*(unsigned short *)(c + 0x300 + 0x26) != 0) {
            if (DecIfAbove0_Short((u16 *)(c + 0x326)) == 0) {
                _ZN5Sound9PlayBank3EjRK7Vector3(0x38, c + 0x74);
            }
        }
        if (DecIfAbove0_Short((u16 *)(c + 0x328)) == 0) {
            {
                short vx = *(short *)(c + 0x300 + 0x1e);
                short vy = *(short *)(c + 0x300 + 0x22);
                short *accelP = (short *)(((int)c + 0x324) & 0xFFFFFFFF);
                if (vy * vx > 0) {
                    vx = -vx;
                    *(short *)(c + 0x300 + 0x1e) = vx;
                }
                {
                    short spd = *(short *)(c + 0x300 + 0x20);
                    short pos = *(short *)(c + 0x300 + 0x1e);
                    short accel = *accelP;
                    *accelP = (short)(spd * pos + accel);
                }
            }
            if (data_0209f2c0 == 2 && *(short *)(c + 0x300 + 0x24) == 0) {
                int r0 = RandomIntInternal(&data_0209e650);
                if ((unsigned)r0 % 3 != 0)
                    *(short *)(c + 0x300 + 0x20) = 0xd;
                else
                    *(short *)(c + 0x300 + 0x20) = 0x2a;
                if ((r0 & 1) == 0) {
                    *(short *)(c + 0x300 + 0x28) = ((unsigned)r0 >> 0x1b) + 3;
                }
            }
            if (*(short *)(c + 0x300 + 0x24) == 0) {
                *(short *)(c + 0x300 + 0x26) = *(unsigned short *)(c + 0x300 + 0x28) + 0xf;
            }
            I16(0x322) = I16(0x322) + *(short *)(c + 0x300 + 0x24);
        }
        *(short *)(c + 0x90) = *(short *)(c + 0x300 + 0x22);
    }

    func_ov065_0211a114(c);
    func_ov065_02119fe8(c);
    if (_ZN10dBgActor_c13IsClsnInRangeE5Fix12IiES1_(c, 0x300000, -0x200000) != 0)
        _ZN10dBgActor_c19UpdateClsnPosAndRotEv(c);
    return 1;
}
