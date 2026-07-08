extern void func_02020190(void);
extern int func_ov075_02119dc4(void* c, void* p);
extern int func_02020168(void);
extern void func_020200e0(void);
extern void func_02030994(void);
extern void _ZN5Scene14StartSceneFadeEjjt(unsigned int a, unsigned int b, unsigned short c);
extern int func_0203d9b4(void);
extern int func_ov075_02116ce4(void* c);
extern void func_02020124(void);

extern unsigned char data_0209fc58;
extern int data_ov075_0211d870[];
extern unsigned char TOP_SCREEN_RELATED;
extern int data_02086b58[];
extern unsigned char DAT_02092778;

struct W2 { int a, b; };

void func_ov075_02117e84(void* c)
{
    if (data_0209fc58 != 0) {
        func_02020190();
        func_ov075_02119dc4(c, data_ov075_0211d870);
        return;
    }
    if (func_02020168()) {
        func_02020190();
        func_020200e0();
        func_02030994();
        TOP_SCREEN_RELATED = TOP_SCREEN_RELATED & ~0xe;
        *(unsigned char*)((char*)c + 0x287) = 0;
        *(struct W2*)((char*)c + 0x64) = *(struct W2*)data_02086b58;
        _ZN5Scene14StartSceneFadeEjjt(6, 2, 0);
        DAT_02092778 = 1;
        return;
    }
    if (func_0203d9b4() == 0)
        return;
    if (func_ov075_02116ce4(c) == 0)
        return;
    func_02020124();
}
