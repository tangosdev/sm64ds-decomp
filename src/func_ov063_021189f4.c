typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b, unsigned int c, s32 d, int e);
extern int _ZN5Sound15PlaySecretSoundEP5ActorPt(void *actor, u16 *p);
extern void func_ov063_02119074(char *c);
extern void func_ov063_02118f74(char *c);
extern void func_ov063_02118f50(void *c);
extern void func_ov063_02118cd8(char *self);
extern void func_ov063_02118b98(char *self);
extern int func_ov063_02118b2c(char *c);
extern void func_ov063_0211aa34(char *self);

void func_ov063_021189f4(char *c)
{
    if (*(u16 *)(c + 0x500 + 0xc6) != 0) {
        if (*(u8 *)(c + 0x5cf) == 0xd) {
            if (*(u16 *)(c + 0x500 + 0xc6) < 0x4b) {
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f, 0x6b000, 1);
                *(u16 *)(((long long)(int)(c + 0x5c6))) += 1;
            } else {
                int r = _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, *(s32 *)(c + 0x57c) >> 0xc, 0, 0x7f000, 1);
                if (r != 0)
                    *(u16 *)(c + 0x500 + 0xc6) = 0;
            }
        } else {
            int r = _ZN5Sound15PlaySecretSoundEP5ActorPt(c, (u16 *)(c + 0x5c6));
            if (r != 0)
                *(u16 *)(c + 0x500 + 0xc6) = 0;
        }
    }

    switch (*(u8 *)(c + 0x5cc)) {
    case 0:
        func_ov063_02119074(c);
        break;
    case 1:
        func_ov063_02118f74(c);
        break;
    case 2:
        func_ov063_02118f50(c);
        break;
    case 3:
        func_ov063_02118cd8(c);
        break;
    case 4:
        func_ov063_02118b98(c);
        break;
    case 5:
        func_ov063_02118b2c(c);
        break;
    }

    func_ov063_0211aa34(c);
}
