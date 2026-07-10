//cpp
extern "C" {
int _ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(void* c, short a, short b, short d, int e);
void func_02039394(int* p, int v);
void func_020393a4(int* p, int v);
void _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(void* c, void* sm, void* mtx, int s, int x, int y, unsigned int j);
int _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(void* c, int a, int b);

int _ZN14ArrowSignRight8BehaviorEv(char* c)
{
    if (_ZN8Platform20UpdateKillByMegaCharEsss5Fix12IiE(c, -0x2000, 0, 0, 0x96000))
        return 1;
    func_02039394((int*)(c+0x124), 0xc0000);
    func_020393a4((int*)(c+0x124), 0xe0000);
    _ZN5Actor18DropShadowScaleXYZER11ShadowModelR9Matrix4x35Fix12IiES5_S5_j(
        c, (void*)(c+0x320), (void*)(c+0x348), 0x10e000, 0x64000, 0x46000, 0xf);
    _ZN8Platform21IsClsnInRangeOnScreenE5Fix12IiES1_(c, 0x600000, 0);
    return 1;
}
}
