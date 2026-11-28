/* NitroSDK dgt hash2.c as linked by this older SDK: the RFC 3174 SHA-1
 * implementation, the overlay-table digest mode, and the Hash2 HMAC helper.
 * mwccarm emits the bodies in reverse source order. */
#include "types.h"
#include "SHA1Context.h"

/* NitroSDK DGT_SetOverlayTableMode: set the flag that makes the SHA-1 block
 * function skip each overlay-table entry's file_id (SHA1ProcessMessageBlock), and
 * return the previous setting. FS_AttachOverlayTable turns it on around the
 * overlay table's digest check. */
extern int data_020a80c4;

int DGT_SetOverlayTableMode(int flag) {
    int prev = data_020a80c4;
    data_020a80c4 = flag;
    return prev;
}

/* RFC 3174 SHA1Reset: 1 (shaNull) without a context, otherwise zero the
 * length and block index, load the initial hash H0..H4, clear Computed and
 * Corrupted and return 0 (shaSuccess). The context is RFC 3174's
 * SHA1Context (SHA1Context.h). */
int SHA1Reset(SHA1Context *context) {
    if (context == 0)
        return 1;
    context->Length_Low = 0;
    context->Length_High = 0;
    context->Message_Block_Index = 0;
    context->Intermediate_Hash[0] = 0x67452301;
    context->Intermediate_Hash[1] = 0xefcdab89;
    context->Intermediate_Hash[2] = 0x98badcfe;
    context->Intermediate_Hash[3] = 0x10325476;
    context->Intermediate_Hash[4] = 0xc3d2e1f0;
    context->Computed = 0;
    context->Corrupted = 0;
    return 0;
}

extern void SHA1PadMessage(SHA1Context *context);

/* RFC 3174 SHA1Result: 1 (shaNull) without a context or digest, or the
 * stored error. The first call pads the message (SHA1PadMessage), clears
 * the block and the length and marks the context Computed; every call then
 * writes the 20-byte digest big-endian. */
int SHA1Result(SHA1Context *context, unsigned char *out) {
    int i;
    unsigned int r;
    if (context == 0 || out == 0)
        return 1;
    r = context->Corrupted;
    if (r != 0)
        return r;
    if (context->Computed == 0) {
        SHA1PadMessage(context);
        for (i = 0; i < 64; i++)
            context->Message_Block[i] = 0;
        context->Length_Low = 0;
        context->Length_High = 0;
        context->Computed = 1;
    }
    for (i = 0; i < 20; i++) {
        unsigned int idx = i >> 2;
        int shift = (3 - (i & 3)) * 8;
        out[i] = (unsigned char)(context->Intermediate_Hash[idx] >> shift);
    }
    return 0;
}

extern void SHA1ProcessMessageBlock(SHA1Context *context);

/* RFC 3174 SHA1Input(context, message_array, length): 0 (shaSuccess),
 * 1 (shaNull) without a context or message, 3 (shaStateError) after the
 * result was computed, or the stored error. */
int SHA1Input(SHA1Context *context, const u8 *p, int count) {
    if (count == 0)
        return 0;
    if (context == 0 || p == 0)
        return 1;
    if (context->Computed != 0) {
        context->Corrupted = 3;
        return 3;
    }
    if (context->Corrupted != 0)
        return context->Corrupted;
    while (count-- != 0 && context->Corrupted == 0) {
        context->Message_Block[context->Message_Block_Index++] = *p;
        context->Length_Low += 8;
        if (context->Length_Low == 0) {
            context->Length_High += 1;
            if (context->Length_High == 0)
                context->Corrupted = 1;
        }
        if (context->Message_Block_Index == 64)
            SHA1ProcessMessageBlock(context);
        p++;
    }
    return 0;
}

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
     `mov r0, #0` above the two save loads. */

typedef struct {
    unsigned int w[4];
} T4;

extern int data_020a80c4;
extern T4 data_0209a054;

#define SHA1CircularShift(bits, word) (((word) << (bits)) | ((word) >> (32 - (bits))))

