#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
    void *f00c;
    void *f010;
    void *f014;
    void *f018;
    void *f01c;
    void *f020;
    void *f024;
    void *f028;
} data_ov045_02112d48_Layout;
typedef char data_ov045_02112d48_size[sizeof(data_ov045_02112d48_Layout) == 0x2c ? 1 : -1];

extern data_ov045_02112d48_Layout data_ov045_02112d48;
extern u8 _ZN8dActor_c15OnGroundPoundedERS_[];
extern u8 _ZN8dActor_c11OnAttacked1ERS_[];
extern u8 _ZN8dActor_c11OnAttacked2ERS_[];
extern u8 _ZN8dActor_c8OnKickedERS_[];
extern u8 _ZN8dActor_c8OnPushedERS_[];
extern u8 _ZN8dActor_c24OnHitByCannonBlastedCharERS_[];
extern u8 _ZN8dActor_c15OnHitByMegaCharER7daPly_c[];
extern u8 _ZN8dActor_c19OnHitFromUnderneathERS_[];
extern u8 _ZN8dActor_c16OnAimedAtWithEggEv[];
extern u8 _ZN8dActor_c25OnAimedAtWithEggReturnVecEv[];
extern u8 _ZN10dBgActor_c4KillEv[];

data_ov045_02112d48_Layout data_ov045_02112d48 = {
    &_ZN8dActor_c15OnGroundPoundedERS_[0],
    &_ZN8dActor_c11OnAttacked1ERS_[0],
    &_ZN8dActor_c11OnAttacked2ERS_[0],
    &_ZN8dActor_c8OnKickedERS_[0],
    &_ZN8dActor_c8OnPushedERS_[0],
    &_ZN8dActor_c24OnHitByCannonBlastedCharERS_[0],
    &_ZN8dActor_c15OnHitByMegaCharER7daPly_c[0],
    &_ZN8dActor_c19OnHitFromUnderneathERS_[0],
    &_ZN8dActor_c16OnAimedAtWithEggEv[0],
    &_ZN8dActor_c25OnAimedAtWithEggReturnVecEv[0],
    &_ZN10dBgActor_c4KillEv[0],
};
