#include "types.h"

/* Definition order is what makes the compiler's size sort match address order. */

typedef struct {
    void *f000;
    void *f004;
    void *f008;
} data_ov025_021139f4_Layout;
typedef char data_ov025_021139f4_size[sizeof(data_ov025_021139f4_Layout) == 0xc ? 1 : -1];

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
    void *f02c;
    void *f030;
    void *f034;
    void *f038;
    void *f03c;
    void *f040;
    void *f044;
    void *f048;
    void *f04c;
    void *f050;
    u32 f054;
    u32 f058;
    u32 f05c;
} data_ov025_02113a00_Layout;
typedef char data_ov025_02113a00_size[sizeof(data_ov025_02113a00_Layout) == 0x60 ? 1 : -1];

extern data_ov025_021139f4_Layout data_ov025_021139f4;
extern data_ov025_02113a00_Layout data_ov025_02113a00;
extern u8 _ZN8dActor_c13AfterBehaviorEj[];
extern u8 _ZN10daDpLift_c6RenderEv[];
extern u8 _ZN8dActor_c12BeforeRenderEv[];
extern u8 _ZN8dActor_c11AfterRenderEj[];
extern u8 _ZN7fBase_c16OnPendingDestroyEv[];
extern u8 _ZN7fBase_c9Virtual34Ejj[];
extern u8 _ZN7fBase_c9Virtual38Ejj[];
extern u8 _ZN7fBase_c13OnHeapCreatedEv[];
extern u8 _ZN10daDpLift_cD1Ev[];
extern u8 _ZN10daDpLift_cD0Ev[];
extern u8 _ZN8dActor_c13OnYoshiTryEatEv[];
extern u8 _ZN8dActor_c13OnTurnIntoEggER7daPly_c[];
extern u8 _ZN8dActor_c9Virtual50Ev[];
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

#pragma define_section sName ".data.dkkTail"                                                       \
                             ".data.dkkTail"                                                       \
                             ".data.dkkTail" far_data
#pragma section sName begin

data_ov025_021139f4_Layout data_ov025_021139f4 = {
    &_ZN8dActor_c13AfterBehaviorEj[0],
    &_ZN10daDpLift_c6RenderEv[0],
    &_ZN8dActor_c12BeforeRenderEv[0],
};

data_ov025_02113a00_Layout data_ov025_02113a00 = {
    &_ZN8dActor_c11AfterRenderEj[0],
    &_ZN7fBase_c16OnPendingDestroyEv[0],
    &_ZN7fBase_c9Virtual34Ejj[0],
    &_ZN7fBase_c9Virtual38Ejj[0],
    &_ZN7fBase_c13OnHeapCreatedEv[0],
    &_ZN10daDpLift_cD1Ev[0],
    &_ZN10daDpLift_cD0Ev[0],
    &_ZN8dActor_c13OnYoshiTryEatEv[0],
    &_ZN8dActor_c13OnTurnIntoEggER7daPly_c[0],
    &_ZN8dActor_c9Virtual50Ev[0],
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
    0x00000000,
    0x00000000,
    0x00000000,
};
