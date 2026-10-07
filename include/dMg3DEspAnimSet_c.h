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

    /* The flash loop the model's states arm: mRepeats phase cycles of
       mPeriod frames at mSpeed. */
    void SetRepeats(s16 repeats);
    void SetPeriod(s16 period);
    void SetSpeed(s32 speed);

    ModelAnim mModels[3];                 /* 0x000 */
    MaterialChanger mMaterialChangers[3]; /* 0x12c */
    s32 mActive[3];                       /* 0x168 */
    s32 mSpeed;                           /* 0x174 */
    s16 mRepeats;                         /* 0x178 */
    s16 mPhase;                           /* 0x17a */
    s16 mPeriod;                          /* 0x17c */
    u8 pad_17e[0x02];                     /* 0x17e */
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char dMg3DEspAnimSet_c_size_must_be_0x180[
    sizeof(dMg3DEspAnimSet_c) == 0x180 ? 1 : -1];
#endif

#endif
