/* SHA-1 block transform (RFC 3174 SHA1ProcessMessageBlock), plus the
   SDK's overlay-table mode: while DGT_SetOverlayTableMode's flag
   (data_020a80c4) is set, words 6 and 14 of the block (bytes 0x18 and 0x38)
   are zeroed for the transform and put back after it. Those are the file_id
   fields of the two 0x20-byte overlay-table entries a 64-byte block holds
   (FSOverlayInfoHeader, nitro/fs/overlay.h), so an overlay table's digest
   does not depend on where its files sit in the ROM.

   Matching notes:
   - RFC 3174 declaration order is load-bearing: K, t, temp, W, A..E.
     `temp` lives at function scope, not inside the round loops, and the
     circular shift is the inline macro (no named rotate local). Block-scoped
     locals form their own late rank class and rotate the whole callee-saved
     assignment; hoisting them to the top is what lands the ROM coloring.
   - `save[2]` must NOT be volatile. Volatile stores are high-priority for the
     list scheduler and sink to the block front, which hoists the shared
     `mov r0, #0` above the two save loads.
   - The round-1 K read goes through a volatile pointer. Register pressure in
     the round-1 body is what keeps K[0] un-hoisted in the ROM; rounds 2..4
     hoist their K into the preheader on their own. */

#include "SHA1Context.h"
#include "ds_width.h"

typedef struct
{
    unsigned int w[4];
} T4;

extern int data_020a80c4;
extern unsigned int data_0209a054[4];

#define SHA1CircularShift(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))

void func_0205e3d4(void *ctx)
{
    SHA1Context *context = (SHA1Context *)ctx;
    unsigned int K[4];
    int t;
    unsigned int temp;
    unsigned int W[80];
    unsigned int A, B, C, D, E;
    unsigned int save[2];
    unsigned char *blk;

    *(T4 *)K = *(T4 *)data_0209a054;
    blk = context->Message_Block;
    if (data_020a80c4 != 0)
    {
        save[0] = ((unsigned int *)blk)[6];
        save[1] = ((unsigned int *)blk)[14];
        ((unsigned int *)blk)[6] = 0;
        ((unsigned int *)blk)[14] = 0;
    }

    for (t = 0; t < 16; t++)
    {
        W[t] = (unsigned int)context->Message_Block[t * 4] << 24;
        W[t] |= (unsigned int)context->Message_Block[t * 4 + 1] << 16;
        W[t] |= (unsigned int)context->Message_Block[t * 4 + 2] << 8;
        W[t] |= (unsigned int)context->Message_Block[t * 4 + 3];
    }

    for (t = 16; t < 80; t++)
    {
        W[t] = SHA1CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
    }

    A = context->Intermediate_Hash[0];
    B = context->Intermediate_Hash[1];
    C = context->Intermediate_Hash[2];
    D = context->Intermediate_Hash[3];
    E = context->Intermediate_Hash[4];

    for (t = 0; t < 20; t++)
    {
        temp = SHA1CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] +
               ((volatile unsigned int *)K)[0];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 20; t < 40; t++)
    {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[1];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 40; t < 60; t++)
    {
        temp = SHA1CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 60; t < 80; t++)
    {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K[3];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    context->Intermediate_Hash[0] += A;
    *(unsigned int *)((DS_IADDR(&context->Intermediate_Hash[1])) & ~0ULL) += B;
    *(unsigned int *)((DS_IADDR(&context->Intermediate_Hash[2])) & ~0ULL) += C;
    *(unsigned int *)((DS_IADDR(&context->Intermediate_Hash[3])) & ~0ULL) += D;
    *(unsigned int *)((DS_IADDR(&context->Intermediate_Hash[4])) & ~0ULL) += E;
    context->Message_Block_Index = 0;

    if (data_020a80c4 != 0)
    {
        ((unsigned int *)blk)[6] = save[0];
        ((unsigned int *)blk)[14] = save[1];
    }
}
