#include "types.h"
#include "private/mb_views.h"
#include "nitro/mi/memory.h"
/* A calibrated touch-panel sample (nitro/spi/tp.h's TPData, as func_0203df40 uses;
   x, y, touch, validity). */
struct Col4 {
    u16 x, y, touch, validity;
};

/* The per-frame input record, named as func_0203df40's view: data_020a1040 is
   the local one, data_020a1154 the four players'. */
typedef struct {
    s32 frame;         /* 0x00 */
    struct Col4 touch; /* 0x04 */
    u16 flags;         /* 0x0c */
    u16 keys;          /* 0x0e */
    s16 inputYaw;      /* 0x10 */
    u8 settings[0x10]; /* 0x12 -- the link settings bytes (GetPlayerFlagByte, func_0203db3c) */
    u8 unk22;
    u8 unk23;
} Rec;

extern void func_020408b0(u16 arg);
extern s32 func_02040714(void);
extern void func_02040820(void);
extern void func_02040790(void);
extern void func_02040c34(s32 a, s32 b, void *c, void *d, void *e);
extern s32 func_020406b4(void *a, void *b);
extern int func_02040704(void);
extern void *func_0204068c(u16 aid);
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
/* data_020a1020 is the packet this console sends each frame, packed from
   the local record and unpacked the same way into data_020a1154 below: +0x0
   flags, +0x2 frame, +0x6 keys, +0x8/+0x9/+0xa touch x/y/touch (one byte
   each), +0xb inputYaw, +0xd data_020a0f08, +0xe data_020a0f00, +0xf the
   0x11 bytes at Rec 0x12. While data_020a0ef8 counts down it carries instead
   a chunk index at +0x2 and one of the 0x16-byte chunks at data_020a10a4 at
   +0x3. */
extern u8 data_020a1020[0x20];
extern Rec data_020a1040;
extern u8 data_020a0f08;
extern u8 data_020a0f00;
extern MBUserInfo data_020a10a4[4];
extern u8 data_020a1112[0x16];
extern u8 data_020a0f04;
extern Rec data_020a1154[4];
extern u8 data_020a0f10;
extern u8 data_020a0fa6[24];
extern u16 data_020a0fbe[4][3];
extern u16 data_020a0f2c;
extern u16 data_020a0f30;
extern u8 data_020a0efc;

/* The link step for modes 1 and 2 (func_0203df40): pack the local input
   record into the send packet (see data_020a1020), hand it to the link, and
   unpack each player's received packet (func_0204068c, by player index) into
   data_020a1154. The loop runs until the link reports the frame synced or
   the timeout (0x12C frames, 0x4B0 the first time) runs out. */
