// THE WIDE CONDUCTOR. Run vs16, hosted-conductor follow-up.
//
// src/func_0203ea5c.c -- the ROM's own wireless exchange and unpack loop -- is
// what stopped a fifth player playing: nine hard-coded `< 4` bounds
// (:310, :326, :347, :352, :367, :403, :428, :471, :499). It is byte-matched
// ROM code, so it cannot be edited; it is HOSTED instead, the way the port
// hosts many functions, and this file is that hosting.
//
// THE DISPATCH, AND WHY THE NARROW PATH IS THE REAL TU. port/CMakeLists.txt
// compiles src/func_0203ea5c.c with `func_0203ea5c=func_0203ea5c_narrow`, the
// same per-source rename a dozen actor TUs already use. The matched TU stays
// in the link, byte for byte, and `func_0203ea5c` here is a two-line gate in
// front of it: a session of four or fewer runs the ROM's own code -- not a
// transcription of it -- and a session of five or more runs the wide copy
// below. The gate reads port::comms_session_players(), which is the SAME
// number the carrier derives the wire from (comms_loopback.cpp, THE TWO
// WIRES), so the record bounds and the datagram layout cannot disagree. No
// transport installed answers 0, which is narrow, which is solo: unchanged.
//
// THE WIDE COPY IS A TRANSCRIPTION, NOT A REWRITE. Same locals, same
// staging, same switch, same gotos, same store order -- diffable against
// src/func_0203ea5c.c line by line. Exactly one thing changes: SEVEN of the
// nine bounds walk the per-slot RECORD run (data_020a1154, 0x24 stride) and
// now walk all sixteen records the host gives that run
// (hal/camera_bridges.cpp). Sixteen is the ARRAY's width, mirroring the ROM's
// own walk-the-whole-array shape; presence is still the record's 0x8000 bit,
// exactly as the ROM decides it.
//
// THE TWO BOUNDS THAT DO NOT MOVE (:347, :352), AND THE INFO PATH AROUND
// THEM. The countdown at data_020a0ef8 drives the DS lobby's name-exchange
// rounds: 0x16-byte payloads staged from the four-slot buffer band at
// data_020a10a4 and merged through the six-byte id bands at data_020a0fa6 /
// data_020a0fbe (hal/comms_conductor.cpp, bands B, C and D -- four-slot ROM
// storage, all of it). That path CANNOT ARM in this port's flow: the trigger
// (loop_75 below) requires every live peer to advertise bit 0 of its record
// flags, a peer only advertises bit 0 while its own countdown runs (:183),
// and the countdown starts at zero and is started nowhere the port reaches --
// the DS wireless lobby that starts it is not on any scene path this port
// registers, and the port's names travel as SM64DS_VS_NAMES, not as info
// rounds. So the info path is kept at the cartridge's own shape, storage and
// bounds alike, and if a wide session ever DOES arm it, that is said out loud
// once rather than silently exchanging four of sixteen names.
//
// WHAT HAD TO EXIST FIRST (same commit set): the record run grown to sixteen
// at its tail (hal/camera_bridges.cpp) so this file's walks and the ROM's own
// slot-indexed readers (func_0203d950, func_0203dabc, GetAngleToCamera) have
// records to land on; the seam's open face zeroing records 4..15 at
// session-arm (hal/comms_seam.cpp) the way src/func_0203db64.c:64 zeroes
// 0..3; and the width getter (comms_loopback.cpp). The transport side --
// sixteen blocks on wire v3, func_0204068c bounded at kCommsMaxPlayers --
// has been wide since the vs16 mainline.

#include <cstdio>

#include "comms_loopback.h"
#include "vs_width.h"

namespace {

typedef signed char s8;
typedef unsigned char u8;
typedef signed short s16;
typedef unsigned short u16;
typedef signed int s32;
typedef unsigned int u32;

// The record run's width here, the array's width in camera_bridges, and the
// transport's slot cap are one fact; the static_assert in vs_width.h ties the
// transport's copy to this one.
enum { kRecs = kPortMaxPlayers };

}  // namespace

