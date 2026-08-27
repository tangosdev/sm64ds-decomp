//cpp
extern "C" {
void _ZN2GX11LoadOBJPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
void _ZN2GX10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
void _ZN3GXS11LoadOBJPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
void _ZN3GXS10LoadBGPlttEPKvjj(const void* p, unsigned int a, unsigned int b);
}

extern "C" void func_ov007_020c32e8(char* c, int sel, unsigned int n, int off){
    unsigned int a1 = n << 5;
    unsigned int a2 = (unsigned int)((*(unsigned short*)(c+4) - off) << 1);
    switch (sel) {
    case 0:
        _ZN2GX11LoadOBJPlttEPKvjj((char*)(*(void**)c) + off, a1, a2);
        break;
    case 1:
        _ZN2GX10LoadBGPlttEPKvjj((char*)(*(void**)c) + off, a1, a2);
        break;
    case 2:
        _ZN3GXS11LoadOBJPlttEPKvjj((char*)(*(void**)c) + off, a1, a2);
        break;
    case 3:
        _ZN3GXS10LoadBGPlttEPKvjj((char*)(*(void**)c) + off, a1, a2);
        break;
    }
}
