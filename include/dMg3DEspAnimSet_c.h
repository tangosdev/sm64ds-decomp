#ifndef DMG3DESPANIMSET_C_H
#define DMG3DESPANIMSET_C_H

#include "MaterialChanger.h"
#include "ModelAnim.h"

/* The source spelling and ordinary method names are inferred from the sole
 * owning minigame and each method's role. The object boundary and member order
 * are compiler-proven: its paired helpers construct
 * three ModelAnim objects followed by three MaterialChanger objects, and tear
 * them down in reverse order. The ordinary methods then prove the trailing
 * 0x18 bytes of per-animation state. */
struct dMg3DEspAnimSet_c {
    dMg3DEspAnimSet_c();
    ~dMg3DEspAnimSet_c();

    void Reset();
    void Behavior();
    void Render();
    void InitResources();

    ModelAnim mModels[3];                 /* 0x000 */
    MaterialChanger mMaterialChangers[3]; /* 0x12c */
    s32 unk_168[3];                       /* 0x168 */
    s32 unk_174;                          /* 0x174 */
    s16 unk_178;                          /* 0x178 */
    s16 unk_17a;                          /* 0x17a */
    s16 unk_17c;                          /* 0x17c */
    u8 pad_17e[0x02];                     /* 0x17e */
};

typedef char dMg3DEspAnimSet_c_size_must_be_0x180[
    sizeof(dMg3DEspAnimSet_c) == 0x180 ? 1 : -1];

#endif
