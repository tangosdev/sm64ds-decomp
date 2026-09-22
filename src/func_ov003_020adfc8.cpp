//cpp
extern "C" {
extern int SublevelToLevel(int i);
extern int _ZN8SaveData13GetCoinRecordEj(unsigned int);
extern void func_ov003_020ae1a4(void* sl, int r);
extern void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int b, void* attr, int x, int y, int a, int c, void* m);
extern signed char data_02092110[];
extern void* _ZN3OAM7NUMBERSE[];
extern void* _ZN3OAM5TIMESE[];
extern void* _ZN3OAM4COINE[];
void func_ov003_020adfc8(char* sl) {
    int sb = 0xb8;
    int lvl = SublevelToLevel(data_02092110[0]);
    int coin = _ZN8SaveData13GetCoinRecordEj(lvl);
    func_ov003_020ae1a4(sl, coin);
    int i;
    for (i = 2; i >= 0; i--) {
        signed char d = *(signed char*)(sl + i + 0x121);
        if (d >= 0) {
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, _ZN3OAM7NUMBERSE[d], sb, 0x4c, 8, -1, 0);
            sb -= 9;
        }
    }
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, _ZN3OAM5TIMESE, sb, 0x54, -1, -1, 0);
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, _ZN3OAM4COINE, sb - 0x10, 0x4c, -1, -1, 0);
}
}