extern "C" {

// The record, exactly as src/func_0203ea5c.c lays it out over the 0x24 run.
typedef struct {
    s32 unk0;
    u8 unk4;
    u8 unk5;
    u8 unk6;
    u8 unk7;
    u8 unk8;
    u8 unk9;
    u8 unkA;
    u8 unkB;
    u16 unkC;
    u16 unkE;
    u16 unk10;
    u8 unk12[0x10];
    u8 unk22;
    u8 unk23;
} Rec;

static_assert(sizeof(Rec) == 0x24, "the record is the ROM's 0x24 stride");

// The callees, declared as src/func_0203ea5c.c declares them (its declaration
// fixes D1 and D3 included), so the wide copy and the narrow TU present the
// same shapes to the same link.
extern void func_020408b0(s32 a);
extern void CpuCopy8(void *dst, void *src, s32 len);
extern void func_0205a588(void *dst, s32 val, s32 len);
extern s32 func_02040714(void);
extern void func_02040820(void);
extern void func_02040790(void);
extern void func_02040c34(s32 a, s32 b, void *c, void *d, s32 e);
extern s32 func_020406b4(void *a, void *b);
extern u8 func_02040704(s32 a);
extern void *func_0204068c(u16 a);
extern s32 func_0204271c(void *o);
extern s32 func_02042748(void *a, void *b);
extern s32 func_02040a84(void);
extern void func_02040a5c(u32 val);
extern s32 func_0203fd64(void);
extern void func_02042778(void);
extern void *func_0203f604(int unused, unsigned int size, void *ptr);
extern void func_0203f644(void);

extern u8 data_02099e1c;
extern u8 data_02099e18;
extern u8 data_020a0ef0;
extern u16 data_020a0f1c;
extern u16 data_020a0f28;
extern u8 data_020a0ef8;
extern u16 data_020a1020;
extern u16 data_020a104c;
extern u8 data_020a1022;
extern u8 data_020a1023[0x16];
extern u16 data_020a1026;
extern u8 data_020a1028;
extern u8 data_020a1029;
extern u8 data_020a102a;
extern u16 data_020a102b;
extern u8 data_020a102d;
extern u8 data_020a102e;
extern u8 data_020a102f[0x11];
extern Rec data_020a1040;
extern u8 data_020a1044;
extern u8 data_020a1046;
extern u8 data_020a1048;
extern u16 data_020a104e;
extern u16 data_020a1050;
extern u8 data_020a0f08;
extern u8 data_020a0f00;
extern u8 data_020a1052[0x11];
extern u8 data_020a10a4[0x16];
extern u8 data_020a10ba[0x16];
extern u8 data_020a10d0[0x16];
extern u8 data_020a10e6[0x16];
extern u8 data_020a1112[0x16];
extern u8 data_020a0f04;
extern u16 data_020a1160;
extern Rec data_020a1154[kRecs];
extern Rec data_020a1178;
extern u8 data_020a0f10;
extern u8 data_020a0fa6[24];
extern u8 data_020a0fbe[24];
extern s16 data_020a0f2c;
extern u16 data_020a0f30;
extern u8 data_020a0efc;

// The narrow arm: src/func_0203ea5c.c itself, under the build's rename.
extern void func_0203ea5c_narrow(void);

}  // extern "C"

