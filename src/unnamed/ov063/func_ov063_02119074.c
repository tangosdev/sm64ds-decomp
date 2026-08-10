typedef short s16;
typedef unsigned short u16;
typedef unsigned int u32;
typedef int s32;

struct Vector3 { int x, y, z; };

extern void func_ov063_02116bf0(char *c);
extern void func_ov063_02119c18(void *c, unsigned int id);
extern int func_ov063_0211ad00(char *c);
extern void _ZN5Model12SetPolygonIDEi(void *self, int id);
extern int _ZN5Sound7PlaySubEjjj5Fix12IiEb(unsigned int a, unsigned int b,
                                            unsigned int c, int fix, int loop);
extern char *_ZN5Actor15FindWithActorIDEjPS_(unsigned int id, void *p);
extern s16 Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
extern void func_ov063_02116244(char *c);
extern void func_ov063_0211adfc(char *p);

extern int data_ov063_0211e1d0[];
extern int data_0209b490[];

void func_ov063_02119074(char *self)
{
    volatile int dummy[2];
    (void)&dummy;
    /* ROM: beq body on ==0xc; bne after on !=0xf  =>  body when 0xc OR 0xf */
    if (*(unsigned char *)(self + 0x5cf) == 0xc ||
        *(unsigned char *)(self + 0x5cf) == 0xf) {
        func_ov063_02116bf0(self);
        *(int *)(self + 0x180) = 0xa;
    }

    if (*(unsigned char *)(self + 0x5cf) == 0xd)
        func_ov063_02119c18(self, 0x9f);

    *(int *)(self + 0x490) = 0;
    *(unsigned char *)(self + 0x5c8) = 0x28;

    if (func_ov063_0211ad00(self) != 0 &&
        *(int *)(self + 0x180) >= 5) {
        char *r;
        s32 scale;

        *(u16 *)(self + 0x5d4) |= 8;
        *(unsigned char *)(self + 0x5ca) = 3;
        *(int *)(self + 0x584) =
            data_ov063_0211e1d0[*(unsigned char *)(self + 0x5cf) - 0xc];
        scale = *(int *)(self + 0x584);
        *(int *)(self + 0x80) = scale;
        *(int *)(self + 0x84) = scale;
        *(int *)(self + 0x88) = scale;

        _ZN5Model12SetPolygonIDEi(self + 0x380, 0x16);

        if (*(unsigned char *)(self + 0x5cf) == 0xd) {
            *(int *)(self + 0x57c) = data_0209b490[0];

            if (((u32)(*(u16 *)(self + 0x5d4) << 22) >> 31) == 0) {
                _ZN5Sound7PlaySubEjjj5Fix12IiEb(0x20, 0x14, 0x7f,
                                                 0x6b000, 1);
                *(u16 *)(self + 0x500 + 0xc6) = 1;
                *(u16 *)((int)self + 0x5d4) |= 0x200;
            }

            *(int *)(self + 0x98) = 0x800;
            *(unsigned char *)(self + 0x5cc) = 5;
            r = _ZN5Actor15FindWithActorIDEjPS_(0x9f, 0);

            if (r != 0)
                *(s16 *)(self + 0x94) =
                    Vec3_HorzAngle((struct Vector3 *)(self + 0x5c),
                                   (struct Vector3 *)(r + 0x5c));
        } else {
            if (*(unsigned char *)(self + 0x5cf) == 0xe)
                func_ov063_02116244(self);
            *(unsigned char *)(self + 0x5cc) = 1;
        }

        *(unsigned char *)(self + 0x5c9) = 0xff;
        *(int *)(self + 0x188) =
            *(int *)(self + 0x590) * *(int *)(self + 0x584);
        *(int *)(self + 0x18c) =
            *(int *)(self + 0x594) * *(int *)(self + 0x584);
        *(int *)(self + 0x19c) &= ~1;
    } else {
        *(u16 *)((long long)((int)self + 0x5d4)) &= ~8;
        *(int *)((int)self + 0x19c) |= 1;
        func_ov063_0211adfc(self);
    }
}
