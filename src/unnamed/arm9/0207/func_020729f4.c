//cpp
#include "types.h"
#include "private/arm9_action_walk.h"
/* std::terminate takes no arguments. The default arm still passes the
 * opcode and the 0x80 test: that cast is a direct bl to _ZSt9terminatev,
 * and it keeps r1 live from the flag test. An ellipsis declaration links
 * to a different symbol. The file stays .c: delinks.txt names this path. */
namespace std {
void terminate();
}
extern "C" {
extern void *func_02071910(ThrowContext *c, void *s);
extern void func_02072dac(int x, ActionIterator *b);
extern void func_02071864(ThrowContext *t, ActionIterator *it);
extern u8 *ReadSignedVarInt(u8 *p, int *out);
extern u8 *func_02071a50(u8 *p, int *out);
u8 func_020729f4(ActionWalk *c) {
    ThrowContext *frame = &c->mThrow;
    u8 *ip;
    u8 op;
    int m;
    u8 *p1;
    u8 *p2;
    u8 *p3;
    u8 *p4;
    ActionWalk *ctx;
    void *x;
    int buf[40];

    for (;;) {
        ip = c->iterator.mAction;
        if (ip != 0) {
            op = *ip;
            m = op & 0x80;
            if (m == 0) {
                break;
            }
        }
        x = func_02071910(frame, c);
        func_02072dac((int)x, &c->iterator);
        if (c->iterator.mActionTable == 0) {
            std::terminate();
        }
        func_02071864(frame, &c->iterator);
        if (c->iterator.mAction == 0) {
            continue;
        }
        goto tail;
    }

    switch (op & 0x1f) {
    case 2:
        p1 = ReadSignedVarInt(ip + 1, &buf[0]);
        c->iterator.mAction = p1 + 4;
        break;
    case 3:
        p1 = ReadSignedVarInt(ip + 1, &buf[1]);
        p2 = ReadSignedVarInt(p1, &buf[2]);
        c->iterator.mAction = p2 + 4;
        break;
    case 4:
        p1 = ReadSignedVarInt(ip + 1, &buf[3]);
        c->iterator.mAction = p1 + 4;
        break;
    case 5:
        p1 = ReadSignedVarInt(ip + 1, &buf[4]);
        p2 = func_02071a50(p1, &buf[5]);
        p3 = func_02071a50(p2, &buf[6]);
        c->iterator.mAction = p3 + 4;
        break;
    case 6:
        p1 = ReadSignedVarInt(ip + 1, &buf[7]);
        p2 = ReadSignedVarInt(p1, &buf[8]);
        c->iterator.mAction = p2 + 4;
        break;
    case 7:
        p1 = ReadSignedVarInt(ip + 1, &buf[9]);
        p2 = ReadSignedVarInt(p1, &buf[10]);
        c->iterator.mAction = p2 + 4;
        break;
    case 8:
        p1 = ReadSignedVarInt(ip + 1, &buf[11]);
        p2 = ReadSignedVarInt(p1, &buf[12]);
        p3 = ReadSignedVarInt(p2, &buf[13]);
        c->iterator.mAction = p3 + 4;
        break;
    case 9:
        p1 = ReadSignedVarInt(ip + 1, &buf[14]);
        p2 = ReadSignedVarInt(p1, &buf[15]);
        p3 = func_02071a50(p2, &buf[16]);
        p4 = func_02071a50(p3, &buf[17]);
        c->iterator.mAction = p4 + 4;
        break;
    case 10:
        p1 = ReadSignedVarInt(ip + 1, &buf[18]);
        c->iterator.mAction = p1 + 4;
        break;
    case 11:
        p1 = ReadSignedVarInt(ip + 1, &buf[19]);
        p2 = ReadSignedVarInt(p1, &buf[20]);
        c->iterator.mAction = p2 + 4;
        break;
    case 12:
        p1 = func_02071a50(ip + 5, &buf[21]);
        p2 = ReadSignedVarInt(p1, &buf[22]);
        c->iterator.mAction = p2;
        break;
    case 13:
        p1 = ReadSignedVarInt(ip + 1, &buf[23]);
        c->iterator.mAction = p1;
        break;
    case 15:
        p1 = func_02071a50(ip + 1, &buf[24]);
        p2 = func_02071a50(p1, &buf[25]);
        p3 = ReadSignedVarInt(p2, &buf[26]);
        c->iterator.mAction = p3 + (buf[24] * 4);
        break;
    case 16:
        p1 = ReadSignedVarInt(ip + 1, &buf[27]);
        p2 = ReadSignedVarInt(p1, &buf[28]);
        buf[30] = ((p2[0] | (p2[1] << 8)) | (p2[2] << 16)) | (p2[3] << 24);
        p3 = ReadSignedVarInt(p2 + 4, &buf[29]);
        c->iterator.mAction = p3 + 4;
        break;
    case 17:
        p1 = ReadSignedVarInt(ip + 1, &buf[31]);
        p2 = ReadSignedVarInt(p1, &buf[33]);
        p3 = ReadSignedVarInt(p2 + 1, &buf[32]);
        p4 = ReadSignedVarInt(p3, &buf[34]);
        c->iterator.mAction = p4 + 4;
        break;
    case 18:
        p1 = ReadSignedVarInt(ip + 1, &buf[35]);
        p2 = ReadSignedVarInt(p1 + 1, &buf[36]);
        p3 = func_02071a50(p2, &buf[37]);
        c->iterator.mAction = p3 + 4;
        break;
    case 19:
        p1 = ReadSignedVarInt(ip + 1, &buf[38]);
        c->iterator.mAction = p1;
        break;
    case 0:
    case 1:
    case 14:
    default:
        ((void (*)(int, int))std::terminate)(op & 0x1f, m);
        break;
    }

tail:
    op = (*c->iterator.mAction) & 0x1f;
    ctx = c;
    if (op != 1) {
        return op;
    }
    {
        int *pv = &buf[39];
        u8 **curptr = &c->iterator.mAction;
        do {
            ReadSignedVarInt(ctx->iterator.mAction + 1, pv);
            *curptr = (*curptr) + buf[39];
            op = (*c->iterator.mAction) & 0x1f;
        } while (op == 1);
    }
    return op;
}
}
