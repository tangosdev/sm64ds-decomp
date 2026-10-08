extern void *func_ov007_020c3df4(int a, int b);
extern int func_02046564(void *p);
extern int func_020462bc(void *p);
extern void *func_ov007_020c92ac(void);
extern void func_ov007_020bc47c(void *thiz);

void *func_ov007_020bc6d4(char *r6, void *r7)
{
    char *o;
    void **f0;
    int n;

    o = (char *)func_ov007_020c3df4(0, 0xa0);
    *(void **)o = func_ov007_020c3df4(0, 0x14);
    f0 = *(void ***)o;
    *(void **)(o + 0x84) = func_ov007_020c3df4(0, *(int *)(r6 + 0x24) << 2);
    *(void **)(o + 0x9c) = r7;
    *(int *)(o + 0x10) = 0;
    *(int *)(o + 0x14) = 0;
    *(int *)(o + 0x18) = 0;
    *(int *)(o + 0x1c) = 0;
    *(int *)(o + 0x20) = 0;
    *(int *)(o + 0x24) = 0;
    *(int *)(o + 0x28) = 0;
    *(int *)(o + 0x2c) = 0;
    *(int *)(o + 0x30) = 0;
    n = func_02046564(r6);
    *(void **)(o + 4) = func_ov007_020c3df4(0, n);
    if (*(int *)(o + 0x9c) & 2)
        *(void **)(o + 8) = func_ov007_020c3df4(0, n);
    else
        *(int *)(o + 8) = 0;
    if (*(int *)(o + 0x9c) & 4) {
        *(void **)(o + 0xc) = func_ov007_020c3df4(0, func_020462bc(r6));
        *(void **)(o + 0x88) = func_ov007_020c92ac();
    } else {
        *(int *)(o + 0xc) = 0;
        *(int *)(o + 0x88) = 0;
    }
    *f0 = r6;
    func_ov007_020bc47c(o);
    return o;
}
