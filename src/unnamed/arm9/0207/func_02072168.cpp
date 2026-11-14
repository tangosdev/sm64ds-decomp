//cpp
/* Load-bearing: the ROM build passes -Cpp_exceptions off, and the
   .exceptix record at 0x02073928 (func_02072168, length 0x88c)
   exists only because this pragma turns exceptions back on. */
#pragma exceptions on
/* func_02072168 @ 0x02072168 (arm9, size 0x88c)   [mwccarm 1.2/base, 1.2/sp2, 1.2/sp2p3, 2004/b56]
 * C++ exception unwinder: interprets a function's exception-action table, running
 * the destructors of everything live in the frame, until it reaches `end`. (An
 * earlier banner called it a wireless bytecode VM and a sibling of MBi_ChildCallback;
 * the ThrowContext/CatchInfo evidence below says otherwise.) When s->mAction runs
 * out, func_02071910 unwinds one frame, func_02072dac finds that frame's table
 * (std::terminate runs when none covers the PC) and func_02071864 reads the
 * frame header. LEB128 operands come via ReadSignedVarInt (signed) / func_02071a50
 * (unsigned); each destroy action ends in a 4-byte destructor address. Opcode bits:
 * 0x20/0x40 take the operand from c->mFrame.mGPR[] instead of the frame at c->mFP, and
 * 0x80 ends the list (s->mAction = 0); cases 0xc/0xf return when they reach `end`.
 *
 * EXTENT: the compiled object is 0x88c bytes and is byte-identical to the ROM over
 * 0x02072168..0x020729f4. The final 12 bytes are func_020729e8, the severed epilogue
 * the symbol table had split off (notes/mwccarm-codegen.md 9a(3)); the symbol map
 * now merges the pair (func_02072168 size 0x88c) and this file is the source of
 * the full extent.
 *
 * Load-bearing spellings (do not "clean up"):
 *   - case 9's `- (-v1)` beats the canonicalizer: `a + v1` and `v1 + a` both
 *     re-canonicalize to +(v1, a) and walk right-to-left, rotating three scratch
 *     registers away from the ROM; `a - (-v1)` survives to late strength reduction,
 *     which folds it to the add with +(a, v1) order intact.
 *   - each case takes its register flag as `fl = s->mAction[0] & 0x20` (0x40 in
 *     case 3), re-reading the action byte instead of using op. The compiler
 *     CSEs the load into op but keeps fl as its own value, which is computed
 *     before the operand reads as in the ROM (`and r6, r5, #0x20` ahead of the
 *     bl). With `fl = op & 0x20` the and sinks past the call: 72 words differ
 *     and the function is 0x20 bytes short. These also miss:
 *     - `(op & m) != 0` and `!!(op & m)` (0x8ec)
 *     - `? 1 : 0` and `if (fl) fl = 1` (693 or more words)
 *     - a narrower or bool fl (165 to 708 words)
 *     - fl at function scope (76 words)
 *     - an inline ActionFlag(op, m) helper, by value or by reference (72 or
 *       215 words)
 *     - `*p & m` (72 words)
 *   - case 0x10's volatile k: the ROM spills k to [sp, #0x84] across the
 *     ReadSignedVarInt for v2 and reloads it. These cost 83 words:
 *     - plain int or unsigned k
 *     - k declared first or as an initialized declaration
 *     - an inline ReadAddr(q, &k / k) helper
 */
#include "private/throw_context.h"

namespace std {
void terminate();
}

