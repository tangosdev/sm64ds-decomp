#include "types.h"

/* RFC 3174 SHA1Reset: 1 (shaNull) without a context, otherwise zero the
 * length and block index, load the initial hash H0..H4, clear Computed and
 * Corrupted and return 0 (shaSuccess). The context is RFC 3174's
 * SHA1Context, as func_0205e3d4.c spells it, with the 32-bit block index this
 * build stores; func_0205e0f4 (the HMAC) holds it in 0x68 bytes. */
typedef struct {
    u32 Intermediate_Hash[5]; /* 0x00 */
    u32 Length_Low;           /* 0x14 */
    u32 Length_High;          /* 0x18 */
    u32 Message_Block_Index;  /* 0x1c */
    u8 Message_Block[64];     /* 0x20 */
    int Computed;             /* 0x60 -- func_0205e6e4 refuses more input once set */
    int Corrupted;            /* 0x64 -- func_0205e6e4's error code, 0 while good */
} SHA1Context;

int func_0205e89c(u32 *ctx)
{
    SHA1Context *context = (SHA1Context *)ctx;
    if (ctx == 0) return 1;
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
