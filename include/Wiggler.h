#ifndef WIGGLER_H
#define WIGGLER_H

#include "types.h"

/* The Wiggler. Five body segments, so five of everything -- and EIGHT arrays,
 * which close on eight consecutive boundaries:
 *
 *     dEnemyBase_c                        ends 0x110
 *     ModelAnim[5]                 0x110 + 5*0x64 = 0x304  -> MaterialChanger
 *     MaterialChanger[5]           0x304 + 5*0x14 = 0x368  -> TextureSequence
 *     TextureSequence[5]           0x368 + 5*0x14 = 0x3cc  -> the first triple
 *     Vector3[5]                   0x3cc + 5*0x0c = 0x408  -> the second
 *     Vector3[5]                   0x408 + 5*0x0c = 0x444  -> the shorts
 *     Vector3s[5]                  0x444 + 5*0x06 = 0x462
 *     dCcAcPos_c[5] 0x478 + 5*0x40 = 0x5b8  -> the second set
 *     dCcAcPos_c[5] 0x5b8 + 5*0x40 = 0x6f8
 *
 * The Vector3s array is what named func_02011508 -- see the note on Vector3s in
 * include/types.h and src/_ZN8Vector3sD1Ev.cpp.
 *
 * SM64DS RTTI names the implementation daHanachan_c. The reconstructed factory
 * daHanachan_c_classInit (historical alias Wiggler_Spawn) installs this class's
 * cartridge vtable; the reconstructed profile global g_profile_HANACHAN
 * (historical alias Wiggler_SpawnInfo) is its registry descriptor.
 */

#ifdef __cplusplus

#include "dEnemyBase_c.h"
#include "ModelAnim.h"
#include "MaterialChanger.h"
#include "TextureSequence.h"
#include "dBgCh_Actr.h"
#include "dCcAcPos_c.h"

struct Wiggler : dEnemyBase_c {
    ModelAnim mModelAnims[5];                        /* 0x110 */
    MaterialChanger mMaterialChangers[5];            /* 0x304 */
    TextureSequence mTextureSequences[5];            /* 0x368 */
    Vector3 unk_3cc[5];                              /* 0x3cc */
    Vector3 unk_408[5];                              /* 0x408 */
    Vector3s unk_444[5];                             /* 0x444 */
    u8  pad_462[0x16];
    dCcAcPos_c mdCc_cs1[5];    /* 0x478 */
    dCcAcPos_c mdCc_cs2[5];    /* 0x5b8 */
    u8  pad_6f8[0x10];
    dBgCh_Actr mWithMeshClsn;                      /* 0x708 */
    /* trailing extent the ROM's `new Wiggler` literal proves; see tools/opnew_sizes.py */
    u8 pad_8c4[0x24];

    virtual ~Wiggler();

    /* An override the cartridge proves and this header never declared. _ZTV7Wiggler
       slot 6 pointed at fBase_c::Behavior; the ROM has ov034:_ZN7Wiggler8BehaviorEv
       (0x02112b5c, 0x6e0 bytes), named in symbols.txt but not yet decompiled -- the
       slot needs the symbol, not a body, so declaring it is the whole fix.
       No `virtual` keyword, matching the overrides beside it: a derived declaration
       of a base virtual overrides whether or not it repeats the word.
       Measured by tools/romdata_check.py, the only gate that reads vtable bytes --
       objisolate drops every non-.text section, so 106/106 is blind here. */
    int Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
};

typedef char Wiggler_size_must_be_0x8e8[sizeof(struct Wiggler) == 0x8e8 ? 1 : -1];

#else

/* The same object for a C translation unit, flat. */
struct Wiggler {
    u8  pad_000[0x708];
    u8  mWithMeshClsn[0x1bc];      /* 0x708 */
    /* trailing extent the ROM's `new Wiggler` literal proves; see tools/opnew_sizes.py */
    u8 pad_8c4[0x24];
};

#endif /* __cplusplus */

#endif /* WIGGLER_H */
