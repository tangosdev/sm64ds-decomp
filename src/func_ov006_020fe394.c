// NONMATCHING: size 0x3ac vs target 0x3bc (div=size). Whole body is present and
// every instruction reproduced; residue is codegen only: in branch A the ROM
// reloads data_020a0dea[k*4] / data_020a0deb[k*4] from memory where mwcc CSEs
// them, plus the dist<0x10 snap block is laid out inline instead of at the tail.
// Logic verified instruction-by-instruction vs the ROM disassembly.
// dScMgPachinko_c state slot 0: per-frame homing of a launched bob-omb toward
// the target (data_020a0de8[] course table), sqrt/atan2 velocity, hit sounds.
typedef unsigned char u8;
typedef unsigned short u16;

extern u8 data_020a0e40;
extern u8 data_020a0de8[];
extern u8 data_020a0dea[];
extern u8 data_020a0deb[];
extern const short data_02082214[];

extern int _ZN4cstd4sqrtEy(long long);
extern int _ZN4cstd5atan2E5Fix12IiES1_(int, int);
extern int Sound_PlayIfNotActive(int, int, int, int);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned);
extern void func_ov006_020fbd38(char *o);

void func_ov006_020fe394(char *o, int i)
{
    int k = data_020a0e40;

    if (data_020a0de8[k * 4] != 0)
    {
        int idx = i * 0x38;
        int t, u, dx, dy, cx, cy, dist, prev;

        *(int *)(o + 0x4ed8 + idx) = *(int *)(o + 0x4ee8 + idx) + (data_020a0dea[k * 4] << 12);
        *(int *)(o + 0x4edc + idx) = *(int *)(o + 0x4eec + idx) + (data_020a0deb[k * 4] << 12);

        t = *(int *)(o + 0x4ed8 + idx) >> 12;
        if (t >= 0xf8) *(int *)(o + 0x4ed8 + idx) = 0xf8000;
        if (t <= 8)    *(int *)(o + 0x4ed8 + idx) = 0x8000;

        u = *(int *)(o + 0x4edc + idx) >> 12;
        if (u >= 0xb8) *(int *)(o + 0x4edc + idx) = 0xb8000;
        if (u <= 8)    *(int *)(o + 0x4edc + idx) = 0x8000;

        dx = (*(int *)(o + 0x4ed8 + idx) >> 12) - data_020a0dea[k * 4];
        dy = (*(int *)(o + 0x4edc + idx) >> 12) - data_020a0deb[k * 4];
        *(int *)(o + 0x4ee8 + idx) = dx << 12;
        *(int *)(o + 0x4eec + idx) = dy << 12;

        cy = 0x20 - (*(int *)(o + 0x4edc + idx) >> 12);
        cx = 0x80 - (*(int *)(o + 0x4ed8 + idx) >> 12);
        dist = _ZN4cstd4sqrtEy((long long)(cy * cy + cx * cx));

        prev = *(int *)(o + 0x4f04 + idx);
        *(int *)(o + 0x4f04 + idx) = dist;
        if (dist > prev)
        {
            if (dist > prev + 0xa)
                *(int *)(o + 0x4f00 + idx) = Sound_PlayIfNotActive(*(int *)(o + 0x4f00 + idx), 2, 0x17b, 0);
            else
                *(int *)(o + 0x4f00 + idx) = Sound_PlayIfNotActive(*(int *)(o + 0x4f00 + idx), 2, 0x17c, 0);
        }
    }
    else
    {
        int idx = i * 0x38;
        int *pf0 = (int *)(o + 0x4ed8 + idx);
        int *pf4 = (int *)(o + 0x4edc + idx);
        u8 *pf35 = (u8 *)(o + 0x4f0d + idx);
        int cx, cy, distSq, dist;

        *pf35 = 2;

        cy = 0x20 - (*pf4 >> 12);
        cx = 0x80 - (*pf0 >> 12);
        distSq = cy * cy + cx * cx;
        dist = _ZN4cstd4sqrtEy((long long)distSq);

        if (dist < 0x10)
        {
            *pf0 = 0x80000;
            *pf4 = 0x28000;
            *pf35 = 0;
        }
        else
        {
            *(u16 *)(o + 0x4f08 + idx) = _ZN4cstd5atan2E5Fix12IiES1_(cy, cx);

            *(int *)(o + 0x4ef8 + idx) = _ZN4cstd4sqrtEy((long long)distSq) << 8;
            *(int *)(o + 0x4ef8 + idx) += *(int *)(o + 0x4ef8 + idx) >> 2;
            *(int *)(o + 0x4ef8 + idx) += *(int *)(o + 0x4ef8 + idx) >> 2;
            *(int *)(o + 0x4ef8 + idx) += *(int *)(o + 0x4ef8 + idx) >> 2;
            if (*(int *)(o + 0x4ef8 + idx) >= 0xa000) *(int *)(o + 0x4ef8 + idx) = 0xa000;

            *(u16 *)(o + 0x5c1c) = *(int *)(o + 0x4ef8 + idx) >> 10;
            if (*(u16 *)(o + 0x5c1c) == 0) *(u16 *)(o + 0x5c1c) = 1;
            *(u8 *)(o + 0x5c31) = 0;

            *(int *)(o + 0x4ee0 + idx) = (int)(((long long)data_02082214[(*(u16 *)(o + 0x4f08 + idx) >> 4) * 2 + 1] * *(int *)(o + 0x4ef8 + idx) + 0x800) >> 12);
            *(int *)(o + 0x4ee4 + idx) = (int)(((long long)data_02082214[(*(u16 *)(o + 0x4f08 + idx) >> 4) * 2] * *(int *)(o + 0x4ef8 + idx) + 0x800) >> 12);
            *(u16 *)(o + 0x4f0a + idx) = 0;

            if (dist >= 0x40)
                _ZN5Sound12PlayBank2_2DEj(0x17e);
            else
                _ZN5Sound12PlayBank2_2DEj(0x17d);
        }
    }

    func_ov006_020fbd38(o);
}
