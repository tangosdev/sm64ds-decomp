typedef unsigned char u8;
typedef unsigned short u16;
typedef signed short s16;
typedef unsigned int u32;

typedef struct Vec3 { int x, y, z; } Vec3;
typedef struct RaycastGround {
    char pre[0x10];
    char result[0x34];
    int clsnY;
    char post[0x8];
} RaycastGround;

extern void _ZN13RaycastGroundC1Ev(RaycastGround *self);
extern void _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(RaycastGround *self, Vec3 *pos, void *actor);
extern int _ZN13RaycastGround10DetectClsnEv(RaycastGround *self);
extern int _ZNK10ClsnResult9GetClsnIDEv(void *self);
extern void _ZN13RaycastGroundD1Ev(RaycastGround *self);
extern void func_ov060_02113a94(void *c);
extern void func_ov060_02111cc0(void *c, int a, int b);
extern void func_02012694(int a, void *p);
extern void _Z14ApproachLinearRiii(int *v, int target, int step);
extern int func_ov060_021145d4(void *c);
extern void func_ov060_02115b0c(void *c);
extern void *_ZN5Actor15FindWithActorIDEjPS_(u32 id, void *p);
extern void func_ov060_02115018(void *c);
extern int func_ov060_02115a30(void *c);

void func_ov060_02113740(char *c)
{
    Vec3 pos;
    RaycastGround rc;
    int hit;
    int ground;

    ground = *(int*)(c + 0x3b4) - 0x7d0000;
    hit = 0;
    if (*(int*)(c + 0x60) >= *(int*)(c + 0x3b4)) {
        _ZN13RaycastGroundC1Ev(&rc);
        {
            int pz = *(int*)(c + 0x64);
            int py = *(int*)(c + 0x3b4) + 0x96000;
            int px = *(int*)(c + 0x5c);
            pos.x = px;
            pos.y = py;
            pos.z = pz;
        }
        _ZN13RaycastGround12SetObjAndPosERK7Vector3P5Actor(&rc, &pos, c);
        if (_ZN13RaycastGround10DetectClsnEv(&rc)) {
            ground = rc.clsnY;
            if (_ZNK10ClsnResult9GetClsnIDEv(&rc.result) != -1)
                hit = 1;
        }
        _ZN13RaycastGroundD1Ev(&rc);
    }

    *(int*)(((long long)(int)(c + 0x418)) & 0xFFFFFFFFFFFFFFFFLL) |= 0x10000;

    switch (*(u8*)(c + 0x423)) {
    case 0:
    {
        s16 *p8c = (s16*)(c + 0x8c);
        s16 *p90 = (s16*)(c + 0x90);
        if (*(u16*)(c + 0x3fc) == 0) {
            *(s16*)(c + 0x90) = 0;
            *(s16*)(c + 0x8c) = *(s16*)(c + 0x90);
        }
        *p8c += 0x800;
        *p90 += 0x800;
        if ((*(s16*)(c + 0x8c) & 0xffff) == 0)
            (*(u8*)(((int)c + 0x423) & 0xFFFFFFFFFFFFFFFFLL))++;
        func_ov060_02113a94(c);
        return;
    }
    case 1:
        func_ov060_02111cc0(c, 0xb, 0x40000000);
        if (((*(u32*)(c + 0x12c) << 4) >> 16) >= 0x20) {
            *(s16*)(c + 0x8e) = *(s16*)(c + 0x408);
            *(s16*)(c + 0x94) = *(s16*)(c + 0x8e);
            *(int*)(c + 0xa8) = 0x28000;
            func_02012694(0xb1, c + 0x74);
            *(int*)(c + 0x9c) = 0;
            *(u8*)(c + 0x41d) = 0xff;
            *(s16*)(c + 0x3fe) = 0;
            (*(u8*)(((int)c + 0x423) & 0xFFFFFFFFFFFFFFFFLL))++;
            return;
        }
        func_ov060_02113a94(c);
        return;
    case 2:
    {
        int thr;
        if (*(u8*)(c + 0x414) == 1) thr = 0x8fc000; else thr = 0x9c4000;
        if (*(int*)(c + 0x60) >= *(int*)(c + 0x3b4)) {
            *(int*)(c + 0x9c) = -0x1000;
            if (*(int*)(c + 0x3f4) < thr) {
                int d = ground - *(int*)(c + 0x3b4);
                if (d < 0) d = -d;
                if (d < 0x64000)
                    _Z14ApproachLinearRiii((int*)(c + 0x98), 0, 0x5000);
                else
                    _Z14ApproachLinearRiii((int*)(c + 0x98), 0x4b000, 0x2000);
            } else {
                _Z14ApproachLinearRiii((int*)(c + 0x98), 0x4b000, 0x2000);
            }
        }
        if (func_ov060_021145d4(c)) {
            (*(u8*)(((int)c + 0x423) & 0xFFFFFFFFFFFFFFFFLL))++;
            if (hit == 0) {
                func_ov060_02115b0c(c);
            } else {
                if (*(u8*)(c + 0x414) == 2) {
                    *(int*)(c + 0x40c) = 0xd;
                    *(int*)(c + 0x3f8) = 0x4000;
                    *(int*)(c + 0x9c) = -0x2000;
                }
            }
            if (*(u8*)(c + 0x414) == 1) {
                char *r;
                *(int*)(c + 0x40c) = 0x13;
                r = (char*)_ZN5Actor15FindWithActorIDEjPS_(0xa6, 0);
                if (r) *(int*)(c + 0x3ac) = *(int*)(r + 4);
                *(int*)(c + 0x9c) = -0x2000;
            }
        }
        func_ov060_02115018(c);
        return;
    }
    case 3:
        if (func_ov060_02115a30(c) != 0) {
            *(int*)(c + 0x40c) = 0;
            *(int*)(((int)c + 0x418) & 0xFFFFFFFFFFFFFFFFLL) &= ~0x10000;
            *(int*)(c + 0x9c) = -0x2000;
        }
        return;
    }
}