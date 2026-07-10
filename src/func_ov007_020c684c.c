typedef struct { int a, b, c; } Vec3;

void func_ov007_020c684c(char *o)
{
    Vec3 tmp;
    Vec3 *tp;
    *(int *)(o + 4) = 1;
    *(int *)(o) = 0;
    *(int *)(o + 0x78) = 0;
    *(int *)(o + 0x74) = *(int *)(o + 0x78);
    *(int *)(o + 0x70) = *(int *)(o + 0x74);
    *(int *)(o + 0x90) = 0;
    *(int *)(o + 0x8c) = *(int *)(o + 0x90);
    *(int *)(o + 0x88) = *(int *)(o + 0x8c);
    *(Vec3 *)(o + 0x7c) = *(Vec3 *)(o + 0x70);
    *(Vec3 *)(o + 0x98) = *(Vec3 *)(o + 0x70);
    tp = (Vec3 *)(((long long)(int)&tmp) & 0xFFFFFFFFFFFFFFFFLL);
    tp->a = 0;
    tp->b = 0;
    tp->c = 0;
    *(Vec3 *)(o + 0xc4) = *tp;
    *(Vec3 *)(o + 0xb8) = *(Vec3 *)(o + 0xc4);
    *(int *)(o + 0x5c) = -1;
    *(int *)(o + 0x58) = *(int *)(o + 0x5c);
    *(int *)(o + 0x68) = 0;
    *(int *)(o + 0x64) = *(int *)(o + 0x68);
    *(int *)(o + 0x60) = *(int *)(o + 0x64);
    *(int *)(o + 0xac) = 0;
    *(int *)(o + 0xb0) = 0;
}
