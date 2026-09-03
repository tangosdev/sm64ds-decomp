#ifndef DAOBJKM2_AMI_BOU_C_H
#define DAOBJKM2_AMI_BOU_C_H

#include "types.h"
#include "dBgActor_c.h"
#include "dCcAc_c.h"
#include "dBgW_KcMbg.h"

/* Derives from dBgActor_c: the destructor stores this class's vtable, then
 * dBgActor_c's -- inlined -- then destroys the dBgW_KcMbg at 0x124 and
 * the Model at 0xd4 before chaining to dActor_c. All three belong to dBgActor_c.
 * Everything this header used to restate below 0x31e was dActor_c's and
 * dBgActor_c's, and is inherited now.
 *
 * SIZE IS THE OBSERVED FIELD SPAN, rounded up. It guards this declaration; it
 * is not independent evidence about the ROM.
 *
 * SM64DS RTTI names the implementation daObjKm2_Ami_Bou_c at ov045:0x02112d74.
 * The reconstructed factory daObjKm2_Ami_Bou_c_classInit (historical alias
 * PoleLift_Spawn) allocates 0x358 and installs this class's cartridge vtable.
 *
 * The flat C twin this header used to carry below `#else` is gone: every
 * translation unit that includes it is C++, and the one file that used to need
 * the flat spelling -- the standalone D0 source -- no longer exists.
 */
struct daObjKm2_Ami_Bou_c : dBgActor_c {
    u8  pad_31e[0x2];                   /* 0x31e */
    dCcAc_c mdCcAc_c;                   /* 0x320 */
    u16 mHeightAng;                     /* 0x354 */

    /* Inline, and declared FIRST. This TU defines every virtual the class has,
     * so it emits the vtable and RTTI; out of line, mwccarm emits the D2/D1/D0
     * triple in D0-before-D1 order, but retail puts D1 (0x0211150c) ABOVE D0
     * (0x02111558), and objisolate then refuses the whole TU for emitting out
     * of ROM address order. The inline body emits only the retail D1/D0 pair,
     * in retail order, and emits no D2. */
    virtual ~daObjKm2_Ami_Bou_c() {}    /* slots 16 (D1), 17 (D0) */

    virtual int InitResources();        /* slot  0 */
    virtual int CleanupResources();     /* slot  3 */
    virtual int Behavior();             /* slot  6 */
    virtual int Render();               /* slot  9 */
};

typedef char daObjKm2_Ami_Bou_c_size_must_be_0x358[
    sizeof(daObjKm2_Ami_Bou_c) == 0x358 ? 1 : -1];

#endif /* DAOBJKM2_AMI_BOU_C_H */
