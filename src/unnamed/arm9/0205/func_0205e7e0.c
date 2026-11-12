#include "types.h"
#include "SHA1Context.h"

extern void func_0205e280(SHA1Context *context);

/* RFC 3174 SHA1Result: 1 (shaNull) without a context or digest, or the
 * stored error. The first call pads the message (func_0205e280,
 * SHA1PadMessage), clears the block and the length and marks the context
 * Computed; every call then writes the 20-byte digest big-endian. */
int func_0205e7e0(SHA1Context *context, unsigned char *out) {
    int i;
    unsigned int r;
    if (context == 0 || out == 0)
        return 1;
    r = context->Corrupted;
    if (r != 0) return r;
    if (context->Computed == 0) {
        func_0205e280(context);
        for (i = 0; i < 0x40; i++)
            context->Message_Block[i] = 0;
        context->Length_Low = 0;
        context->Length_High = 0;
        context->Computed = 1;
    }
    for (i = 0; i < 0x14; i++) {
        unsigned int idx = i >> 2;
        int shift = (3 - (i & 3)) * 8;
        out[i] = (unsigned char)(context->Intermediate_Hash[idx] >> shift);
    }
    return 0;
}
