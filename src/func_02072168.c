/* func_02072168 @ 0x02072168 (arm9, size 0x880)   [mwccarm 1.2/base, 1.2/sp2, 1.2/sp2p3, 2004/b56]
 * Bytecode-VM dispatcher for the wireless callback stream; sibling of func_020690b0.
 * s+8 = IP into the byte stream, s+4 = more-data flag, c+0x18 = data segment base,
 * c+0x1c = int[] locals table. Refill via func_02071910/02072dac/020731fc/02071864;
 * LEB128 operands via ReadSignedVarInt (signed) / func_02071a50 (unsigned). Opcode bits:
 * 0x20/0x40 select array-indexed vs base-relative operand source, 0x80 restores s+8
 * from the checkpoint; cases 0xc/0xf early-return when the IP reaches `end`.
 *
 * EXTENT: the compiled object is 0x88c bytes and is byte-identical to the ROM over
 * 0x02072168..0x020729f4. The final 12 bytes are func_020729e8, the severed epilogue
 * the symbol table had split off (notes/mwccarm-codegen.md 9a(3)); the symbol map
 * now merges the pair (func_02072168 size 0x88c) and this file is the source of
 * the full extent.
 *
 * Load-bearing spellings (do not "clean up"):
 *   - case 1's u64 no-op mask keeps the s+8 RMW address materialized (6g launder).
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

extern int func_02071910(char *c, char *s);
extern void func_02072dac(int a, char *s);
extern void func_020731fc(void);
extern void func_02071864(char *c, char *s);
extern unsigned char *ReadSignedVarInt(unsigned char *p, int *out);
extern unsigned char *func_02071a50(unsigned char *p, int *out);
extern int func_01ffadf0(int a, int b);

void func_02072168(char *c, char *s, char *end) {
    int saved = 0;
    int neg1 = -1;
    unsigned char op;
    unsigned char *p;

    for (;;) {
        p = *(unsigned char **)(s + 8);
        if (p == 0) {
            func_02072dac(func_02071910(c, s), s);
            if (*(int *)(s + 4) == 0)
                func_020731fc();
            func_02071864(c, s);
            p = *(unsigned char **)(s + 8);
            if (p == 0)
                continue;
        }
        op = *p;
        switch (op & 0x1f) {
        case 1: {
            int v0;
            ReadSignedVarInt(p + 1, &v0);
            (*(int *)(((long long)(int)(s + 8)) & 0xFFFFFFFFFFFFFFFFLL)) += v0;
            break;
        }
        case 2: {
            int v0;
            unsigned char *q;
            int fp;
            q = ReadSignedVarInt(p + 1, &v0);
            fp = q[0] | (q[1] << 8) | (q[2] << 16) | (q[3] << 24);
            ((F2)fp)((int)(*(char **)(c + 0x18) + v0), neg1);
            *(unsigned char **)(s + 8) = q + 4;
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
                t = ((int *)(c + 0x1c))[v0] & 0xff;
            else
                t = *(unsigned char *)(*(char **)(c + 0x18) + v0);
            if (t != 0)
                ((F2)fp)((int)(*(char **)(c + 0x18) + v1), neg1);
            *(unsigned char **)(s + 8) = adv;
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
                a = ((int *)(c + 0x1c))[v0];
            else
                a = *(int *)(*(char **)(c + 0x18) + v0);
            ((F2)fp)(a, neg1);
            *(unsigned char **)(s + 8) = adv;
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
            b = (int)(*(char **)(c + 0x18) + v0);
            n = v1;
            b += n * v2;
            if (n != 0) {
                do {
                    b -= v2;
                    ((F2)fp)(b, neg1);
                } while (--n != 0);
            }
            *(unsigned char **)(s + 8) = adv;
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
                a = ((int *)(c + 0x1c))[v0];
            else
                a = *(int *)(*(char **)(c + 0x18) + v0);
            ((F2)fp)(a + v1, saved);
            *(unsigned char **)(s + 8) = adv;
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
                a = ((int *)(c + 0x1c))[v0];
            else
                a = *(int *)(*(char **)(c + 0x18) + v0);
            ((F2)fp)(a + v1, neg1);
            *(unsigned char **)(s + 8) = adv;
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
                t = (short)((int *)(c + 0x1c))[v0];
            else
                t = *(short *)(*(char **)(c + 0x18) + v0);
            if (t != 0) {
                if (fl != 0)
                    a = ((int *)(c + 0x1c))[v1];
                else
                    a = *(int *)(*(char **)(c + 0x18) + v1);
                ((F2)fp)(a + v2, neg1);
            }
            *(unsigned char **)(s + 8) = adv;
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
                b = ((int *)(c + 0x1c))[v0] - (-v1);
            else
                b = *(int *)(*(char **)(c + 0x18) + v0) - (-v1);
            n = v2;
            b += n * v3;
            if (n != 0) {
                do {
                    b -= v3;
                    ((F2)fp)(b, neg1);
                } while (--n != 0);
            }
            *(unsigned char **)(s + 8) = adv;
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
                a = ((int *)(c + 0x1c))[v0];
            else
                a = *(int *)(*(char **)(c + 0x18) + v0);
            ((F1)fp)(a);
            *(unsigned char **)(s + 8) = adv;
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
                t = ((int *)(c + 0x1c))[v0] & 0xff;
            else
                t = *(unsigned char *)(*(char **)(c + 0x18) + v0);
            if (t != 0) {
                if (fl != 0)
                    a = ((int *)(c + 0x1c))[v1];
                else
                    a = *(int *)(*(char **)(c + 0x18) + v1);
                ((F1)fp)(a);
            }
            *(unsigned char **)(s + 8) = adv;
            break;
        }
        case 0xc: {
            int v0;
            int v1;
            unsigned char *q;
            if (end == (char *)p)
                return;
            q = ReadSignedVarInt(func_02071a50(p + 5, &v0), &v1);
            *(unsigned char **)(s + 8) = q;
            break;
        }
        case 0xd: {
            int v0;
            unsigned char *q;
            char *e;
            int fp;
            q = ReadSignedVarInt(p + 1, &v0);
            e = *(char **)(c + 0x18) + v0;
            fp = *(int *)(e + 8);
            if (fp != 0) {
                if (*(int *)(c + 4) == *(int *)e)
                    *(int *)(c + 8) = fp;
                else
                    ((F2)fp)(*(int *)e, neg1);
            }
            *(unsigned char **)(s + 8) = q;
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
            *(unsigned char **)(s + 8) = q + v0 * 4;
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
                a = ((int *)(c + 0x1c))[v0];
            else
                a = *(int *)(*(char **)(c + 0x18) + v0);
            ((F2)fp)(a + v1, k + v2);
            *(unsigned char **)(s + 8) = adv;
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
                a = ((int *)(c + 0x1c))[v0];
            else
                a = *(int *)(*(char **)(c + 0x18) + v0);
            if (fl2 != 0)
                b = ((int *)(c + 0x1c))[v1];
            else
                b = *(int *)(*(char **)(c + 0x18) + v1);
            ((F2)fp)(a + v2, b + v3);
            *(unsigned char **)(s + 8) = adv;
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
                    a = ((int *)(c + 0x1c))[v0];
                else
                    a = *(int *)(*(char **)(c + 0x18) + v0);
                if (fl2 != 0)
                    b = ((int *)(c + 0x1c))[v1];
                else
                    b = *(int *)(*(char **)(c + 0x18) + v1);
                a += b;
                n = func_01ffadf0(b, v2);
                if (n != 0) {
                    do {
                        a -= v2;
                        ((F2)fp)(a, neg1);
                    } while (--n != 0);
                }
            }
            *(unsigned char **)(s + 8) = adv;
            break;
        }
        case 0x13: {
            int v0;
            unsigned char *q;
            q = ReadSignedVarInt(p + 1, &v0);
            *(unsigned char **)(s + 8) = q;
            break;
        }
        default:
            func_020731fc();
            break;
        }
        if ((op & 0x80) != 0)
            *(int *)(s + 8) = saved;
    }
}
