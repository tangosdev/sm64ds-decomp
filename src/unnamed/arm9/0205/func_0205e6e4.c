#include "types.h"
#pragma opt_loop_invariants off

/* RFC 3174 SHA1Context, as func_0205e3d4.c and func_0205e89c.c spell it, with
 * the 32-bit block index this build stores. */
typedef struct {
    u32 Intermediate_Hash[5]; /* 0x00 */
    u32 Length_Low;           /* 0x14 */
    u32 Length_High;          /* 0x18 */
    u32 Message_Block_Index;  /* 0x1c */
    u8 Message_Block[64];     /* 0x20 */
    int Computed;             /* 0x60 -- more input is refused once set */
    int Corrupted;            /* 0x64 -- the error code, 0 while good */
} SHA1Context;

/* The parameter keeps its old struct Obj tag, as does the
 * func_0205e3d4 extern: retyping either changes a declaration the
 * declaration-agreement baseline already records. */
struct Obj;

extern void func_0205e3d4(struct Obj*);

/* RFC 3174 SHA1Input(context, message_array, length): 0 (shaSuccess),
 * 1 (shaNull) without a context or message, 3 (shaStateError) after the
 * result was computed, or the stored error. func_0205e3d4 is
 * SHA1ProcessMessageBlock. */
int func_0205e6e4(struct Obj *obj, u8 *p, int count) {
    SHA1Context *context = (SHA1Context *)obj;
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
        if (context->Message_Block_Index == 0x40)
            func_0205e3d4(obj);
        p++;
    }
    return 0;
}
