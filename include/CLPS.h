#ifndef CLPS_H
#define CLPS_H

#include "types.h"

/* The collision-property entry a KCL triangle's attribute word indexes into.
 *
 * EIGHT BYTES, two words, pinned by the lookup func_020381cc:
 *
 *     e = *(u8 **)block;
 *     if (e == 0 || *(u16 *)(e + 4) != 8) -> the default entry at data_020a0c78
 *     *out = e + 8 + (index << 3);
 *
 * so the block is an 8-byte header carrying the entry stride at +4 (checked to
 * be exactly 8), the entries begin at +8, and the stride is 8. Every accessor
 * below reads only w0 and w1, which is the second, independent confirmation of
 * the width.
 *
 * dBgW_Kc::GetSurfaceInfo hands tri->attribute to that lookup UNMASKED --
 * in this game the KCL attribute word IS the CLPS index.
 *
 * BIT LAYOUT, decoded from the matched accessors rather than guessed. Each of
 * these is a one-line matched function in src/, named here beside what it reads:
 *
 *   w0 & 0x1f            CLPS::GetSoundType      sound-table offset: daPly_c plays
 *                                                0xe2 + it, daShl_c 0xf2 + it
 *   w0 & 0x20            SurfaceInfo_TestFlag0x20   water
 *   (w0 >> 6)  & 0x3f    CLPS::GetViewID         dCamera_c passes it to GetViewObj;
 *                                                0x3f is "no view"
 *   (w0 >> 12) & 0x7     CLPS::GetFloorClass     daPly_c.h's slipperiness class
 *   (w0 >> 15) & 0xf     CLPS::GetCameraBehavior dCamera_c::func_0200c394 picks
 *                                                its framing by it; 0 is none
 *   (w0 >> 19) & 0x1f    CLPS::GetSurfaceType    dBgCh::ShouldPassThrough
 *                                                Impl special-cases 0x11 and 0x14
 *   w0 & 0x01000000      CLPS::IsCameraPassThrough
 *   w0 & 0x02000000      func_02037e20           toxic (dBgCh::IsDetectingToxic)
 *   w0 & 0x04000000      CLPS::IsCameraOnly
 *   w1 & 0xff            CLPS::GetPathID         daPly_c's current push hands it to
 *                                                PathPtr::FromID; 0xff is "no path"
 *
 * The fields are left as the two raw words on purpose: naming them would mean
 * choosing bitfield boundaries the accessors do not evidence as a partition
 * (0x1f and 0x20 overlap the same low byte, and bits 5 and 27..31 are read by
 * nothing in src/). The accessors are the interface.
 *
 * The six the player's floor update (daPly_c::SetFloorSurfaceInfo) reads are
 * const members, each named for what its consumers do with the value (the
 * table above): dBgCh::ShouldPassThroughImpl and the player both treat
 * GetSurfaceType's value as the surface type, and include/daPly_c.h traces
 * GetFloorClass's value through five slope and slide tables. The water and
 * toxic tests still take a raw word pointer; the camera tests are const
 * members returning the original masks.
 */

struct CLPS {
    u32 w0;                 /* 0x00 */
    u32 w1;                 /* 0x04 */

#ifdef __cplusplus
    int GetPathID() const;
    int GetSoundType() const;
    int GetViewID() const;
    int GetFloorClass() const;
    int GetCameraBehavior() const;
    int GetSurfaceType() const;
    /* Inferred from camera mode in dBgCh::ShouldPassThroughImpl. */
    int IsCameraPassThrough() const;
    int IsCameraOnly() const;
    /* Store the default entry (0xfc0, 0xff), the words dBgPc's constructor
       also writes; 0x02037e9c, coined name. */
    void SetDefault();
#endif
};

#ifndef SM64DS_PLATFORM_PC
/* ROM layout under mwccarm; host ABI divergence is tracked separately. */
typedef char CLPS_size_must_be_0x8[sizeof(struct CLPS) == 0x8 ? 1 : -1];
#endif

/* The block a level or actor hands to dBgW_Kc::SetFile: an 8-byte header
 * whose halfword at +4 is the entry stride, then N entries. Kept opaque because
 * the first four bytes are read by nothing in src/ -- func_020381cc validates
 * +4 and indexes from +8, and no other function touches the header. */
struct CLPS_Block;

#endif /* CLPS_H */
