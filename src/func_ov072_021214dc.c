extern void func_ov072_02121d50(void *thiz, int a);
extern int _ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(void *self, void *actor, unsigned int msg, const void *pos, unsigned int a, unsigned int b);
extern int func_0201267c(unsigned int a, void *b);
extern int _ZN6Player12GetTalkStateEv(void *self);
extern void _ZN6Player9DropActorEv(void *self);

int func_ov072_021214dc(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char state = *(unsigned char *)(c + 0x36e);

    switch (state) {
    case 0:
    {
        int f = *(int *)(c + 0xb0);
        int b0 = (int)((f & 0x40000) != 0);
        if (b0 != 0) {
            (*(volatile unsigned char *)(((int)c + 0x36e)))++;
        } else {
            int b1 = (int)((f & 0x20000) != 0);
            if (b1 != 0) break;
            if (b0 != 0) break;
            *(int *)(c + 0xd0) = 0;
            func_ov072_02121d50(c, 0);
        }
        break;
    }
    case 1:
    {
        unsigned int a, b;
        if (_ZN6Player11ShowMessageER7fBase_cjPK7Vector3hh(*(void **)(c + 0x360), c, 0xb2, 0, a = 0, b = 0)) {
            func_0201267c(0xf2, c + 0x74);
            (*(volatile unsigned char *)(((int)c + 0x36e)))++;
        }
        int b2 = (int)((*(int *)(c + 0xb0) & 0x80000) != 0);
        if (b2 != 0) {
            func_ov072_02121d50(c, 5);
        }
        break;
    }
    case 2:
        if (_ZN6Player12GetTalkStateEv(*(void **)(c + 0x360)) == -1) {
            _ZN6Player9DropActorEv(*(void **)(c + 0x360));
            (*(volatile unsigned char *)(((int)c + 0x36e)))++;
        }
        break;
    case 3:
    {
        int b = (int)((*(int *)(c + 0xb0) & 0x80000) != 0);
        if (b != 0) {
            func_ov072_02121d50(c, 5);
        }
        break;
    }
    }
    return 1;
}
