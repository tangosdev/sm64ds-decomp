
extern int func_ov007_020c0128(void* c);
extern int _ZN4cstd3divEii(int a, int b);
extern void func_ov007_020bffb8(void* c);
extern void _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(void* eye, void* at, void* up, int persp, void* m);
extern void func_ov007_020c0078(void* c);
extern char* data_ov007_0210342c;

void func_ov007_020b1fa4(int self)
{
    char* r4 = *(char**)((char*)data_ov007_0210342c + 0x30);
    func_ov007_020c0128(r4);
    switch (self)
    {
    case 1:
        switch (*(int*)((char*)data_ov007_0210342c + 0xec))
        {
        case 0:
            *(int*)(r4 + 0x18) = -0x3e8;
            *(int*)(r4 + 0x2c) = 0x1e000;
            *(short*)(r4 + 0xd4) = (short)_ZN4cstd3divEii(0xf0000, 0x168);
            break;
        case 1:
            *(int*)(r4 + 0x18) = -0x7d0;
            *(int*)(r4 + 0x2c) = 0x28000;
            *(short*)(r4 + 0xd4) = (short)_ZN4cstd3divEii(0x120000, 0x168);
            break;
        }
        break;
    case 2:
        *(int*)(r4 + 0x2c) = 0xa000;
        break;
    case 3:
        *(int*)(r4 + 0x2c) = 0x5000;
        *(short*)(r4 + 0xd4) = (short)_ZN4cstd3divEii(0x140000, 0x168);
        break;
    case 4:
        *(int*)(r4 + 0x18) = -0x3e8;
        *(int*)(r4 + 0x2c) = 0x1e000;
        *(short*)(r4 + 0xd4) = (short)_ZN4cstd3divEii(0xf0000, 0x168);
        break;
    }
    func_ov007_020bffb8(r4);
    _ZN3G3i7LookAt_EPK7Vector3S2_S2_bP9Matrix4x3(r4 + 8, r4 + 0x20, r4 + 0x14, 1, r4 + 0x44);
    func_ov007_020c0078(r4);
    *(int*)((char*)data_ov007_0210342c + 0x24) = self;
}
