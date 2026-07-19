typedef struct { int x, y, z; } Vec3;
typedef struct { signed char flag, amt; short x, y, z; } Elem;

extern void func_020effb8(void* a0, int m, Vec3* v0, Vec3* v1, Vec3* v2, Vec3* v3);
extern int func_02053200(int x);

int func_02008b4c(void* a0, short* p1, int* p2, char* tbl) {
    int idx;
    char* elem;
    int i;
    Vec3 v[4];
    int lo, hi;
    int m;

    idx = *p1;
    if (idx + 3 >= *(unsigned char*)tbl) {
        return 1;
    }

    elem = *(char**)(tbl + 4) + (idx << 3);

    if (*elem < 0 || *(elem + 8) < 0 || *(elem + 0x10) < 0) {
        return 1;
    }

    {
        Elem* et = (Elem*)elem;
        for (i = 0; i < 4; i++) {
            int x = et[i].x << 12;
            int z = et[i].z << 12;
            int y = et[i].y << 12;
            v[i].x = x;
            v[i].y = y;
            v[i].z = z;
        }
    }

    func_020effb8(a0, *p2, &v[0], &v[1], &v[2], &v[3]);

    lo = 0;
    hi = 0;
    if (*(unsigned char*)(elem + 9) != 0) {
        lo = func_02053200(*(unsigned char*)(elem + 9) << 12);
    }
    if (*(unsigned char*)(elem + 0x11) != 0) {
        hi = func_02053200(*(unsigned char*)(elem + 0x11) << 12);
    }

    {
        int d = hi - lo;
        m = *p2;
        *p2 = lo + (int)(((long long)d * m + 0x800) >> 12) + m;
    }

    if (*p2 >= 0x1000) {
        *p1 = *p1 + 1;
        *p2 -= 0x1000;
    }

    return 0;
}
