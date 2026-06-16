extern void* _ZN5Actor10FindWithIDEj(unsigned int);
static int normalize(int r) { if (r) return 1; return 0; }
int func_ov085_02129f8c(char *c) {
    unsigned int id = *(unsigned int*)(c+0x184);
    void *actor;
    unsigned short kind;
    int r;
    if (id == 0) return id;
    actor = _ZN5Actor10FindWithIDEj(id);
    if (actor == 0) return (int)actor;
    kind = *(unsigned short*)((char*)actor+0xc);
    if (kind == 0xbf) r = 1; else r = 0;
    return normalize(r);
}
