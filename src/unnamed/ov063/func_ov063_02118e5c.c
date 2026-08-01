extern void func_ov063_02118eac(void *c);
extern void *_ZN5Actor10FindWithIDEj(unsigned int id);

void func_ov063_02118e5c(void *c) {
    func_ov063_02118eac(c);
    unsigned int id = *(unsigned int*)((char*)c + 0x498);
    if (id == 0) return;
    void *actor = _ZN5Actor10FindWithIDEj(id);
    *(void**)((char*)c + 0x48c) = actor;
    void *p = *(void**)((char*)c + 0x48c);
    if (p != 0) {
        *(char*)((char*)p + 0x150) = 1;
    }
    *(void**)((char*)c + 0x48c) = 0;
}
