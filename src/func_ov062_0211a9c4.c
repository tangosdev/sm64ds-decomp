struct Vec3
{
    int x;
    int y;
    int z;
};

extern void *_ZN5Actor13ClosestPlayerEv(void *self);
extern int Vec3_Dist(struct Vec3 *a, struct Vec3 *b);
extern int _ZN6Player9StartTalkER9ActorBaseb(void *thiz, void *ref, int b);
extern void *_ZN5Actor15FindWithActorIDEjPS_(unsigned id, void *p);
extern short Vec3_HorzAngle(struct Vec3 *a, struct Vec3 *b);

void func_ov062_0211a9c4(char *c)
{
    unsigned short val;
    struct Vec3 v;
    char *a;
    struct Vec3 *sp;

    val = *(unsigned short *)(c + 0x100);
    if (val != 0) {
        *(unsigned short *)(c + 0x100) = val - 1;
        return;
    }

    *(void **)(c + 0x398) = _ZN5Actor13ClosestPlayerEv(c);
    if (*(void **)(c + 0x398) == 0)
        return;

    sp = (struct Vec3 *)(((int)*(void **)(c + 0x398) + 0x5c) & 0xFFFFFFFFFFFFFFFFULL);
    v.x = sp->x;
    v.y = sp->y;
    v.z = sp->z;

    if (Vec3_Dist((struct Vec3 *)(c + 0x5c), &v) >= 0xc8000)
        return;

    if (_ZN6Player9StartTalkER9ActorBaseb(*(void **)(c + 0x398), c, 1) == 0)
        return;

    a = (char *)_ZN5Actor15FindWithActorIDEjPS_(0xcd, 0);
    if (a == 0)
        return;

    *(int *)(c + 0x394) = *(int *)(a + 4);
    *(unsigned char *)(a + 0x16e) = 0;
    *(int *)(c + 0x38c) = 1;
    {
        char *base = c + 0x300;
        *(short *)(base + 0xa8) = Vec3_HorzAngle((struct Vec3 *)(c + 0x5c), &v);
    }
    *(unsigned char *)(c + 0x390) = 0;
}
