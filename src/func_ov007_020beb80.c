typedef unsigned char u8;
typedef unsigned short u16;
typedef long long s64;
typedef struct { int x, y, z; } Vec3i;
typedef struct { short x, y, z; } Vec3s;

extern int func_020538b8(int a, int b);
extern int func_ov007_020b7690(int i);
extern int func_02053644(void *v);
extern int _ZN4cstd4fdivEii(int a, int b);
extern void func_02053380(void *a, void *b);
void func_ov007_020beb80(char *self) {
    char *B = *(char **)(*(char **)(self + 4));
    char *P = *(char **)(B + 0xc);
    int *q = (int *)(int)(((long long)(unsigned)(P + 0x18)));
    int r;
    Vec3s dd;
    char *e;
    int frames;
    int vel;
    int sc;
    Vec3i *tp = (Vec3i *)(int)(((long long)(int)(P + 0x24)));
    r = func_020538b8(q[0], q[2]);
    dd.x = (short)(tp->x - *(int *)(self + 0xc));
    dd.y = (short)(tp->y - *(int *)(self + 0x10));
    dd.z = (short)(tp->z - *(int *)(self + 0x14));
    if (*(int *)(*(char **)(self + 4) + 0x98) != 0) {
        *(int *)(self + 8) = r;
        *(Vec3i *)(self + 0xc) = *tp;
    }
    e = (char *)func_ov007_020b7690(*(u8 *)(self + 2));
    {
        char *A = *(char **)(self + 4);
        if (*(int *)(A + 0x94) != 0 && (unsigned)*(*(int **)(A + 0x18)) <= 1) {
            vel = 0;
            frames = 0x1000;
            if (dd.x != 0 || dd.y != 0 || dd.z != 0) {
                int mag = (short)func_02053644(&dd);
                int t;
                if (mag <= 0) sc = 0;
                else if (mag >= 0xfa0) sc = 0x1000;
                else sc = _ZN4cstd4fdivEii(mag, 0xfa0);
                if (mag <= 0x1f4) t = 0;
                else if (mag >= 0x3e8) t = 0x1000;
                else t = _ZN4cstd4fdivEii(mag - 0x1f4, 0x1f4);
                frames -= t;
                func_02053380(&dd, &dd);
                *(Vec3s *)(e + 0x3c) = dd;
                vel = (short)(((s64)sc * 0x460 + 0x800) >> 12);
            }
            *(u8 *)(e + 0x58) = (u8)(((frames << 2) >> 12) + 1);
            *(int *)(e + 0x4c) = vel;
            *(int *)(((int)e + 0x1c)) &= ~2;
            *(Vec3i *)(e + 0x20) = *tp;
        } else {
            *(int *)(((int)e + 0x1c)) |= 2;
        }
    }
}
