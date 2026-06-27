//cpp
extern "C" void *_ZN5Actor10FindWithIDEj(unsigned int id);

extern "C" unsigned char func_ov063_021163d0(char *c) {
    unsigned int id = *(unsigned int *)(c + 0x498);
    if (id == 0) return 0;
    char *actor = (char *)_ZN5Actor10FindWithIDEj(id);
    if (actor == 0) return 0;
    return *(unsigned char *)(actor + 0x153);
}
