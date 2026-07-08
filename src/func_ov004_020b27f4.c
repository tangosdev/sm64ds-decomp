extern void SetBg1Offset(int a, int b);
extern int func_ov004_020ad674(void);
extern void* func_02054ea8(void);
extern unsigned int LoadCompressedFileAt(int fileID, void *target);
extern void *_ZN2G212GetBG1ScrPtrEv(void);
extern unsigned char TOP_SCREEN_RELATED[];
extern int data_ov004_020bbff8[];

void func_ov004_020b27f4(void)
{
    int f;
    *(volatile unsigned short *)0x400000a = *(volatile unsigned short *)0x400000a & ~3;
    *(volatile unsigned short *)0x400000a = (*(volatile unsigned short *)0x400000a & 0x43) | 0x1000;
    *(volatile unsigned short *)0x400000a = *(volatile unsigned short *)0x400000a & ~0x40;
    SetBg1Offset(0, 0);
    TOP_SCREEN_RELATED[0] &= ~2;
    f = func_ov004_020ad674();
    LoadCompressedFileAt(data_ov004_020bbff8[f], (void *)func_02054ea8());
    LoadCompressedFileAt(0x67, _ZN2G212GetBG1ScrPtrEv());
}
