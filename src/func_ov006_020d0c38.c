/*
 * ov006 minigame: try to place a new line segment between two 16-bit
 * points (sl, sb).  Rejects segments shorter than 8 in x, with no
 * direction, or under the minimum half-length; when the full vector is
 * short of 0x30000 it re-scales the half vector (y stretched by 0xc00)
 * to a fixed length and rewrites both endpoints around the midpoint.
 * Then finds the first free slot (state byte +0x328 == 0) among the
 * three, rejecting it if the segment crosses any active (state 1) slot's
 * own segment (+0x50/+0x54), and initialises the slot through
 * func_ov006_020d01e0, returning its address (0 when none is free).
 *
 * Shape notes: the midpoint pair needs named x<<12 / y<<12 temps so the
 * y shift lands in place; the re-scale block reads half[1] inline at
 * both the multiply and the two-word copy into tmp (a named copy of it
 * takes r4 ahead of the umull low word).
 */
typedef short s16;

extern void func_0203b958(s16* o, s16* a, s16* b);
extern int func_0203d434(int* in);
extern int func_0203d614(int* v);
extern int _ZN4cstd4fdivEii(int, int);
extern void func_0203d630(int* p, int m);
extern void func_0203d704(int* o, int* a, int* b);
extern void func_0203d6d0(int* o, int* a, int* b);
extern void func_ov006_020d01e0(s16* slot, s16* a, s16* b);

extern char data_ov006_02140990[];
extern s16 data_ov006_02140cb4[];
extern s16 data_ov006_02140cb6[];
extern s16 data_ov006_02140cae[];

int func_ov006_020d0c38(s16* sl, s16* sb) {
    s16 d[2];
    int fx[2];
    int half[2];
    int mid[2];
    int tmp[2];
    int p1[2];
    int p2[2];
    s16 v0[2];
    s16 v1[2];
    s16 v2[2];
    s16 v3[2];
    s16 v4[2];
    s16 v5[2];
    s16 a[2];
    s16 b[2];
    s16 s0;
    s16 s1;
    int abs0;
    int i;
    int j;
    char* base;
    char* p;
    int cross1;
    int cross2;
    long long m;
    int scaled;
    int len;
    int off;
    s16* slot;

    func_0203b958(d, sl, sb);

    s0 = d[0];
    if (s0 < 0)
        abs0 = (s16)(-s0);
    else
        abs0 = s0;
    if (abs0 < 8)
        return 0;

    s1 = d[1];
    fx[0] = (int)s0 << 12;
    fx[1] = (int)s1 << 12;
    half[0] = ((int)s0 >> 1) << 12;
    half[1] = ((int)s1 >> 1) << 12;

    {
        int ay = (s16)sl[1];
        int by = (s16)sb[1];
        int ax = (s16)sl[0];
        int bx = (s16)sb[0];
        int y = (ay + by) >> 1;
        int x = (ax + bx) >> 1;
        int x12 = x << 12;
        int y12 = y << 12;
        mid[0] = x12;
        mid[1] = y12;
    }

    if (func_0203d434(half) == 0)
        return 0;

    {
        int ah = half[0];
        if (ah < 0)
            ah = -ah;
        if (ah < 1401)
            return 0;
    }

    if (func_0203d614(fx) < 0x30000) {
        m = (long long)half[1] * 0xc00 + 0x800;
        scaled = (int)(m >> 12);
        tmp[0] = half[0];
        tmp[1] = half[1];
        tmp[1] = scaled;
        len = func_0203d614(tmp);
        func_0203d630(half, _ZN4cstd4fdivEii(0x18000, len));

        func_0203d704(p1, mid, half);
        func_0203d6d0(p2, mid, half);

        sl[0] = (s16)(p1[0] >> 12);
        sl[1] = (s16)(p1[1] >> 12);
        sb[0] = (s16)(p2[0] >> 12);
        sb[1] = (s16)(p2[1] >> 12);
    }

    base = data_ov006_02140990;
    i = 0;
    do {
        if ((unsigned char)base[0x328] == 0) {
            p = data_ov006_02140990;
            j = 0;
            do {
                if (i != j) {
                    if ((unsigned char)p[0x328] == 1) {
                        func_0203b958(v0, sb, sl);
                        func_0203b958(v1, (s16*)(p + 0x50), sl);
                        func_0203b958(v2, (s16*)(p + 0x54), sl);

                        {
                            s16 ax = v0[0];
                            s16 ay = v0[1];
                            s16 bx = v1[0];
                            s16 by = v1[1];
                            s16 cy = v2[1];
                            cross1 = (int)ax * (int)by - (int)ay * (int)bx;
                            {
                                s16 cx = v2[0];
                                cross2 = (int)ax * (int)cy - (int)ay * (int)cx;
                            }
                        }
                        if (cross1 * cross2 <= 0) {
                            func_0203b958(v3, (s16*)(p + 0x54), (s16*)(p + 0x50));
                            v0[0] = v3[0];
                            v0[1] = v3[1];
                            func_0203b958(v4, sl, (s16*)(p + 0x50));
                            v1[0] = v4[0];
                            v1[1] = v4[1];
                            func_0203b958(v5, sb, (s16*)(p + 0x50));
                            v2[0] = v5[0];
                            v2[1] = v5[1];

                            {
                                s16 ax = v0[0];
                                s16 ay = v0[1];
                                s16 bx = v1[0];
                                s16 by = v1[1];
                                s16 cy = v2[1];
                                cross1 = (int)ax * (int)by - (int)ay * (int)bx;
                                {
                                    s16 cx = v2[0];
                                    cross2 = (int)ax * (int)cy - (int)ay * (int)cx;
                                }
                            }
                            if (cross1 * cross2 <= 0)
                                return 0;
                        }
                    }
                }
                j++;
                p += 0x32c;
            } while (j < 4);

            a[0] = sl[0];
            a[1] = sl[1];
            b[0] = sb[0];
            b[1] = sb[1];
            off = i * 0x32c;
            slot = (s16*)((int)data_ov006_02140990 + off);
            func_ov006_020d01e0(slot, a, b);
            *(s16*)((int)data_ov006_02140cb4 + off) = 1;
            *(s16*)((int)data_ov006_02140cb6 + off) = 0;
            *(s16*)((int)data_ov006_02140cae + off) = (s16)i;
            return (int)slot;
        }
        i++;
        base += 0x32c;
    } while (i < 3);

    return 0;
}
