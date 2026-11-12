//cpp
#include "types.h"
#include "private/mb_views.h"
#include "nitro/mi/memory.h"
#include "private/player_input_record.h"
#include "nitro/mb/mb.h"
#include "private/wm_views.h"
#include "private/arm9_a0f90_channel_measure.h"
#include "Memory.h"
#include "private/fs_views.h"
#include "nitro/wm/wm_api.h"
#include "nitro/pad/pad.h"
#include "nitro/spi/tp.h"
#include "nitro/os/thread.h"
#include "nitro/wm/wm.h"

/* The link module: the input record each console samples every frame
   (data_020a1040), the four players' copies of it (data_020a1154), the
   link's status and error bits (data_020a0f1c) and their accessors, and the
   per-frame step for each link mode (func_0203df40: local only, wireless,
   download-play parent). The name is coined from the first address; no
   file name for it survives in the ROM.

   One translation unit, 0x0203d7b8..0x0203f604, 38 functions:
   - Start: cstd::sqrt (0x0203d744..0x0203d7b8) is its own unit with its
     own .init and .ctors, and func_0203d7b8 is the first function after it.
   - Data: the bss run 0x020a0ef0..0x020a15e4 (the status words, the send
     packet data_020a1020, the two input records, the parent parameters
     data_020a1064 and the download-play profiles data_020a10a4/10fc) is
     used by every function here and by the wireless code that follows.
   - Order: mwccarm emits a file's functions in reverse source order, so
     the file runs from the highest address (func_0203ea5c) down.
     func_0203ea5c's call to func_0203da4c is inlined, and only a
     function in the same file can be inlined.
   - End: the wireless layer from func_0203f604 uses the same bss run, and
     its words interleave with the link's, so it most likely continues this
     file. It is left out until its own functions are folded.

   Everything here has C linkage; it is C++ only because func_0203e20c
   calls Memory::Allocate. */

/* opt_dead_assignments is off for the whole file. func_0203e20c keeps a
   counter it never reads (step 3 counts the children that
   MB_CommResponseRequest accepts), and the default optimiser deletes it
   (0x75c against 0x76c). mwccarm generates a C++ file's functions only
   after reading all of it, so the pragma has to be in force at the end of
   the file. Every other function here builds the same either way. */
#pragma opt_dead_assignments off

