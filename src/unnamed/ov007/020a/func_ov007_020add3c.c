extern int func_ov007_020ae558(void);
extern void func_ov007_020ae834(void* p);
extern void func_ov007_020ae2d0(void* c);
extern void func_ov007_020ae070(void* c);
extern void func_ov007_020ae454(void* c);

#define P0 (*(char**)(c))
#define P1 (*(char**)(c+4))

void func_ov007_020add3c(char* c)
{
    int r = func_ov007_020ae558();
    *(short*)(*(char**)(P0+4)+2) = (short)r;
    func_ov007_020ae834(P0);
    switch (*(short*)(*(char**)(P0+4))) {
    case 0:
        func_ov007_020ae2d0(c);
        break;
    case 1: break;
    case 2: break;
    case 3:
        func_ov007_020ae070(c);
        break;
    case 4: break;
    case 5: break;
    case 6: break;
    case 7: break;
    }
    func_ov007_020ae454(c);
    *(int*)(P1+0) = *(int*)(P0+8);
    *(int*)(P1+4) = *(int*)(P0+0xc);
    *(int*)(P1+8) = *(int*)(P0+0x20);
    if (*(int*)(P1+8) != 0) {
        if (*(int*)(P1+4) <= -0x64000) *(int*)(P1+8) = 0;
    }
}
