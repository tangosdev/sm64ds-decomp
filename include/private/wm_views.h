#ifndef PRIVATE_WM_VIEWS_H
#define PRIVATE_WM_VIEWS_H

#include "WmWork.h"

/* NitroSDK WMArm9Buf / WMStatus, the part of them the WM_* wrappers read
 * through WM_GetSystemWork(). One definition for every wrapper in src/ that
 * used to carry its own padded copy. Each offset below is one that some
 * wrapper reads at that width; everything else stays padding.
 *
 * Offsets are this ROM's SDK revision, not the public NitroSDK header's:
 * linkLevel and child_bitmap sit 4 bytes lower than in pret's pokediamond.
 * Plain C types: some consumers do not include types.h. */
typedef struct WMStatus {
    unsigned short state; /* 0x000 -- 7/8 idle, 9 MP_PARENT, 0xa MP_CHILD, 0xb DCF_CHILD */
    char pad_002[0x10 - 0x2];
    int mp_flag; /* 0x010 -- 1 once MP has started */
    char pad_014[0x46 - 0x14];
    unsigned short
        mp_recvBufSize; /* 0x046 -- size WmReceiveFifo invalidates at a callback's recvBuf */
    char pad_048[0x86 - 0x48];
    unsigned short mp_readyBitmap; /* 0x086 -- children the parent's data sets wait for */
    char pad_088[0xb8 - 0x88];
    unsigned short linkLevel; /* 0x0b8 -- WMLinkLevel, 0 (weakest) to 3 */
    char pad_0ba[0xe4 - 0xba];
    WmParentParam pparam; /* 0x0e4 -- maxEntry (+0xe) read by WM_GetMPReceiveBufferSize */
    char pad_124[0x17e - 0x124];
    unsigned short child_bitmap; /* 0x17e -- connected children, by AID bit */
    char pad_180[0x184 - 0x180];
    unsigned short aid; /* 0x184 -- 0 for the parent */
    char pad_186[0x18e - 0x186];
    unsigned short mp_maxSendSize; /* 0x18e */
    unsigned short mp_maxRecvSize; /* 0x190 */
    char pad_192[0x194 - 0x192];
} WMStatus;

typedef char WMStatus_size_must_be_0x194[sizeof(WMStatus) == 0x194 ? 1 : -1];

/* WM_Init carves the 0x1300-byte WM system buffer data_020a89ac into ARM7's
 * work area (WM7), the status block, the two FIFO blocks and these callback
 * tables. callbackTable has one slot per WM API id below 0x25; WmReceiveFifo
 * handles ids from 0x25 up (the indication, port and 0x81 events) apart. */
typedef struct WMArm9Buf {
    void *WM7;        /* 0x000 -- buf + 0x600, ARM7's work area */
    WMStatus *status; /* 0x004 */
    char pad_08[0xc - 0x8];
    void *fifo9to7;                 /* 0x00c -- command block sent to ARM7, 0x100 bytes */
    void *fifo7to9;                 /* 0x010 -- callback block ARM7 answers in, 0x100 bytes */
    void *callbackTable[0x25];      /* 0x014 -- by API id */
    void *indCallback;              /* 0x0a8 -- the indication callback (op 0x80) */
    void *portCallbackTable[16];    /* 0x0ac */
    void *portCallbackArgument[16]; /* 0x0ec */
    char pad_12c[0x13c - 0x12c];
    int initCleared; /* 0x13c -- WM_Init zeroes it; nothing else in src/ touches it */
    char pad_140[0x560 - 0x140];
    unsigned short dmaNo; /* 0x560 -- the DMA channel WM_Init was given */
} WMArm9Buf;

#endif
