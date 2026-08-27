//cpp
extern "C" {
int _ZN6Player21IsOpeningDoorWithStarEv(void* p);
int func_ov100_021451c4(void* a, void* b, void* c);
}
extern char data_ov100_02148204[];
extern int data_0209caa0[];
extern char data_ov100_021488f4[];

extern "C" int func_ov100_02144bf4(char* c, char* a2)
{
    if (!_ZN6Player21IsOpeningDoorWithStarEv(a2)) {
        signed char sh = ((signed char*)(data_ov100_02148204 + (*(int*)(c + 8) << 4)))[0xa];
        data_0209caa0[1] |= 0x8000 << sh;
        func_ov100_021451c4(c, data_ov100_021488f4, a2);
    }
    return 1;
}
