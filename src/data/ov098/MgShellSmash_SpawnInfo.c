/* ov098 MgShellSmash_SpawnInfo at 0x0213c434 (data, .data.blockHead, 36 bytes): cartridge data. */
#include "types.h"

#pragma define_section dc8_section ".data.blockHead" ".data.blockHead" ".data.blockHead" far_data
#pragma section dc8_section begin
u32 MgShellSmash_SpawnInfo[9] = {
    0x02010140,  /* _ZN8dActor_c11OnAttacked2ERS_ */
    0x0201013c,  /* _ZN8dActor_c8OnKickedERS_ */
    0x02010138,  /* _ZN8dActor_c8OnPushedERS_ */
    0x02010134,  /* _ZN8dActor_c24OnHitByCannonBlastedCharERS_ */
    0x02137d80,  /* _ZN15daObjYajirusi_c15OnHitByMegaCharER7daPly_c */
    0x0201012c,  /* _ZN8dActor_c19OnHitFromUnderneathERS_ */
    0x02010124,  /* _ZN8dActor_c16OnAimedAtWithEggEv */
    0x020100dc,  /* _ZN8dActor_c25OnAimedAtWithEggReturnVecEv */
    0x02137ccc,  /* _ZN15daObjYajirusi_c4KillEv */
};

#pragma section dc8_section end
