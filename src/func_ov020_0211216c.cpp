//cpp
extern "C" {
extern void Matrix4x3_FromRotationZXYExt(void* m, int x, int y, int z);
extern void _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(void* thiz, void* sm, void* mtx, int f, int g, unsigned int h);

void func_ov020_0211216c(char* c)
{
    char* m = (*(unsigned char*)(c + 0x450) != 0) ? (c + 0x12c) : (c + 0x190);
    Matrix4x3_FromRotationZXYExt(m, *(short*)(c + 0x8c), *(short*)(c + 0x8e), *(short*)(c + 0x90));
    *(int*)(m + 0x24) = *(int*)(c + 0x5c) >> 3;
    *(int*)(m + 0x28) = *(int*)(c + 0x60) >> 3;
    *(int*)(m + 0x2c) = *(int*)(c + 0x64) >> 3;
    int b = (*(unsigned short*)(c + 0xc) == 0x147);
    if (b == 0) return;
    *(int*)(c + 0x210) = *(int*)(c + 0x5c) >> 3;
    *(int*)(c + 0x214) = *(int*)(c + 0x430) >> 3;
    *(int*)(c + 0x218) = *(int*)(c + 0x64) >> 3;
    _ZN5Actor19DropShadowRadHeightER11ShadowModelR9Matrix4x35Fix12IiES5_j(
        c, c + 0x1c4, c + 0x1ec, *(int*)(c + 0x80) * 0x64, 0x12c000, 0xf);
}
}
