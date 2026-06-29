//cpp
// func_ov007_020c33d0 at 0x020c33d0
// Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov007).
extern "C" {
void _ZN2GX7LoadOBJEPKvjj(const void* p, unsigned int a, unsigned int b);
void _ZN2GX11LoadBG0CharEPKvjj(const void* p, unsigned int a, unsigned int b);
void _ZN2GX11LoadBG1CharEPKvjj(const void* p, unsigned int a, unsigned int b);
void func_020560d4(const void* p, unsigned int a, unsigned int b);
void func_02056014(const void* p, unsigned int a, unsigned int b);
void func_020565b4(const void* p, unsigned int a, unsigned int b);
void _ZN3GXS11LoadBG0CharEPKvjj(const void* p, unsigned int a, unsigned int b);
void _ZN3GXS11LoadBG1CharEPKvjj(const void* p, unsigned int a, unsigned int b);
void func_02056074(const void* p, unsigned int a, unsigned int b);
void func_02055fb4(const void* p, unsigned int a, unsigned int b);
}

extern "C" void func_ov007_020c33d0(char* c, int sel, unsigned int off){
    unsigned int n = *(int*)(c+4) == 0 ? 0x20 : 0x40;
    unsigned int size = (unsigned int)*(unsigned char*)(c+8) * *(unsigned char*)(c+9) * n;
    switch (sel) {
    case 0:
        _ZN2GX7LoadOBJEPKvjj(*(void**)c, off, size);
        break;
    case 1:
        _ZN2GX11LoadBG0CharEPKvjj(*(void**)c, off, size);
        break;
    case 2:
        _ZN2GX11LoadBG1CharEPKvjj(*(void**)c, off, size);
        break;
    case 3:
        func_020560d4(*(void**)c, off, size);
        break;
    case 4:
        func_02056014(*(void**)c, off, size);
        break;
    case 5:
        func_020565b4(*(void**)c, off, size);
        break;
    case 6:
        _ZN3GXS11LoadBG0CharEPKvjj(*(void**)c, off, size);
        break;
    case 7:
        _ZN3GXS11LoadBG1CharEPKvjj(*(void**)c, off, size);
        break;
    case 8:
        func_02056074(*(void**)c, off, size);
        break;
    case 9:
        func_02055fb4(*(void**)c, off, size);
        break;
    }
}
