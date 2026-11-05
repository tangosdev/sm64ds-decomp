#include "types.h"
#include "SHA1Context.h"

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
