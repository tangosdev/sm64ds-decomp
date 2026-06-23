typedef unsigned char u8;
typedef unsigned short u16;
typedef short s16;
typedef int s32;
typedef unsigned int u32;

struct Player {
    char pad8[8];
    int field_8;
    char pad1[0x74 - 0xc];
    char field_74[0x18];
    char pad2[0x8e - 0x8c];
    s16 field_8e;
    char pad3[0x94 - 0x90];
    s16 field_94;
    char pad4[0x98 - 0x96];
    int field_98;
    char pad5[0xa8 - 0x9c];
    int field_a8;
    char pad6[0x6d9 - 0xac];
    u8 field_6d9;
    char pad7[0x6de - 0x6da];
    u8 field_6de;
    u8 field_6df;
    char pad8b[0x6e1 - 0x6e0];
    u8 field_6e1;
    char pad9[0x6e3 - 0x6e2];
    u8 field_6e3;
    char pad10[0x6e5 - 0x6e4];
    u8 field_6e5;
    char pad11[0x70d - 0x6e6];
    u8 field_70d;
    char pad12[0x712 - 0x70e];
    u8 field_712;
    char pad13[0x71b - 0x713];
    u8 field_71b;
};

extern void _ZN6Player7SetAnimEji5Fix12IiEj(struct Player *thiz, u32 a, int b, int c, u32 d);
extern void func_ov002_020bf2d8(void *c, int a);
extern void _ZN5Sound13PlayCharVoiceEjjRK7Vector3(u32 a, u32 b, const void *v);

int func_ov002_020e17f8(struct Player *thiz)
{
    thiz->field_71b = 0;
    thiz->field_712 = 1;
    thiz->field_70d = 0;
    thiz->field_6e1 = 0;
    thiz->field_6de = 1;
    thiz->field_6df = 0;
    _ZN6Player7SetAnimEji5Fix12IiEj(thiz, 0x28, 0x40000000, 0x1000, 0);
    if (thiz->field_8 == 2) {
        thiz->field_a8 = 0x2aa00;
    } else {
        func_ov002_020bf2d8(thiz, 0x3e000);
    }
    thiz->field_98 = 0x18000;
    thiz->field_8e += 0x8000;
    thiz->field_94 = thiz->field_8e;
    thiz->field_6e5 = 1;
    thiz->field_6e3 = 0;
    _ZN5Sound13PlayCharVoiceEjjRK7Vector3(thiz->field_6d9, 0xc, thiz->field_74);
    return 1;
}
