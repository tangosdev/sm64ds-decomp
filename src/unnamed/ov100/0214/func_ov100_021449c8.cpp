//cpp
extern "C" {
int _ZN6Player12Unk_020ca488Ev(void* p);
int func_ov100_021453d8(void* a, void* b, void* c);
}
extern char data_ov100_02148204[];
extern int data_0209caa0[];
extern char data_ov100_021488b4[];

extern "C" int func_ov100_021449c8(char* c, char* a2)
{
    if (!_ZN6Player12Unk_020ca488Ev(a2)) {
        signed char sh = ((signed char*)(data_ov100_02148204 + (*(int*)(c + 8) << 4)))[0xa];
        data_0209caa0[1] |= 0x100 << sh;
        func_ov100_021453d8(c, data_ov100_021488b4, a2);
    }
    return 1;
}
