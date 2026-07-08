extern void func_0200d438(int a, unsigned char b);
extern int CAMERA;

int func_ov002_020df300(char* p) {
    if (*(unsigned char*)(p + 0x6e3)) return 0;
    *(unsigned char*)(p + 0x6e3) = 1;
    func_0200d438(CAMERA, *(unsigned char*)(p + 0x6d8));
    return 1;
}
