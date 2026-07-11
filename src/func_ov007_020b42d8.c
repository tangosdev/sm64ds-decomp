typedef unsigned short u16;
typedef unsigned char u8;
typedef short s16;

typedef struct {
    char _0[4];
    int f4;
    int f8;
    u16 fc;
    u16 fe;
    int f10;
    char _14[0x5c - 0x14];
    int *f5c;
    char _60[0x64 - 0x60];
    int f64;
    int f68;
    int f6c;
    u16 f70;
    u16 f72;
} Obj;

typedef struct {
    char _0[4];
    void *f4;
} Base14c;

typedef struct {
    char _0[0x34];
    int f34;
    char _38[0x80 - 0x38];
    int f80;
    char _84[0x14c - 0x84];
    Base14c *f14c;
    char _150[0x174 - 0x150];
    Obj *arr174[1];
} Base;

extern Base *data_ov007_0210342c;
extern s16 data_ov007_020d761c[];
extern s16 data_ov007_020d761e[];
extern u8 data_ov007_020d75f0[];
extern u8 data_ov007_020d75f1[];
extern u8 data_ov007_020d75e0[];
extern u8 data_ov007_020d75e4[];

extern Obj *func_ov007_020c44e4(int a);
extern void func_ov007_020c41dc(char *a0, char *a1, int a2);

Obj *func_ov007_020b42d8(int index) {
    void *tmp = data_ov007_0210342c->f14c->f4;
    Obj *obj;
    data_ov007_0210342c->arr174[index] = func_ov007_020c44e4(1);
    obj = data_ov007_0210342c->arr174[index];
    obj->f4 = *(s16 *)((char *)data_ov007_020d761c + (index << 2)) << 12;
    obj->f8 = *(s16 *)((char *)data_ov007_020d761e + (index << 2)) << 12;
    obj->f10 = 0x14;
    obj->fc = data_ov007_020d75f0[index * 2];
    obj->fe = data_ov007_020d75f1[index * 2];
    *obj->f5c = data_ov007_0210342c->f80;
    obj->f64 = 3;
    obj->f68 = 0;
    obj->f6c = 1;
    obj->f70 = data_ov007_020d75e0[index];
    obj->f72 = data_ov007_020d75e4[index];
    func_ov007_020c41dc((char *)tmp, (char *)obj, data_ov007_0210342c->f34);
    return obj;
}
