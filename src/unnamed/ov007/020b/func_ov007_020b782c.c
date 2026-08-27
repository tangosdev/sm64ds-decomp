int func_ov007_020c3df4(int a, void* b);
char* func_0204a4c8(char* (*alloc)(int), int a, int b, int c, unsigned short arg5);
int func_ov007_020b3f20(int a);
void func_0204a17c(int a, int b);

extern char data_ov007_02102dd0[];
extern void* data_ov007_02103448;
extern int data_ov007_02103444;
extern char* (func_ov007_020b78d0)(int);
extern void* data_ov007_0210344c;
extern int data_ov007_02103450[];

void func_ov007_020b782c(void)
{
    int i, j;
    data_ov007_02103448 = (void*)func_ov007_020c3df4((int)&data_ov007_02102dd0, (void*)0x3c00);
    data_ov007_02103444 = 0;
    data_ov007_0210344c = func_0204a4c8(func_ov007_020b78d0, 2, 0xc8, 0x23, 0x25);
    i = 0;
    j = i;
    do {
        data_ov007_02103450[i] = j;
        i++;
    } while (i < 2);
    func_0204a17c((int)data_ov007_0210344c, func_ov007_020b3f20(0x17));
}
