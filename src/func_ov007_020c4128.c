typedef struct { int x, y, z; } Vec3;

extern void func_ov007_020c4388(char* r0, int r1);
extern void SubVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern void func_ov007_020c421c(char *r4);

#define ADDR(p) ((int *)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov007_020c4128(char *self, char *dest, int arg2)
{
    int save10, save4, save8;

    *(int *)(dest + 0x3c) = *(int *)(self + 0x3c);
    *(int *)(dest + 0x40) = *(int *)(self + 0x40);
    *(unsigned short *)(dest + 0x44) = *(unsigned short *)(self + 0x44);
    *(int *)(dest + 0x50) = *(int *)(self + 0x50);

    save10 = *(int *)(dest + 0x10);
    save4 = *(int *)(dest + 4);
    save8 = *(int *)(dest + 8);

    *ADDR(dest + 4) = *ADDR(dest + 4) + *(int *)(self + 4);
    *ADDR(dest + 8) = *ADDR(dest + 8) + *(int *)(self + 8);
    *ADDR(dest + 0x10) = *ADDR(dest + 0x10) + *(int *)(self + 0x10);

    func_ov007_020c4388(dest, arg2);
    SubVec3((Vec3 *)(dest + 0x14), (Vec3 *)(self + 0x14), (Vec3 *)(dest + 0x14));
    func_ov007_020c421c(dest);

    *(int *)(dest + 4) = save4;
    *(int *)(dest + 8) = save8;
    *(int *)(dest + 0x10) = save10;
}
