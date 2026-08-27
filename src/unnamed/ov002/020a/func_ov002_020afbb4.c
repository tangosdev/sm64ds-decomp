extern int _ZN8dActor_c15IsPlayerInRangeEi(char* c, int r);
extern void func_ov002_020aefb8(void);
extern void func_ov002_020afde4(char* c);
extern void func_ov002_020aeee4(char* c);
extern void func_ov002_020af3a8(char* c);
extern int func_ov002_020af248(char* c, int n);
extern int func_ov002_020af218(char* c, int n);

void func_ov002_020afbb4(char* c)
{
    switch (*(int*)(c + 0x388)) {
    case 0:
        if (_ZN8dActor_c15IsPlayerInRangeEi(c, 0x3e8)) {
            *(int*)(c + 0xa8) = 0x28000;
            *(int*)(c + 0x388) = 1;
        }
        break;
    case 1:
        func_ov002_020aefb8();
        func_ov002_020afde4(c);
        func_ov002_020aeee4(c);
        break;
    case 2:
        func_ov002_020aefb8();
        func_ov002_020af3a8(c);
        func_ov002_020af248(c, 0x1e);
        break;
    }
    func_ov002_020af218(c, 0xbb8);
}
