extern void *func_020511d4(void *owner, unsigned int elemSize, void *cb, void *obj, unsigned int p4);
extern int func_0205d368(void *o, int r1, int sel);
extern int func_0205d3d4(void *o, int a, int b);
extern void func_02050988(void);
extern void func_0205097c(void);
extern void func_02050970(void);

int func_02050d54(char *obj, void *owner, int flag) {
    unsigned int sz;
    int r;
    *(void **)(obj + 0x84) = func_020511d4(owner, *(unsigned int *)(obj + 0x1c), (void *)func_02050988, obj, 0);
    if (*(void **)(obj + 0x84) == 0) return 0;
    *(void **)(obj + 0x7c) = func_020511d4(owner, *(unsigned int *)(obj + 0x24), (void *)func_0205097c, obj, 0);
    if (*(void **)(obj + 0x7c) == 0) return 0;
    if (flag != 0 && (sz = *(unsigned int *)(obj + 0x14)) != 0) {
        *(void **)(obj + 0x80) = func_020511d4(owner, sz, (void *)func_02050970, obj, 0);
        if (*(void **)(obj + 0x80) == 0) return 0;
    }
    if (func_0205d368((void *)(obj + 0x30), *(int *)(obj + 0x18), 0) == 0) return 0;
    r = func_0205d3d4((void *)(obj + 0x30), *(int *)(obj + 0x84), *(int *)(obj + 0x1c));
    if (r != *(int *)(obj + 0x1c)) return 0;
    if (func_0205d368((void *)(obj + 0x30), *(int *)(obj + 0x20), 0) == 0) return 0;
    r = func_0205d3d4((void *)(obj + 0x30), *(int *)(obj + 0x7c), *(int *)(obj + 0x24));
    if (r != *(int *)(obj + 0x24)) return 0;
    if (flag != 0 && *(int *)(obj + 0x14) != 0) {
        if (func_0205d368((void *)(obj + 0x30), *(int *)(obj + 0x10), 0) == 0) return 0;
        r = func_0205d3d4((void *)(obj + 0x30), *(int *)(obj + 0x80), *(int *)(obj + 0x14));
        if (r != *(int *)(obj + 0x14)) return 0;
    }
    return 1;
}
