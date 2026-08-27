typedef long long s64;
typedef unsigned long long u64;

extern int _ZN4cstd4fdivEii(int, int);
extern int _ZN4cstd11fdiv_resultEv(void);
extern s64 _ZN4cstd11ldiv_resultEv(void);

void _ZN3G3i13PerspectiveW_E5Fix12IiES1_S1_S1_S1_S1_bP9Matrix4x3(
    int sinF, int cosF, int aspect, int n, int f, int scaleW, int flag, int *mtx)
{
    int tmp;
    volatile int *port;
    int xx;
    s64 q;
    int zz;
    int v;
    int wz;

    tmp = _ZN4cstd4fdivEii(cosF, sinF);
    if (scaleW != 0x1000)
        tmp = tmp * scaleW / 0x1000;

    *(volatile u64 *)0x4000290 = (u64)(unsigned)tmp << 32;
    *(volatile u64 *)0x4000298 = (unsigned)aspect;
    if (flag) {
        *(volatile int *)0x4000440 = 0;
        port = (volatile int *)0x4000458;
    }
    if (mtx) {
        mtx[1] = 0;
        mtx[2] = 0;
        mtx[3] = 0;
        mtx[4] = 0;
        mtx[6] = 0;
        mtx[7] = 0;
        mtx[8] = 0;
        mtx[9] = 0;
        mtx[11] = -scaleW;
        mtx[12] = 0;
        mtx[13] = 0;
        mtx[15] = 0;
    }
    xx = _ZN4cstd11fdiv_resultEv();
    *(volatile u64 *)0x4000290 = (u64)0x1000 << 32;
    *(volatile u64 *)0x4000298 = (unsigned)(n - f);
    if (flag) {
        *port = xx;
        *port = 0;
        *port = 0;
        *port = 0;
        *port = 0;
        *port = tmp;
        *port = 0;
        *port = 0;
        *port = 0;
        *port = 0;
    }
    if (mtx) {
        mtx[0] = xx;
        mtx[5] = tmp;
    }
    q = _ZN4cstd11ldiv_resultEv();
    if (scaleW != 0x1000)
        q = q * scaleW / 0x1000;
    zz = (int)(((f + n) * q + 0x80000000LL) >> 32);
    v = (int)(((s64)(n << 1) * f + 0x800) >> 12);
    wz = (int)((q * v + 0x80000000LL) >> 32);
    if (flag) {
        *port = zz;
        *port = -scaleW;
        *port = 0;
        *port = 0;
        *port = wz;
        *port = 0;
    }
    if (mtx) {
        mtx[10] = zz;
        mtx[14] = wz;
    }
}
