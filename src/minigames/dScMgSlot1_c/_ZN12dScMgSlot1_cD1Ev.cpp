//cpp
// @symbol _ZN12dScMgSlot1_cD1Ev
/* ~dScMgSlot1_c() (D1, complete-object destructor) -- explicitly tears
   down the embedded betIcon_c subobject at 0x4660 by hand: writes its own
   vtable (data_ov006_0213e5d4), then overwrites it with its base
   dThIcon_c's vtable (data_ov001_020ad494), the same "derived writes its own
   vtable, then chains to its base which writes ITS OWN vtable, both fully
   inlined to nothing but the two writes" idiom dScMgBase_c.h documents for
   its own touchIcon_0f4 -- see include/dScMgSlot1_c.h's file banner for
   the full identification of what betIcon_c/dThIcon_c are. The own-
   vtable-write and base-D2 call are compiler generated. */
#include "dScMgSlot1_c.h"

dScMgSlot1_c::~dScMgSlot1_c()
{
    *(void **)((char *)this + 0x4660) = &data_ov006_0213e5d4;
    *(void **)((char *)this + 0x4660) = (void *)&data_ov001_020ad494;
}
