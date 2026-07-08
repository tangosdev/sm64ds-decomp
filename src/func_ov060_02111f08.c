struct Vector3 { int x, y, z; };

extern void* CAMERA;
extern short SINE_TABLE[];

extern void* _ZN5Actor13ClosestPlayerEv(void);
extern void _ZN6Camera9SetFlag_3Ev(void*);
extern void _ZN6Camera9SetLookAtERK7Vector3(void*, const struct Vector3*);
extern void _ZN6Camera6SetPosERK7Vector3(void*, const struct Vector3*);
extern int Vec3_HorzDist(const struct Vector3*, const struct Vector3*);
extern short Vec3_HorzAngle(const struct Vector3*, const struct Vector3*);
extern int func_020092c4(void*, void*, void*);
extern int _ZN6Player7IsInAirEv(void*);
extern void _Z14ApproachLinearRiii(int*, int, int);

int func_ov060_02111f08(void* arg0)
{
    char* self = (char*)arg0;
    void* cam = CAMERA;
    char* player = (char*)_ZN5Actor13ClosestPlayerEv();
    struct Vector3 sp;
    struct Vector3* pv;
    unsigned char* p;
    int dist;
    int v;
    int k;
    int d;

    if (player == 0)
        return 1;

    switch (*(unsigned char*)(self + 0x444)) {
    case 0:
        _ZN6Camera9SetFlag_3Ev(cam);
        pv = (struct Vector3*)(((long long)(int)(player + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        sp.x = pv->x;
        sp.y = pv->y;
        sp.z = pv->z;
        sp.y = *(int*)(self + 0x60);
        _ZN6Camera9SetLookAtERK7Vector3(cam, &sp);
        *(int*)(self + 0x42c) = sp.x;
        *(int*)(self + 0x430) = sp.y;
        *(int*)(self + 0x434) = sp.z;
        dist = Vec3_HorzDist((struct Vector3*)(self + 0x5c), &sp);
        k = (unsigned short)(short)(Vec3_HorzAngle((struct Vector3*)(self + 0x5c), &sp) - 0x2000) >> 4;
        v = (int)(((long long)dist * 0xA00 + 0x800) >> 12);
        *(int*)(self + 0x438) = *(int*)(self + 0x5c) + (int)(((long long)v * SINE_TABLE[k * 2] + 0x800) >> 12);
        *(int*)(self + 0x43c) = *(int*)(self + 0x60) + 0xc8000;
        *(int*)(self + 0x440) = *(int*)(self + 0x64) + (int)(((long long)v * SINE_TABLE[k * 2 + 1] + 0x800) >> 12);
        _ZN6Camera6SetPosERK7Vector3(cam, (struct Vector3*)(self + 0x438));
        p = (unsigned char*)(((long long)(int)(self + 0x444)) & 0xFFFFFFFFFFFFFFFFLL);
        *p = *p + 1;
        break;
    case 1:
        func_020092c4(cam, (char*)cam + 0x80, self + 0x42c);
        if (_ZN6Player7IsInAirEv(player) != 0)
            *(unsigned char*)(self + 0x445) = 0;
        else {
            p = (unsigned char*)(((long long)(int)(self + 0x445)) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + 1;
        }
        if (*(unsigned char*)(self + 0x445) > 0x1e) {
            p = (unsigned char*)(((long long)(int)(self + 0x444)) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + 1;
        }
        break;
    case 2:
        pv = (struct Vector3*)(((long long)(int)(player + 0x5c)) & 0xFFFFFFFFFFFFFFFFLL);
        sp.x = pv->x;
        sp.y = pv->y;
        sp.z = pv->z;
        dist = Vec3_HorzDist((struct Vector3*)(self + 0x5c), &sp);
        d = (int)(((long long)dist * 0x600 + 0x800) >> 12);
        if (d < 0x1f4000)
            d = 0x1f4000;
        k = (unsigned short)(short)(Vec3_HorzAngle((struct Vector3*)(self + 0x5c), &sp) - 0x1000) >> 4;
        sp.x = *(int*)(self + 0x5c) + (int)(((long long)d * SINE_TABLE[k * 2] + 0x800) >> 12);
        sp.z = *(int*)(self + 0x64) + (int)(((long long)d * SINE_TABLE[k * 2 + 1] + 0x800) >> 12);
        _Z14ApproachLinearRiii((int*)(self + 0x438), sp.x, 0x4000);
        _Z14ApproachLinearRiii((int*)(self + 0x440), sp.z, 0x4000);
        _Z14ApproachLinearRiii((int*)(self + 0x42c), *(int*)(self + 0x5c), 0x1e000);
        _Z14ApproachLinearRiii((int*)(self + 0x434), *(int*)(self + 0x64), 0x1e000);
        func_020092c4(cam, (char*)cam + 0x8c, self + 0x438);
        if (func_020092c4(cam, (char*)cam + 0x80, self + 0x42c) != 0) {
            p = (unsigned char*)(((long long)(int)(self + 0x444)) & 0xFFFFFFFFFFFFFFFFLL);
            *p = *p + 1;
        }
        break;
    case 3: {
        int ty, tz, tx;
        k = (int)(*(unsigned short*)(self + 0x8e)) >> 4;
        tz = SINE_TABLE[k * 2 + 1] * 0xc0 + *(int*)(self + 0x64);
        ty = *(int*)(self + 0x60) + 0xfa000;
        tx = SINE_TABLE[k * 2] * 0xc0 + *(int*)(self + 0x5c);
        sp.x = tx;
        sp.y = ty;
        sp.z = tz;
    }
        _Z14ApproachLinearRiii((int*)(self + 0x42c), sp.x, 0xa000);
        _Z14ApproachLinearRiii((int*)(self + 0x430), sp.y, 0xa000);
        _Z14ApproachLinearRiii((int*)(self + 0x434), sp.z, 0xa000);
        if (func_020092c4(cam, (char*)cam + 0x80, self + 0x42c) != 0)
            return 1;
        break;
    case 4:
        *(int*)(((long long)(int)((char*)cam + 0x154)) & 0xFFFFFFFFFFFFFFFFLL) &= ~8;
        break;
    default:
        break;
    }
    return 0;
}
