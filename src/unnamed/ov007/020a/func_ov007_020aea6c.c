extern char* func_ov007_020c3df4(int a, int b);
extern int func_ov007_020c9388(int a);
extern void func_ov007_020ae810(char* c);
char* func_ov007_020aea6c(short a, void* b, void* c)
{
    char* o = func_ov007_020c3df4(0, 0x2c);
    *(int*)(o+4) = func_ov007_020c9388(1);
    *(short*)(o+0) = a;
    *(short*)(o+2) = 0;
    *(void**)(o+0x24) = b;
    *(void**)(o+0x28) = c;
    *(int*)(o+0xc) = 0;
    *(int*)(o+8) = *(int*)(o+0xc);
    *(int*)(o+0x20) = 1;
    *(int*)(o+0x10) = 0x1000;
    *(int*)(o+0x14) = 0x1000;
    *(int*)(o+0x18) = 0;
    func_ov007_020ae810(o);
    return o;
}
