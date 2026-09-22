//cpp
// @symbol _ZN12dScMgSlot3_c9Virtual80Ev
// recovered name: dScMgSlot3_c_AfterClsn  -- WRONG, see below
/* recovered: renamed to Class_Method, declarations from a shared header */
#include "decl_common.h"
#include "dScMgSlot3_c.h"
/* recovered: renamed to Class_Method */
/* dScMgSlot3_c::Virtual80 - slot 32, the only override of it in the family.

   Not an AfterClsn: the base slot has no ROM name at all, and the one the
   recovery pass borrowed belongs to dPathLiftActor_c, two forks away.  See the
   slot-32 block in include/dScMgBase_c.h.

   The base's body verbatim -- main BG1CNT reduced to 0x1000, scroll reset, BG1
   cleared from the main BG-enable shadow, this class's own language table and
   the shared screen map -- and then one more write, leaving BG1CNT at 0x1118
   instead of 0x1000: the same layer, pointed at this minigame's own character
   and screen base blocks. */
struct G2 { static void* GetBG1ScrPtr(); };

extern "C" {
    void SetBg1Offset(int a, int b);
    int GetGameLanguage(void);
    void* func_02054ea8(void);
    unsigned int LoadCompressedFileAt(int fileID, void* target);
}

extern "C" {
extern unsigned char data_0209d45c;
}

void dScMgSlot3_c::Virtual80()
{
    int idx;

    *(volatile unsigned short*)0x400000a &= ~3;
    *(volatile unsigned short*)0x400000a = (*(volatile unsigned short*)0x400000a & 0x43) | 0x1000;
    *(volatile unsigned short*)0x400000a &= ~0x40;

    SetBg1Offset(0, 0);

    data_0209d45c &= ~2;

    idx = GetGameLanguage();
    LoadCompressedFileAt(data_ov006_0213e614[idx], func_02054ea8());
    LoadCompressedFileAt(0x67, G2::GetBG1ScrPtr());

    *(volatile unsigned short*)0x400000a = (*(volatile unsigned short*)0x400000a & 0x43) | 0x1118;
}
