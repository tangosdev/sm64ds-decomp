/* func_ov018_02112234 at 0x02112234
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov018).
 */
extern void func_ov018_02111b3c(char* c);
extern void* func_ov018_021118fc(char* c);
extern void func_ov018_02111968(char* c, void* a, int v);
extern char* _ZN5Actor13ClosestPlayerEv(char* thisptr);
extern void func_ov018_02111a48(char* c, char* p);
extern void _ZN5Actor9UpdatePosEP12CylinderClsn(char* thisptr, char* clsn);
extern void func_ov018_02111bf0(char* c, char* p);
extern void func_0201267c(int a, char* p);

int func_ov018_02112234(char* c) {
    void* a;
    int v;
    char* p;
    if (*(unsigned char*)(c + 0x386) != 0)
        func_ov018_02111b3c(c);
    a = func_ov018_021118fc(c);
    v = 0;
    if (a != 0) {
        if (*(int*)((char*)a + 8) != 3)
            v = *(int*)((char*)a + 0x358);
        func_ov018_02111968(c, a, v);
    }
    p = _ZN5Actor13ClosestPlayerEv(c);
    func_ov018_02111a48(c, p);
    _ZN5Actor9UpdatePosEP12CylinderClsn(c, c + 0x174);
    func_ov018_02111bf0(c, c + 0x1a8);
    if (*(unsigned char*)(c + 0x386) == 0 && *(unsigned char*)(c + 0x387) == 0) {
        unsigned int x = ((unsigned int)*(int*)(c + 0x12c) << 4) >> 16;
        if (x == 0x10 || x == 0x25)
            func_0201267c(0xdf, c + 0x74);
    }
    return 1;
}