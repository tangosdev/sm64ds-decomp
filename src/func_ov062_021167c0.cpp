//cpp
extern "C" {
extern void ApproachAngle(void* p, short a, int b, int cc, int d);
extern int AngleDiff(int a, int b);
extern void func_ov062_02116cd8(void* c, void* p);
extern int data_ov062_0211dee0;
}

extern "C" int func_ov062_021167c0(char* c){
    ApproachAngle(c + 0x94, *(short*)(c + 0x3f4), 0xa, 0x200, 0x100);
    if (AngleDiff(*(short*)(c + 0x3f4), *(short*)(c + 0x8e)) < 0x100) {
        *(int*)(c + 0x98) = 0xa000;
    } else {
        *(short*)(c + 0x100) = 0x14;
    }
    if (*(unsigned short*)(c + 0x100) == 0) {
        *(short*)(c + 0x3e8) = 0x1e;
        func_ov062_02116cd8(c, &data_ov062_0211dee0);
    }
    return 1;
}
