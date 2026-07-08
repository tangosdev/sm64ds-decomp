//cpp
struct G2 { static void* GetBG1ScrPtr(); };

extern "C" {
    void SetBg1Offset(int a, int b);
    int func_ov004_020ad674(void);
    void* func_02054ea8(void);
    unsigned int LoadCompressedFileAt(int fileID, void* target);
}

extern unsigned char TOP_SCREEN_RELATED;
extern int data_ov006_0213e614[];

extern "C" void func_ov006_0210aa60(void)
{
    int idx;

    *(volatile unsigned short*)0x400000a &= ~3;
    *(volatile unsigned short*)0x400000a = (*(volatile unsigned short*)0x400000a & 0x43) | 0x1000;
    *(volatile unsigned short*)0x400000a &= ~0x40;

    SetBg1Offset(0, 0);

    TOP_SCREEN_RELATED &= ~2;

    idx = func_ov004_020ad674();
    LoadCompressedFileAt(data_ov006_0213e614[idx], func_02054ea8());
    LoadCompressedFileAt(0x67, G2::GetBG1ScrPtr());

    *(volatile unsigned short*)0x400000a = (*(volatile unsigned short*)0x400000a & 0x43) | 0x1118;
}
