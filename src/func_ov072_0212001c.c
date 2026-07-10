struct Vec3 { int x, y, z; };
extern void *_ZN5Actor13ClosestPlayerEv(void *self);
extern int Vec3_HorzDist(void *a, void *b);
extern int _ZN6Player9StartTalkER9ActorBaseb(void *player, void *actor, int b);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *player, void *actor, unsigned msg, const void *pos, unsigned a, unsigned b);
extern int _ZN6Player12GetTalkStateEv(void *player);
extern void *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned id, unsigned param, const void *pos, const void *ang, int a, int b);
extern void _ZN9Animation7AdvanceEv(void *anim);
extern int _Z14ApproachLinearRsss(short *p, short to, short step);

int func_ov072_0212001c(char *c)
{
    struct Vec3 v;
    unsigned char *st;
    unsigned int param;
    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    v.y = v.y + 0x1c2000;
    switch (*(unsigned char *)(c + 0x334)) {
    case 0:
        *(void **)(c + 0x32c) = _ZN5Actor13ClosestPlayerEv(c);
        if (Vec3_HorzDist(c + 0x5c, *(char **)(c + 0x32c) + 0x5c) < 0x118000) {
            if (_ZN6Player9StartTalkER9ActorBaseb(*(void **)(c + 0x32c), c, 1)) {
                st = (unsigned char *)(((int)c + 0x334) & 0xFFFFFFFFFFFFFFFFLL);
                *st = *st + 1;
            }
        }
        break;
    case 1:
        if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(void **)(c + 0x32c), c, 0xb1, &v, 0, 2)) {
            st = (unsigned char *)(((int)c + 0x334) & 0xFFFFFFFFFFFFFFFFLL);
            *st = *st + 1;
        }
        break;
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x32c)) == -1) {
            param = *(int *)(c + 8) & 0xf;
            param = param & 0xff;
            param = param | 0x40;
            _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
                0xb2,
                param,
                c + 0x5c,
                0,
                *(signed char *)(c + 0xcc),
                -1);
            st = (unsigned char *)(((int)c + 0x334) & 0xFFFFFFFFFFFFFFFFLL);
            *st = *st + 1;
        }
        break;
    }
    _ZN9Animation7AdvanceEv(c + 0x124);
    _Z14ApproachLinearRsss((short *)(c + 0x8e), (short)-0x4000, 0x514);
    *(int *)(c + 0x60) = (int)0xffc427c0;
    return 1;
}
