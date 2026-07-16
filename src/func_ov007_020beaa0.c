typedef unsigned char u8;

typedef struct Obj {
    u8 f0;
    u8 f1;
    u8 f2;
    u8 pad3;
    void *f4;
    int f8;
    int fc;
    int f10;
    int f14;
    int f18;
} Obj;

typedef struct Slot {
    void *p;
    int x;
} Slot;

extern void *func_ov007_020c3df4(int a, int b);
extern void *func_ov007_020b3f20(int idx);
extern int func_ov007_020aebac(void);
extern void *func_ov007_020bc6d4(void *r6, int flags);
extern void func_ov007_020bc6b4(int *c, int r1, int r2);

extern int data_ov007_020d7c5c[][2];
extern Slot data_ov007_02104bc4[];
extern Slot data_ov007_02104bc8[];

Obj *func_ov007_020beaa0(int p0, int p1)
{
    Obj *o;
    void *r6;
    int flag;
    int flags;

    o = (Obj *)func_ov007_020c3df4(0, 0x1c);
    o->f0 = (u8)p0;
    o->f1 = (u8)p1;
    flag = (int)((p0 == 0) ? (char *)0 : (char *)1);
    o->f2 = (u8)flag;

    r6 = func_ov007_020b3f20(3);

    data_ov007_02104bc4[p0].p = func_ov007_020b3f20(data_ov007_020d7c5c[p1][p0]);

    data_ov007_02104bc8[p0].p = func_ov007_020b3f20(func_ov007_020aebac() != 0 ? 0xe : 0xf);

    flags = 2;
    if (p0 == 1)
        flags |= 8;
    o->f4 = func_ov007_020bc6d4(r6, flags);

    func_ov007_020bc6b4((int *)o->f4, (int)&data_ov007_02104bc4[p0], 2);

    o->f8 = 0;
    o->f14 = 0;
    o->f10 = o->f14;
    o->fc = o->f10;
    o->f18 = 0;
    return o;
}
