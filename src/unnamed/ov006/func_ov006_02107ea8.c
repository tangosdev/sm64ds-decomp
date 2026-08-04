typedef struct { int m[12]; } Mtx43;
typedef struct { int x, y, z; } Vec3;
typedef struct { short v[12]; } ShortTbl12;

extern int _ZN4cstd4fdivEii(int a, int b);
extern void _Z14ApproachLinearRiii(int *ref, int target, int step);
extern void _ZN5Sound12PlayBank2_2DEj(unsigned int id);
extern void Matrix4x3_ApplyInPlaceToTranslation(Mtx43 *mF, int x, int y, int z);

extern short data_02082214[];
extern short data_ov006_0213e334[];
extern short data_ov006_0212ed34[];
extern Mtx43 data_020a0e68;

void func_ov006_02107ea8(char *self)
{
    int absv;
    int d1;
    short sum;
    ShortTbl12 buf;
    short diff;
    short idx;
    int extra;
    int angidx, idx2;
    short t0, t1;
    int x, y, z;
    Vec3 t;

    absv = *(int *)(self + 0xb8) - 0x140;
    if (absv < 0) absv = -absv;
    d1 = _ZN4cstd4fdivEii((int)((((long long)absv * absv) + 0x800) >> 12), *(int *)(self + 0xb0)) - 0xc00;

    *(short *)(self + 0xc6) = _ZN4cstd4fdivEii(
        (int)((((long long)(*(int *)(self + 0xb8)) * 0x500) + 0x800) >> 12),
        *(int *)(self + 0xb0));

    *(short *)(((int)self + 0xc0)) += *(short *)(self + 0xc6);

    *(int *)(self + 0xb4) = (int)((((long long)(*(int *)(self + 0xb4)) * 0x600) + 0x800) >> 12);
    *(int *)(((int)self + 0xb4)) += d1;
    *(int *)(((int)self + 0xb0)) += *(int *)(self + 0xb4);

    if (*(int *)(self + 0xb0) > 0x2c00) {
        *(int *)(self + 0xb0) = 0x2c00;
        *(int *)(self + 0xb4) = 0;
    } else if (*(int *)(self + 0xb0) < 0x1600) {
        *(int *)(self + 0xb0) = 0x1600;
        *(int *)(self + 0xb4) = 0;
    }

    sum = *(short *)(self + 0xc0) + *(short *)(self + 0xc4);

    buf = *(ShortTbl12 *)data_ov006_0213e334;

    for (idx = 0; idx < 12; idx++) {
        short ad;
        diff = sum - buf.v[idx];
        ad = diff < 0 ? (short)(-diff) : diff;
        if (ad < 0xaab) break;
    }

    *(short *)(self + 0xca) = data_ov006_0212ed34[idx];

    extra = 0;
    if (*(int *)(self + 0xb0) < 0x2000) {
        if (diff < -0x400 && (diff - *(short *)(self + 0xc6)) >= -0x400) {
            *(short *)(self + 0xc0) = (buf.v[idx] - 0x400) - *(short *)(self + 0xc4);
            *(int *)(self + 0xb8) = -(int)((((long long)(*(int *)(self + 0xb8)) * 0xf00) + 0x800) >> 12);
            _Z14ApproachLinearRiii((int *)(self + 0xb8), 0xfffff667, 0x30);
            if (*(short *)(self + 0xcc) != 0) {
                _ZN5Sound12PlayBank2_2DEj(0x159);
                *(short *)(self + 0xcc) = 0;
            }
        } else if (diff > 0x400 && (diff - *(short *)(self + 0xc6)) <= 0x400) {
            *(short *)(self + 0xc0) = (buf.v[idx] + 0x400) - *(short *)(self + 0xc4);
            *(int *)(self + 0xb8) = -(int)((((long long)(*(int *)(self + 0xb8)) * 0xe00) + 0x800) >> 12);
            if (*(short *)(self + 0xcc) != 0) {
                _ZN5Sound12PlayBank2_2DEj(0x159);
                *(short *)(self + 0xcc) = 0;
            }
        } else {
            if (diff < 0) diff = -diff;
            if (diff > 0x400) {
                *(int *)(self + 0xb0) = 0x2000;
                *(int *)(self + 0xb4) = 0;
            }
        }
        extra = (int)((((long long)(*(int *)(self + 0xb0) - 0x2000) * 0x100) + 0x800) >> 12);
    }

    _Z14ApproachLinearRiii((int *)(self + 0xb8), 0, 0x30);

    angidx = ((unsigned short)(sum + *(short *)(self + 0xc2))) >> 4;
    idx2 = angidx * 2;
    t0 = data_02082214[idx2 + 1];
    t1 = data_02082214[idx2];

    x = (int)((((long long)(*(int *)(self + 0xb0)) * t0) + 0x800) >> 12);
    y = (int)((((long long)(*(int *)(self + 0xb0)) * 0x400) + 0x800) >> 12) + 0x780 + extra;
    z = (int)((((long long)(*(int *)(self + 0xb0)) * t1) + 0x800) >> 12);
    ((int *)&t)[0] = x; ((int *)&t)[1] = y; ((int *)&t)[2] = z;
    Matrix4x3_ApplyInPlaceToTranslation(&data_020a0e68, ((int *)&t)[0], ((int *)&t)[1], ((int *)&t)[2]);

    *(Mtx43 *)(self + 0x7c) = data_020a0e68;
}
