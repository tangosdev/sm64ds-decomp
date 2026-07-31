#include "types.h"
typedef struct Vec3 { int x, y, z; } Vec3;

typedef struct Sub {
    unsigned char _pad[0x96];
    u16 state;      /* 0x96 */
    unsigned char _pad2;
    s8 flag;        /* 0x99 */
} Sub;

extern int data_0209b454;
extern void *data_0209f318;

extern void _ZN6Camera9SetFlag_3Ev(void *cam);
extern void _ZN6Camera9SetLookAtERK7Vector3(void *cam, Vec3 *v);
extern void _ZN6Camera6SetPosERK7Vector3(void *cam, Vec3 *v);
extern int func_ov002_020e7c90(char *self, void *cam);
extern void func_ov002_020e7934(char *self, void *cam);

void func_ov002_020e763c(char *self)
{
    Vec3 v;
    void *cam;

    if ((data_0209b454 & 0x4000000) == 0)
        return;

    if (((Sub *)(self + 0x400))->state == 0xffff)
        return;

    v.x = *(int *)(self + 0x5c);
    cam = data_0209f318;
    v.y = *(int *)(self + 0x60);
    v.z = *(int *)(self + 0x64);

    switch (((Sub *)(self + 0x400))->state) {
    case 0:
    {
        int *s1 = (int *)(((int)cam + 0x80));
        int *s2 = (int *)(((int)cam + 0x8c));
        *(int *)(self + 0x460) = s1[0];
        *(int *)(self + 0x464) = s1[1];
        *(int *)(self + 0x468) = s1[2];
        *(int *)(self + 0x46c) = s2[0];
        *(int *)(self + 0x470) = s2[1];
        *(int *)(self + 0x474) = s2[2];
    }
        func_ov002_020e7c90(self, cam);
        _ZN6Camera9SetFlag_3Ev(cam);
        if (func_ov002_020e7c90(self, cam) == 0)
            func_ov002_020e7934(self, cam);
        *(u16 *)(((int)self + 0x496)) += 1;
        break;
    case 1:
        if (*(int *)(self + 0x440) != 2)
            return;
        _ZN6Camera9SetLookAtERK7Vector3(cam, &v);
        break;
    case 0x64:
    {
        int *s1 = (int *)(((int)cam + 0x80));
        int *s2 = (int *)(((int)cam + 0x8c));
        *(int *)(self + 0x460) = s1[0];
        *(int *)(self + 0x464) = s1[1];
        *(int *)(self + 0x468) = s1[2];
        *(int *)(self + 0x46c) = s2[0];
        *(int *)(self + 0x470) = s2[1];
        *(int *)(self + 0x474) = s2[2];
    }
        _ZN6Camera9SetFlag_3Ev(cam);
        if (func_ov002_020e7c90(self, cam) == 0)
            func_ov002_020e7934(self, cam);
        *(u16 *)(((int)self + 0x496)) += 1;
        break;
    case 0x65:
        _ZN6Camera9SetLookAtERK7Vector3(cam, &v);
        if (*(int *)(self + 0x80) == 0x1000 || *(int *)(self + 0x80) == 0)
            ((Sub *)(self + 0x400))->state = 0x1b6;
        break;
    case 0x1f4:
        _ZN6Camera9SetLookAtERK7Vector3(cam, (Vec3 *)(self + 0x460));
        _ZN6Camera6SetPosERK7Vector3(cam, (Vec3 *)(self + 0x46c));
        *(u16 *)(((int)self + 0x496)) += 1;
        break;
    case 0x1f5:
        *(int *)(((int)cam + 0x154)) &= ~8;
        *(int *)(((int)self + 0xb0)) &= ~0x4000000;
        data_0209b454 &= ~0x4000000;
        ((Sub *)(self + 0x400))->state = 0xffff;
        *(s8 *)(self + 0xcc) = ((Sub *)(self + 0x400))->flag;
        break;
    default:
        *(u16 *)(((int)self + 0x496)) += 1;
        break;
    }
}
