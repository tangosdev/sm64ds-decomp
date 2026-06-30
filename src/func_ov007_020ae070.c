typedef unsigned char u8;
typedef unsigned short u16;
extern u8 data_ov007_0210342c[];
extern void func_ov007_020c108c(char *c, int r1, int r2, int r3, int sp0);

void func_ov007_020ae070(char *self)
{
    int *p = *(int**)self;
    u16 state = *(u16*)p;
    int ip2 = *(int*)((char*)*(int**)((char*)p + 4) + 0xc);
    int *g = *(int**)data_ov007_0210342c;
    short idx = *(short*)(*(int**)((char*)g + 0xc));

    if (state != 1) return;
    if (ip2 != 0) return;

    switch (idx) {
    case 3:  func_ov007_020c108c(*(char**)((char*)self + 4), 0, 0, 0, 0); break;
    case 14: func_ov007_020c108c(*(char**)((char*)self + 4), 3, 0, 0, 0); break;
    case 4:  func_ov007_020c108c(*(char**)((char*)self + 4), 2, 0, 0, 0); break;
    case 5:  func_ov007_020c108c(*(char**)((char*)self + 4), 1, 0, 0, 0); break;
    case 7:
        if (*(int*)((char*)g + 0x20) == 1)
            func_ov007_020c108c(*(char**)((char*)self + 4), 1, 0, 0, 0);
        else
            func_ov007_020c108c(*(char**)((char*)self + 4), 2, 0, 0, 0);
        break;
    case 8:  func_ov007_020c108c(*(char**)((char*)self + 4), 4, 0, 0, 0); break;
    case 9:  func_ov007_020c108c(*(char**)((char*)self + 4), 5, 0, 0, 0); break;
    case 10: func_ov007_020c108c(*(char**)((char*)self + 4), 6, 0, 0, 0); break;
    case 12:
        if (*(int*)((char*)g + 0x58) == -1)
            func_ov007_020c108c(*(char**)((char*)self + 4), 7, 0, 0, 0);
        else
            func_ov007_020c108c(*(char**)((char*)self + 4), 8, 0, 0, 0);
        break;
    case 13: func_ov007_020c108c(*(char**)((char*)self + 4), 9, 0, 0, 0); break;
    }
}
