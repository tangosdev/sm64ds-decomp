/* candidate for func_ov066_021171b0 */
typedef struct Vec3 { int x, y, z; } Vec3;

extern void *_ZN5Actor13ClosestPlayerEv(void *thiz);
extern void func_02012694(int a, void *b);
extern void _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(void *out, void *tgt, int step);
extern int Vec3_Dist(void *a, void *b);
extern void func_ov066_02116ac4(void *c, int v);
extern int Vec3_ApproachHorz(void *out, void *a, int step);
extern int Vec3_HorzDist(void *a, void *b);
extern void func_ov066_02119454(void *c, void *p);
extern unsigned char data_ov066_0211ae0c;
extern unsigned char data_ov066_0211ae00;
extern unsigned char data_ov066_0211ae08;
extern unsigned char data_ov066_0211ae04;
extern char data_ov066_0211b06c;

int func_ov066_021171b0(void *thiz)
{
    char *c = (char *)thiz;

    switch (*(int *)(c + 0x4a0)) {
    case 0: {
        char *p = (char *)_ZN5Actor13ClosestPlayerEv(c);
        if (p == 0)
            break;
        {
            Vec3 *pp = (Vec3 *)(((int)p + 0x5c) & 0xFFFFFFFFFFFFFFFF);
            *(int *)(c + 0x4bc) = pp->x;
            *(int *)(c + 0x4c0) = pp->y;
            *(int *)(c + 0x4c4) = pp->z;
        }
        *(int *)(((int)c + 0x4c4) & 0xFFFFFFFFFFFFFFFF) -= 0xc8000;
        if (*(int *)(c + 0x4c4) < (int)0xff3ae000) {
            *(int *)(c + 0x4c4) = (int)0xff3ae000;
        } else if (*(int *)(c + 0x4c4) > (int)0xff8c6000) {
            *(int *)(c + 0x4c4) = (int)0xff8c6000;
        }
        if (data_ov066_0211ae0c == 1) {
            *(short *)(c + 0x94) = -0x4000;
            *(int *)(c + 0x4bc) = 0x334000;
            if (*(int *)(c + 0x49c) == 1) {
                *(int *)(((int)c + 0x4bc) & 0xFFFFFFFFFFFFFFFF) -= 0xf2000;
            }
        } else {
            *(short *)(c + 0x94) = 0x4000;
            *(int *)(c + 0x4bc) = (int)0xffe8e000;
            if (*(int *)(c + 0x49c) == 1) {
                *(int *)(((int)c + 0x4bc) & 0xFFFFFFFFFFFFFFFF) -= 0xf2000;
            }
        }
        func_02012694(0x144, c + 0x74);
        *(int *)(c + 0x4c0) = *(int *)(c + 0x4a8) + 0x1c2000;
        *(int *)(c + 0x4a0) = 1;
        break;
    }
    case 1:
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(c + 0x5c, c + 0x4bc, 0x28000);
        if (Vec3_Dist(c + 0x5c, c + 0x4bc) > 0x28000)
            break;
        *(int *)(c + 0x5c) = *(int *)(c + 0x4bc);
        *(int *)(c + 0x60) = *(int *)(c + 0x4c0);
        *(int *)(c + 0x64) = *(int *)(c + 0x4c4);
        data_ov066_0211ae00 |= *(int *)(c + 0x49c);
        if (data_ov066_0211ae00 != 3)
            break;
        *(unsigned short *)(c + 0x4d0) = 0xa;
        if (data_ov066_0211ae0c == 1) {
            if (*(int *)(c + 0x49c) == 2)
                *(unsigned short *)(c + 0x4d0) = 0x12;
        } else {
            if (*(int *)(c + 0x49c) == 1)
                *(unsigned short *)(c + 0x4d0) = 0x12;
        }
        *(int *)(c + 0x4c0) = *(int *)(c + 0x4a8) + 0x1a000;
        *(int *)(c + 0x4a0) = 2;
        break;
    case 2:
        if (*(unsigned short *)(c + 0x4d0) != 0)
            break;
        _Z14ApproachLinearR7Vector3RKS_5Fix12IiE(c + 0x5c, c + 0x4bc, 0x32000);
        if (Vec3_Dist(c + 0x5c, c + 0x4bc) > 0x32000)
            break;
        *(int *)(c + 0x5c) = *(int *)(c + 0x4bc);
        *(int *)(c + 0x60) = *(int *)(c + 0x4c0);
        *(int *)(c + 0x64) = *(int *)(c + 0x4c4);
        func_ov066_02116ac4(c, 0x7d0000);
        *(unsigned short *)(c + 0x4d0) = 0xf;
        *(int *)(c + 0x4a0) = 3;
        break;
    case 3: {
        unsigned short st = *(unsigned short *)(c + 0x4d0);
        if (st != 0) {
            if (st != 1)
                break;
            *(int *)(c + 0xa8) = 0x7c000;
            *(int *)(c + 0x9c) = -0x14000;
            *(int *)(c + 0x98) = 0x1e000;
            *(int *)(c + 0xb0) = 0x2000000;
            break;
        }
        if (*(int *)(c + 0x9c) == 0)
            break;
        if (*(int *)(c + 0x4a8) < *(int *)(c + 0x60))
            break;
        *(int *)(c + 0x60) = *(int *)(c + 0x4a8);
        *(int *)(c + 0xa8) = 0;
        *(int *)(c + 0x9c) = 0;
        *(int *)(c + 0x98) = 0;
        func_ov066_02116ac4(c, 0x7d0000);
        *(unsigned short *)(c + 0x4d0) = 0xf;
        *(int *)(((int)c + 0x494) & 0xFFFFFFFFFFFFFFFF) += 1;
        if (*(int *)(c + 0x494) < 3)
            *(int *)(c + 0x4a0) = 3;
        else
            *(int *)(c + 0x4a0) = 4;
        break;
    }
    case 4:
        if (*(unsigned short *)(c + 0x4d0) == 1) {
            data_ov066_0211ae00 ^= *(int *)(c + 0x49c);
        }
        if (data_ov066_0211ae00 != 0)
            break;
        Vec3_ApproachHorz(c + 0x5c, c + 0x4a4, 0x28000);
        if (Vec3_HorzDist(c + 0x5c, c + 0x4a4) > 0x28000)
            break;
        data_ov066_0211ae0c = 0;
        *(int *)(c + 0x5c) = *(int *)(c + 0x4a4);
        *(int *)(c + 0x60) = *(int *)(c + 0x4a8);
        *(int *)(c + 0x64) = *(int *)(c + 0x4ac);
        data_ov066_0211ae08 += 1;
        *(int *)(c + 0x4a0) = 5;
        break;
    case 5:
        if (data_ov066_0211ae04 == 8)
            break;
        *(int *)(c + 0xb0) = 0;
        func_ov066_02119454(c, &data_ov066_0211b06c);
        break;
    }
    return 1;
}
