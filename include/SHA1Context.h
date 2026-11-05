#ifndef SHA1_CONTEXT_H
#define SHA1_CONTEXT_H

#include "types.h"

/* RFC 3174 SHA1Context, with the 32-bit block index this build stores. The
 * RFC's routines are func_0205e89c (SHA1Reset), func_0205e6e4 (SHA1Input)
 * and func_0205e3d4 (SHA1ProcessMessageBlock); DGT_Hash2CalcHmac (the HMAC)
 * holds one in 0x68 bytes. */
typedef struct SHA1Context {
    u32 Intermediate_Hash[5]; /* 0x00 */
    u32 Length_Low;           /* 0x14 */
    u32 Length_High;          /* 0x18 */
    u32 Message_Block_Index;  /* 0x1c */
    u8 Message_Block[64];     /* 0x20 */
    int Computed;             /* 0x60 -- SHA1Input refuses more input once set */
    int Corrupted;            /* 0x64 -- SHA1Input's error code, 0 while good */
} SHA1Context;

#endif
