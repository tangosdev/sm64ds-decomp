//cpp
/* Load-bearing: the ROM build passes -Cpp_exceptions off, and the
   .exceptix record at 0x020738f8 (func_02071d3c, length 0x1e0)
   exists only because this pragma turns exceptions back on. */
#pragma exceptions on
#include "types.h"
#include "private/arm9_action_walk.h"

namespace std {
void terminate();
}

extern "C" {

extern unsigned char func_02072d90(struct ActionWalk *c);
extern u8 *func_02071a50(u8 *p, int *out);
extern u8 *ReadSignedVarInt(u8 *p, int *out);
extern int __throw_catch_compare(const u8 *throwtype, const u8 *catchtype, int *offset_result);
extern int func_02071f88(void *a, ExSpecification *spec);
extern void func_02071f1c(ThrowContext *context, ActionIterator *it, ExSpecification *spec,
                          unsigned char *action);
}

extern unsigned char func_020729f4(struct ActionWalk *c);

struct Tmp {
    int val;
    int a;
    int b;
    ExSpecification spec;
};

/* EH runtime: search the current frame's actions for a handler, walking
   them with func_02072d90 (current) and func_020729f4 (next). Action 12 is
   a catch clause: its type string is matched against the thrown type with
   __throw_catch_compare. Action 15 is an exception
   specification: a thrown type it does not list (func_02071f88) goes to
   func_02071f1c. Actions 1 and 14, and any unknown action, call the
   terminate path (std::terminate). The other actions are skipped. */
u8 *func_02071d3c(ThrowContext *self, struct ActionCursor *src, int *flag) {
    struct Tmp tmp;
    struct ActionWalk buf;
    int state;

    buf.iterator = src->iterator;
    buf.unknown_14 = src->unknown_14;
    buf.mThrow = *self;

    state = func_02072d90(&buf);

    for (;; state = func_020729f4(&buf)) {
        switch (state) {
        case 12:
        {
            u8 *cur = buf.iterator.mAction;
            tmp.val = cur[1] | (cur[2] << 8) | (cur[3] << 16) | (cur[4] << 24);
            u8 *p = func_02071a50(cur + 5, &tmp.a);
            p = ReadSignedVarInt(p, &tmp.b);
            if (__throw_catch_compare((u8 *)self->mThrowType, (u8 *)tmp.val, flag) == 0) {
                continue;
            }
            break;
        }

        case 15:
        {
            u8 *cur = buf.iterator.mAction;
            u8 *p = func_02071a50(cur + 1, &tmp.spec.mSpecCount);
            p = func_02071a50(p, &tmp.spec.mPCOffset);
            p = ReadSignedVarInt(p, &tmp.spec.mCatchInfoRef);
            tmp.spec.mSpecs = p;
            if (func_02071f88((void *)self->mThrowType, &tmp.spec) == 0) {
                func_02071f1c(self, &src->iterator, &tmp.spec, buf.iterator.mAction);
            }
            continue;
        }

        case 1:
        case 14:
        default:
            std::terminate();
            break;

        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 13:
        case 16:
        case 17:
        case 18:
        case 19:
            continue;
        }
        break;
    }

    return buf.iterator.mAction;
}
