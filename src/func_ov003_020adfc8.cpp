//cpp
extern "C" {
extern int SublevelToLevel(int i);
extern int _ZN8SaveData13GetCoinRecordEj(unsigned int);
extern void func_ov003_020ae1a4(void* sl, int r);
extern void _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(int b, void* attr, int x, int y, int a, int c, void* m);
extern signed char NEXT_LEVEL_ID[];
extern void* func_020aba70[];
extern void* func_020ab9c8[];
extern void* func_020abad8[];
void func_ov003_020adfc8(char* sl) {
    int sb = 0xb8;
    int lvl = SublevelToLevel(NEXT_LEVEL_ID[0]);
    int coin = _ZN8SaveData13GetCoinRecordEj(lvl);
    func_ov003_020ae1a4(sl, coin);
    int i;
    for (i = 2; i >= 0; i--) {
        signed char d = *(signed char*)(sl + i + 0x121);
        if (d >= 0) {
            _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, func_020aba70[d], sb, 0x4c, 8, -1, 0);
            sb -= 9;
        }
    }
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, func_020ab9c8, sb, 0x54, -1, -1, 0);
    _ZN3OAM6RenderEbP7OamAttriiiiP9Matrix2x2(0, func_020abad8, sb - 0x10, 0x4c, -1, -1, 0);
}
}
