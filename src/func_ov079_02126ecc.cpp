//cpp
// @symbol func_ov079_02126ecc
/* recovered: renamed to Class_Method, RTTI class fields named, declarations from a shared header */
#include "decl_Platform.h"
/* recovered: renamed to Class_Method, RTTI class fields named */
#include "daObjBkKillerdai_c.h"
// recovered name: daObjBkKillerdai_c_OnHitByMegaChar
/* recovered: renamed to Class_Method */
/* daObjBkKillerdai_c::OnHitByMegaChar - verified 2004/b56 byte-match (ov079), strict-reloc */
extern "C" {
extern void _ZN6Player16IncMegaKillCountEv(void*);
void func_ov079_02126ecc(char *c, void *p){
    struct daObjBkKillerdai_c *self = (struct daObjBkKillerdai_c *)(void *)c;
  _ZN6Player16IncMegaKillCountEv(p);
  _ZN8Platform14KillByMegaCharER6Player(c, p);
  self->unk_08e = self->unk_094;
}
}
