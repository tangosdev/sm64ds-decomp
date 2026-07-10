extern void CleanCommonModelDataArr(void);
extern void func_ov006_020e7110(void* x);
extern void func_ov004_020adc5c(void* x);
extern void func_ov004_020b0840(void* a, int b);
extern void* data_ov006_02141a4c;
extern void* data_ov006_02141a48;
extern unsigned char data_0209f5f8;

void _ZN17MgBounceAndPounce21AfterCleanupResourcesEj(void* a, int b)
{
    if (b == 2) {
        CleanCommonModelDataArr();
        *(int*)0x40004c8 = 0x296a5800;
        *(int*)0x40004cc = 0x7fff;
        *(int*)0x40004c8 = 0x696a5800;
        *(int*)0x40004cc = 0x40007fff;
        func_ov006_020e7110(a);
        func_ov004_020adc5c(data_ov006_02141a4c);
        func_ov004_020adc5c(data_ov006_02141a48);
    }
    data_0209f5f8 = 0;
    func_ov004_020b0840(a, b);
}