void SHA1ProcessMessageBlock(SHA1Context *context) {
    T4 K;
    int t;
    unsigned int temp;
    unsigned int W[80];
    unsigned int A, B, C, D, E;
    unsigned int save[2];
    unsigned char *blk;

    K = data_0209a054;
    blk = context->Message_Block;
    if (data_020a80c4 != 0) {
        save[0] = ((unsigned int *)blk)[6];
        save[1] = ((unsigned int *)blk)[14];
        ((unsigned int *)blk)[6] = 0;
        ((unsigned int *)blk)[14] = 0;
    }

    for (t = 0; t < 16; t++) {
        W[t] = (unsigned int)context->Message_Block[t * 4] << 24;
        W[t] |= (unsigned int)context->Message_Block[t * 4 + 1] << 16;
        W[t] |= (unsigned int)context->Message_Block[t * 4 + 2] << 8;
        W[t] |= (unsigned int)context->Message_Block[t * 4 + 3];
    }

    for (t = 16; t < 80; t++) {
        W[t] = SHA1CircularShift(1, W[t - 3] ^ W[t - 8] ^ W[t - 14] ^ W[t - 16]);
    }

    A = context->Intermediate_Hash[0];
    B = context->Intermediate_Hash[1];
    C = context->Intermediate_Hash[2];
    D = context->Intermediate_Hash[3];
    E = context->Intermediate_Hash[4];

    for (t = 0; t < 20; t++) {
        temp = SHA1CircularShift(5, A) + ((B & C) | ((~B) & D)) + E + W[t] + K.w[0];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 20; t < 40; t++) {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K.w[1];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 40; t < 60; t++) {
        temp = SHA1CircularShift(5, A) + ((B & C) | (B & D) | (C & D)) + E + W[t] + K.w[2];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    for (t = 60; t < 80; t++) {
        temp = SHA1CircularShift(5, A) + (B ^ C ^ D) + E + W[t] + K.w[3];
        E = D;
        D = C;
        C = SHA1CircularShift(30, B);
        B = A;
        A = temp;
    }

    context->Intermediate_Hash[0] += A;
    context->Intermediate_Hash[1] += B;
    context->Intermediate_Hash[2] += C;
    context->Intermediate_Hash[3] += D;
    context->Intermediate_Hash[4] += E;
    context->Message_Block_Index = 0;

    if (data_020a80c4 != 0) {
        ((unsigned int *)blk)[6] = save[0];
        ((unsigned int *)blk)[14] = save[1];
    }
}

extern void SHA1ProcessMessageBlock(SHA1Context *context);

/* RFC 3174 SHA1PadMessage: append the 0x80 terminator and zero-fill the
 * block, processing it (SHA1ProcessMessageBlock) first if the
 * 64-bit length no longer fits, then store the length big-endian in the last
 * eight bytes and process the final block. */
void SHA1PadMessage(SHA1Context *context) {
    if (context->Message_Block_Index > 55) {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while (context->Message_Block_Index < 64) {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
        SHA1ProcessMessageBlock(context);
        while (context->Message_Block_Index < 56) {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    } else {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while (context->Message_Block_Index < 56) {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
    }

    context->Message_Block[56] = context->Length_High >> 24;
    context->Message_Block[57] = context->Length_High >> 16;
    context->Message_Block[58] = context->Length_High >> 8;
    context->Message_Block[59] = context->Length_High;
    context->Message_Block[60] = context->Length_Low >> 24;
    context->Message_Block[61] = context->Length_Low >> 16;
    context->Message_Block[62] = context->Length_Low >> 8;
    context->Message_Block[63] = context->Length_Low;

    SHA1ProcessMessageBlock(context);
}

/* RFC 3174 SHA1Reset, SHA1Input and SHA1Result. */
extern int SHA1Reset(SHA1Context *context);
extern int SHA1Input(SHA1Context *context, const unsigned char *data, int len);
extern int SHA1Result(SHA1Context *context, unsigned char *out);

void DGT_Hash2CalcHmac(unsigned char *out, unsigned char *msg, int msglen, unsigned char *key,
                       int keylen) {
    unsigned char ipad[64];
    unsigned char opad[64];
    unsigned char digest[20];
    SHA1Context ctx;
    unsigned char *dg = digest;
    int i;

    if (out == 0)
        return;
    if (msg == 0)
        return;
    if (msglen == 0)
        return;
    if (key == 0)
        return;
    if (keylen == 0)
        return;

    if (keylen > 64) {
        SHA1Reset(&ctx);
        SHA1Input(&ctx, key, keylen);
        SHA1Result(&ctx, dg);
        key = dg;
        keylen = 20;
    }

    for (i = 0; i < keylen; i++) {
        ipad[i] = *key ^ 0x36;
        opad[i] = *key ^ 0x5c;
        key++;
    }
    for (; i < 64; i++) {
        ipad[i] = 0x36;
        opad[i] = 0x5c;
    }

    SHA1Reset(&ctx);
    SHA1Input(&ctx, ipad, 64);
    SHA1Input(&ctx, msg, msglen);
    SHA1Result(&ctx, dg);

    SHA1Reset(&ctx);
    SHA1Input(&ctx, opad, 64);
    SHA1Input(&ctx, dg, 20);
    SHA1Result(&ctx, out);
}
