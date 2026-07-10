extern short Vec3_HorzAngle(const void *a, const void *b);
extern int _Z14ApproachLinearRsss(short *p, short target, short step);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *self, void *actor, unsigned int msg, const void *pos, unsigned int a, unsigned int b);
extern int _ZN6Player12GetTalkStateEv(void *self);
extern void func_ov072_021205d4(void *thiz, int a);

int func_ov072_02120358(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;

    switch (*(unsigned char *)(c + 0x334)) {
    case 0:
    {
        short ang = Vec3_HorzAngle(c + 0x5c, *(unsigned char **)(c + 0x32c) + 0x5c);
        if (_Z14ApproachLinearRsss((short *)(c + 0x8e), ang, 0x514)) {
            int v[3];
            int y;
            v[0] = *(int *)(c + 0x5c);
            y = *(int *)(c + 0x60);
            v[1] = y;
            v[2] = *(int *)(c + 0x64);
            v[1] = y + 0xfa000;
            if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(*(void **)(c + 0x32c), c, 0xaf, v, 0, 0)) {
                (*(volatile unsigned char *)(((int)c + 0x334) & 0xFFFFFFFFFFFFFFFF))++;
            }
        }
        break;
    }
    case 1:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x32c)) == -1) {
            func_ov072_021205d4(c, 0);
        }
        break;
    }
    return 1;
}
