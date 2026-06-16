extern void* _ZN5Actor10FindWithIDEj(unsigned int id);

void func_ov026_02112280(char* c) {
    unsigned int id = *(unsigned int*)(c + 0x134);
    if (id == 0) return;
    void *r = _ZN5Actor10FindWithIDEj(id);
    if (r == 0) return;
}
