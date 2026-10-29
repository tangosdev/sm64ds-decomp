/* func_02072168 @ 0x02072168 (arm9, size 0x880)   [mwccarm 1.2/base, 1.2/sp2, 1.2/sp2p3, 2004/b56]
 * C++ exception unwinder: interprets a function's exception-action table, running
 * the destructors of everything live in the frame, until it reaches `end`. (An
 * earlier banner called it a wireless bytecode VM and a sibling of func_020690b0;
 * the ThrowContext/CatchInfo evidence below says otherwise.) When s->mAction runs
 * out, func_02071910 unwinds one frame, func_02072dac finds that frame's table
 * (func_020731fc terminates when none covers the PC) and func_02071864 reads the
 * frame header. LEB128 operands come via ReadSignedVarInt (signed) / func_02071a50
 * (unsigned); each destroy action ends in a 4-byte destructor address. Opcode bits:
 * 0x20/0x40 take the operand from c->mGPR[] instead of the frame at c->mFP, and
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
 *   - the `fl = fl ? fl : fl;` self-selects and the volatile k in case 0x10 pin
 *     flag webs; inherited from the 195->12 structural draft.
 */
#pragma opt_loop_invariants off
typedef void (*F2)(int, int);
typedef void (*F1)(int);

/* File-local views. The roles match CodeWarrior's exception runtime: case 0xd
 * is its ACTIVECATCHBLOCK (hand a live catch object's destructor on to a
 * rethrow at the same location, else destroy it), and func_02071ccc fills the
 * CatchInfo the same way that runtime's catch setup does ('*' pointer types
 * get a copied pointer). Names follow those roles. */
typedef struct ThrowContext {
    int mThrowType; /* 0x00 -- type string; func_02071ccc tests it for '*' */
    int mLocation;  /* 0x04 -- thrown object; matched against a CatchInfo */
    int mDtor;      /* 0x08 -- thrown object's destructor (case 0xd) */
    char pad_0c[0x8];
    char *mSP; /* 0x14 -- func_02071910: mFP + frame size after unwind */
    /* 0x18 -- base for frame-relative operands; set from mSP, r7 or r11 by
     * func_02071864 */
    char *mFP;
    /* 0x1c -- r0..r15; func_02071910 restores the saved registers here,
     * mGPR[14] is the return address */
    int mGPR[16];
} ThrowContext;

typedef struct ActionIterator {
    int mFuncStart;              /* 0x00 -- func_02072dac: start of the covered range */
    unsigned char *mActionTable; /* 0x04 -- null when no table covers the PC */
    unsigned char *mAction;      /* 0x08 -- next action byte; null at end of list */
} ActionIterator;

typedef struct CatchInfo {
    int mLocation; /* 0x00 -- func_02071ccc: ThrowContext.mLocation */
    int mTypeInfo; /* 0x04 -- func_02071ccc: ThrowContext.mThrowType */
    int mDtor;     /* 0x08 -- func_02071ccc: ThrowContext.mDtor */
} CatchInfo;

extern int func_02071910(char *c, char *s);
extern void func_02072dac(int a, char *s);
extern void func_020731fc(void);
extern void func_02071864(char *c, char *s);
extern unsigned char *ReadSignedVarInt(unsigned char *p, int *out);
extern unsigned char *func_02071a50(unsigned char *p, int *out);
extern int __aeabi_uidiv(int a, int b);

void func_02072168(ThrowContext *c, ActionIterator *s, char *end) {
    int saved = 0;
    int neg1 = -1;
    unsigned char op;
    unsigned char *p;

    for (;;) {
        p = s->mAction;
        if (p == 0) {
            func_02072dac(func_02071910((char *)c, (char *)s), (char *)s);
            if (s->mActionTable == 0)
                func_020731fc();
            func_02071864((char *)c, (char *)s);
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
            fl = op & 0x40;
            fl = fl ? fl : fl;
            q = ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                t = c->mGPR[v0] & 0xff;
            else
                t = *(unsigned char *)(c->mFP + v0);
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
            fl = op & 0x20;
            fl = fl ? fl : fl;
            q = ReadSignedVarInt(p + 1, &v0);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                a = c->mGPR[v0];
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
            fl = op & 0x20;
            fl = fl ? fl : fl;
            q = ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                a = c->mGPR[v0];
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
            fl = op & 0x20;
            fl = fl ? fl : fl;
            q = ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                a = c->mGPR[v0];
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
            fl = op & 0x20;
            q = ReadSignedVarInt(
                ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1), &v2);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if ((op & 0x40) != 0)
                t = (short)c->mGPR[v0];
            else
                t = *(short *)(c->mFP + v0);
            if (t != 0) {
                if (fl != 0)
                    a = c->mGPR[v1];
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
            fl = op & 0x20;
            fl = fl ? fl : fl;
            q = func_02071a50(
                func_02071a50(ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1), &v2), &v3);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                b = c->mGPR[v0] - (-v1);
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
            fl = op & 0x20;
            fl = fl ? fl : fl;
            q = ReadSignedVarInt(p + 1, &v0);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                a = c->mGPR[v0];
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
            fl = op & 0x20;
            q = ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if ((op & 0x40) != 0)
                t = c->mGPR[v0] & 0xff;
            else
                t = *(unsigned char *)(c->mFP + v0);
            if (t != 0) {
                if (fl != 0)
                    a = c->mGPR[v1];
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
            fl = op & 0x20;
            fl = fl ? fl : fl;
            q = ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v1);
            k = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            q = ReadSignedVarInt(q + 4, &v2);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                a = c->mGPR[v0];
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
            fl = op & 0x20;
            fl = fl ? fl : fl;
            q = ReadSignedVarInt(ReadSignedVarInt(p + 1, &v0), &v2);
            fl2 = *q++ & 0x20;
            q = ReadSignedVarInt(ReadSignedVarInt(q, &v1), &v3);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            if (fl != 0)
                a = c->mGPR[v0];
            else
                a = *(int *)(c->mFP + v0);
            if (fl2 != 0)
                b = c->mGPR[v1];
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
            fl = op & 0x20;
            fl = fl ? fl : fl;
            q = ReadSignedVarInt(p + 1, &v0);
            fl2 = *q++ & 0x20;
            fl2 = fl2 ? fl2 : fl2;
            q = func_02071a50(ReadSignedVarInt(q, &v1), &v2);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            adv = q + 4;
            {
                int a;
                if (fl != 0)
                    a = c->mGPR[v0];
                else
                    a = *(int *)(c->mFP + v0);
                if (fl2 != 0)
                    b = c->mGPR[v1];
                else
                    b = *(int *)(c->mFP + v1);
                a += b;
                n = __aeabi_uidiv(b, v2);
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
            func_020731fc();
            break;
        }
        if ((op & 0x80) != 0)
            s->mAction = (unsigned char *)saved;
    }
}
