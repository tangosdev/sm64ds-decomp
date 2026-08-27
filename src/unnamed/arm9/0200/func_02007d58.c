typedef struct Vector3 Vector3;
extern short Vec3_HorzAngle(const Vector3 *v0, const Vector3 *v1);
extern int SublevelToLevel(int i);
extern void func_020071a8(void *cam, short ang, int threshold);
extern signed char data_0209f2f8;
extern unsigned char data_0209f224;

int func_02007d58(char *cam) {
    int lvl;
    void *p;
    *(short*)(cam + 0x100 + 0x9a) = 0;
    *(short*)(cam + 0x100 + 0xa2) = 0;
    *(short*)(cam + 0x100 + 0xa4) = 0;
    p = *(void**)(cam + 0x148);
    if (p != 0) {
        *(short*)(cam + 0x100 + 0x9a) =
            Vec3_HorzAngle((const Vector3*)p, (const Vector3*)(cam + 0x8c));
    }
    lvl = SublevelToLevel(data_0209f2f8);
    if (lvl == 3 && data_0209f224 == 4) {
        func_020071a8(cam, 0x5600, 0x800);
    } else if (lvl == 0xb && data_0209f224 == 2) {
        func_020071a8(cam, 0, 0x800);
    } else if (lvl == 9 && data_0209f224 == 1) {
        func_020071a8(cam, 0x2000, 0x800);
    } else if (lvl == 0xe && data_0209f224 == 3) {
        func_020071a8(cam, 0, 0x800);
    }
    return 1;
}
