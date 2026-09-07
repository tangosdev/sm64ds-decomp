extern int func_ov030_021141a8(void *a, int b);
extern void *_ZN8dActor_c13ClosestPlayerEv(void *a);
extern void *_ZN8dActor_c10FindWithIDEj(unsigned int id);
extern int _ZN6Player7TryGrabER8dActor_c(void *p, void *a);

int func_ov030_02111bc4(void *thiz)
{
    unsigned char *c = (unsigned char *)thiz;
    unsigned char *r4;
    int b;

    b = (int)((*(int *)(c + 0xb0) & 0x20000) != 0);
    if (b != 0 && *(int *)(c + 0x3b4) != 2) {
        *(int *)(c + 0x3a8) = *(int *)(c + 0xd0);
        b = (int)(*(unsigned short *)(c + 0xc) == 0x10b);
        if (b != 0) {
            func_ov030_021141a8(c, 5);
        } else {
            b = (int)(*(unsigned short *)(c + 0xc) == 0x10c);
            if (b != 0)
                func_ov030_021141a8(c, 6);
        }
        return 1;
    }

    if (*(unsigned int *)(c + 0x184) == 0)
        return 0;

    if ((*(int *)(c + 0x180) & 0x40000) && *(int *)(c + 0x3b4) != 2) {
        *(void **)(c + 0x3a8) = _ZN8dActor_c13ClosestPlayerEv(c);
        *(int *)(c + 0x3b8) = *(int *)(c + 0x3b4);
        func_ov030_021141a8(c, 2);
        return 1;
    }

    r4 = (unsigned char *)_ZN8dActor_c10FindWithIDEj(*(unsigned int *)(c + 0x184));
    if (r4 == 0 || (b = (int)(*(unsigned short *)(r4 + 0xc) == 0xbf)) == 0)
        return 0;

    if (*(unsigned char *)(r4 + 0x706) != 0)
        return 0;
    if (*(unsigned char *)(r4 + 0x6f9) != 0)
        return 0;
    if (*(unsigned char *)(r4 + 0x6fb) != 0)
        return 0;
    if (*(unsigned char *)(r4 + 0x6ff) != 0)
        return 0;

    if (*(int *)(c + 0x180) & 0x1000) {
        if (_ZN6Player7TryGrabER8dActor_c(r4, c)) {
            *(void **)(c + 0x3a8) = r4;
            b = (int)(*(unsigned short *)(c + 0xc) == 0x10b);
            if (b != 0) {
                func_ov030_021141a8(c, 3);
            } else {
                b = (int)(*(unsigned short *)(c + 0xc) == 0x10c);
                if (b != 0)
                    func_ov030_021141a8(c, 4);
            }
        }
    }
    return 1;
}
