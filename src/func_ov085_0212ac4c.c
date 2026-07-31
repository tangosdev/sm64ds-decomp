extern char *_ZN5Actor13ClosestPlayerEv(void *self);
extern char *_ZN5Actor10FindWithIDEj(unsigned int id);
extern int _ZN6Player9StartTalkER9ActorBaseb(void *self, void *other, int b);
extern int func_ov085_0212bc78(void *c, void *p);
extern int data_ov085_021306dc;

int func_ov085_0212ac4c(char *c)
{
    unsigned short h;
    int ok;
    char *o;
    char *cp;
    int *p128;
    int b;
    int a;
    int flags;
    char *obj;

    obj = *(char **)(c + 0x45c);
    if (obj != 0) {
        flags = *(int *)(c + 0xb0);
        a = (flags & 0x400) ? 1 : 0;
        if (a == 0) {
            b = (flags & 0x2000) ? 1 : 0;
            if (b == 0)
                goto after_clear;
        }
        if (a != 0) {
            if (obj != 0) {
                *(short *)(c + 0x94) = *(short *)(obj + 0x8e);
            }
        }
        p128 = (int *)(((long long)(int)(c + 0x128)));
        b = 0;
        *p128 = (*p128) & ~2;
        *(int *)(c + 0x45c) = b;
    after_clear:
        a = (*(int *)(c + 0xb0) & 0x100) ? 1 : 0;
        if (a == 0) {
            *(int *)(c + 0x45c) = 0;
        }
        obj = *(char **)(c + 0x45c);
        if (obj != 0) {
            if (*(unsigned char *)(obj + 0x706) != 0) {
                *(int *)(c + 0x45c) = 0;
            }
        }
    }

    cp = _ZN5Actor13ClosestPlayerEv(c);
    if (cp != 0) {
        h = *(unsigned short *)(cp + 0x600 + 0xce);
        h = (unsigned short)(h & 0x800);
        if (h != 0)
            return 1;
    }

    if ((*(int *)(c + 0x130) & 0x8000000) != 0) {
        o = _ZN5Actor10FindWithIDEj(*(unsigned int *)(c + 0x134));
        if (o != 0) {
            ok = (int)(*(unsigned short *)(o + 0xc) == (unsigned short)0xbf);
            if (ok != 0) {
                *(char **)(c + 0x460) = o;
                o = *(char **)(c + 0x460);
                if (_ZN6Player9StartTalkER9ActorBaseb(o, c, 0) != 0) {
                    func_ov085_0212bc78(c, &data_ov085_021306dc);
                }
            }
        }
    }

    return 1;
}
