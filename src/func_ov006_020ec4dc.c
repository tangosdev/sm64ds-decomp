/* dScMgHanachan_c wiggler set-up variant 3 of 4, the fifteen-wiggler board.
 *
 * src/func_ov006_020ecdb8.c switches on data_ov006_02141fd8 and reaches this
 * arm on case 3 and on default; variants 0, 1 and 2 are func_ov006_020ecba4,
 * func_ov006_020ec9c0 and func_ov006_020ec84c. The round dealer
 * src/func_ov006_020ed8a4.c only ever selects kind 3 at clear count 8 or 9,
 * and both of those rows ask for FIFTEEN wigglers, which is what the /3 and
 * %3 below lay out: five columns of three.
 *
 * Its nearest matched sibling is func_ov006_020ec9c0 (variant 1) and the tail
 * from the placement loop down is the same code with a different source table.
 * The differences are the whole content of this variant: variant 1 places one
 * random column, this one places idx on a grid, interpolating a Y between two
 * endpoints on the column index and an X between two on the row index, and it
 * also writes the s16 at +0x84 that variant 1 leaves alone.
 *
 * The two four-way fdiv results are barycentric weights: A and B run over the
 * columns (count/3 - 1 of them) and C and D over the three rows against a
 * fixed 2.0 denominator. The four constants the two arms select are the
 * left/right X endpoints, mirrored with the facing at +0x76.
 */

extern int data_ov006_0213c958;
extern int data_ov006_0213ca24[];
extern int _ZN4cstd4fdivEii(int, int);
extern void func_ov006_020ec2bc(void* self);

typedef struct { int x, y; } Vec2;

#pragma opt_strength_reduction off

void func_ov006_020ec4dc(char* self, int idx)
{
    int n1;
    int A, B, C, D;
    int q, m;
    int cx1, cx2;
    int pos[2];
    int i;
    int off;

    *(int*)(self + 0x40) = 0xa00;
    *(int*)(self + 0x44) = 0xa00;
    m = idx % 3;
    q = idx / 3;
    n1 = data_ov006_0213c958 / 3 - 1;
    A = _ZN4cstd4fdivEii(q << 12, n1 << 12);
    B = _ZN4cstd4fdivEii((n1 - q) << 12, n1 << 12);
    C = _ZN4cstd4fdivEii(m << 12, 0x2000);
    D = _ZN4cstd4fdivEii((2 - m) << 12, 0x2000);

    if (q & 1) {
        *(short*)(self + 0x76) = -0x4000;
        *(short*)(self + 0x84) = -0x8000;
        cx1 = 0x8000;
        cx2 = 0xfe000;
    } else {
        *(short*)(self + 0x76) = 0x4000;
        *(short*)(self + 0x84) = 0;
        cx1 = 0x100000;
        cx2 = 0xa000;
    }

    *(int*)((char*)pos + 0) = 0;
    *(int*)((char*)pos + 4) = 0;
    *(int*)((char*)pos + 0) = (int)(((long long)cx1 * C + 0x800) >> 12)
                            + (int)(((long long)cx2 * D + 0x800) >> 12);
    *(int*)((char*)pos + 4) = (int)(((long long)0x10000 * A + 0x800) >> 12)
                            + (int)(((long long)0xb0000 * B + 0x800) >> 12);

    for (i = 0, off = 0; i < 5; i++) {
        *(int*)(self + i * 8 + 0x18) = pos[0];
        *(int*)(self + i * 8 + 0x1c) = pos[1];
        if (*(short*)(self + 0x76) < 0)
            *(int*)(self + i * 8 + 0x18) += off;
        else
            *(int*)(self + i * 8 + 0x18) -= off;
        *(short*)(self + i * 2 + 0x7a) = *(short*)(self + 0x76);
        off += 0x20000;
    }

    func_ov006_020ec2bc(self);
    *(int*)(self + 0x68) = 0;
    *(int*)(self + 0x6c) = 0x1ec;
    *(Vec2*)(self + 0) = *(Vec2*)data_ov006_0213ca24;
    *(Vec2*)(self + 8) = *(Vec2*)(self + 0);
}
