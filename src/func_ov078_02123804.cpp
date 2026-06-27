//cpp
extern "C" void *_ZN5Actor10FindWithIDEj(unsigned int id);
extern "C" int func_ov078_02125c48(char *c, void *p);
extern int data_ov078_02126ffc;

extern "C" int func_ov078_02123804(char *c) {
    unsigned int id = *(unsigned int *)(c + 0x3a0);
    if (id == 0) return 0;
    if (_ZN5Actor10FindWithIDEj(id) == 0) return 0;
    if ((*(unsigned int *)(c + 0x39c) & 0x4000) == 0) return 0;
    func_ov078_02125c48(c, &data_ov078_02126ffc);
    return 1;
}
