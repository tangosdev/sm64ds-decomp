//cpp
/* .data data at 0x021138fc; bytes from the cartridge. */
#include "types.h"

struct dActor_c;
struct daObjSm_Lift_c;
struct daPly_c;

/* local extern: ROM vtable slot word. */
extern "C" void _ZN7fBase_c16OnPendingDestroyEv(void *);
/* local extern: ROM vtable slot word. */
extern "C" int _ZN7fBase_c9Virtual34Ejj(void *, unsigned int, unsigned int);
/* local extern: ROM vtable slot word. */
extern "C" int _ZN7fBase_c9Virtual38Ejj(void *, unsigned int, unsigned int);
/* local extern: ROM vtable slot word. */
extern "C" bool _ZN7fBase_c13OnHeapCreatedEv(void *);
/* local extern: ROM vtable slot word. */
extern "C" struct daObjSm_Lift_c *_ZN14daObjSm_Lift_cD1Ev(void *);
/* local extern: ROM vtable slot word. */
extern "C" void _ZN14daObjSm_Lift_cD0Ev();
/* local extern: ROM vtable slot word. */
extern "C" int _ZN8dActor_c13OnYoshiTryEatEv(void *);
/* local extern: ROM vtable slot word. */
extern "C" void _ZN8dActor_c13OnTurnIntoEggER7daPly_c(void *, struct daPly_c &);
/* local extern: ROM vtable slot word. */
extern "C" int _ZN8dActor_c9Virtual50Ev(void *);
/* local extern: ROM vtable slot word. */
extern "C" void _ZN8dActor_c15OnGroundPoundedERS_(void *, struct dActor_c &);
/* local extern: ROM vtable slot word. */
extern "C" int _ZN8dActor_c11OnAttacked1ERS_(void *, struct dActor_c &);
/* local extern: ROM vtable slot word. */
extern "C" int _ZN8dActor_c11OnAttacked2ERS_(void *, struct dActor_c &);
/* local extern: ROM vtable slot word. */
extern "C" void _ZN8dActor_c8OnKickedERS_(void *, struct dActor_c &);
/* local extern: ROM vtable slot word. */
extern "C" int _ZN8dActor_c8OnPushedERS_(void *, struct dActor_c &);

unsigned int data_ov018_021138fc[14] = {
    (u32)_ZN7fBase_c16OnPendingDestroyEv, (u32)_ZN7fBase_c9Virtual34Ejj,
    (u32)_ZN7fBase_c9Virtual38Ejj,        (u32)_ZN7fBase_c13OnHeapCreatedEv,
    (u32)_ZN14daObjSm_Lift_cD1Ev,         (u32)_ZN14daObjSm_Lift_cD0Ev,
    (u32)_ZN8dActor_c13OnYoshiTryEatEv,   (u32)_ZN8dActor_c13OnTurnIntoEggER7daPly_c,
    (u32)_ZN8dActor_c9Virtual50Ev,        (u32)_ZN8dActor_c15OnGroundPoundedERS_,
    (u32)_ZN8dActor_c11OnAttacked1ERS_,   (u32)_ZN8dActor_c11OnAttacked2ERS_,
    (u32)_ZN8dActor_c8OnKickedERS_,       (u32)_ZN8dActor_c8OnPushedERS_};
