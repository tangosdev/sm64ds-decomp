typedef unsigned short u16;

extern int func_02054168(void);
extern int data_020a60b4;
extern u16 data_02086314[];
extern int data_020a60b0;

void _ZN2GX16BeginLoadTexPlttEv(void)
{
    int r = func_02054168();
    data_020a60b4 = r;
    data_020a60b0 = (int)data_02086314[r >> 4] << 12;
}
