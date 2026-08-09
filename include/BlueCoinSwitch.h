/* Hand-written from matched-function evidence:
 * class BlueCoinSwitch, ov002 0x020f11b0-0x020f15fc (9 functions, no other
 * class in the TU -- tu_map.py).
 *
 * Two names again, as with CastleWater. The symbols mangle from
 * `BlueCoinSwitch`, so the struct is spelled that way; the RTTI record calls it
 * `daObjBC_Switch_c` and include/daObjBC_Switch_c.h is the generated view under
 * that name. Same object, and this is the header methods are defined against.
 *
 * It is a Platform -- Behavior and InitResources both call
 * Platform::UpdateModelPosAndRotY and Platform::UpdateClsnPosAndRot on it -- so
 * mPos and mAngleY sit at Actor's offsets. Written FLAT with the inherited
 * slots restated, as every other generated header here is.
 *
 * Field NAMES are placeholders - renaming cannot change codegen.
 */
#ifndef BLUECOINSWITCH_H
#define BLUECOINSWITCH_H
#include "types.h"
#include "math/Matrix.h"

struct BlueCoinSwitch {
    u8  pad_000[0x8];
    /* Spawn word, unpacked into two fields by InitResources: bits 0-3 become
       the event bit unk_32d, bits 8-15 the timer seed unk_32a. */
    u32 mParam;            /* 0x008 */
    u8  pad_00c[0x50];
    s32 mPosX;            /* 0x05c */
    /* The switch top, and it MOVES: once pressed, Behavior walks mPosY down by
       0x14000 a frame until it reaches unk_320. Render draws the model only
       while mPosY is still above unk_320, so the switch disappears exactly when
       it bottoms out rather than on a separate flag. */
    s32 mPosY;            /* 0x060 */
    s32 mPosZ;            /* 0x064 */
    u8  pad_068[0x26];
    s16 mAngleY;            /* 0x08e */
    u8  pad_090[0x3c];
    /* Set to -1 the moment the switch is pressed, and its ORIGINAL value was
       copied into unk_32e at init -- so the area it belongs to is remembered
       after the switch stops claiming membership. */
    s8  mAreaId;            /* 0x0cc */
    u8  pad_0cd[0x7];
    u8  mModel;            /* 0x0d4 */
    u8  pad_0d5[0x4f];
    u8  mMeshCollider;            /* 0x124 */
    u8  pad_125[0x1c7];
    /* Real type, not a marker: InitResources hands it to
       MovingMeshCollider::SetFile as the collider's transform. */
    Matrix4x3 mMatrix;            /* 0x2ec */
    u8  pad_31c[0x4];
    /* Where mPosY stops: seeded at init as mPosY - 0x64000, so the switch
       always sinks the same distance from wherever it was placed. */
    s32 unk_320;            /* 0x320 */
    s32 unk_324;            /* 0x324 */
    /* The blue-coin countdown, in frames, and it is BOTH the timer and the
       state: non-zero means running, and the code parks it at 1 rather than 0
       to mean "expired but still latched". */
    u16 unk_328;            /* 0x328 */
    /* Its seed, from mParam's second byte -- multiplied by 10 at init, or
       forced to 0xfa when that byte is 0 or 0xff. So the spawn word carries
       tenths of the real count. */
    u16 unk_32a;            /* 0x32a */
    u8  unk_32c;            /* 0x32c */
    u8  unk_32d;            /* 0x32d */
    u8  unk_32e;            /* 0x32e */
#ifdef __cplusplus
    /* methods */
    s32 Behavior();
    int CleanupResources();
    int InitResources();
    int Render();
#endif
};

#endif
