//cpp
/* .data data at 0x02113b90; bytes from the cartridge. */
#include "types.h"

struct dActor_c;
struct daPly_c;

extern "C" int _ZN8dActor_c11OnAttacked2ERS_(void *, struct dActor_c &);  /* local extern: linker-name reference for a data word */
extern "C" void _ZN8dActor_c8OnKickedERS_(void *, struct dActor_c &);  /* local extern: linker-name reference for a data word */
extern "C" int _ZN8dActor_c8OnPushedERS_(void *, struct dActor_c &);  /* local extern: linker-name reference for a data word */
extern "C" int _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void *, struct dActor_c &);  /* local extern: linker-name reference for a data word */
extern "C" void _ZN15daObjIceBoard_c15OnHitByMegaCharER7daPly_c(void *, struct daPly_c &);  /* local extern: linker-name reference for a data word */
extern "C" int _ZN8dActor_c19OnHitFromUnderneathERS_(void *, struct dActor_c &);  /* local extern: linker-name reference for a data word */
extern "C" int _ZN8dActor_c16OnAimedAtWithEggEv(void *);  /* local extern: linker-name reference for a data word */
extern "C" Vector3 _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void *);  /* local extern: linker-name reference for a data word */
extern "C" void _ZN15daObjIceBoard_c4KillEv(void *);  /* local extern: linker-name reference for a data word */

unsigned int data_ov018_02113b90[12] = {
    (u32)_ZN8dActor_c11OnAttacked2ERS_, (u32)_ZN8dActor_c8OnKickedERS_, (u32)_ZN8dActor_c8OnPushedERS_, (u32)_ZN8dActor_c24OnHitByCannonBlastedCharERS_, (u32)_ZN15daObjIceBoard_c15OnHitByMegaCharER7daPly_c, (u32)_ZN8dActor_c19OnHitFromUnderneathERS_,
    (u32)_ZN8dActor_c16OnAimedAtWithEggEv, (u32)_ZN8dActor_c25OnAimedAtWithEggReturnVecEv, (u32)_ZN15daObjIceBoard_c4KillEv, 0x00000000, 0x00000000, 0x00000000
};
