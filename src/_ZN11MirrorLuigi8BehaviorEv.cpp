//cpp
// @symbol _ZN11MirrorLuigi8BehaviorEv
/* recovered: named members + shared header, real C++ method, declarations from a shared header */
#include "decl_common.h"
/* recovered: named members + shared header, real C++ method */
#include "MirrorLuigi.h"
extern "C" {
extern int data_0209caa0[];
extern unsigned char data_0209f250;
extern char *data_0209f394[];
extern char *data_0209f318;
extern void _Z14ApproachLinearRiii(int *r, int target, int step);
extern void Matrix4x3_FromTranslation(char *m, int x, int y, int z);
extern void func_ov002_020e4374(char *c, int *p1, int *p2);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(char *a, char *sh, char *m, int height, int rad, unsigned int frames);
}

struct C5;
typedef void (C5::*PMF)(char *);
struct Node {
    int pad0, pad1;
    PMF fn;
};

int MirrorLuigi::Behavior()
{
    int a, b;
    int f;
    char *val;
    Node *node;

    f = data_ov055_02111b68;
    if (f & 1) {
        data_ov055_02111b6c = 0;
        data_ov055_02111a90 = 0x1ffff;
        data_ov055_02111b60 = 0;
    } else if ((f & 2) && !(data_0209caa0[1] & 0x10)) {
        _Z14ApproachLinearRiii(&data_ov055_02111b6c, 0x1ffff, 0x400);
        data_ov055_02111b64 = (data_ov055_02111b64 & ~0x20000) + (0x1ffff - data_ov055_02111b6c);
    }
    val = data_0209f394[data_0209f250];
    node = *(Node **)((char *)&unk_1d8);
    if (*(int *)&node->fn != 0)
        (((C5 *)((char *)this))->*node->fn)(val);
    Matrix4x3_FromTranslation(((char *)this) + 0x1dc, mPosX >> 3, mPosY >> 3, mPosZ >> 3);
    func_ov002_020e4374(val, &a, &b);
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(((char *)this), ((char *)this) + 0x188, ((char *)this) + 0x1dc, b, a, 0xf);
    *(char **)(data_0209f318 + 0x114) = ((char *)this);
    return 1;
}
