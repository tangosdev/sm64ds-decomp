//cpp
/* .data data at 0x021138f0; bytes from the cartridge. */
#include "types.h"

/* local extern: ROM vtable slot word. */
extern "C" int _ZN14daObjSm_Lift_c6RenderEv(void *);
/* local extern: ROM vtable slot word. */
extern "C" int _ZN8dActor_c12BeforeRenderEv(void *);
/* local extern: ROM vtable slot word. */
extern "C" void _ZN8dActor_c11AfterRenderEj(void *, unsigned int);

unsigned int data_ov018_021138f0[3] = {(u32)_ZN14daObjSm_Lift_c6RenderEv,
                                       (u32)_ZN8dActor_c12BeforeRenderEv,
                                       (u32)_ZN8dActor_c11AfterRenderEj};
