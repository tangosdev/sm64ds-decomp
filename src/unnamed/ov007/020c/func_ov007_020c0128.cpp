//cpp
extern "C" {
extern int _ZN4cstd3divEii(int a, int b);
extern long long _ZN4cstd4ldivEii(int a, int b);
}
extern "C" void Matrix4x3_LoadIdentity(void *m);
extern "C" void func_02052ec8(void *p);

extern "C" void func_ov007_020c0128(char *c)
{
    *(int *)(c + 0xc) = 0;
    *(int *)(c + 8) = *(int *)(c + 0xc);
    *(int *)(c + 0x10) = 0x1000;
    *(int *)(c + 0x1c) = 0;
    *(int *)(c + 0x18) = *(int *)(c + 0x1c);
    *(int *)(c + 0x14) = *(int *)(c + 0x18);
    *(int *)(c + 0x2c) = 0;
    *(unsigned short *)(c + 0x34) = 0;
    *(unsigned short *)(c + 0x32) = *(unsigned short *)(c + 0x34);
    *(unsigned short *)(c + 0x30) = *(unsigned short *)(c + 0x32);
    *(int *)(c + 0x28) = 0;
    *(int *)(c + 0x20) = *(int *)(c + 0x28);
    *(int *)(c + 0x24) = 0x1000;
    *(int *)(c + 0xc4) = 0x1000;
    *(int *)(c + 0xc8) = 0x190000;
    *(short *)(c + 0xd4) = (short)_ZN4cstd3divEii(0x1e0000, 0x168);
    long long l = _ZN4cstd4ldivEii(0x4000, 0x3000);
    *(int *)(c + 0xbc) = (int)l;
    *(int *)(c + 0xc0) = (int)(l >> 32);
    *(unsigned char *)(c + 0xb8) = 0;
    *(unsigned char *)(c + 0xb9) = 0xff;
    *(unsigned char *)(c + 0xba) = 0;
    *(unsigned char *)(c + 0xbb) = 0xbf;
    *(int *)(c + 0xcc) = 0x1000;
    *(int *)(c + 0xd0) = 0xc00;
    Matrix4x3_LoadIdentity(c + 0x44);
    func_02052ec8(c + 0x74);
    *(int *)(c + 0xb4) = 0x1000;
    *(int *)(c + 0) = 0;
    *(int *)(c + 4) = 0;
}
