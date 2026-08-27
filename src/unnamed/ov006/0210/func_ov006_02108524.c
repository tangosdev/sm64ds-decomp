extern int RandomIntInternal(int *seed);
extern void func_ov006_02108508(char *p);
extern int data_0209e650[];
void func_ov006_02108524(char *c) {
    unsigned r;
    *(int *)(c + 0xb0) = 0x4200;
    *(int *)(c + 0xb4) = 0;
    *(short *)(c + 0xc0) = 0;
    *(short *)(c + 0xc2) = 0;
    *(short *)(c + 0xc6) = 0x400;
    *(short *)(c + 0xc8) = -0x200;
    r = (unsigned)RandomIntInternal(data_0209e650);
    *(int *)(c + 0xb8) = ((r & 0x7fffffff) >> 0x13) + 0x2000;
    *(int *)(c + 0xd0) = 0;
    func_ov006_02108508(c);
}
