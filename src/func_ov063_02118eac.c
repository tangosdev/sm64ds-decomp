struct Vector3 {
    int x;
    int y;
    int z;
};

extern void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern void func_ov063_02118f24(void *c, void *vec);

void func_ov063_02118eac(char *c)
{
    struct Vector3 v;
    void *p;
    unsigned int id;

    v.x = *(int *)(c + 0x5c);
    v.y = *(int *)(c + 0x60);
    v.z = *(int *)(c + 0x64);
    *(int *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF) =
        *(int *)(((int)c + 0x60) & 0xFFFFFFFFFFFFFFFF) + 0x64000;
    if (*(unsigned int *)(c + 0x49c) == 0) {
        return;
    }
    id = *(unsigned int *)(c + 0x49c);
    p = _ZN5Actor10FindWithIDEj(id);
    *(void **)(c + 0x48c) = p;
    if (*(void **)(c + 0x48c) != 0) {
        func_ov063_02118f24(*(void **)(c + 0x48c), &v);
    }
    *(void **)(c + 0x48c) = 0;
}