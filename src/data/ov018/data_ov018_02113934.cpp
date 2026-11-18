//cpp
/* .data data at 0x02113934; bytes from the cartridge. */
#include "types.h"

struct dActor_c;
struct daPly_c;

/* local extern: ROM vtable slot word. */
extern "C" int _ZN8dActor_c24OnHitByCannonBlastedCharERS_(void *, struct dActor_c &);
/* local extern: ROM vtable slot word. */
extern "C" void _ZN14daObjSm_Lift_c15OnHitByMegaCharER7daPly_c(void *, struct daPly_c &);
/* local extern: ROM vtable slot word. */
extern "C" int _ZN8dActor_c19OnHitFromUnderneathERS_(void *, struct dActor_c &);
/* local extern: ROM vtable slot word. */
extern "C" int _ZN8dActor_c16OnAimedAtWithEggEv(void *);
/* local extern: ROM vtable slot word. */
extern "C" Vector3 _ZN8dActor_c25OnAimedAtWithEggReturnVecEv(void *);
/* local extern: ROM vtable slot word. */
extern "C" void _ZN10dBgActor_c4KillEv(void *);

unsigned int data_ov018_02113934[6] = {(u32)_ZN8dActor_c24OnHitByCannonBlastedCharERS_,
                                       (u32)_ZN14daObjSm_Lift_c15OnHitByMegaCharER7daPly_c,
                                       (u32)_ZN8dActor_c19OnHitFromUnderneathERS_,
                                       (u32)_ZN8dActor_c16OnAimedAtWithEggEv,
                                       (u32)_ZN8dActor_c25OnAimedAtWithEggReturnVecEv,
                                       (u32)_ZN10dBgActor_c4KillEv};
