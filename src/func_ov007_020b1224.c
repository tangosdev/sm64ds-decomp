extern void func_ov007_020b1390(int x);
extern char* func_ov007_020b2cdc(void);
extern void* data_ov007_0210342c;

void func_ov007_020b1224(void)
{
    char* r1 = (char*)data_ov007_0210342c;
    {
        char* p1 = *(char**)(r1 + 0x134);
        char* p2 = *(char**)(p1);
        char* p3 = *(char**)(p2 + 4);
        if (*(short*)p3 != 0) return;
    }
    switch (*(int*)(r1 + 0x180)) {
    case 9: case 10: case 11: func_ov007_020b1390(0); break;
    case 12: case 13: func_ov007_020b1390(1); break;
    case 8: func_ov007_020b1390(3); break;
    }
    {
        int val = *(int*)((char*)data_ov007_0210342c + 0x180);
        if (val == -1) return;
        if ((unsigned int)(val - 0xa) <= 1) {
            char* p = func_ov007_020b2cdc();
            *(short*)(p + 2) = 9;
        } else {
            char* p = func_ov007_020b2cdc();
            *(short*)(p + 2) = (short)val;
        }
    }
}