void func_0203ea5c(void) {
    /* Declared in the ROM's register-assignment order: grouping these by
       use moves 42 words. */
    s32 timeout;
    s32 synced;
    u8 *mac;
    s32 slot;
    u8 *user;
    u16 sendStatus;
    Rec *endRec;
    Rec *inRec;
    u16(*peerMac)[3];
    Rec *frameRec;
    s32 frameIdx;
    s32 allReady;
    Rec *anyRec;
    u8 *info;
    s32 endIdx;
    Rec *verRec;
    s32 anyIdx;
    s32 allEnd;
    s32 peer;
    s32 inIdx;
    Rec *own;
    s32 verIdx;
    s32 aid;
    s32 allIn;
    u8 *hostPkt;
    u8 *pkt;
    u8 chunk;
    s32 anyIn;
    Rec *rdyRec;
    u32 peers;
    s32 rdyIdx;

    /* allReady, allIn, anyIn and allEnd stay 0/1 search flags: an inline
       AllPeersHave()/AnyPeerHas() predicate re-colours registers (13-17 insns).
       Their loops walk a Rec pointer beside the index; indexing
       data_020a1154[i] directly changes the loop's codegen. */
    if (data_02099e1c != 0) {
        func_020408b0(2);
        data_02099e1c = 0;
    }
    synced = 0;
    timeout = data_020a0ef0 != 0 ? 0x12C : 0x4B0;
    data_020a0f1c |= 0x2000;
    data_020a0f1c &= 0xF0FF;
    data_020a0f1c |= (data_020a0f28 << 8) & 0xF00;
    while (!synced && timeout != 0) {
        if (data_020a0ef8 != 0) {
            if (data_020a0f1c & 0x4000) {
                data_020a0ef8--;
            }
            if (data_020a0ef8 != 0) {
                if (data_020a0ef8 > 4) {
                    chunk = 3;
                } else {
                    chunk = data_020a0ef8 - 1;
                }
                data_020a1040.flags |= 1;
                MI_CpuCopy8(&data_020a1040.flags, data_020a1020, 2);
                MI_CpuCopy8(&chunk, &data_020a1020[0x2], 1);
                MI_CpuCopy8(&data_020a10a4[chunk], &data_020a1020[0x3], 0x16);
            } else {
                data_020a1040.flags &= 0xFFFE;
            }
        }
        if (data_020a0ef8 == 0) {
            MI_CpuCopy8(&data_020a1040.flags, data_020a1020, 2);
            MI_CpuCopy8(&data_020a1040.frame, &data_020a1020[0x2], 4);
            MI_CpuCopy8(&data_020a1040.keys, &data_020a1020[0x6], 2);
            MI_CpuCopy8(&data_020a1040.touch.x, &data_020a1020[0x8], 1);
            MI_CpuCopy8(&data_020a1040.touch.y, &data_020a1020[0x9], 1);
            MI_CpuCopy8(&data_020a1040.touch.touch, &data_020a1020[0xa], 1);
            MI_CpuCopy8(&data_020a1040.inputYaw, &data_020a1020[0xb], 2);
            MI_CpuCopy8(&data_020a0f08, &data_020a1020[0xd], 1);
            MI_CpuCopy8(&data_020a0f00, &data_020a1020[0xe], 1);
            MI_CpuCopy8(data_020a1040.settings, &data_020a1020[0xf], 0x11);
        }
        data_020a0f1c &= 0xBFFF;
        switch (func_02040714()) {
        case 0:
            if (data_020a0f04 == 1) {
                func_02040820();
                func_02040c34(1, 1, &func_0203f644, &func_0203f604, 0);
            } else if (data_020a0f04 == 2) {
                func_02040790();
                func_02040c34(0, 1, &func_0203f644, &func_0203f604, 0);
            }
            synced = 0;
            break;
        case 1:
            synced = 0;
            break;
        case 2:
            synced = 0;
            break;
        case 3:
            synced = func_020406b4(data_020a1020, &sendStatus);
            if (synced != 0) {
                data_020a0f1c |= 0xC000;
            }
            if (data_020a1154[0].flags & 0x2000) {
                synced = 1;
            }
            break;
        case 4:
            if ((data_020a1040.flags & 0x4000) && (data_020a1154[0].flags & 0x2000)) {
                synced = 1;
            } else {
                synced = func_020406b4(data_020a1020, &sendStatus);
                if (synced != 0) {
                    data_020a0f1c |= 0xC000;
                }
                if (data_020a1040.flags & 0x4000) {
                    synced = 1;
                }
            }
            break;
        default:
            synced = 0;
            break;
        }
        if (data_020a0f1c & 0x4000) {
            data_020a0f10 = func_02040704();
            data_02099e18 = 0;
            timeout = 0x12C;
            hostPkt = func_0204068c(0);
            if (hostPkt != 0) {
                MI_CpuCopy8(hostPkt, &data_020a1154[0].flags, 2);
                if (data_020a1154[0].flags & 0x8000) {
                    if (data_020a1154[0].flags & 1) {
                        if (data_020a0f10 != 0) {
                            MI_CpuCopy8(hostPkt + 2, &chunk, 1);
                            switch (chunk) {
                            default:
                                break;
                            case 0:
                                data_020a1040.flags &= 0xFFFE;
                                MI_CpuClear8(&data_020a10a4[0], 0x16);
                                MI_CpuCopy8(hostPkt + 3, &data_020a10a4[0], 0x16);
                                break;
                            case 1:
                                MI_CpuClear8(&data_020a10a4[1], 0x16);
                                MI_CpuCopy8(hostPkt + 3, &data_020a10a4[1], 0x16);
                                break;
                            case 2:
                                MI_CpuClear8(&data_020a10a4[2], 0x16);
                                MI_CpuCopy8(hostPkt + 3, &data_020a10a4[2], 0x16);
                                break;
                            case 3:
                                MI_CpuClear8(&data_020a10a4[3], 0x16);
                                MI_CpuCopy8(hostPkt + 3, &data_020a10a4[3], 0x16);
                                break;
                            }
                        }
                        synced = 0;
                    } else {
                        for (aid = 0; aid < 4; aid++) {
                            pkt = func_0204068c(aid);
                            MI_CpuClear8(&data_020a1154[aid], sizeof(Rec));
                            if (pkt != 0) {
                                MI_CpuCopy8(pkt, &data_020a1154[aid].flags, 2);
                                if (data_020a1154[aid].flags & 0x8000) {
                                    data_02099e18++;
                                    MI_CpuCopy8(pkt + 2, &data_020a1154[aid].frame, 4);
                                    MI_CpuCopy8(pkt + 6, &data_020a1154[aid].keys, 2);
                                    MI_CpuCopy8(pkt + 8, &data_020a1154[aid].touch.x, 1);
                                    MI_CpuCopy8(pkt + 9, &data_020a1154[aid].touch.y, 1);
                                    MI_CpuCopy8(pkt + 0xA, &data_020a1154[aid].touch.touch, 1);
                                    MI_CpuCopy8(pkt + 0xB, &data_020a1154[aid].inputYaw, 2);
                                    MI_CpuCopy8(pkt + 0xF, data_020a1154[aid].settings, 0x11);
                                    if ((data_020a0f10 != 0) && (aid == 0)) {
                                        MI_CpuCopy8(pkt + 0xD, &data_020a0f08, 1);
                                        MI_CpuCopy8(pkt + 0xE, &data_020a0f00, 1);
                                    }
                                }
                            }
                        }
                        if (data_020a1154[data_020a0f10].flags & 0x8000) {
                            if (data_020a0f10 == 0) {
                                if (data_020a0ef8 == 0) {
                                    peers = data_02099e18;
                                    if (peers > 1) {
                                        allReady = 1;
                                        for (rdyRec = &data_020a1154[1], rdyIdx = 1; rdyIdx < 4;
                                             rdyRec++, rdyIdx++) {
                                            if ((rdyRec->flags & 0x8000) && !(rdyRec->flags & 1)) {
                                                allReady = 0;
                                                break;
                                            }
                                        }
                                        if ((allReady != 0) && (peers > 1)) {
                                            data_020a0ef8 = 5;
                                            for (slot = 1, mac = data_020a0fa6,
                                                user = (u8 *)&data_020a10a4[1];
                                                 slot < 4; mac += 6, user += 0x16, slot++) {
                                                if (func_0204271c(mac) == 0) {
                                                    for (peerMac = data_020a0fbe,
                                                        info = data_020a1112, peer = 1;
                                                         peer < 4;
                                                         peerMac++, info += 0x16, peer++) {
                                                        if (func_02042748(mac, peerMac) != 0) {
                                                            MI_CpuCopy8(info, user, 0x16);
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                                if (data_02099e18 > 1) {
                                    allIn = 1;
                                    for (inRec = &data_020a1154[1], inIdx = 1; inIdx < 4;
                                         inRec++, inIdx++) {
                                        if ((inRec->flags & 0x8000) && !(inRec->flags & 0x4000)) {
                                            allIn = 0;
                                            break;
                                        }
                                    }
                                    if ((allIn != 0) && !(data_020a1154[0].flags & 0x2000) &&
                                        !(data_020a1040.flags & 0x2000)) {
                                        if (data_02099e18 > 2) {
                                            func_02040a5c(func_02040a84());
                                        }
                                        data_020a0f2c = 0;
                                        while (func_0203fd64() != 1) {
                                        }
                                        data_020a1040.flags |= 0x2000;
                                    }
                                    anyIn = 0;
                                    for (anyRec = &data_020a1154[1], anyIdx = 1; anyIdx < 4;
                                         anyRec++, anyIdx++) {
                                        if ((anyRec->flags & 0x8000) && (anyRec->flags & 0x4000)) {
                                            anyIn = 1;
                                            break;
                                        }
                                    }
                                    if ((anyIn == 0) && (data_020a1154[0].flags & 0x2000) &&
                                        (data_020a1040.flags & 0x2000)) {
                                        data_020a0f2c = 1;
                                        while (func_0203fd64() != 1) {
                                        }
                                        data_020a1040.flags &= 0xDFFF;
                                    }
                                }
                            }
                            for (frameRec = data_020a1154, frameIdx = 0; frameIdx < 4;
                                 frameRec++, frameIdx++) {
                                if ((frameRec->flags & 0x8000) &&
                                    (frameRec->frame != data_020a1154[data_020a0f10].frame)) {
                                    data_020a1040.frame = data_020a1154[0].frame;
                                    data_020a0f1c |= 2;
                                    if (data_020a0efc == 0) {
                                        synced = 0;
                                    }
                                    break;
                                }
                            }
                        } else {
                            synced = 0;
                        }
                    }
                } else {
                    synced = 0;
                }
            } else {
                synced = 0;
            }
        }
        if (timeout != 0) {
            timeout--;
        }
        if (synced == 0) {
            func_02042778();
            if (data_020a0f1c & 0x4000) {
                func_02042778();
            }
        }
    }
    data_020a0f1c &= 0xDFFF;
    if (timeout != 0) {
        own = &data_020a1154[data_020a0f10];
        for (verIdx = 0, verRec = data_020a1154; verIdx < 4; verIdx++, verRec++) {
            if (verRec->flags & 0x8000) {
                if (verRec->unk22 != own->unk22) {
                    data_020a0f1c |= 8;
                    break;
                } else if (verRec->unk22 != own->unk22) {
                    /* Unreachable: the cartridge tests the same byte twice
                       (unk23 here moves the code). */
                    data_020a0f1c |= 0x10;
                    break;
                }
            }
        }
        if ((data_020a1154[0].flags & 0x2000) || (data_020a1040.flags & 0x4000)) {
            data_020a0efc = 3;
        }
        if (data_020a0efc != 0) {
            data_020a0f1c &= 0xFF60;
            /* The cartridge stores unk22 twice and never clears unk23
               (unk23 in the second store moves 22 words). */
            data_020a1040.unk22 = 0;
            data_020a1040.unk22 = 0;
            data_020a0f30 = 0;
            data_020a1040.flags &= 0xEFFF;
            data_020a0efc--;
        }
        if (data_020a0efc != 0) {
            return;
        }
        allEnd = 1;
        for (endIdx = 0, endRec = data_020a1154; endIdx < 4; endIdx++, endRec++) {
            if ((endRec->flags & 0x8000) && !(endRec->flags & 0x1000)) {
                allEnd = 0;
                break;
            }
        }
        if (!(data_020a1040.flags & 0x1000)) {
            return;
        }
        if (allEnd == 0) {
            return;
        }
        data_020a1040.flags &= 0xEFFF;
        data_020a0f30 = 1;
        return;
    }
    data_020a0f04 = 0;
    data_020a0f1c |= 1;
}