extern "C" {

typedef void (*F2)(int, int);
typedef void (*F1)(int);

/* File-local views. The roles match CodeWarrior's exception runtime: case 0xd
 * is its ACTIVECATCHBLOCK (hand a live catch object's destructor on to a
 * rethrow at the same location, else destroy it), and func_02071ccc fills the
 * CatchInfo the same way that runtime's catch setup does ('*' pointer types
 * get a copied pointer). Names follow those roles. ActionIterator and
 * CatchInfo are shared (private/throw_context.h). */
extern void *func_02071910(ThrowContext *c, void *s);
extern void func_02072dac(int a, ActionIterator *s);
extern void func_02071864(ThrowContext *t, ActionIterator *it);
extern unsigned char *ReadSignedVarInt(unsigned char *p, int *out);
extern unsigned char *func_02071a50(unsigned char *p, int *out);

void func_02072168(ThrowContext *c, ActionIterator *s, char *end) {
    int saved = 0;
    int neg1 = -1;
    unsigned char op;
    unsigned char *p;

    for (;;) {
        p = s->mAction;
        if (p == 0) {
            func_02072dac((int)func_02071910(c, s), s);
            if (s->mActionTable == 0)
                std::terminate();
            func_02071864(c, s);
            p = s->mAction;
            if (p == 0)
                continue;
        }
        op = *p;
        switch (op & 0x1f) {
        case 1: {
            int v0;
            ReadSignedVarInt(p + 1, &v0);
            s->mAction += v0;
            break;
        }
        case 2: {
            int v0;
            unsigned char *q;
            int fp;
            q = ReadSignedVarInt(p + 1, &v0);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            ((F2)fp)((int)(c->mFP + v0), neg1);
            s->mAction = q + 4;
            break;
        }
        case 3: {
            int v0;
            int v1;
            int fl;
            unsigned char *q;
            unsigned char *adv;
            int fp;
            int t;
            fl = s->mAction[0] & 0x40;
            q = ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                t = c->mFrame.mGPR[v0] & 0xff;
            else
                t = (unsigned char)c->mFP[v0];
            if (t != 0)
                ((F2)fp)((int)(c->mFP + v1), neg1);
            s->mAction = adv;
            break;
        }
        case 4: {
            int v0;
            int fl;
            unsigned char *q;
            unsigned char *adv;
            int fp;
            int a;
            fl = s->mAction[0] & 0x20;
            q = ReadSignedVarInt(p + 1, &v0);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                a = c->mFrame.mGPR[v0];
            else
                a = *(int *)(c->mFP + v0);
            ((F2)fp)(a, neg1);
            s->mAction = adv;
            break;
        }
        case 5: {
            int v0;
            int v1;
            int v2;
            unsigned char *q;
            unsigned char *adv;
            int fp;
            int n;
            int b;
            q = func_02071a50(func_02071a50(ReadSignedVarInt(p + 1, &v0), &v1), &v2);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            b = (int)(c->mFP + v0);
            n = v1;
            b += n * v2;
            if (n != 0) {
                do {
                    b -= v2;
                    ((F2)fp)(b, neg1);
                } while (--n != 0);
            }
            s->mAction = adv;
            break;
        }
        case 6: {
            int v0;
            int v1;
            int fl;
            unsigned char *q;
            unsigned char *adv;
            int fp;
            int a;
            fl = s->mAction[0] & 0x20;
            q = ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                a = c->mFrame.mGPR[v0];
            else
                a = *(int *)(c->mFP + v0);
            ((F2)fp)(a + v1, saved);
            s->mAction = adv;
            break;
        }
        case 7: {
            int v0;
            int v1;
            int fl;
            unsigned char *q;
            unsigned char *adv;
            int fp;
            int a;
            fl = s->mAction[0] & 0x20;
            q = ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                a = c->mFrame.mGPR[v0];
            else
                a = *(int *)(c->mFP + v0);
            ((F2)fp)(a + v1, neg1);
            s->mAction = adv;
            break;
        }
        case 8: {
            int v0;
            int v1;
            int v2;
            int fl;
            unsigned char *q;
            unsigned char *adv;
            int fp;
            int t;
            int a;
            fl = s->mAction[0] & 0x20;
            q = ReadSignedVarInt(
                ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1), &v2);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if ((op & 0x40) != 0)
                t = (short)c->mFrame.mGPR[v0];
            else
                t = *(short *)(c->mFP + v0);
            if (t != 0) {
                if (fl != 0)
                    a = c->mFrame.mGPR[v1];
                else
                    a = *(int *)(c->mFP + v1);
                ((F2)fp)(a + v2, neg1);
            }
            s->mAction = adv;
            break;
        }
        case 9: {
            int v0;
            int v1;
            int v2;
            int v3;
            int fl;
            unsigned char *q;
            unsigned char *adv;
            int fp;
            int n;
            int b;
            fl = s->mAction[0] & 0x20;
            q = func_02071a50(
                func_02071a50(ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1), &v2), &v3);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                b = c->mFrame.mGPR[v0] - (-v1);
            else
                b = *(int *)(c->mFP + v0) - (-v1);
            n = v2;
            b += n * v3;
            if (n != 0) {
                do {
                    b -= v3;
                    ((F2)fp)(b, neg1);
                } while (--n != 0);
            }
            s->mAction = adv;
            break;
        }
        case 0xa: {
            int v0;
            int fl;
            unsigned char *q;
            unsigned char *adv;
            int fp;
            int a;
            fl = s->mAction[0] & 0x20;
            q = ReadSignedVarInt(p + 1, &v0);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                a = c->mFrame.mGPR[v0];
            else
                a = *(int *)(c->mFP + v0);
            ((F1)fp)(a);
            s->mAction = adv;
            break;
        }
        case 0xb: {
            int v0;
            int v1;
            int fl;
            unsigned char *q;
            unsigned char *adv;
            int fp;
            int t;
            int a;
            fl = s->mAction[0] & 0x20;
            q = ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if ((op & 0x40) != 0)
                t = c->mFrame.mGPR[v0] & 0xff;
            else
                t = (unsigned char)c->mFP[v0];
            if (t != 0) {
                if (fl != 0)
                    a = c->mFrame.mGPR[v1];
                else
                    a = *(int *)(c->mFP + v1);
                ((F1)fp)(a);
            }
            s->mAction = adv;
            break;
        }
        case 0xc: {
            int v0;
            int v1;
            unsigned char *q;
            if (end == (char *)p)
                return;
            q = ReadSignedVarInt(func_02071a50(p + 5, &v0), &v1);
            s->mAction = q;
            break;
        }
        case 0xd: {
            int v0;
            unsigned char *q;
            CatchInfo *e;
            int fp;
            q = ReadSignedVarInt(p + 1, &v0);
            e = (CatchInfo *)(c->mFP + v0);
            fp = e->mDtor;
            if (fp != 0) {
                if (c->mLocation == e->mLocation)
                    c->mDtor = fp;
                else
                    ((F2)fp)(e->mLocation, neg1);
            }
            s->mAction = q;
            break;
        }
        case 0xf: {
            int v0;
            int v1;
            int v2;
            unsigned char *q;
            if (end == (char *)p)
                return;
            q = ReadSignedVarInt(func_02071a50(func_02071a50(p + 1, &v0), &v1), &v2);
            s->mAction = q + v0 * 4;
            break;
        }
        case 0x10: {
            int v0;
            int v1;
            int v2;
            volatile int k;
            int fl;
            unsigned char *q;
            unsigned char *adv;
            int fp;
            int a;
            fl = s->mAction[0] & 0x20;
            q = ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1);
            k = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            q = ReadSignedVarInt(q + 4, &v2);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                a = c->mFrame.mGPR[v0];
            else
                a = *(int *)(c->mFP + v0);
            ((F2)fp)(a + v1, k + v2);
            s->mAction = adv;
            break;
        }
        case 0x11: {
            int v0;
            int v1;
            int v2;
            int v3;
            int fl;
            int fl2;
            unsigned char *q;
            unsigned char *adv;
            int fp;
            int a;
            int b;
            fl = s->mAction[0] & 0x20;
            q = ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v2);
            fl2 = *q++ & 0x20;
            q = ReadSignedVarInt(ReadSignedVarInt(q, &v1), &v3);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                a = c->mFrame.mGPR[v0];
            else
                a = *(int *)(c->mFP + v0);
            if (fl2 != 0)
                b = c->mFrame.mGPR[v1];
            else
                b = *(int *)(c->mFP + v1);
            ((F2)fp)(a + v2, b + v3);
            s->mAction = adv;
            break;
        }
        case 0x12: {
            int v0;
            int v1;
            int v2;
            int fl2;
            int fl;
            unsigned char *q;
            unsigned char *adv;
            int fp;
            int b;
            int n;
            fl = s->mAction[0] & 0x20;
            q = ReadSignedVarInt(p + 1, &v0);
            fl2 = *q++ & 0x20;
            q = func_02071a50(ReadSignedVarInt(q, &v1), &v2);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            {
                int a;
                if (fl != 0)
                    a = c->mFrame.mGPR[v0];
                else
                    a = *(int *)(c->mFP + v0);
                if (fl2 != 0)
                    b = c->mFrame.mGPR[v1];
                else
                    b = *(int *)(c->mFP + v1);
                a += b;
                n = (u32)b / (u32)v2;
                if (n != 0) {
                    do {
                        a -= v2;
                        ((F2)fp)(a, neg1);
                    } while (--n != 0);
                }
            }
            s->mAction = adv;
            break;
        }
        case 0x13: {
            int v0;
            unsigned char *q;
            q = ReadSignedVarInt(p + 1, &v0);
            s->mAction = q;
            break;
        }
        default:
            std::terminate();
            break;
        }
        if ((op & 0x80) != 0)
            s->mAction = (unsigned char *)saved;
    }
}
}
