typedef short s16;

typedef struct { int x, y; } V2;

struct C {
    char pad0[0x8];
    int dx;             /* 0x8 */
    int dy;             /* 0xc */
    char pad10[0x22];   /* 0x10 */
    s16 angle;          /* 0x32 */
    char pad34[0x4];    /* 0x34 */
    V2 v[8];            /* 0x38 */
};

extern void func_0203d388(V2* p, int angle);

void func_ov006_02110e28(struct C* c)
{
    c->v[0].x = 0x20000;
    c->v[0].y = 0;
    c->v[1].x = 0;
    c->v[1].y = -0xb000;
    c->v[2].x = 0;
    c->v[2].y = 0xb000;
    c->v[3].x = -0x20000;
    c->v[3].y = 0;
    c->v[4].x = 0;
    c->v[4].y = 0x20000;
    c->v[5].x = -0xb000;
    c->v[5].y = 0;
    c->v[6].x = 0xb000;
    c->v[6].y = 0;
    c->v[7].x = 0;
    c->v[7].y = -0x20000;

    func_0203d388(&c->v[0], c->angle);
    func_0203d388(&c->v[1], c->angle);
    func_0203d388(&c->v[2], c->angle);
    func_0203d388(&c->v[3], c->angle);
    func_0203d388(&c->v[4], c->angle);
    func_0203d388(&c->v[5], c->angle);
    func_0203d388(&c->v[6], c->angle);
    func_0203d388(&c->v[7], c->angle);

    *(int*)(((int)c + 0x38)) += c->dx;
    *(int*)(((int)c + 0x3c)) += c->dy;
    *(int*)(((int)c + 0x40)) += c->dx;
    *(int*)(((int)c + 0x44)) += c->dy;
    *(int*)(((int)c + 0x48)) += c->dx;
    *(int*)(((int)c + 0x4c)) += c->dy;
    *(int*)(((int)c + 0x50)) += c->dx;
    *(int*)(((int)c + 0x54)) += c->dy;
    *(int*)(((int)c + 0x58)) += c->dx;
    *(int*)(((int)c + 0x5c)) += c->dy;
    *(int*)(((int)c + 0x60)) += c->dx;
    *(int*)(((int)c + 0x64)) += c->dy;
    *(int*)(((int)c + 0x68)) += c->dx;
    *(int*)(((int)c + 0x6c)) += c->dy;
    *(int*)(((int)c + 0x70)) += c->dx;
    *(int*)(((int)c + 0x74)) += c->dy;
}
