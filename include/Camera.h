/* AUTO-GENERATED from matched-function evidence by tools/gen_header.py
 * class Camera: 12 matched functions, 17 evidenced fields.
 * Offsets/widths are observed, not guessed. Gaps are explicit padding.
 * Field NAMES are placeholders - renaming cannot change codegen. */
#ifndef CAMERA_H
#define CAMERA_H
#include "types.h"

/* fwd */
struct j;
struct Camera {
    /* Nested, and only ever needed as a pointer -- Camera::ChangeState is
       mangled `PNS_5StateE`, which IS the nesting, so the declaration is
       evidence rather than convention. Layout not recovered; nothing here
       dereferences one.

       Spelled WITHOUT a preprocessor guard, and the field below is
       `struct State*` rather than `State*`, because both forms have to survive
       C as well: src/game/camera/Camera/_ZN6CameraC1Ev.c is a C file that includes this header.
       The elaborated form works in both languages and means the same thing --
       in C++ it finds the nested Camera::State, in C the tag declared here.

       Bare `State*` cost _ZN6CameraC1Ev its eligibility (caught by the
       eligible.py bracket -- the only gate that sees a header edit break
       another file). Wrapping this in `#ifdef __cplusplus` fixed that and
       broke something worse: a `#` inside the body ends check_header_offsets'
       field walk, so Camera.h reported "0 commented fields ... spans 0x0" and
       exited 0 -- a clean-looking pass that checked nothing. Keep both this
       line and the field free of the preprocessor. */
    struct State;

    u8  pad_000[0x80];
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
#ifdef __cplusplus
    /* methods */
    int Behavior();
    int ChangeState(State * state);
    int CleanupResources();
    void GoBehindPlayer(unsigned int j);
    int IsUnderwater() const;
    void OnPendingDestroy();
    void SaveCameraStateBeforeTalk();
    void SetLookAt(const Vector3 & lookAt_);
    void SetPos(const Vector3 & pos_);
#endif
};

typedef char Camera_size_must_be_0x1a8[sizeof(struct Camera) == 0x1a8 ? 1 : -1];

#endif
