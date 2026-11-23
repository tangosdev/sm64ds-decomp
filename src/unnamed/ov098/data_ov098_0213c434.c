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
} MgShellSmash_SpawnInfo_Layout;
typedef char MgShellSmash_SpawnInfo_size[sizeof(MgShellSmash_SpawnInfo_Layout) == 0x24 ? 1 : -1];

extern MgShellSmash_SpawnInfo_Layout MgShellSmash_SpawnInfo;
extern u8 _ZN8dActor_c11OnAttacked2ERS_[];
extern u8 _ZN8dActor_c8OnKickedERS_[];
extern u8 _ZN8dActor_c8OnPushedERS_[];
extern u8 _ZN8dActor_c24OnHitByCannonBlastedCharERS_[];
extern u8 _ZN15daObjYajirusi_c15OnHitByMegaCharER7daPly_c[];
extern u8 _ZN8dActor_c19OnHitFromUnderneathERS_[];
extern u8 _ZN8dActor_c16OnAimedAtWithEggEv[];
extern u8 _ZN8dActor_c25OnAimedAtWithEggReturnVecEv[];
extern u8 _ZN15daObjYajirusi_c4KillEv[];

#pragma define_section sName ".data.blockHead"                                                     \
                             ".data.blockHead"                                                     \
                             ".data.blockHead" far_data
#pragma section sName begin

MgShellSmash_SpawnInfo_Layout MgShellSmash_SpawnInfo = {
    &_ZN8dActor_c11OnAttacked2ERS_[0],
    &_ZN8dActor_c8OnKickedERS_[0],
    &_ZN8dActor_c8OnPushedERS_[0],
    &_ZN8dActor_c24OnHitByCannonBlastedCharERS_[0],
    &_ZN15daObjYajirusi_c15OnHitByMegaCharER7daPly_c[0],
    &_ZN8dActor_c19OnHitFromUnderneathERS_[0],
    &_ZN8dActor_c16OnAimedAtWithEggEv[0],
    &_ZN8dActor_c25OnAimedAtWithEggReturnVecEv[0],
    &_ZN15daObjYajirusi_c4KillEv[0],
};
