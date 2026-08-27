extern int func_ov007_020c3df4(int a, int b);
extern void func_ov007_020c29b4(char* c);
extern void func_ov007_020c28ac(void* c);

int func_ov007_020c2c68(int a, int b, int cc)
{
    char* p = (char*)func_ov007_020c3df4(0, 0x30);
    func_ov007_020c29b4(p);
    *(int*)(p) = a;
    *(int*)(p + 4) = cc;
    *(short*)(p + 0xa) = b;
    switch (a) {
    case 0:
        *(int*)(p + 0x24) = func_ov007_020c3df4(0, b << 2);
        *(int*)(p + 0x28) = func_ov007_020c3df4(0, b << 2);
        *(int*)(p + 0x1c) = func_ov007_020c3df4(0, b << 1);
        break;
    case 1:
        *(int*)(p + 0x2c) = func_ov007_020c3df4(0, b * 0xc);
        break;
    }
    *(int*)(p + 0x18) = func_ov007_020c3df4(0, b << 2);
    if (*(int*)(p + 4) & 1) {
        *(int*)(p + 0x20) = func_ov007_020c3df4(0, b << 2);
    }
    func_ov007_020c28ac(p);
    return (int)p;
}
