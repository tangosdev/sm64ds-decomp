#ifndef BCA_FILE_H
#define BCA_FILE_H

#include "types.h"

/* The BCA0 bone-animation file, as the game reads it in place.
 *
 * LAYOUT is read out of the matched code, not out of format docs:
 *
 *   * dExtAnmModel_c::SetAnim and dExtBlendAnmModel_c::SetAnim read numFrames
 *     at +0x02 and hand it to dExtFrameCtrl_c::SetAnimation.
 *   * func_ov007_020bc3dc reads the byte at +0x04 to pick the loop mode
 *     (zero plays once, nonzero wraps).
 *   * dExtFrameCtrl_c::UpdateFileOffsets rebases the words at +0x08, +0x0c,
 *     +0x10 and +0x14 by the file address, which is the evidence that all
 *     four are file-relative on disk and pointers once loaded.
 *   * func_0204547c (the per-bone sampler under ModelComponents::UpdateBones)
 *     reads the header as: numFrames at +0x02; a word array at +0x08 that it
 *     seeds as 0x1000 (1.0 fixed point) when a bone has no scale channel
 *     bits, so scale; a 16-bit array at +0x0c whose samples it shifts left by
 *     4 into 16-bit angle fields, so rotation; a word array at +0x10 stored
 *     straight into the bone's last three word fields, so translation; and a
 *     table at +0x14 indexed by bone * 0x24, so one track record per bone.
 *
 * Within a track record the sampler reads nine 4-byte channels in the order
 * scale xyz, rotation xyz, translation xyz. Each is {u8 shift, u8 animated,
 * u16 index}: animated == 0 means the channel is the constant at
 * values[0]; otherwise values[index + ...] is sampled, with keys spaced
 * 1 << shift frames apart and linearly interpolated between (shift == 0 is
 * one key per frame).
 *
 * Bytes 0x00-0x01 and 0x05-0x07 are touched by no matched function yet and
 * stay padding. Field NAMES are inferred from that behaviour and cannot
 * change codegen; offsets are pinned by the bytes.
 */

struct BCA_Channel {
    u8 shift;    /* 0x00 - key spacing is 1 << shift frames */
    u8 animated; /* 0x01 - zero: constant, values[0] */
    u16 index;   /* 0x02 - start of this channel in the value array */
};

/* One 0x24-byte track record per bone: nine channels. */
struct BCA_BoneTrack {
    struct BCA_Channel scale[3];
    struct BCA_Channel rotation[3];
    struct BCA_Channel translation[3];
};

struct BCA_File {
    u8 pad_00[2];
    u16 numFrames; /* 0x02 */
    u8 looping;    /* 0x04 - zero plays once, nonzero wraps */
    u8 pad_05[3];
    s32 *scaleValues;                 /* 0x08 - file-relative, fixed up on load */
    s16 *rotationValues;              /* 0x0c - file-relative, fixed up on load */
    s32 *translationValues;           /* 0x10 - file-relative, fixed up on load */
    struct BCA_BoneTrack *boneTracks; /* 0x14 - file-relative, fixed up on load */
};

#endif
