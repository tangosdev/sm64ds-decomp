extern char *_ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(unsigned int a, unsigned int b, void *v, void *d, int e, int f);
void func_ov063_02116244(char *c) {
    char *r;
    if (*(int*)(c + 0x180) < 5) return;
    if (*(int*)(c + 0x49c) != 0) return;
    *(char**)(c + 0x48c) = _ZN5Actor5SpawnEjjRK7Vector3PK10Vector3_16ii(
        0xd3, *(unsigned int*)(c + 8), c + 0x5c, 0,
        *(signed char*)(c + 0x500 + 0xd0), -1);
    r = *(char**)(c + 0x48c);
    if (r != 0) *(int*)(c + 0x49c) = *(int*)(r + 4);
    *(char**)(c + 0x48c) = 0;
}
