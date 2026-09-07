/* Camera's evidenced object layout and real View inheritance.
 * Offsets and widths are observed from matched functions; gaps remain explicit.
 * Field names are descriptive and do not affect code generation. */
#ifndef CAMERA_H
#define CAMERA_H
#include "View.h"

struct Camera : View {
    /* Nested, and only ever needed as a pointer -- Camera::ChangeState is
       mangled `PNS_5StateE`, which IS the nesting, so the declaration is
       evidence rather than convention. Layout not recovered; nothing here
       dereferences one.

       The mangling is the evidence for the nesting. Layout is unnecessary:
       every use is through a pointer. */
    struct State;

    /* View occupies 0x00..0x7f; its Matrix4x3 is at 0x50. */
    /* The camera proper: where it looks and where it is. Both are twelve-byte
       Vector3s -- Camera::SetLookAt and Camera::SetPos each write exactly three
       words, at 0x80 and 0x8c. */
    Vector3 lookAt;            /* 0x080 */
    Vector3 pos;            /* 0x08c */
    u8  pad_098[0x18];
    /* The backup pair. SaveCameraStateBeforeTalk copies 0x80->0xb0 and
       0x8c->0xbc as six 4-byte words and then sets flag 0x4000 to mark the
       save taken, so these mirror the two above field for field.
       That copy is also what fixes the WIDTH: 0x094 and 0x0c4 were declared
       u8 and are read and written as full words at both ends, so both were
       too narrow -- they are the z of their respective vectors. */
    Vector3 savedLookAt;            /* 0x0b0 */
    Vector3 savedPos;            /* 0x0bc */
    u8  pad_0c8[0x48];
    s32 mTargetPlayer;            /* 0x110 */
    u8  pad_114[0x24];
    /* Current state. ChangeState compares its argument against this, and only
       swaps it -- and clears unk_1a6 -- when they differ. */
    struct State* mState;            /* 0x138 */
    s32 mState_13c;            /* 0x13c */
    u8  pad_140[0x8];
    /* Owned; CleanupResources deletes it. */
    void* mFixedViewPos;            /* 0x148 */
    u8  pad_14c[0x8];
    /* Flag word. Bit 0 is under-water (Camera::IsUnderwater returns it
       masked, not normalised); 0x10 vetoes a state change; 0x4000 records
       that the pre-talk save has been taken. */
    u32 mFlags;            /* 0x154 */
    u8  pad_158[0x4e];
    u8  unk_1a6;            /* 0x1a6 */
    Camera();
    virtual ~Camera();

    /* fBase_c virtual overrides, in their inherited slots. */
    virtual s32 InitResources();
    virtual s32 CleanupResources();
    virtual s32 Behavior();
    virtual s32 Render();
    virtual void OnPendingDestroy();

    /* Non-virtual camera operations. */
    int ChangeState(State * state);
    void GoBehindPlayer(u32 playerID);
    int IsUnderwater() const;
    void SaveCameraStateBeforeTalk();
    void SetLookAt(const Vector3 & lookAt_);
    void SetPos(const Vector3 & pos_);
};

typedef char Camera_size_must_be_0x1a8[sizeof(struct Camera) == 0x1a8 ? 1 : -1];

#endif
