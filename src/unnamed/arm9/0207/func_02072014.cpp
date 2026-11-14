//cpp
/* Load-bearing: the ROM build passes -Cpp_exceptions off, and the
   .exceptix record at 0x0207391c (func_02072014, length 0x154)
   exists only because this pragma turns exceptions back on. */
#pragma exceptions on
#include "private/arm9_action_walk.h"
namespace std {
void terminate();
}

extern "C" {

extern unsigned char func_02072d90(struct ActionWalk *c);
}

extern unsigned char func_020729f4(struct ActionWalk *c);
extern "C" {

extern unsigned char *ReadSignedVarInt(unsigned char *p, int *out);
}

/* EH runtime, rethrow: walk a copy of the context's actions to action 13,
   read its signed offset into the frame (ReadSignedVarInt) and return the
   catch record there. c takes the rethrown exception back from it: its type
   and object, no destructor, and the record itself as mCatchInfo. Actions 1
   and 14, and any unknown action, call the terminate path
   (std::terminate). */
CatchInfo *func_02072014(ThrowContext *c, struct ActionCursor *cursor) {
    struct ActionWalk l;
    unsigned char sel;
    int tmp;
    CatchInfo *info;

    l.iterator = cursor->iterator;
    l.unknown_14 = cursor->unknown_14;
    l.mThrow = *c;

    sel = func_02072d90(&l);
    for (;;) {
        switch (sel) {
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
        case 12:
        case 15:
        case 16:
        case 17:
        case 18:
            sel = func_020729f4(&l);
            continue;
        case 13:
            break;
        }
        break;
    }
    ReadSignedVarInt(l.iterator.mAction + 1, &tmp);
    info = (CatchInfo *)(l.mThrow.mFP + tmp);
    c->mThrowType = info->mTypeInfo;
    c->mLocation = info->mLocation;
    c->mDtor = 0;
    c->mCatchInfo = info;
    return info;
}
