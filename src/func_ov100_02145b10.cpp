//cpp
extern "C" {
int _ZN6Player21IsOpeningDoorWithStarEv(void* p);
int func_ov100_02145e10(void* a, void* b);
}
struct E6 { char b0; signed char b1; char b2,b3,b4,b5; };
extern E6 data_ov100_02148390[];
extern int SAVE_DATA[];

extern "C" int func_ov100_02145b10(char* c, char* a2)
{
    if (!_ZN6Player21IsOpeningDoorWithStarEv(a2)) {
        signed char sh = ((signed char*)((char*)data_ov100_02148390 + *(int*)(c + 8) * 6))[1];
        SAVE_DATA[1] |= 0x8000 << sh;
        func_ov100_02145e10(c, a2);
    }
    return 1;
}