extern "C" {

extern void func_020408b0(u16 arg);
void func_0203da4c(void);
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
   0x11 bytes at the record's 0x12. While data_020a0ef8 counts down it
   carries instead a chunk index at +0x2 and one of the 0x16-byte chunks at
   data_020a10a4 at +0x3. */
extern u8 data_020a1020[0x20];
extern u8 data_020a0f08;
extern u8 data_020a0f00;
extern MBUserInfo data_020a10a4[4];
extern u8 data_020a1112[0x16];
extern u8 data_020a0f04;
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
    PlayerInputRecord *endRec;
    PlayerInputRecord *inRec;
    u16(*peerMac)[3];
    PlayerInputRecord *frameRec;
    s32 frameIdx;
    s32 allReady;
    PlayerInputRecord *anyRec;
    u8 *info;
    s32 endIdx;
    PlayerInputRecord *verRec;
    s32 anyIdx;
    s32 allEnd;
    s32 peer;
    s32 inIdx;
    PlayerInputRecord *own;
    s32 verIdx;
    s32 aid;
    s32 allIn;
    u8 *hostPkt;
    u8 *pkt;
    u8 chunk;
    s32 anyIn;
    PlayerInputRecord *rdyRec;
    u32 peers;
    s32 rdyIdx;

    /* allReady, allIn, anyIn and allEnd stay 0/1 search flags: an inline
       AllPeersHave()/AnyPeerHas() predicate re-colours registers (13-17 insns).
       Their loops walk a PlayerInputRecord pointer beside the index; indexing
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
            hostPkt = (u8 *)func_0204068c(0);
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
                            pkt = (u8 *)func_0204068c(aid);
                            MI_CpuClear8(&data_020a1154[aid], sizeof(PlayerInputRecord));
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
            func_0203da4c();
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

extern u8 data_020a0fb8[];
extern int data_020a0f70;

/* The parent state callback func_0203e20c installs
   (MB_CommSetParentStateCallback). When child aid (0..3) connects it keeps
   the child's MAC address from the WM record in data_020a0fb8; on
   MB_COMM_PSTATE_END it raises data_020a0f70, which func_0203e20c waits on.
   The other states are ignored. */
void func_0203e9a8(u16 aid, u32 status, void *arg) {
    switch (status) {
    case MB_COMM_PSTATE_NONE:
    case MB_COMM_PSTATE_INIT_COMPLETE:
        break;
    case MB_COMM_PSTATE_CONNECTED:
        if (aid >= 4)
            return;
        if (arg == 0)
            return;
        MI_CpuCopy8(((WmStartParentCallback *)arg)->macAddress, data_020a0fb8 + aid * 6, 6);
        break;
    case MB_COMM_PSTATE_DISCONNECTED:
    case MB_COMM_PSTATE_KICKED:
    case MB_COMM_PSTATE_REQ_ACCEPTED:
    case MB_COMM_PSTATE_SEND_PROCEED:
    case MB_COMM_PSTATE_SEND_COMPLETE:
    case MB_COMM_PSTATE_BOOT_REQUEST:
    case MB_COMM_PSTATE_BOOT_STARTABLE:
    case MB_COMM_PSTATE_REQUESTED:
    case MB_COMM_PSTATE_MEMBER_FULL:
        break;
    case MB_COMM_PSTATE_END:
        data_020a0f70 = 1;
        break;
    case MB_COMM_PSTATE_ERROR:
    case MB_COMM_PSTATE_WAIT_TO_SEND:
        break;
    }
}

extern unsigned char data_020a0f0c;

/* The callback func_0203e20c hands func_0203f9d4: on success, keep the
   channel in data_020a0f28 (as func_020408b0 sets it from the parent's
   WMBssDesc) and raise data_020a0f0c. */
void func_0203e978(ChannelMeasureReply *reply) {
    if (reply->result != 0)
        return;
    data_020a0f28 = reply->channel;
    data_020a0f0c = 1;
}

extern u16 data_020a0f20;
extern void *data_020a0f3c;
extern s32 data_020a0f40;
extern s32 data_020a0f50;
extern s32 data_020a0f6c;
extern s32 data_020a0f78;
extern void *data_020a0f7c;
extern u8 data_020a0f9d[4];
extern MBGameRegistry data_020a0fec;
extern WMParentParam data_020a1064;
extern MBUserInfo data_020a10fc[4];
extern WMStatus data_020a1680;
extern u8 data_020a1780[];

extern void func_0203e978(ChannelMeasureReply *reply);
extern int OS_GetTickLo(void);
extern int MB_Init(void *work, const MBUserInfo *user, int ggid0, int ggid1, u16 tgid, int dma);
extern int MB_SetParentCommParam(int a, int b, int c);
extern void func_0203e9a8(u16 aid, u32 status, void *arg);
extern int MB_StartParent(int param0);
extern u32 MB_GetSegmentLength(FSFile *file);
extern int MB_ReadSegment(FSFile *file, void *buf, u32 len);
extern int MB_RegisterFile(MBGameRegistry *game_reg, const void *buf);
extern void MB_End(void);

#define H(base, off) (*(u16 *)((char *)(base) + (off)))

/* Link mode 3's step (func_0203df40), a download-play parent by its
   shape. Step 0 allocates 0xb000 bytes of work and picks a channel
   (func_0203f9d4, with func_0203e978 keeping it). Step 2 starts the
   parent with ids from data_020a1064 (the block func_0203db64 keeps),
   this console's profile and func_0203e9a8 as the state callback. Step 3
   seats up to three children: a child in state 0xa is answered with 1,
   one past the third with 0, and once data_020a0f70 is up and all are
   counted every child is answered with 2. Step 4 waits until each seated
   child is ready (MB_CommIsBootable), step 5 answers them with 3, and step 6
   ends it. A stop request (data_020a0f40) in step 3, or a child lost in
   steps 4 and 5, jumps to step 6; a lost child also sets link error bit
   4. Every frame the local input record data_020a1040 is copied into all
   four records of data_020a1154, and the channel goes into bits 8-11 of
   data_020a0f1c. */
void func_0203e20c(void) {
    if (data_020a0f40 != 0 && data_020a0f50 == 3) {
        data_020a0f50 = 6;
    }
    if (data_020a0f50 > 3 && data_020a0f50 < 6 &&
        (data_020a0f08 != (u8)(MB_CommGetChildrenNumber() + 1) ||
         (u32)MB_CommGetChildrenNumber() < 1)) {
        data_020a0f40 = 1;
        data_020a0f50 = 6;
        data_020a0f1c |= 4;
    }

    switch (data_020a0f50) {
    case 0: {
        void *ptr = Memory::Allocate(0xB000, 0x20);
        data_020a0f3c = ptr;
        data_020a0f0c = 0;
        func_0203f9d4(ptr, func_0203e978);
        data_020a0f50 += 1;
        data_020a0f1c &= 0xFFFB;
        break;
    }

    case 1:
        if (data_020a0f0c != 0) {
            data_020a0f50 += 1;
        }
        break;

    case 2: {
        void *p;
        int size;
        data_020a0f20 = (u16)OS_GetTickLo();
        MB_Init(data_020a0f3c, data_020a10fc, data_020a1064.ggid[0], data_020a1064.ggid[1],
                data_020a0f20, 1);
        MB_SetParentCommParam(0x1fe, 8, 3);
        MB_CommSetParentStateCallback(func_0203e9a8);
        MB_StartParent(data_020a0f28);
        size = MB_GetSegmentLength(0);
        p = Memory::Allocate(size, 0x20);
        data_020a0f7c = p;
        if (MB_ReadSegment(0, p, size) != 0) {
            MB_RegisterFile(&data_020a0fec, data_020a0f7c);
        }
        data_020a0f70 = 0;
        data_020a0f50 += 1;
        break;
    }

    case 3: {
        int cnt2;
        int i2;
        u8 *p9;
        u8 *p11;
        const MBUserInfo *found;
        u8 *pIdx;
        u8 *base1112;
        int idx;
        MBCommResponseRequestType sel1;
        int oneVal;

        cnt2 = 1;
        i2 = cnt2;
        p9 = data_020a0f9d;
        p11 = data_020a1112;

        data_020a0f1c |= 0x9000;
        data_020a0f08 = cnt2;
        do {
            *p9 = 0;
            MI_CpuClear8(p11, 0x16);
            i2 += 1;
            p9 += 1;
            p11 += 0x16;
        } while (i2 < 4);

        WM_ReadStatus(&data_020a1680);

        pIdx = data_020a0f9d;
        base1112 = data_020a1112;
        idx = 1;
        sel1 = MB_COMM_RESPONSE_REQUEST_ACCEPT;
        oneVal = idx;
        do {
            found = MB_CommGetChildUser(idx);
            if (found != 0 && ((H(data_020a1780, 0x7e) >> idx) & 1)) {
                if (idx < 4) {
                    if (MB_CommGetParentState(idx) == MB_COMM_PSTATE_REQUESTED) {
                        MB_CommResponseRequest(idx, sel1);
                    } else if (MB_CommGetParentState(idx) == MB_COMM_PSTATE_WAIT_TO_SEND) {
                        cnt2 += 1;
                    }
                    *pIdx = oneVal;
                    MI_CpuCopy8(found, base1112, 0x16);
                    data_020a0f08 += 1;
                } else if (MB_CommGetParentState(idx) == MB_COMM_PSTATE_REQUESTED) {
                    MB_CommResponseRequest(idx, MB_COMM_RESPONSE_REQUEST_KICK);
                }
            }
            idx += 1;
            pIdx += 1;
            base1112 += 0x16;
        } while (idx < 0x10);

        if (data_020a0f70 != 0 && data_020a0f08 == cnt2) {
            if ((u32)data_020a0f08 > 1U) {
                u8 j;
                u8 deadcnt;
                MBCommResponseRequestType sel2;
                deadcnt = 0;
                j = 1;
                sel2 = MB_COMM_RESPONSE_REQUEST_DOWNLOAD;
                do {
                    if (MB_CommResponseRequest(j, sel2) == 1) {
                        deadcnt = (u8)(deadcnt + 1);
                    }
                    j = (u8)(j + 1);
                } while ((u32)j <= 0xF);
            }
            data_020a0f50 += 1;
        }
        break;
    }

    case 4: {
        int ok = 1;
        u8 *p = data_020a0f9d;
        int k = 1;
        do {
            if (*p != 0 && MB_CommIsBootable(k) != 1) {
                ok = 0;
            }
            k += 1;
            p += 1;
        } while (k < 4);
        if (ok != 0) {
            data_020a0f50 += 1;
            data_020a0f6c = 0;
            data_020a0f78 = 1;
        }
        break;
    }

    case 5:
        if ((u32)data_020a0f08 > 1U) {
            int result;
            do {
                u8 m;
                u8 cnt;
                cnt = 0;
                m = 1;
                do {
                    if (MB_CommResponseRequest(m, MB_COMM_RESPONSE_REQUEST_BOOT) == 1) {
                        cnt = (u8)(cnt + 1);
                    }
                    m = (u8)(m + 1);
                } while ((u32)m <= 0xF);
                result = (cnt != 0) ? 1 : 0;
            } while (result == 0);
        }
        data_020a0f50 += 1;
        break;

    case 6:
        data_020a0f70 = 0;
        data_020a0f1c &= 0x6FFF;
        MB_End();
        data_020a0f50 += 1;
        break;

    case 7:
        if (data_020a0f70 != 0) {
            data_020a0f50 += 1;
        }
        break;

    default:
        data_020a0f50 = 0;
        data_020a0f04 = 0;
        Memory::Deallocate(data_020a0f3c);
        Memory::Deallocate(data_020a0f7c);
        if (data_020a0f40 != 0) {
            u8 *p9d = data_020a0f9d;
            u8 *p1112 = data_020a1112;
            u16(*pbe)[3] = data_020a0fbe;
            int sl;
            data_020a0f40 = 0;
            data_020a0f08 = 0;
            sl = 1;
            do {
                *p9d = 0;
                MI_CpuClear8(p1112, 0x16);
                MI_CpuClear8(pbe, 6);
                sl += 1;
                p9d += 1;
                p1112 += 0x16;
                pbe += 1;
            } while (sl < 4);
        }
        break;
    }

    /* The same copy as func_0203e0ac, without clearing 0x8000. Here the
       settings bytes (and unk22) go through two walking byte pointers: an
       indexed settings[i] loop colours the registers differently. */
    {
        u8 *src = (u8 *)&data_020a1040;
        u8 *dst = (u8 *)data_020a1154;
        int i;

        data_020a0f1c &= 0xF0FF;
        data_020a0f1c |= (data_020a0f28 << 8) & 0xF00;
        data_020a1154[0].flags = data_020a1154[1].flags = data_020a1154[2].flags =
            data_020a1154[3].flags = data_020a1040.flags;
        data_020a1154[0].keys = data_020a1154[1].keys = data_020a1154[2].keys =
            data_020a1154[3].keys = data_020a1040.keys;
        data_020a1154[0].touch = data_020a1154[1].touch = data_020a1154[2].touch =
            data_020a1154[3].touch = data_020a1040.touch;
        data_020a1154[0].inputYaw = data_020a1154[1].inputYaw = data_020a1154[2].inputYaw =
            data_020a1154[3].inputYaw = data_020a1040.inputYaw;
        for (i = 0; i < 0x11; i++) {
            dst[0x12] = dst[0x36] = dst[0x5a] = dst[0x7e] = src[0x12];
            src++;
            dst++;
        }
    }
}

/* The local-only link step (func_0203df40, mode 0): clear the record's 0x8000
   flag and copy this frame's input record (data_020a1040) into all four
   player records of data_020a1154. Each field is copied with one chained
   assignment, record 3 first: the cartridge reads every record back before
   it stores the next, which is how mwccarm evaluates a = b = c, and takes
   the touch samples of records 3, 2 and 1 from their own addresses (the
   pool's 0x020a11c4, 0x020a11a0 and 0x020a117c). The byte loop runs 0x11
   times, through settings and on into unk22. */
void func_0203e0ac(void) {
    int i;

    data_020a1040.flags &= 0x7fff;
    data_020a1154[0].flags = data_020a1154[1].flags = data_020a1154[2].flags =
        data_020a1154[3].flags = data_020a1040.flags;
    data_020a1154[0].keys = data_020a1154[1].keys = data_020a1154[2].keys = data_020a1154[3].keys =
        data_020a1040.keys;
    data_020a1154[0].touch = data_020a1154[1].touch = data_020a1154[2].touch =
        data_020a1154[3].touch = data_020a1040.touch;
    data_020a1154[0].inputYaw = data_020a1154[1].inputYaw = data_020a1154[2].inputYaw =
        data_020a1154[3].inputYaw = data_020a1040.inputYaw;
    for (i = 0; i < 0x11; i++) {
        data_020a1154[0].settings[i] = data_020a1154[1].settings[i] = data_020a1154[2].settings[i] =
            data_020a1154[3].settings[i] = data_020a1040.settings[i];
    }
}

extern int data_020a0f98;

extern void func_0203b9bc(TPData *out);
extern void func_0203ea5c(void);
extern void func_0203e20c(void);
extern void func_0203e0ac(void);
extern void func_02040724(void);

/* Sample this frame's local input into data_020a1040, the buttons and the
   calibrated touch point, then run the link step for the current mode
   (data_020a0f04: 0 local only, func_0203e0ac; 1 and 2 func_0203ea5c; 3
   func_0203e20c) and count the frame. When the mode has just dropped back to
   0 from 1 or 2, it shuts the link down with func_02040724. */
void func_0203df40(void) {
    TPData sample;
    u8 sel;

    data_020a1040.keys = PAD_Read();

    func_0203b9bc(&sample);

    data_020a1040.touch.x = sample.x;
    data_020a1040.touch.y = sample.y;
    data_020a1040.touch.touch = sample.touch;
    data_020a1040.touch.validity = sample.validity;
    data_020a1040.flags |= 0x8000;

    sel = data_020a0f04;
    switch (sel) {
    case 1:
        func_0203ea5c();
        break;
    case 2:
        func_0203ea5c();
        break;
    case 3:
        func_0203e20c();
        break;
    default:
        data_020a0f04 = 0;
    case 0:
        func_0203e0ac();
        break;
    }

    data_020a1040.frame += 1;

    if (data_020a0f04 == 0) {
        switch (data_020a0f98) {
        default:
        case 0:
        case 3:
            break;
        case 1:
        case 2:
            data_020a0f1c &= 0x7fff;
            func_02040724();
            data_02099e18 = 1;
            data_020a0f10 = 0;
            break;
        }
    }

    data_020a0f98 = data_020a0f04;
}

extern void func_0203df40(void);
extern int func_02040a94(void);
extern void func_0203d854(void);
extern int func_0203d81c(void);
extern void func_0203d918(void);
extern int func_0203d8fc(void);
extern void OS_GetOwnerInfo(void *dst);
extern void OS_GetMacAddress(void *p);

/* A player's 0x16-byte profile as the link exchanges it (func_0203ea5c sends
   them a chunk at a time): the owner's favourite colour and nickname.
   data_020a10fc and data_020a10a4 are each a table of four (0x58 bytes,
   cleared below); func_0203d8a0 hands out player i's from data_020a10fc in
   link mode 3 and from data_020a10a4 otherwise. Off a download-play child
   this fills in entry 0 of both from the owner settings; data_020a10fe and
   data_020a10a6 are those entries' nickName fields. */
extern u16 data_020a0fa0[][3];
extern u8 data_020a10fe[];
extern u8 data_020a10a6[];

/* The owner settings OS_GetOwnerInfo copies out of the firmware's user info at
   0x27ffc80: NitroSDK's OSOwnerInfo, which OS_GetOwnerInfo fills in (the
   language there is the one func_0201fec8 switches on). This is the older
   SDK's 0x50-byte layout: nickName and comment have no terminator slot. */
typedef struct {
    u8 language;        /* 0x00 */
    u8 favoriteColor;   /* 0x01 */
    u8 birthday[2];     /* 0x02 -- month, day */
    u16 nickName[10];   /* 0x04 */
    u16 nickNameLength; /* 0x18 */
    u16 comment[26];    /* 0x1a */
    u16 commentLength;  /* 0x4e */
} OwnerInfo;

/* Start the link (func_0201fec8 passes per-language data): keep the 0x40 and
   0x1c bytes it is given, clear the input records, profiles and link state.
   A download-play child (MB_IsMultiBootChild) takes mode 2 and runs the
   link step (func_0203df40) until each of four handshake waits is met, 0x4b0
   frames for the first and 0x258 for the rest; a wait that runs out flags
   data_020a0f1c bit 0, shuts the link down and parks the thread. Otherwise
   this console fills in its own profile from the owner settings. */
void func_0203db64(void *arg0, void *arg1) {
    OwnerInfo buf;

    data_020a0ef0 = 0;
    MI_CpuCopy8(arg0, &data_020a1064, sizeof(WMParentParam));
    MI_CpuCopy8(arg1, &data_020a0fec, sizeof(MBGameRegistry));
    MI_CpuClear8(&data_020a1040, 0x24);
    MI_CpuClear8(data_020a0fb8, 0x18);
    MI_CpuClear8(data_020a0fa0, 0x18);
    MI_CpuClear8(data_020a10fc, 0x58);
    MI_CpuClear8(data_020a10a4, 0x58);
    MI_CpuClear8(data_020a1154, 0x90);
    data_020a0f08 = 0;
    data_020a0f1c = 0;
    data_020a0f30 = 0;
    data_020a0f28 = 0;
    data_02099e18 = 1;
    data_020a0f10 = 0;
    data_020a0f04 = 0;
    if (MB_IsMultiBootChild()) {
        int count;

        data_020a0f04 = 2;
        count = 0x4b0;
        while (!(func_02040a94() == 2 && data_02099e18 == data_020a0f08 && data_020a0f08 != 0) &&
               count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }

        count = 0x258;
        data_020a1040.flags |= 1;
        while ((data_020a1040.flags & 1) && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }

        func_0203d854();
        count = 0x258;
        while (func_0203d81c() == 0 && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }

        func_0203d918();
        count = 0x258;
        while (func_0203d8fc() == 0 && count != 0) {
            data_020a0f1c |= 0x2000;
            func_02042778();
            func_0203df40();
            count--;
        }
        if (count == 0) {
            data_020a0f1c |= 1;
            func_02040724();
            OS_SleepThread(0);
        }
    } else {
        OS_GetOwnerInfo(&buf);
        MI_CpuClear8(&data_020a10fc[0], 0x16);
        data_020a10fc[0].favoriteColor = buf.favoriteColor;
        data_020a10fc[0].nameLength = buf.nickNameLength;
        data_020a10a4[0].favoriteColor = data_020a10fc[0].favoriteColor;
        data_020a10fc[0].playerNo = 0;
        data_020a10a4[0].playerNo = data_020a10fc[0].playerNo;
        data_020a10a4[0].nameLength = data_020a10fc[0].nameLength;
        MI_CpuCopy8(buf.nickName, data_020a10fe, buf.nickNameLength * 2);
        MI_CpuCopy8(buf.nickName, data_020a10a6, buf.nickNameLength * 2);
        data_020a0f00 = buf.language;
        OS_GetMacAddress(data_020a0fb8);
        OS_GetMacAddress(data_020a0fa0);
    }
    data_020a0ef0 = 1;
}

extern unsigned char data_020a1166[];
/* Byte p of player i's link settings: bytes 0x12.. of the players' 0x24-byte
   input records (data_020a1154, so data_020a1166 is the first one's 0x12).
   func_0203db3c writes the same bytes in this console's own record,
   data_020a1040. The accessors func_02020028..func_02020388 give the bits
   their roles (ready, go, course, slot mask, controller mode). */
unsigned char GetPlayerFlagByte(int i, int p) {
    return *(unsigned char *)((char *)data_020a1166 + i * 0x24 + p);
}

extern unsigned char data_020a1052[];
/* Set link settings byte `byte` of this console, at 0x12 of its own input
   record data_020a1040 (data_020a1052): the writer the 0202 accessors pair
   with GetPlayerFlagByte. */
void func_0203db3c(int byte, int value) {
    data_020a1052[byte] = value;
}

/* data_020a1154 holds the four players' 0x24-byte input records (func_0203df40's
 * Rec). data_020a1164 is the first record's inputYaw field (0x10), so a view
 * one record long, starting there, steps to player i's. */
struct RecInputYaw {
    short inputYaw; /* Rec 0x10 -- the camera's input yaw (func_0203db0c) */
    char rest[0x24 - 2];
};
extern struct RecInputYaw data_020a1164[];

short GetAngleToCamera(int i) {
    return data_020a1164[i].inputYaw;
}

/* Set player idx's input yaw (GetAngleToCamera reads it back). */
void func_0203db0c(int idx, unsigned short val) {
    data_020a1164[idx].inputYaw = val;
}

/* Store the camera's input yaw in the input record (data_020a1040.inputYaw);
   dCamera_c::Behavior passes mAngleY + mInputYawLag. */
void func_0203dafc(int v) {
    data_020a1040.inputYaw = v;
}

/* data_020a1154 holds the four players' 0x24-byte input records (func_0203df40's
 * Rec). data_020a1162 is the first record's keys field (0x0e), so a view one
 * record long, starting there, steps to player idx's. */
struct RecKeys {
    unsigned short keys; /* Rec 0x0e -- the buttons held this frame */
    char rest[0x24 - 2];
};
extern struct RecKeys data_020a1162[];

/* daPly_c idx's held buttons (func_0203bc7c and the DSMT scene read them). */
unsigned short func_0203dae4(int idx) {
    return data_020a1162[idx].keys;
}

/* The input record's frame counter (data_020a1040.frame, counted up by
   func_0203df40). The VS stage seeds from it. */
int func_0203dad4(void) {
    return data_020a1040.frame;
}

/* daPly_c idx's touch sample (x, y, touch, validity at +0x4); func_0203bb60
   and the DSMT scene read it. */
TPData *func_0203dabc(int idx) {
    return &data_020a1154[idx].touch;
}

/* How many players are present: func_0203ea5c counts the records whose
   flag 0x8000 is set; it is 1 for local play (func_0203df40) and while
   func_0203db64 starts the link. */
int func_0203daac(void) {
    return data_02099e18;
}

/* This console's player index on the link, data_020a0f10 (0 for the
   parent): the AID func_0203ea5c takes from func_02040704. */
int func_0203da9c(void) {
    return data_020a0f10;
}

/* The link status bits of data_020a0f1c that mask x selects.
   func_02030e4c and func_020197b8 test the error bits 0x9d with it. */
unsigned short func_0203da80(unsigned short x) {
    return data_020a0f1c & x;
}

/* Clear the link error bits 0x9f of data_020a0f1c and unk22 (byte 0x22) of
   the local input record data_020a1040. func_02030aa4 calls it when a session
   starts, and dMeter_c when it sets up. The cartridge stores unk22 twice.

   func_0203ea5c's call in its error countdown is the only call the cartridge
   inlines anywhere in this file: the small functions func_0203db64 calls
   (func_0203d81c, d854, d8fc, d918) stay calls. Auto-inlining is therefore
   on for this one function, not the file. mwccarm generates a C++ file's
   functions after reading all of it, which is why a function defined after
   its caller can still be inlined into it. */
#pragma auto_inline on
void func_0203da4c(void) {
    data_020a0f1c &= 0xff60;
    data_020a1040.unk22 = 0;
    data_020a1040.unk22 = 0;
}
#pragma auto_inline reset

/* The link mode data_020a0f04 that func_0203da2c sets (0 local only, 1 and 2
   func_0203ea5c, 3 func_0203e20c). */
int func_0203da3c(void) {
    return data_020a0f04;
}

/* Set the link mode data_020a0f04 (func_0203df40 runs the link step for it:
   0 local only, 1 and 2 func_0203ea5c, 3 func_0203e20c). */
void func_0203da2c(int v) {
    data_020a0f04 = v;
}

/* While link bit 0x8000 of data_020a0f1c is set (func_0203e20c sets it
   with 0x1000), WM_GetLinkLevel's value, else 0. func_0203083c indexes its
   link icon table data_020927a0 with it. */
int func_0203d9f4(void) {
    if ((data_020a0f1c & 0x8000) == 0)
        return 0;
    return WM_GetLinkLevel();
}

/* This console's player index on the link (see func_0203d974.cpp). */

/* Nonzero only on the host: not a download-play child
   (MB_IsMultiBootChild) and player 0. The inverse of func_0203d974. */
int func_0203d9b4(void) {
    if (MB_IsMultiBootChild())
        return 0;
    return data_020a0f10 == 0;
}

/* data_020a0f10 is this console's player index on the link: the AID that
   func_0203ea5c takes from func_02040704, 0 for the parent. */

/* Nonzero unless this console is the host: a download-play child
   (MB_IsMultiBootChild) or any player but 0.
   func_0203d9b4 is the same test the other way round. */
int func_0203d974(void) {
    if (MB_IsMultiBootChild())
        return 1;
    return data_020a0f10 != 0;
}

/* data_020a1154 holds the four players' 0x24-byte input records (func_0203df40's
 * Rec). data_020a1160 is the first record's flags field (0x0c), so a view one
 * record long, starting there, steps to player i's. */
struct RecFlags {
    unsigned short flags; /* Rec 0x0c */
    unsigned char pad[0x22];
};
extern struct RecFlags data_020a1160[];

/* Whether player i's record has flag 0x8000 (func_0203df40 sets it in each
 * frame's local record, data_020a1040). */
int func_0203d950(int i) {
    return (data_020a1160[i].flags & 0x8000) != 0;
}

/* Drop the load request, flag 0x4000 of this console's input record
   (func_0203d918), once the course resources are in (dScEntry_c, dScMB_c). */
void func_0203d930(void) {
    data_020a1040.flags &= 0xbfff;
}

/* Raise flag 0x4000 of this console's input record, the load request:
   once every present child has it, player 0 sets 0x2000 in its own record
   (func_0203ea5c), which func_0203d8fc reads. dScEntry_c::EnterMatchLoading
   and func_0203db64 raise it; func_0203d930 drops it. */
void func_0203d918(void) {
    data_020a1040.flags |= 0x4000;
}

/* Flag 0x2000 of player 0's input record (data_020a1154[0].flags): the load
   signal player 0 raises once every present child has posted its load
   request (func_0203d918), and drops when none has. */
int func_0203d8fc(void) {
    return (data_020a1154[0].flags & 0x2000) != 0;
}

/* Raise data_020a0f70, the flag the mode-3 link step (func_0203e20c)
   waits on in steps 3 and 7; func_0203e9a8 raises it on parent state 12.
   dScEntry_c reaches it through the veneer func_02030a6c. */
void func_0203d8e8(void) {
    data_020a0f70 = 1;
}

/* Raise data_020a0f40, which tells the mode-3 link step (func_0203e20c)
   to wind down. func_02030a38 calls it. */
void func_0203d8d4(void) {
    data_020a0f40 = 1;
}

/* daPly_c idx's 0x16-byte profile: from data_020a10fc in link mode 3,
   else from data_020a10a4 (four entries each; see func_0203db64). */
MBUserInfo *func_0203d8a0(int idx) {
    if (data_020a0f04 == 3)
        return &data_020a10fc[idx];
    return &data_020a10a4[idx];
}

/* data_020a0f08, the player count the parent posts: a child copies it from
   byte 0xd of the parent's packet (func_0203ea5c), and func_0203db64 waits
   until the counted players (func_0203daac) reach it. */
int func_0203d890(void) {
    return data_020a0f08;
}

int GetOwnerLanguage(void) {
    return data_020a0f00;
}

/* Raise the link handshake flag, 0x1000 of this console's input record
   (func_020308b4; func_0203d81c polls it). */
void func_0203d868(void) {
    data_020a1040.flags |= 4096;
}

/* Clear the link handshake's passed latch (func_020308b4). */
void func_0203d854(void) {
    data_020a0f30 = 0;
}

/* The link handshake's poll (func_020308a8): keep flag 0x1000 raised in this
   console's input record (data_020a1040.flags) and return 1, clearing the
   latch, once func_0203ea5c has set data_020a0f30 -- which it does when every
   present player's record carries 0x1000. */
int func_0203d81c(void) {
    data_020a1040.flags |= 0x1000;
    if (data_020a0f30 != 0) {
        data_020a0f30 = 0;
        return 1;
    }
    return 0;
}

/* Check a file operation's result: nonzero passes (1). Zero sets link
   error bit 0x80 in data_020a0f1c and returns whether this is a
   download-play child (MB_IsMultiBootChild), which cannot read the
   card again; func_02018e68 then sleeps the thread (see myFS_ReadFile.c). */
int func_0203d7d4(int arg) {
    if (arg != 0)
        return 1;
    data_020a0f1c |= 0x80;
    return MB_IsMultiBootChild() != 0;
}

/* Whether a file read has failed (link error bit 0x80 of data_020a0f1c,
   set by func_0203d7d4). LoadOverlay, UnloadOverlay and func_02017e94
   then leave the overlay's RAM alone, and dScMb_c waits on it. */
int func_0203d7b8(void) {
    return (data_020a0f1c & 128) != 0;
}

} /* extern "C" */
