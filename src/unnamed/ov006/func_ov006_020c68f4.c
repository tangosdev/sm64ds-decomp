typedef struct { int x, y, z; } Vec3;
typedef struct { int x, y, z, w; } Vec4;

extern void func_020731dc(int a, int b, void **node);
extern void AddVec3(Vec3 *a, Vec3 *b, Vec3 *c);
extern int func_020072c0(void);

extern int data_ov006_02140310;
extern Vec3 data_ov006_02140394;
extern int data_ov006_02140388;
extern int data_ov006_02140320;
extern Vec3 data_ov006_021403ac;
extern int data_ov006_021403a0;
typedef struct { int a, b; } Pair;
extern Pair data_ov006_0213af60;

void func_ov006_020c68f4(char* dst, char* src)
{
    if (!(data_ov006_02140310 & 1)) {
        data_ov006_02140394.x = -0x100000;
        data_ov006_02140394.y = 0;
        data_ov006_02140394.z = 0;
        func_020731dc((int)&data_ov006_02140394, (int)&func_020072c0, (void**)&data_ov006_02140388);
        data_ov006_02140310 |= 1;
    }
    if (!(data_ov006_02140320 & 1)) {
        data_ov006_021403ac.x = 0x100000;
        data_ov006_021403ac.y = 0;
        data_ov006_021403ac.z = 0;
        func_020731dc((int)&data_ov006_021403ac, (int)&func_020072c0, (void**)&data_ov006_021403a0);
        data_ov006_02140320 |= 1;
    }

    *(int*)(dst) = *(int*)(src);
    *(Vec3*)(dst + 4) = *(Vec3*)(src + 4);
    *(Vec4*)(dst + 0x10) = *(Vec4*)(src + 0x10);
    *(Vec3*)(dst + 0x20) = *(Vec3*)(src + 0x20);
    *(int*)(dst + 0x2c) = *(int*)(src + 0x2c);

    if (*(volatile int*)(dst + 0x20) > 0) {
        Vec3 *p = (Vec3*)(dst + 0x9c);
        *(int*)(dst + 0x9c) = *(int*)(dst + 0x20);
        *(int*)(dst + 0xa0) = *(int*)(dst + 0x24);
        *(int*)(dst + 0xa4) = *(int*)(dst + 0x28);
        AddVec3(p, &data_ov006_021403ac, p);
    } else {
        Vec3 *q = (Vec3*)(dst + 0x9c);
        *(int*)(dst + 0x9c) = *(int*)(dst + 0x20);
        *(int*)(dst + 0xa0) = *(int*)(dst + 0x24);
        *(int*)(dst + 0xa4) = *(int*)(dst + 0x28);
        AddVec3(q, &data_ov006_02140394, q);
    }

    *(short*)(dst + 0xec) = 8;
    *(int*)(dst + 0xcc) = 0x1400;
    *(int*)(dst + 0xd0) = 0x1400;
    *(int*)(dst + 0xd4) = 0x1400;
    *(int*)(dst + 0xd8) = *(int*)dst;
    *(short*)(dst + 0xe6) = 0;
    *(int*)(dst + 0xe0) = 0;
    *(Pair*)(dst + 0x30) = data_ov006_0213af60;
}
