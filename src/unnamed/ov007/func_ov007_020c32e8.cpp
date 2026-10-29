//cpp

namespace GX {
void LoadBGPltt(const void *p, unsigned int a, unsigned int b);
void LoadOBJPltt(const void *p, unsigned int a, unsigned int b);
}

namespace GXS {
void LoadBGPltt(const void *p, unsigned int a, unsigned int b);
void LoadOBJPltt(const void *p, unsigned int a, unsigned int b);
}

extern "C" void func_ov007_020c32e8(char* c, int sel, unsigned int n, int off){
    unsigned int a1 = n << 5;
    unsigned int a2 = (unsigned int)((*(unsigned short*)(c+4) - off) << 1);
    switch (sel) {
    case 0:
        GX::LoadOBJPltt((char *)(*(void **)c) + off, a1, a2);
        break;
    case 1:
        GX::LoadBGPltt((char *)(*(void **)c) + off, a1, a2);
        break;
    case 2:
        GXS::LoadOBJPltt((char *)(*(void **)c) + off, a1, a2);
        break;
    case 3:
        GXS::LoadBGPltt((char *)(*(void **)c) + off, a1, a2);
        break;
    }
}
