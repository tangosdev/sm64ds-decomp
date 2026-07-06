extern void func_ov001_020aa6cc(int r0);
extern void func_ov001_020aa6b0(int* r0, int r1);
extern void* _ZN5Actor10FindWithIDEj(unsigned id);
extern int func_0202a8e0(int a, unsigned char b);
extern void func_ov001_020aa6e4(int idx, unsigned r1, int* obj);
extern void func_ov001_020ab110(int* r0);

extern char* data_0209f394;
extern int data_0209f3e8[];
extern unsigned char data_ov001_020ad62c[];
extern int* data_ov001_020ad634[];

#define LAUNDER_U8_PTR(p) ((unsigned char*)(((long long)(int)(p)) & 0xFFFFFFFFFFFFFFFFLL))

void func_ov001_020aadac(void) {
    char* fp = data_0209f394;
    int* sl;
    unsigned i1, i2;

    for (i1 = 0; i1 < 3; i1 = (i1 + 1) & 0xff) {
        if (data_ov001_020ad62c[i1] & 2) continue;
        func_ov001_020aa6cc(i1);
        sl = data_ov001_020ad634[i1];
        while (sl != 0) {
            unsigned char* fl = LAUNDER_U8_PTR((char*)sl + 0x1b);
            *fl &= ~2;
            if (sl[0x14/4] != -1) {
                func_ov001_020aa6b0(sl, 0);
                data_0209f3e8[sl[0x14/4]] = 0;
            }
            sl = (int*)sl[0x10/4];
        }
        if (i1 != *(unsigned char*)(fp + 0x6d9)) {
            data_ov001_020ad62c[i1] |= 2;
        }
    }

    for (i2 = 0; i2 < 3; i2 = (i2 + 1) & 0xff) {
        unsigned char* flags;
        int* o;
        flags = &data_ov001_020ad62c[i2];
        if (*flags & 2) continue;
        o = data_ov001_020ad634[i2];
        if (o == 0) continue;
        do {
            void* fw = _ZN5Actor10FindWithIDEj(o[8/4]);
            if (fw != 0 || (int)fw != o[4/4]) {
                if (*(unsigned char*)((char*)o + 0x19) == 4) {
                    unsigned char* fl = LAUNDER_U8_PTR((char*)o + 0x1b);
                    *fl |= 2;
                    o[0x14/4] = func_0202a8e0(o[4/4], *(unsigned char*)((char*)o + 0x18));
                    func_ov001_020aa6b0(o, 1);
                    func_ov001_020aa6e4(i2, *(unsigned char*)((char*)o + 0x19), 0);
                    *flags |= 2;
                }
            } else {
                func_ov001_020ab110(o);
            }
            o = (int*)o[0x10/4];
        } while (o != 0);
    }
}
