//cpp
/* .data data at 0x02113b80; bytes from the cartridge. */
#include "types.h"

struct dActor_c;
struct daPly_c;

extern "C" void _ZN8dActor_c13OnTurnIntoEggER7daPly_c(void *, struct daPly_c &);  /* local extern: linker-name reference for a data word */
extern "C" int _ZN8dActor_c9Virtual50Ev(void *);  /* local extern: linker-name reference for a data word */
extern "C" void _ZN15daObjIceBoard_c15OnGroundPoundedER8dActor_c(void *, struct dActor_c &);  /* local extern: linker-name reference for a data word */
extern "C" int _ZN8dActor_c11OnAttacked1ERS_(void *, struct dActor_c &);  /* local extern: linker-name reference for a data word */

unsigned int data_ov018_02113b80[4] = {
    (u32)_ZN8dActor_c13OnTurnIntoEggER7daPly_c, (u32)_ZN8dActor_c9Virtual50Ev, (u32)_ZN15daObjIceBoard_c15OnGroundPoundedER8dActor_c, (u32)_ZN8dActor_c11OnAttacked1ERS_
};
