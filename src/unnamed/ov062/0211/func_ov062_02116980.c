extern short Vec3_HorzAngle(void *v0, void *v1);
extern int AngleDiff(int a, int b);

int func_ov062_02116980(char *c) {
    *(int*)(c+0x5c) = *(int*)(c+0x3d8);
    *(int*)(c+0x60) = *(int*)(c+0x3dc);
    *(int*)(c+0x64) = *(int*)(c+0x3e0);
    *(int*)(c+0x98) = 0;
    if (AngleDiff(*(short*)(c+0x3f4), Vec3_HorzAngle(c+0x5c, c+0x3c0)) <= 0x2000) {
        *(short*)(c+0x3f4) = *(short*)(c+0x94) - 0x1000;
    } else {
        *(short*)(c+0x3f4) = Vec3_HorzAngle(c+0x5c, c+0x3c0);
    }
    *(short*)(c+0x100) = 0x46;
    return 1;
}
