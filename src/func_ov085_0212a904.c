struct Vector3 { int x, y, z; };

extern short Vec3_HorzAngle(const struct Vector3 *a, const struct Vector3 *b);
extern int ApproachAngle(void *p, int from, int start, int speed, int max);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void *thiz, void *actor, unsigned int msg, const struct Vector3 *vec, unsigned int a, unsigned int b);
extern void func_02012790(int a);
extern void _ZN7Message13DisplaySavingEt(unsigned short a);
extern void _ZN7Message7EndTalkEv(void);
extern void func_ov085_0212bc78(void *c, void *p);

extern unsigned char data_0209d684;
extern unsigned char data_0209d660;
extern char data_ov085_021306bc[];

int func_ov085_0212a904(void *thiz)
{
    char *self = (char *)thiz;
    char *r4 = *(char **)(self + 0x460);
    struct Vector3 vec;
    unsigned char gb;
    int state;

    *(short *)(self + 0x424) = Vec3_HorzAngle((struct Vector3 *)(self + 0x5c), (struct Vector3 *)(r4 + 0x5c));
    ApproachAngle(self + 0x94, *(short *)(self + 0x424), 1, 0x500, 0x500);

    gb = data_0209d684;
    vec.x = *(int *)(self + 0x5c);
    vec.y = *(int *)(self + 0x60);
    vec.z = *(int *)(self + 0x64);
    vec.y += 0x3c000;

    state = *(int *)(self + 0x41c);
    switch (state) {
    case 0:
        if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(r4, self, 0x148, &vec, 0, 0)) {
            func_02012790(0xa);
            {
                int *p = (int *)(((int)self + 0x41c) & 0xFFFFFFFFFFFFFFFF);
                (*p)++;
            }
        }
        break;
    case 1:
        if (data_0209d660 == 0) {
            if (gb == 1) {
                func_02012790(0x5e);
                _ZN7Message13DisplaySavingEt(0x295);
                {
                    int *p = (int *)(((int)self + 0x41c) & 0xFFFFFFFFFFFFFFFF);
                    (*p)++;
                }
            } else if (gb == 2) {
                func_02012790(0x98);
                {
                    unsigned short *hp = (unsigned short *)(((int)r4 + 0x6ce) & 0xFFFFFFFFFFFFFFFF);
                    *hp &= ~0x800;
                }
                _ZN7Message7EndTalkEv();
                func_ov085_0212bc78(self, data_ov085_021306bc);
            }
        }
        break;
    case 2:
        if (data_0209d660 == 0) {
            unsigned short *hp = (unsigned short *)(((int)r4 + 0x6ce) & 0xFFFFFFFFFFFFFFFF);
            *hp &= ~0x800;
            _ZN7Message7EndTalkEv();
            func_ov085_0212bc78(self, data_ov085_021306bc);
        }
        break;
    }
    return 1;
}
