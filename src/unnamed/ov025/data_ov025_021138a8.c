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
} data_ov025_021138a8_Layout;
typedef char data_ov025_021138a8_size[sizeof(data_ov025_021138a8_Layout) == 0x28 ? 1 : -1];

extern data_ov025_021138a8_Layout data_ov025_021138a8;
extern u8 _ZN8dActor_c11OnAttacked1ERS_[];
extern u8 _ZN8dActor_c11OnAttacked2ERS_[];
extern u8 _ZN8dActor_c8OnKickedERS_[];
extern u8 _ZN8dActor_c8OnPushedERS_[];
extern u8 _ZN8dActor_c24OnHitByCannonBlastedCharERS_[];
extern u8 _ZN8dActor_c15OnHitByMegaCharER7daPly_c[];
extern u8 _ZN8dActor_c19OnHitFromUnderneathERS_[];
extern u8 _ZN7daDkk_c16OnAimedAtWithEggEv[];
extern u8 _ZN8dActor_c25OnAimedAtWithEggReturnVecEv[];
extern u8 _ZN10dBgActor_c4KillEv[];

#pragma define_section sName ".data.dkkTail"                                                       \
                             ".data.dkkTail"                                                       \
                             ".data.dkkTail" far_data
#pragma section sName begin

data_ov025_021138a8_Layout data_ov025_021138a8 = {
    &_ZN8dActor_c11OnAttacked1ERS_[0],
    &_ZN8dActor_c11OnAttacked2ERS_[0],
    &_ZN8dActor_c8OnKickedERS_[0],
    &_ZN8dActor_c8OnPushedERS_[0],
    &_ZN8dActor_c24OnHitByCannonBlastedCharERS_[0],
    &_ZN8dActor_c15OnHitByMegaCharER7daPly_c[0],
    &_ZN8dActor_c19OnHitFromUnderneathERS_[0],
    &_ZN7daDkk_c16OnAimedAtWithEggEv[0],
    &_ZN8dActor_c25OnAimedAtWithEggReturnVecEv[0],
    &_ZN10dBgActor_c4KillEv[0],
};
