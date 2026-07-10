struct Vec3 { int x, y, z; };

extern int Vec3_Dist(void *a, void *b);
extern int func_ov077_02126300(void *c);
extern int func_ov077_02126d5c(void *c, void *p);
extern int _ZNK12WithMeshClsn8IsOnWallEv(void *thiz);
extern void *_ZN5Actor13ClosestPlayerEv(void *thiz);
extern short Vec3_HorzAngle(void *a, void *b);
extern void _Z14ApproachLinearRsss(short *r, short a, short b);
extern unsigned int _ZN5Sound8PlayLongEjjjRK7Vector3j(unsigned int a, unsigned int b, unsigned int c, void *v, unsigned int d);
extern int __aeabi_idiv(int a, int b);
extern char data_ov077_02127d18[];
extern char data_ov077_02127cf8[];

int func_ov077_0212679c(char *c)
{
    struct Vec3 pv;
    struct Vec3 *pp;
    int dist;
    char *p;
    unsigned short spd;

    dist = Vec3_Dist((void *)(c + 0x5c), (void *)(c + 0x404));

    if (func_ov077_02126300(c) != 0) {
        func_ov077_02126d5c(c, data_ov077_02127d18);
        return 1;
    }

    if (_ZNK12WithMeshClsn8IsOnWallEv(c + 0x184) != 0) {
        *(int *)(c + 0x5c) = *(int *)(c + 0x410);
        *(int *)(c + 0x60) = *(int *)(c + 0x414);
        *(int *)(c + 0x64) = *(int *)(c + 0x418);
    }

    p = (char *)_ZN5Actor13ClosestPlayerEv(c);
    if (p != 0) {
        /* u64 launder forces base materialization after the null cmp */
        pp = (struct Vec3 *)(void *)(unsigned long long)(unsigned long)(p + 0x5c);
        pv.x = pp->x;
        pv.y = pp->y;
        pv.z = pp->z;
        *(short *)(c + 0x420) = Vec3_HorzAngle((void *)(c + 0x5c), &pv);
    }

    _Z14ApproachLinearRsss((short *)(c + 0x94), *(short *)(c + 0x420), *(short *)(c + 0x422));
    _Z14ApproachLinearRsss((short *)(c + 0x422), 0x600, 0x100);

    *(unsigned int *)(c + 0x428) = _ZN5Sound8PlayLongEjjjRK7Vector3j(
        *(unsigned int *)(c + 0x428), 3, 0x186, c + 0x74, 0);

    spd = *(unsigned short *)(c + 0x100);
    if (spd < 0x64) {
        *(int *)(c + 0x39c) = __aeabi_idiv(0x1000, 5 - spd / 20);
    }

    if (dist > 0x5dc000 || *(unsigned short *)(c + 0x100) == 0 || func_ov077_02126300(c) != 0)
        func_ov077_02126d5c(c, data_ov077_02127cf8);

    return 1;
}
