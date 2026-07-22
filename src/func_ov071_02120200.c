extern void _ZN12CylinderClsn5ClearEv(void* p);

int func_ov071_02120200(char* c)
{
    int* p = (int*)(((int)c + 0xb0) & 0xFFFFFFFFFFFFFFFFLL);
    int z;
    short ang;

    *p = *p & ~0x10000001;
    *(int*)(c + 0x5c) = *(int*)(c + 0x384);
    z = 0;
    *(int*)(c + 0x60) = *(int*)(c + 0x388);
    *(int*)(c + 0x64) = *(int*)(c + 0x38c);
    *(short*)(c + 0x94) = *(short*)(c + 0x3a6);
    ang = *(short*)(c + 0x3a6);
    *(short*)(c + 0x8c) = z;
    *(short*)(c + 0x8e) = ang;
    *(short*)(c + 0x90) = z;
    *(int*)(c + 0x98) = z;
    *(short*)(c + 0x3a8) = 0x1e;
    _ZN12CylinderClsn5ClearEv(c + 0x160);
    *(int*)(c + 0x39c) = 0;
    return 1;
}
