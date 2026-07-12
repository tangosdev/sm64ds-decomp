typedef short s16;
typedef unsigned char u8;

extern int _ZN6Player9StartTalkER9ActorBaseb(void *target, void *self, int b);
extern short Vec3_HorzAngle(void *a, void *b);
extern int _Z14ApproachLinearRsss(void *dst, short a, short b);
extern void Matrix4x3_FromTranslation(void *m, int x, int y, int z);
extern void Matrix4x3_ApplyInPlaceToRotationY(void *m, short a);
extern void Matrix4x3_ApplyInPlaceToTranslation(void *m, int x, int y, int z);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *self, void *ab, unsigned msg, void *v, unsigned a, unsigned b);
extern void func_0201267c(int id, void *p);
extern int _ZN6Player12GetTalkStateEv(void *self);
extern void _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned a, unsigned b, void *pos, void *c, int d, int e);
extern void _ZN6Player9DropActorEv(void *self);
extern unsigned char DecIfAbove0_Byte(unsigned char *p);
extern int func_ov018_021123d0(void *c, int b);
extern void func_ov018_02111a48(void *c, void *p);
extern char data_020a0e68[];

#define U8P(base, off) ((u8 *)(((unsigned int)(base) + (off)) & 0xFFFFFFFFFFFFFFFFULL))

int func_ov018_02111fac(char *c)
{
    switch (*(unsigned char *)(c + 0x388)) {
    case 0:
        if (*(int *)(c + 0x194) & 0x8000000) {
            if (_ZN6Player9StartTalkER9ActorBaseb(*(void **)(c + 0x374), c, 1)) {
                u8 *p = U8P(c, 0x388);
                *p = *p + 1;
            }
        } else {
            func_ov018_021123d0(c, 0);
        }
        break;
    case 1:
        if (_Z14ApproachLinearRsss(c + 0x8e,
                Vec3_HorzAngle(c + 0x5c, *(char **)(c + 0x374) + 0x5c), 0x514)) {
            Matrix4x3_FromTranslation(data_020a0e68, *(int *)(c + 0x5c), *(int *)(c + 0x60), *(int *)(c + 0x64));
            Matrix4x3_ApplyInPlaceToRotationY(data_020a0e68, *(s16 *)(c + 0x8e));
            Matrix4x3_ApplyInPlaceToTranslation(data_020a0e68, 0, 0x300000, -0x480000);
            {
                int v[3];
                v[0] = *(int *)(data_020a0e68 + 0x24);
                v[1] = *(int *)(data_020a0e68 + 0x28);
                v[2] = *(int *)(data_020a0e68 + 0x2c);
                if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(
                        *(void **)(c + 0x374), c, *(s16 *)(c + 0x300 + 0x84), v, 0, 0)) {
                    func_0201267c(0xdf, c + 0x74);
                    {
                        u8 *p = U8P(c, 0x388);
                        *p = *p + 1;
                    }
                }
            }
        }
        break;
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x374)) == -1) {
            if (*(s16 *)(c + 0x300 + 0x84) == 0xad) {
                unsigned r1;
                unsigned char b;
                *(unsigned char *)(c + 0x386) = 1;
                b = (unsigned char)(*(unsigned *)(c + 8) & 0xf);
                r1 = (unsigned)b | 0x40;
                _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                    0xb2, r1, c + 0x5c, 0, *(signed char *)(c + 0xcc), -1);
            }
            _ZN6Player9DropActorEv(*(void **)(c + 0x374));
            {
                u8 *p = U8P(c, 0x388);
                *p = *p + 1;
            }
        }
        break;
    case 3:
        if (!DecIfAbove0_Byte(U8P(c, 0x389)))
            func_ov018_021123d0(c, 0);
        break;
    }
    func_ov018_02111a48(c, *(void **)(c + 0x374));
    if (*(unsigned char *)(c + 0x386) == 0 && *(unsigned char *)(c + 0x387) == 0) {
        unsigned t = (unsigned)*(int *)(c + 0x12c) << 4 >> 0x10;
        if (t == 0x10 || t == 0x25)
            func_0201267c(0xdf, c + 0x74);
    }
    return 1;
}
