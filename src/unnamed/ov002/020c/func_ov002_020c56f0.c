extern int func_ov002_020d82f0(void* c);
extern int func_ov002_020d91e0(char* c, int a, int b);
extern void _ZN6Player11ChangeStateERNS_5StateE(char* c, void* st);
extern int data_ov002_0211010c;
extern int data_ov002_021103c4;

int func_ov002_020c56f0(unsigned char* c, unsigned char arg)
{
    if (func_ov002_020d82f0(c) == 0) return 0;
    c[0x6e3] = arg;
    if (c[0x6e3] == 0 && func_ov002_020d91e0((char*)c, 0x300, 1) != 0) {
        _ZN6Player11ChangeStateERNS_5StateE((char*)c, &data_ov002_0211010c);
        return 1;
    }
    _ZN6Player11ChangeStateERNS_5StateE((char*)c, &data_ov002_021103c4);
    return 1;
}
