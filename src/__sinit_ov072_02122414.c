extern int func_02017acc(void *, int);
extern void func_020731dc(void *, void *, void **);
extern int _ZN13SharedFilePtr9ConstructEj(void *, int);
extern void func_02017ab4(void);
extern void SharedFilePtr_Destruct_Anim(void);
extern void func_020072c0(void);

typedef struct { int a, b; } S8;
typedef struct { S8 a, b; } S16;
typedef struct { int x, y, z; } V3;

extern S8 data_ov072_02122cb4;
extern S8 data_ov072_02122d00;
extern S8 data_ov072_02122c9c;
extern S8 data_ov072_02122d0c;
extern S8 data_ov072_02122cbc;
extern S8 data_ov072_02122d18;
extern S8 data_ov072_02122c94;
extern S8 data_ov072_02122d24;
extern S8 data_ov072_02122cac;
extern S8 data_ov072_02122d30;
extern S8 data_ov072_02122ca4;
extern S8 data_ov072_02122cc4;

extern S16 data_ov072_02122d6c[6];
extern S8 data_ov072_021229fc;
extern S8 data_ov072_02122a14;
extern S8 data_ov072_02122a4c;
extern S8 data_ov072_02122a0c;
extern S8 data_ov072_02122a44;
extern S8 data_ov072_02122a3c;
extern S8 data_ov072_02122a34;
extern S8 data_ov072_02122a2c;
extern S8 data_ov072_02122a04;
extern S8 data_ov072_02122a1c;
extern S8 data_ov072_021229f4;
extern S8 data_ov072_02122a24;

extern V3 data_ov072_02122d3c;
extern V3 data_ov072_02122d48;
extern V3 data_ov072_02122d54;
extern V3 data_ov072_02122d60;
extern S8 data_ov072_02122cd0;
extern S8 data_ov072_02122cdc;
extern S8 data_ov072_02122ce8;
extern S8 data_ov072_02122cf4;

void __sinit_ov072_02122414(void)
{
    func_02017acc(&data_ov072_02122cb4, 0x3ff);
    func_020731dc(&data_ov072_02122cb4, (void *)&func_02017ab4, (void **)&data_ov072_02122d00);

    _ZN13SharedFilePtr9ConstructEj(&data_ov072_02122c9c, 0x406);
    func_020731dc(&data_ov072_02122c9c, (void *)&SharedFilePtr_Destruct_Anim, (void **)&data_ov072_02122d0c);

    _ZN13SharedFilePtr9ConstructEj(&data_ov072_02122cbc, 0x408);
    func_020731dc(&data_ov072_02122cbc, (void *)&SharedFilePtr_Destruct_Anim, (void **)&data_ov072_02122d18);

    _ZN13SharedFilePtr9ConstructEj(&data_ov072_02122c94, 0x400);
    func_020731dc(&data_ov072_02122c94, (void *)&SharedFilePtr_Destruct_Anim, (void **)&data_ov072_02122d24);

    _ZN13SharedFilePtr9ConstructEj(&data_ov072_02122cac, 0x403);
    func_020731dc(&data_ov072_02122cac, (void *)&SharedFilePtr_Destruct_Anim, (void **)&data_ov072_02122d30);

    _ZN13SharedFilePtr9ConstructEj(&data_ov072_02122ca4, 0x405);
    func_020731dc(&data_ov072_02122ca4, (void *)&SharedFilePtr_Destruct_Anim, (void **)&data_ov072_02122cc4);

    data_ov072_02122d6c[0].a = data_ov072_021229fc;
    data_ov072_02122d6c[0].b = data_ov072_02122a14;
    data_ov072_02122d6c[1].a = data_ov072_02122a4c;
    data_ov072_02122d6c[1].b = data_ov072_02122a0c;
    data_ov072_02122d6c[2].a = data_ov072_02122a44;
    data_ov072_02122d6c[2].b = data_ov072_02122a3c;
    data_ov072_02122d6c[3].a = data_ov072_02122a34;
    data_ov072_02122d6c[3].b = data_ov072_02122a2c;
    data_ov072_02122d6c[4].a = data_ov072_02122a04;
    data_ov072_02122d6c[4].b = data_ov072_02122a1c;
    data_ov072_02122d6c[5].a = data_ov072_021229f4;
    data_ov072_02122d6c[5].b = data_ov072_02122a24;

    data_ov072_02122d3c.x = 0x1c000;
    data_ov072_02122d3c.y = 0;
    data_ov072_02122d3c.z = 0x1e000;
    func_020731dc(&data_ov072_02122d3c, (void *)&func_020072c0, (void **)&data_ov072_02122cd0);

    {
        unsigned int *p = (unsigned int *)&data_ov072_02122d48;
        p[0] = 0x1c000;
        p[1] = 0;
        p[2] = 0x1e000;
        func_020731dc(p, (void *)&func_020072c0, (void **)&data_ov072_02122cdc);
    }
    {
        unsigned int *p = (unsigned int *)&data_ov072_02122d54;
        p[0] = 0x2b000;
        p[1] = 0;
        p[2] = 0x1e000;
        func_020731dc(p, (void *)&func_020072c0, (void **)&data_ov072_02122ce8);
    }
    {
        unsigned int *p = (unsigned int *)&data_ov072_02122d60;
        p[0] = 0x30000;
        p[1] = 0;
        p[2] = 0x32000;
        func_020731dc(p, (void *)&func_020072c0, (void **)&data_ov072_02122cf4);
    }
}
