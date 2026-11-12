#include "types.h"
#include "SHA1Context.h"

extern void func_0205e3d4(SHA1Context *context);

/* RFC 3174 SHA1PadMessage: append the 0x80 terminator and zero-fill the
 * block, processing it (func_0205e3d4, SHA1ProcessMessageBlock) first if the
 * 64-bit length no longer fits, then store the length big-endian in the last
 * eight bytes and process the final block. */
void func_0205e280(SHA1Context *context) {
    if (context->Message_Block_Index > 55) {
        context->Message_Block[context->Message_Block_Index++] = 0x80;
        while (context->Message_Block_Index < 64) {
            context->Message_Block[context->Message_Block_Index++] = 0;
        }
        func_0205e3d4(context);
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

    func_0205e3d4(context);
}
