extern void SetSubBg1Offset(int a, int b);
extern int func_ov004_020ad674(void);
extern unsigned int func_02054e88(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern void *_ZN3G2S12GetBG1ScrPtrEv(void);
extern unsigned char data_0209d454[];
extern int data_ov004_020bc00c[];

void func_ov004_020b2880(void)
{
    int f;
    *(volatile unsigned short *)0x400100a = (*(volatile unsigned short *)0x400100a & 0x43) | 0x10;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~0x40;
    *(volatile unsigned short *)0x400100a = *(volatile unsigned short *)0x400100a & ~3;
    SetSubBg1Offset(0, 0);
    data_0209d454[0] &= ~2;
    f = func_ov004_020ad674();
    LoadCompressedFileAt(data_ov004_020bc00c[f], (void *)func_02054e88());
    LoadCompressedFileAt(0x5b, _ZN3G2S12GetBG1ScrPtrEv());
}
