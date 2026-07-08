extern void func_020226a4(char *p);
extern void func_0204dc84(char *p);
extern void *_ZTVN8Particle25EndingStarGlitterCallbackE;

void *func_02022298(char *self) {
    char *e;
    int i;
    func_020226a4(self);
    *(void**)self = &_ZTVN8Particle25EndingStarGlitterCallbackE;
    e = self + 8;
    i = 0;
    do {
        func_0204dc84(e);
        i++;
        e += 0xc;
    } while (i < 0x40);
    *(int*)(self + 0x308) = 0x1000;
    *(short*)(self + 0x30c) = 0x200;
    return self;
}
