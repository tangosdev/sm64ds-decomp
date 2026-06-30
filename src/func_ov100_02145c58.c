/* func_ov100_02145c58 at 0x02145c58
 *
 * Matched byte-for-byte with mwccarm 1.2/sp2p3 (ov100).
 */
extern int func_ov100_02145e74(char* a, char* b);
extern unsigned char NumStars(void);
extern int func_ov100_02144f84(void);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(char* p, char* actor, unsigned int msg, void* vec, unsigned int e, unsigned int two);
extern void _ZN6Player11OpenBigDoorEv(char* p);
extern int func_ov100_02145f68(char* c, void* p, char* b);
extern int _ZN6Player13TryTalkToDoorEh(char* p, unsigned char x);
extern void func_ov100_02145e10(char* a, char* b);

struct Vec3 { int x, y, z; };

extern signed char data_ov100_02148390[];
extern char data_0209caa0[];
extern char data_ov100_02148984[];
extern char data_ov100_02148994[];

int func_ov100_02145c58(char* a, char* b) {
    signed char* entry;
    int s5, lt, r6, msg;
    int vec[3];

    if (func_ov100_02145e74(a, b)) {
        entry = data_ov100_02148390 + *(int*)(a + 8) * 6;
        s5 = (entry[0] == 0x50);
        lt = ((int)NumStars() < entry[0]);
        r6 = (*(unsigned char*)(data_0209caa0 + 0x41) == 0);

        if (*(int*)(data_0209caa0 + 4) & (0x8000 << entry[1]))
            goto e10;
        if (s5) {
            if (*(int*)(a + 0x88) <= 0) goto e10;
            if (!lt && r6) goto e10;
        }
        {
            int t5c = *(int*)(a + 0x5c);
            int t64 = *(int*)(a + 0x64);
            int t60 = *(int*)(a + 0x60) + 0xb4000;
            vec[0] = t5c;
            vec[1] = t60;
            vec[2] = t64;
        }
        if (!(r6 && !s5 && !lt)) {
            if (func_ov100_02144f84() == 0) return 1;
            msg = r6 ? *(short*)(entry + 2) : *(short*)(entry + 4);
            if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(b, a, msg, vec, 0, 2) == 0) goto e10;
            if (s5) _ZN6Player11OpenBigDoorEv(b);
            func_ov100_02145f68(a, data_ov100_02148984, b);
            return 1;
        } else {
            if (_ZN6Player13TryTalkToDoorEh(b, 1) == 0) goto e10;
            func_ov100_02145f68(a, data_ov100_02148994, b);
            return 1;
        }
    e10:
        func_ov100_02145e10(a, b);
    }
    return 1;
}