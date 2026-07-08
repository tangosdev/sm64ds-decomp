//cpp
extern "C" {
struct Vec3 { int x, y, z; };
extern unsigned char CURRENT_GAMEMODE;
extern int _ZNK6Player14GetBodyModelIDEjb(void *c, unsigned int a, bool b);
extern void MulVec3Mat4x3(void *out, void *mtx, void *vec);
extern void Vec3_MulScalar(void *out, void *vec, int s);
extern void func_ov002_020dc174(char *c, void *r1, int r2, int r3, unsigned int a5, unsigned int a6);

void func_ov002_020db8d8(char *self)
{
    volatile Vec3 backup;
    Vec3 tmp;
    Vec3 dir;
    Vec3 scaled;
    int model;
    char *m;
    int b;
    int cond;
    backup.x = *(int *)(self + 0x5c);
    backup.y = *(int *)(self + 0x60);
    backup.z = *(int *)(self + 0x64);
    b = *(unsigned char *)(self + 0x714);
    m = (char *)(*(int *)(*(int *)(self + ((b << 2) + 3) * 4 + 0x154) + 0x14) + 0x90);
    model = _ZNK6Player14GetBodyModelIDEjb(self, *(int *)(self + 8) & 0xff, 0);
    MulVec3Mat4x3(m + 0x24, (char *)(*(int *)(self + model * 4 + 0xdc)) + 0x1c, &tmp);
    Vec3_MulScalar(&scaled, &tmp, 0x8000);
    *(int *)(self + 0x5c) = scaled.x;
    *(int *)(self + 0x60) = scaled.y;
    *(int *)(self + 0x64) = scaled.z;
    dir.x = 0;
    dir.z = 0;
    dir.y = -0x32000;
    cond = (CURRENT_GAMEMODE == 1);
    if (cond != 0) {
        func_ov002_020dc174(self, &dir, 0x46000, 0x96000, 0x8000, 0);
    } else {
        func_ov002_020dc174(self, &dir, 0x32000, 0x64000, 0x8000, 0);
    }
    *(int *)(self + 0x5c) = backup.x;
    *(int *)(self + 0x60) = backup.y;
    *(int *)(self + 0x64) = backup.z;
}
}
