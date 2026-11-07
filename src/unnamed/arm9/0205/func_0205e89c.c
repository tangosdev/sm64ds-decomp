#include "types.h"
#include "SHA1Context.h"

/* RFC 3174 SHA1Reset: 1 (shaNull) without a context, otherwise zero the
 * length and block index, load the initial hash H0..H4, clear Computed and
 * Corrupted and return 0 (shaSuccess). The context is RFC 3174's
 * SHA1Context (SHA1Context.h). */
int func_0205e89c(SHA1Context *context) {
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
