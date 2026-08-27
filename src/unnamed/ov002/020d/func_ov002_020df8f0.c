typedef short s16;
typedef unsigned short u16;
typedef unsigned char u8;
typedef unsigned int u32;
typedef long long s64;
typedef unsigned long long u64;

extern int _ZN4cstd4sqrtEy(u64 v);
extern void _Z15ApproachLinear2Rsss(s16* p, s16 target, s16 step);
extern u8 data_020a0e40[];
extern s16 data_0209f4a2[];
extern s16 data_0209f4a4[];
extern s16 data_02082214[];

#pragma opt_common_subs off
void func_ov002_020df8f0(char* c)
{
    int vb = *(int*)(c + 0xa8);
    int va = *(int*)(c + 0x98);
    int b = vb >> 8;
    int sq = b * b;
    int a = va >> 8;
    sq += a * a;
    *(int*)(c + 0x640) = _ZN4cstd4sqrtEy((u64)sq) << 8;

    {
        int idx = data_020a0e40[0] * 0x18;
        s16 tval = *(s16*)((char*)data_0209f4a4 + idx);
        int div5 = *(int*)(c + 0x640) / 5;
        int v = (int)(((s64)(tval >> 6) * div5 + 0x800) >> 12);
        if (v < -0x8000) v = -0x8000;
        if (v >= 0x8000) v = 0x7fff;

        if (v > 0) {
            if (*(s16*)(c + 0x69e) < 0) {
                *(s16*)(c + 0x69e) += 0x40;
                if (*(s16*)(c + 0x69e) > 0x20) *(s16*)(c + 0x69e) = 0x20;
            } else {
                s16 step;
                if (*(s16*)(c + 0x69e) < v) step = 0x20; else step = 0x40;
                _Z15ApproachLinear2Rsss((s16*)(c + 0x69e), (s16)v, step);
            }
        } else if (v < 0) {
            if (*(s16*)(c + 0x69e) > 0) {
                *(s16*)(c + 0x69e) -= 0x40;
                if (*(s16*)(c + 0x69e) < -0x20) *(s16*)(c + 0x69e) = -0x20;
            } else {
                s16 step;
                if (*(s16*)(c + 0x69e) < v) step = 0x40; else step = 0x20;
                _Z15ApproachLinear2Rsss((s16*)(c + 0x69e), (s16)v, step);
            }
        } else {
            _Z15ApproachLinear2Rsss((s16*)(c + 0x69e), 0, 0x40);
        }
    }

    {
        int idx = data_020a0e40[0] * 0x18;
        int u2 = *(int*)(c + 0x640);
        s16 tval2 = *(s16*)((char*)data_0209f4a2 + idx);
        int quarter = u2 / 4;
        int v2 = 0 - (int)(((s64)(tval2 >> 6) * quarter + 0x800) >> 12);
        if (v2 < -0x8000) v2 = -0x8000;
        if (v2 >= 0x8000) v2 = 0x7fff;

        if (v2 > 0) {
            if (*(s16*)(c + 0x69c) < 0) {
                *(s16*)(c + 0x69c) += 0x40;
                if (*(s16*)(c + 0x69c) > 0x10) *(s16*)(c + 0x69c) = 0x10;
            } else {
                s16 step;
                if (*(s16*)(c + 0x69c) < v2) step = 0x10; else step = 0x20;
                _Z15ApproachLinear2Rsss((s16*)(c + 0x69c), (s16)v2, step);
            }
        } else if (v2 < 0) {
            if (*(s16*)(c + 0x69c) > 0) {
                *(s16*)(c + 0x69c) -= 0x40;
                if (*(s16*)(c + 0x69c) < -0x10) *(s16*)(c + 0x69c) = -0x10;
            } else {
                s16 step;
                if (*(s16*)(c + 0x69c) < v2) step = 0x20; else step = 0x10;
                _Z15ApproachLinear2Rsss((s16*)(c + 0x69c), (s16)v2, step);
            }
        } else {
            _Z15ApproachLinear2Rsss((s16*)(c + 0x69c), 0, 0x40);
        }
    }

    *(s16*)(c + 0x94) += *(s16*)(c + 0x69c);
    *(s16*)(c + 0x96) = (s16)((0 - *(s16*)(c + 0x69c)) * 0x14);

    *(int*)(c + 0x640) -= ((*(s16*)(c + 0x92) >> 1) + 0x100);
    if (*(int*)(c + 0x640) < 0) *(int*)(c + 0x640) = -*(int*)(c + 0x640);

    {
        int *p = (int*)(c + 0x640);
        *p -= (0x1000 - data_02082214[((u16)(*(s16*)(c + 0x69c)) >> 4) * 2 + 1]) >> 1;
    }
    if (*(int*)(c + 0x640) < 0) *(int*)(c + 0x640) = 0;

    if (*(int*)(c + 0x640) > 0x10000) {
        *(s16*)(c + 0x92) += ((*(int*)(c + 0x640) - 0x20000) * 6) >> 12;
    } else if (*(int*)(c + 0x640) <= 0x4000) {
        *(s16*)(c + 0x92) -= 0x600;
    } else {
        *(s16*)(c + 0x92) += ((*(int*)(c + 0x640) - 0x20000) * 0xa) >> 12;
    }

    *(s16*)(c + 0x92) += *(s16*)(c + 0x69e);
    if (*(s16*)(c + 0x92) > 0x2aaa) *(s16*)(c + 0x92) = 0x2aaa;
    if (*(s16*)(c + 0x92) < (s16)0xd556) *(s16*)(c + 0x92) = (s16)0xd556;

    *(int*)(c + 0xa8) = (int)(((s64)*(int*)(c + 0x640) * data_02082214[(*(u16*)(c + 0x92) >> 4) * 2] + 0x800) >> 12);
    *(int*)(c + 0x98) = (int)(((s64)*(int*)(c + 0x640) * data_02082214[(*(u16*)(c + 0x92) >> 4) * 2 + 1] + 0x800) >> 12);

    {
        *(s16*)(c + 0x8c) = *(s16*)(c + 0x92);
        *(s16*)(c + 0x8e) = *(s16*)(c + 0x94);
        *(s16*)(c + 0x90) = *(s16*)(c + 0x96);
        s16 r = (s16)(0 - *(s16*)(c + 0x92));
        int spd = *(int*)(c + 0x640);
        int t = spd >> 5;
        if (t >= 0x1000) t = 0x1000;
        s16 P = (s16)(int)(((s64)t * 0x2aaaU + 0x800) >> 12);
        if (r > P) r = P;
        if (r < -P) r = (s16)(-P);
        *(s16*)(c + 0x8c) = r;
    }
}
