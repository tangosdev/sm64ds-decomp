// @symbol func_ov098_0213aa28
/* recovered: shared common types, declarations from a shared header */
#include "decl_common.h"
/* recovered: shared common types */
#include "common.h"
extern void _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(void* self, struct Vector3* v, int f);
extern void _ZN5Sound9PlayBank3EjRK7Vector3(unsigned int id, struct Vector3* v);
extern int _Z14ApproachLinearRsss(short* r, short t, short s);
extern short GetAngleToCamera(int i);
extern int _ZN6Player22IsBeingShotOutOfCannonEv(char* c);

void func_ov098_0213aa28(char* c)
{
    struct Vector3 vs[2];
    unsigned char* state;
    int* p174;
    int* p60;
    int* p13c;

    if (*(unsigned char*)(c + 0x185) < 4) {
        char* p;
        int y, z, x;
        /* z first (statement order) colors y into r2 like the ROM */
        z = *(int*)(c + 0x64);
        y = *(int*)(c + 0x60) + 0xd0000;
        p = *(char**)(c + 0x158);
        x = *(int*)(c + 0x5c);
        *(int*)(p + 0x5c) = x;
        *(int*)(p + 0x60) = y;
        *(int*)(p + 0x64) = z;
        if (*(unsigned char*)(c + 0x185) < 3) {
            short ay = *(short*)(c + 0x8e);
            short ax = *(short*)(c + 0x8c);
            p = *(char**)(c + 0x158);
            short z0 = 0;
            *(short*)(p + 0x8c) = ax;
            *(short*)(p + 0x8e) = ay;
            *(short*)(p + 0x90) = z0;
            ay = *(short*)(c + 0x8e);
            ax = *(short*)(c + 0x8c);
            p = *(char**)(c + 0x158);
            *(short*)(p + 0x92) = ax;
            *(short*)(p + 0x94) = ay;
            *(short*)(p + 0x96) = z0;
        }
    }

    switch (*(unsigned char*)(c + 0x185)) {
    case 0:
        p60 = (int*)(((long long)(int)(c + 0x60)));
        *p60 = *p60 + 0xa000;
        if (*(int*)(c + 0x60) < *(int*)(c + 0x160))
            goto end;
        *(int*)(c + 0x60) = *(int*)(c + 0x160);
        state = (unsigned char*)(((long long)(int)(c + 0x185)));
        (*state)++;
        *(int*)(c + 0x174) = 0;
        vs[1].x = *(int*)(c + 0x5c);
        vs[1].y = *(int*)(c + 0x60);
        vs[1].z = *(int*)(c + 0x64);
        _ZN5Actor10EarthquakeERK7Vector35Fix12IiE(c, &vs[1], 0x1f4000);
        _ZN5Sound9PlayBank3EjRK7Vector3(0x14c, (struct Vector3*)(c + 0x74));
        break;

    case 1:
        if (*(int*)(c + 0x174) <= 0xa)
            goto end;
        if (!_Z14ApproachLinearRsss((short*)(c + 0x8e), *(short*)(c + 0x17a), 0x200))
            goto end;
        state = (unsigned char*)(((long long)(int)(c + 0x185)));
        (*state)++;
        *(int*)(c + 0x174) = 0;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x14d, (struct Vector3*)(c + 0x74));
        break;

    case 2:
        if (*(int*)(c + 0x174) <= 0xa)
            goto end;
        if (!_Z14ApproachLinearRsss((short*)(c + 0x8c), 0x2000, 0x200))
            goto end;
        if (!func_ov002_020df300(*(char**)(c + 0x158)))
            goto end;
        _ZN5Sound9PlayBank3EjRK7Vector3(0x14e, (struct Vector3*)(c + 0x74));
        *(int*)(c + 0x174) = 0;
        state = (unsigned char*)(((long long)(int)(c + 0x185)));
        (*state)++;
        break;

    case 3:
        *(short*)(c + 0x8e) = GetAngleToCamera(*(unsigned char*)(*(char**)(c + 0x158) + 0x6d8)) + 0x4000;
        if (!_ZN6Player22IsBeingShotOutOfCannonEv(*(char**)(c + 0x158)))
            goto end;
        state = (unsigned char*)(((long long)(int)(c + 0x185)));
        (*state)++;
        *(int*)(c + 0x174) = 0;
        break;

    case 4:
    {
        unsigned char cnt = 0;
        p60 = (int*)(((long long)(int)(c + 0x60)));
        *p60 = *p60 - 0x32000;
        if (_Z14ApproachLinearRsss((short*)(c + 0x8c), 0, 0x800))
            cnt++;
        if (_Z14ApproachLinearRsss((short*)(c + 0x8e), *(short*)(c + 0x17c), 0x800))
            cnt++;
        if (cnt != 2)
            goto end;
        if (*(int*)(c + 0x60) > *(int*)(c + 0x160) - 0x190000)
            goto end;
        *(int*)(c + 0x180) = 2;
        p13c = (int*)(((long long)(int)(c + 0x13c)));
        *p13c = *p13c & ~1;
        *(int*)(c + 0x60) = *(int*)(c + 0x160) - 0x190000;
        *(int*)(c + 0x158) = 0;
        *(short*)(c + 0x8e) = *(short*)(c + 0x17c);
        *(short*)(c + 0x94) = *(short*)(c + 0x8e);
        break;
    }
    }
end:
    p174 = (int*)(((long long)(int)(c + 0x174)));
    (*p174)++;
}