namespace {

// The info path cannot arm here (see the header); if it ever does, one line,
// once, instead of a silent four-of-sixteen exchange.
void warn_info_mode_armed_wide(void)
{
    static bool said = false;
    if (said) return;
    said = true;
    std::fprintf(stderr,
        "[comms:conductor] WIDE SESSION ENTERED THE DS INFO-EXCHANGE PATH. "
        "Its buffers are the cartridge's four-slot bands, so only slots 0..3 "
        "exchange lobby info; gameplay lockstep is unaffected. This path was "
        "believed unreachable in the port's flow -- see "
        "hal/comms_conductor_wide.cpp and say how this happened in the vs16 "
        "status file.\n");
}

// ---------------------------------------------------------------------------
// The wide copy of src/func_0203ea5c.c. Diff it against that file: the delta
// is `4` -> `kRecs` on the seven record walks, and the warn call above at the
// two info-path heads. Nothing else.
// ---------------------------------------------------------------------------
void conductor_wide(void)
{
    s32 sp4;
    s32 sp8;
    s32 sp28;
    u8 *var_sb;
    s32 sp14;
    s32 sp18;
    s32 spC;
    u8 *var_r8;
    s32 temp_r0_4;
    s32 sp1c;
    s32 temp_r0_7;
    u16 sp2e;
    Rec *var_r3_3;
    s32 sp10;
    Rec *var_r6_4;
    u8 *var_r7_2;
    s32 sp20;
    Rec *var_r1;
    s32 temp_r0;
    u16 temp_r1;
    u32 temp_r0_2;
    s32 temp_r0_8;
    s32 var_r0;
    s32 var_r6_2;
    Rec *var_r5_4;
    u8 *var_r6_3;
    s32 var_r2_4;
    Rec *var_r4;
    s32 var_r3_2;
    s32 var_r4_2;
    s32 var_r5_2;
    s32 var_r5_3;
    Rec *cur133;
    s32 var_r2_3;
    s32 sp24;
    u16 temp_r1_2;
    u8 temp_r0_9;
    u16 temp_r2;
    s32 var_r7;
    s32 var_r3;
    u8 *temp_r0_6;
    u8 *temp_r5;
    s32 temp_r0_3;
    u8 sp2c;
    s32 var_r2_2;
    Rec *var_r6;
    s32 temp_r0_5;
    Rec *var_r5;
    u32 n75;
    s32 var_r2;
    u8 temp_r1_3;
    u16 temp_r1_4;

    (void)temp_r0_7;

    if (data_02099e1c != 0) {
        func_020408b0(2);
        data_02099e1c = 0;
    }
    sp8 = 0;
    if (data_020a0ef0 != 0) {
        sp4 = 0x12C;
    } else {
        sp4 = 0x4B0;
    }
    data_020a0f1c |= 0x2000;
    data_020a0f1c &= 0xF0FF;
    data_020a0f1c |= (data_020a0f28 << 8) & 0xF00;
    sp18 = 4;
    sp14 = 2;
    sp1c = 0x11;
    sp24 = 0x24;
    sp10 = 3;
    sp28 = 5;
    sp20 = 0x12C;
    while ((sp8 == 0) && (sp4 != 0)) {
        if (data_020a0ef8 != 0) {
            warn_info_mode_armed_wide();
            if (data_020a0f1c & 0x4000) {
                data_020a0ef8 -= 1;
            }
            if (data_020a0ef8 != 0) {
                if ((u32) data_020a0ef8 > 4U) {
                    sp2c = (u8) sp10;
                } else {
                    sp2c = data_020a0ef8 - 1;
                }
                data_020a1040.unkC = (u16) (data_020a1040.unkC | 1);
                CpuCopy8(&data_020a104c, &data_020a1020, sp14);
                CpuCopy8(&sp2c, &data_020a1022, 1);
                CpuCopy8((u8 *) ((sp2c * 0x16) + (s32) &data_020a10a4), &data_020a1023, 0x16);
            } else {
                data_020a1040.unkC = (u16) (data_020a1040.unkC & 0xFFFE);
            }
        }
        if (data_020a0ef8 == 0) {
            CpuCopy8(&data_020a104c, &data_020a1020, sp14);
            CpuCopy8(&data_020a1040, &data_020a1022, sp18);
            CpuCopy8(&data_020a104e, &data_020a1026, sp14);
            CpuCopy8(&data_020a1044, &data_020a1028, 1);
            CpuCopy8(&data_020a1046, &data_020a1029, 1);
            CpuCopy8(&data_020a1048, &data_020a102a, 1);
            CpuCopy8(&data_020a1050, &data_020a102b, sp14);
            CpuCopy8(&data_020a0f08, &data_020a102d, 1);
            CpuCopy8(&data_020a0f00, &data_020a102e, 1);
            CpuCopy8(&data_020a1052, &data_020a102f, sp1c);
        }
        data_020a0f1c &= 0xBFFF;
        temp_r0_2 = func_02040714();
        switch (temp_r0_2) {
        case 0:
            if (data_020a0f04 == 1) {
                func_02040820();
                func_02040c34(1, 1, (void *)&func_0203f644, (void *)&func_0203f604, 0);
            } else if (data_020a0f04 == 2) {
                func_02040790();
                func_02040c34(0, 1, (void *)&func_0203f644, (void *)&func_0203f604, 0);
            }
            sp8 = 0;
            break;
        case 1:
            sp8 = 0;
            break;
        case 2:
            sp8 = 0;
            break;
        case 3:
            temp_r0_3 = func_020406b4(&data_020a1020, &sp2e);
            sp8 = temp_r0_3;
            if (temp_r0_3 != 0) {
                data_020a0f1c |= 0xC000;
            }
            if (data_020a1154[0].unkC & 0x2000) {
                sp8 = 1;
            }
            break;
        case 4:
            if ((data_020a1040.unkC & 0x4000) && (data_020a1154[0].unkC & 0x2000)) {
                sp8 = 1;
            } else {
                temp_r0_4 = func_020406b4(&data_020a1020, &sp2e);
                sp8 = temp_r0_4;
                if (temp_r0_4 != 0) {
                    data_020a0f1c |= 0xC000;
                }
                if (data_020a1040.unkC & 0x4000) {
                    sp8 = 1;
                }
            }
            break;
        default:
            sp8 = 0;
            break;
        }
        temp_r0_5 = data_020a0f1c & 0x4000;
        if (temp_r0_5 != 0) {
            data_020a0f10 = func_02040704(temp_r0_5);
            data_02099e18 = 0;
            sp4 = sp20;
            temp_r0_6 = (u8 *)func_0204068c(0U);
            if (temp_r0_6 != 0) {
                CpuCopy8(temp_r0_6, &data_020a1160, sp14);
                if (data_020a1154[0].unkC & 0x8000) {
                    if (data_020a1154[0].unkC & 1) {
                        warn_info_mode_armed_wide();
                        if (data_020a0f10 != 0) {
                            CpuCopy8(temp_r0_6 + 2, &sp2c, 1);
                            switch (sp2c) {
                            default:
                                break;
                            case 0:
                                data_020a1040.unkC = (u16) (data_020a1040.unkC & 0xFFFE);
                                func_0205a588(&data_020a10a4, 0, 0x16);
                                CpuCopy8(temp_r0_6 + 3, &data_020a10a4, 0x16);
                                break;
                            case 1:
                                func_0205a588(&data_020a10ba, 0, 0x16);
                                CpuCopy8(temp_r0_6 + 3, &data_020a10ba, 0x16);
                                break;
                            case 2:
                                func_0205a588(&data_020a10d0, 0, 0x16);
                                CpuCopy8(temp_r0_6 + 3, &data_020a10d0, 0x16);
                                break;
                            case 3:
                                func_0205a588(&data_020a10e6, 0, 0x16);
                                CpuCopy8(temp_r0_6 + 3, &data_020a10e6, 0x16);
                                break;
                            }
                        }
                        sp8 = 0;
                    } else {
                        var_r6 = data_020a1154;
                        var_r7 = 0;
                        do {
                            temp_r5 = (u8 *)func_0204068c(var_r7);
                            func_0205a588(var_r6, 0, sp24);
                            if (temp_r5 != 0) {
                                CpuCopy8(temp_r5, &var_r6->unkC, sp14);
                                if (var_r6->unkC & 0x8000) {
                                    data_02099e18 += 1;
                                    CpuCopy8(temp_r5 + 2, var_r6, sp18);
                                    CpuCopy8(temp_r5 + 6, &var_r6->unkE, sp14);
                                    CpuCopy8(temp_r5 + 8, &var_r6->unk4, 1);
                                    CpuCopy8(temp_r5 + 9, &var_r6->unk6, 1);
                                    CpuCopy8(temp_r5 + 0xA, &var_r6->unk8, 1);
                                    CpuCopy8(temp_r5 + 0xB, &var_r6->unk10, sp14);
                                    CpuCopy8(temp_r5 + 0xF, var_r6->unk12, sp1c);
                                    if ((data_020a0f10 != 0) && (var_r7 == 0)) {
                                        CpuCopy8(temp_r5 + 0xD, &data_020a0f08, 1);
                                        CpuCopy8(temp_r5 + 0xE, &data_020a0f00, 1);
                                    }
                                }
                            }
                            var_r6 += 1;
                            var_r7 += 1;
                        } while ((s32) var_r7 < kRecs);
                        if (data_020a1154[data_020a0f10].unkC & 0x8000) {
                            if (data_020a0f10 == 0) {
                                if (data_020a0ef8 == 0) {
                                    n75 = data_02099e18;
                                    if (n75 > 1U) {
                                    var_r5 = &data_020a1178;
                                    var_r6_2 = 1;
                                    var_r2 = 1;
loop_75:
                                    temp_r1 = var_r5->unkC;
                                    if ((temp_r1 & 0x8000) && !(temp_r1 & 1)) {
                                        var_r6_2 = 0;
                                    } else {
                                        var_r5 += 1;
                                        var_r2 += 1;
                                        if (var_r2 < kRecs) {
                                            goto loop_75;
                                        }
                                    }
                                    if ((var_r6_2 != 0) && (n75 > 1U)) {
                                        warn_info_mode_armed_wide();
                                        spC = 1;
                                        data_020a0ef8 = (u8) sp28;
                                        var_sb = data_020a0fa6;
                                        var_r8 = data_020a10ba;
                                        do {
                                            if (func_0204271c(var_sb) == 0) {
                                                var_r7_2 = data_020a0fbe;
                                                var_r6_3 = data_020a1112;
                                                var_r5_2 = 1;
                                                do {
                                                    if (func_02042748(var_sb, var_r7_2) != 0) {
                                                        CpuCopy8(var_r6_3, var_r8, 0x16);
                                                    }
                                                    var_r7_2 += 6;
                                                    var_r6_3 += 0x16;
                                                    var_r5_2 += 1;
                                                } while (var_r5_2 < 4);
                                            }
                                            var_sb += 6;
                                            var_r8 += 0x16;
                                            spC = spC + 1;
                                        } while (spC < 4);
                                    }
                                }
                                }
                                if ((u32) data_02099e18 > 1U) {
                                    var_r6_4 = &data_020a1178;
                                    var_r3 = 1;
                                    var_r5_3 = 1;
loop_90:
                                    temp_r2 = var_r6_4->unkC;
                                    if ((temp_r2 & 0x8000) && !(temp_r2 & 0x4000)) {
                                        var_r3 = 0;
                                    } else {
                                        var_r6_4 += 1;
                                        var_r5_3 += 1;
                                        if (var_r5_3 < kRecs) {
                                            goto loop_90;
                                        }
                                    }
                                    if ((var_r3 != 0) && !(data_020a1154[0].unkC & 0x2000) && !(data_020a1040.unkC & 0x2000)) {
                                        if ((u32) data_02099e18 > 2U) {
                                            /* The WM status pair, nested for
                                               the stack ABI exactly as the
                                               narrow TU nests it (its D1
                                               note). */
                                            func_02040a5c(func_02040a84());
                                        }
                                        data_020a0f2c = 0;
                                        do {
                                        } while (func_0203fd64() != 1);
                                        data_020a1040.unkC = (u16) (data_020a1040.unkC | 0x2000);
                                    }
                                    var_r5_4 = &data_020a1178;
                                    var_r2_2 = 0;
                                    var_r3_2 = 1;
loop_103:
                                    temp_r1_2 = var_r5_4->unkC;
                                    if ((temp_r1_2 & 0x8000) && (temp_r1_2 & 0x4000)) {
                                        var_r2_2 = 1;
                                    } else {
                                        var_r5_4 += 1;
                                        var_r3_2 += 1;
                                        if (var_r3_2 < kRecs) {
                                            goto loop_103;
                                        }
                                    }
                                    if ((var_r2_2 == 0) && (data_020a1154[0].unkC & 0x2000) && (data_020a1040.unkC & 0x2000)) {
                                        data_020a0f2c = 1;
                                        do {
                                        } while (func_0203fd64() != 1);
                                        data_020a1040.unkC = (u16) (data_020a1040.unkC & 0xDFFF);
                                    }
                                }
                            }
                            var_r0 = 0;
                            var_r1 = data_020a1154;
                            do {
                                if ((var_r1->unkC & 0x8000) && (var_r1->unk0 != data_020a1154[data_020a0f10].unk0)) {
                                    data_020a1040.unk0 = data_020a1154[0].unk0;
                                    data_020a0f1c |= 2;
                                    if (data_020a0efc == 0) {
                                        sp8 = 0;
                                    }
                                    break;
                                }
                                var_r1 += 1;
                                var_r0 += 1;
                            } while (var_r0 < kRecs);
                        } else {
                            sp8 = 0;
                        }
                    }
                } else {
                    sp8 = 0;
                }
            } else {
                sp8 = 0;
            }
        }
        if (sp4 != 0) {
            sp4 -= 1;
        }
        if (sp8 == 0) {
            func_02042778();
            temp_r0_8 = data_020a0f1c & 0x4000;
            if (temp_r0_8 != 0) {
                func_02042778();
            }
        }
    }
    data_020a0f1c &= 0xDFFF;
    if (sp4 != 0) {
        var_r4 = data_020a1154;
        var_r2_3 = 0;
        cur133 = &data_020a1154[data_020a0f10];
loop_133:
        if (var_r4->unkC & 0x8000) {
            temp_r1_3 = cur133->unk22;
            temp_r0_9 = var_r4->unk22;
            if (temp_r0_9 != temp_r1_3) {
                data_020a0f1c |= 8;
            } else if (temp_r0_9 != temp_r1_3) {
                data_020a0f1c |= 0x10;
            } else {
                goto block_138;
            }
        } else {
block_138:
            var_r2_3 += 1;
            var_r4 += 1;
            if (var_r2_3 < kRecs) {
                goto loop_133;
            }
        }
        if ((data_020a1154[0].unkC & 0x2000) || (data_020a1040.unkC & 0x4000)) {
            data_020a0efc = 3;
        }
        if (data_020a0efc != 0) {
            data_020a0f1c &= 0xFF60;
            data_020a1040.unk22 = 0;
            data_020a1040.unk22 = 0;
            data_020a0f30 = 0;
            data_020a1040.unkC = (u16) (data_020a1040.unkC & 0xEFFF);
            data_020a0efc -= 1;
        }
        if (data_020a0efc != 0) {
            return;
        }
        var_r3_3 = data_020a1154;
        var_r4_2 = 1;
        var_r2_4 = 0;
loop_147:
        temp_r1_4 = var_r3_3->unkC;
        if ((temp_r1_4 & 0x8000) && !(temp_r1_4 & 0x1000)) {
            var_r4_2 = 0;
        } else {
            var_r2_4 += 1;
            var_r3_3 += 1;
            if (var_r2_4 < kRecs) {
                goto loop_147;
            }
        }
        temp_r0 = data_020a1040.unkC & 0x1000;
        if (temp_r0 == 0) {
            return;
        }
        if (var_r4_2 == 0) {
            return;
        }
        data_020a1040.unkC = (u16) (data_020a1040.unkC & 0xEFFF);
        data_020a0f30 = 1;
        return;
    }
    data_020a0f04 = 0;
    data_020a0f1c |= 1;
}

}  // namespace

// ---------------------------------------------------------------------------
// THE DISPATCH. One question, asked of the same number that picked the wire.
// ---------------------------------------------------------------------------
extern "C" void func_0203ea5c(void)
{
    if (port::comms_session_players() <= kPortNarrowPlayers) {
        func_0203ea5c_narrow();
        return;
    }
    static bool said = false;
    if (!said) {
        said = true;
        std::fprintf(stderr,
            "[comms:conductor] WIDE CONDUCTOR: src/func_0203ea5c.c is hosted "
            "for this %d-player session; sessions of four or fewer run the "
            "ROM TU itself\n", port::comms_session_players());
    }
    conductor_wide();
}
