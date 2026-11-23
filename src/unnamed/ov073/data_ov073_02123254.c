#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
    void *f010;
    u32 f014;
    u32 f018;
    u32 f01c;
    u32 f020;
    u32 f024;
    u32 f028;
} data_ov073_02123254_Layout;
typedef char data_ov073_02123254_size[sizeof(data_ov073_02123254_Layout) == 0x2c ? 1 : -1];

extern data_ov073_02123254_Layout data_ov073_02123254;
extern u8 _ZN8dActor_c15OnHitByMegaCharER7daPly_c[];
extern u8 _ZN8dActor_c19OnHitFromUnderneathERS_[];
extern u8 _ZN8dActor_c16OnAimedAtWithEggEv[];
extern u8 _ZN8dActor_c25OnAimedAtWithEggReturnVecEv[];
extern u8 _ZN13daObjEwbIce_c4KillEv[];

data_ov073_02123254_Layout data_ov073_02123254 = {
    &_ZN8dActor_c15OnHitByMegaCharER7daPly_c[0],
    &_ZN8dActor_c19OnHitFromUnderneathERS_[0],
    &_ZN8dActor_c16OnAimedAtWithEggEv[0],
    &_ZN8dActor_c25OnAimedAtWithEggReturnVecEv[0],
    &_ZN13daObjEwbIce_c4KillEv[0],
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
};
