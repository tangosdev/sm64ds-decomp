extern int func_ov007_020c3df4(int a, int b);
extern int func_ov007_020c9388(int a);
extern int func_ov007_020b8500(void);
extern int func_ov007_020b84e8(void);
extern void func_ov007_020c0384(int c, void* x);
extern int func_ov007_020b84f4(void);
extern void func_ov007_020c02d8(int r0, int r1, unsigned short r2);
extern int* data_ov007_0210342c;

int func_ov007_020adc08(int type, char* obj) {
    int r5 = func_ov007_020c3df4(0, 0xc);
    int* g = data_ov007_0210342c;
    int r4 = g[0x74/4];
    *(char*)(r5+8) = (char)type;
    *(int*)(r5+4) = (int)obj;
    *(int*)r5 = func_ov007_020c9388(0);
    switch (type) {
    case 6:
        *(int*)(obj+0x10) = func_ov007_020b8500();
        *(int*)(obj+0x14) = func_ov007_020b84e8();
        func_ov007_020c0384(*(int*)(r4+8), obj);
        break;
    case 7:
        *(int*)(obj+0x10) = func_ov007_020b84f4();
        *(int*)(obj+0x14) = func_ov007_020b84e8();
        func_ov007_020c0384(*(int*)(r4+0xc), obj);
        break;
    case 1:
        *(int*)(obj+0x14) = *(int*)(obj+0x10);
        *(int*)(obj+0x10) = func_ov007_020c3df4(0, 0x800);
        func_ov007_020c0384(*(int*)(r4+8), obj);
        break;
    case 2:
        func_ov007_020c0384(*(int*)(r4+0xc), obj);
        break;
    case 3:
    case 4:
        func_ov007_020c0384(*(int*)(r4+0xc), obj);
        break;
    case 5:
        *(int*)(obj+0x10) = func_ov007_020c3df4(0, 0x800);
        func_ov007_020c02d8(*(int*)(obj+0x10), 0x18, 0x300);
        func_ov007_020c0384(*(int*)(r4+0xc), obj);
        break;
    default:
        break;
    }
    return r5;
}
