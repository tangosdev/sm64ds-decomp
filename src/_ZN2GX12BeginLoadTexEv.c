typedef unsigned short u16;

extern int func_0205417c(void);
extern u16 data_02086324[];
extern u16 data_02086326[];
extern u16 data_02086328[];
extern int data_020a60b8;
extern int data_020a60ac;
extern int data_020a60bc;
extern int data_020a60c0;

void _ZN2GX12BeginLoadTexEv(void)
{
    int r = func_0205417c();
    int i = r * 6;
    data_020a60b8 = r;
    data_020a60ac = (int)*(u16 *)((char *)data_02086324 + i) << 12;
    data_020a60bc = (int)*(u16 *)((char *)data_02086326 + i) << 12;
    data_020a60c0 = (int)*(u16 *)((char *)data_02086328 + i) << 12;
}
