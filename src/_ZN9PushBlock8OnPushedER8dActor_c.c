// @symbol _ZN9PushBlock8OnPushedER8dActor_c
/* recovered: renamed to Class_Method, RTTI class fields named
 *
 * PushBlock::OnPushed(dActor_c&) -- vtable slot 25, ov002 0x020b8d3c.
 * Attributed by the vtable: _ZTV9PushBlock carries this address at slot 25 --
 * vtable + 0x64 -- where _ZTV10dBgActor_c carries dActor_c's generic
 * 0x02010138. include/dActor_c.h's own slot 25 declaration supplies the
 * signature, `virtual int OnPushed(dActor_c &other)`.
 *
 * Confirmed with tools/mangle.py, not hand-mangled:
 * _ZN9PushBlock8OnPushedER8dActor_c -- NOT ...ERS_. The Itanium `S_`
 * back-reference only fires when the parameter type has already appeared
 * earlier in the same mangled name (as it does inside dActor_c's own
 * methods); here the enclosing class is PushBlock, so the dActor_c parameter
 * is spelled out in full.
 *
 * Kept as an extern free function under the literal mangled name, reading the
 * flat C spelling of PushBlock by hand offset -- same shape as
 * _ZN9PushBlock13InitResourcesEv.cpp. A reference parameter and a pointer
 * parameter are the same thing at the ABI level, so `r1` staying a raw
 * pointer here does not change codegen. */
#include "PushBlock.h"
void _ZN9PushBlock8OnPushedER8dActor_c(char* r0, char* r1){
    struct PushBlock *self = (struct PushBlock *)(void *)r0;
  if(r1==0) return;
  self->unk_094=*(short*)(r1+0x8e);
  if(*(int*)(r1+8)==2) self->unk_098=0x8000;
  else self->unk_098=0x4000;
}
