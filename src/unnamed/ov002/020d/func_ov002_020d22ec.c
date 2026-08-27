extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* st);
extern void func_ov002_020cabe0(char* c);
extern int func_ov002_020d36d8(char* c, int x);
extern unsigned char data_020a0e40;
extern char data_0209f4a0[];
extern int data_ov002_0211013c;

int func_ov002_020d22ec(char* c, int arg)
{
    if (*(short*)(data_0209f4a0 + data_020a0e40 * 0x18) > arg) {
        _ZN6Player11ChangeStateERNS_5StateE(c, &data_ov002_0211013c);
        return 1;
    }
    if ((unsigned short)(*(unsigned short*)(c + 0x6ce) & 4)) {
        func_ov002_020cabe0(c);
        return 1;
    }
    return func_ov002_020d36d8(c, 0);
}
