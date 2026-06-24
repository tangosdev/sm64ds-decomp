//cpp
extern "C" {
extern void func_ov007_020b283c(void);
extern void func_ov007_020b9770(void);
extern void func_ov007_020bf304(void);
extern void func_ov007_020b6544(void);
extern void func_ov007_020bcba8(int i);
extern void func_ov007_020bca40(int i);
extern void func_ov007_020bfacc(int a, int b, int c);
extern void func_ov007_020bd1c0(int i);
extern void func_ov007_020b131c(void);
extern char *data_ov007_0210342c;
}

typedef int (*FP)(int);
#define G  (data_ov007_0210342c)

extern "C" void func_ov007_020b066c(void)
{
    switch (*(short*)(*(char**)(G + 4) + 0xa)) {
    case 6:
        func_ov007_020b283c();
        break;
    case 4:
        func_ov007_020b9770();
        break;
    case 5:
        func_ov007_020bf304();
        (*(FP*)(G + 0x2c))(0x11);
        break;
    default:
        *(unsigned char*)(*(char**)(G + 0x28) + 0x38) = (unsigned char)(*(FP*)(G + 0x2c))(0x15);
        *(unsigned char*)(*(char**)(G + 0x28) + 0x37) = (unsigned char)(*(FP*)(G + 0x2c))(0x14);
        *(unsigned char*)(*(char**)(G + 0x28) + 0x39) = 0;
        break;
    }

    func_ov007_020b6544();
    func_ov007_020bcba8(1);
    func_ov007_020bca40(0);

    {
        char *bb = G;
        if ((unsigned short)(short)(*(short*)(*(char**)(bb + 4) + 0xa) - 4) <= 2) {
            *(short*)(*(char**)(bb + 8) + 2) = 5;
        } else {
            switch (*(int*)(*(char**)(*(char**)(bb + 0x28)) + 4)) {
            case 0:
            case 2:
                *(short*)(*(char**)(bb + 8) + 2) = 0xb;
                break;
            case 1:
                *(short*)(*(char**)(bb + 8) + 2) = 0xa;
                func_ov007_020bfacc(3, 0xa, 0);
                break;
            }
        }
    }

    *(short*)(*(char**)(G + 0xc) + 2) = 0;

    *(int*)(G + 0xec) = *(int*)(G + 0xf0);

    if (*(int*)(G + 0xf0) == 0)
        *(int*)(G + 0xf0) = 1;
    else
        *(int*)(G + 0xf0) = 0;

    func_ov007_020bd1c0(0);

    *(int*)(*(char**)(G + 0xf4) + 0x94) = 0;
    *(int*)(*(char**)(*(char**)(G + 0x100) + 4) + 0x94) = 0;
    *(int*)(*(char**)(*(char**)(G + 0x104) + 4) + 0x94) = 0;
    func_ov007_020b131c();
}
