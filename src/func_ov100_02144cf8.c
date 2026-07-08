extern int func_ov100_021452e4(char* a, char* b);
extern unsigned char NumStars(void);
extern int func_ov100_02144f84(void);
extern int _ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(void* thisp, void* actor, unsigned int msg, void* vec, unsigned int e, unsigned int two);
extern int func_ov100_021453d8(void* c, void* p, void* b);
extern int _ZN6Player13TryTalkToDoorEh(void* thisp, unsigned char x);
extern int _ZN8SaveData22NumGlowingRabbitsFoundEv(void);
extern int _ZN6Player16TryTalkToKeyDoorEv(void* thisp);
extern int func_ov100_021451c4(void* a, void* p, void* b);

extern signed char data_ov100_02148204[];
typedef struct GlobCaa0 { int w0; unsigned int flags; unsigned int w2; } GlobCaa0;
extern GlobCaa0 SAVE_DATA;
extern char data_ov100_021488c4[];
extern char data_ov100_021488d4[];
extern char data_ov100_021488e4[];
extern char data_ov100_02148904[];
extern char data_ov100_021488f4[];

int func_ov100_02144cf8(char* a, char* b) {
    signed char* entry;

    if (func_ov100_021452e4(a, b)) {
        entry = data_ov100_02148204 + *(int*)(a + 8) * 16;
        if (entry[8] >= 0) {
            if (SAVE_DATA.flags & (0x8000 << entry[0xa])) goto L240;
            if (NumStars() < entry[8]) {
                if (func_ov100_02144f84() == 0) return 1;
                if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(b, a, *(short*)(entry + 0xc), a + 0xa4, 0, 2) == 0) goto L240;
                func_ov100_021453d8(a, data_ov100_021488c4, b);
                return 1;
            } else {
                if (_ZN6Player13TryTalkToDoorEh(b, 0) == 0) goto L240;
                func_ov100_021453d8(a, data_ov100_021488d4, b);
                return 1;
            }
        } else {
            int e9 = entry[9];
            int val;
            if (e9 < 0) goto L240;
            val = SAVE_DATA.flags;
            if (val & (0x100 << entry[0xa])) goto L240;
            if (val & (2 << e9)) goto L210;
            if (func_ov100_02144f84() == 0) return 1;
            {
                int msg;
                if (entry[9] == 6) {
                    msg = _ZN8SaveData22NumGlowingRabbitsFoundEv() ? 0x28 : 0x23;
                } else {
                    int idx2 = *(int*)(a + 8);
                    int sel = 1;
                    int inRange = (unsigned)idx2 >= 9 && (unsigned)idx2 <= 0xb;
                    if (!inRange) {
                        if (!(SAVE_DATA.flags & 0xbe)) sel = 0;
                    }
                    msg = sel ? *(short*)(entry + 0xc) : 0x17;
                }
                if (_ZN6Player11ShowMessageER9ActorBasejPK7Vector3jj(b, a, (short)msg, a + 0xa4, 0, 2) == 0) goto L240;
            }
            func_ov100_021453d8(a, data_ov100_021488c4, b);
            if (entry[9] == 5)
                SAVE_DATA.w2 |= 0x10000;
            return 1;
        L210:
            if (_ZN6Player16TryTalkToKeyDoorEv(b) == 0) goto L240;
            func_ov100_021453d8(a, data_ov100_021488e4, b);
            return 1;
        }
    L240:
        func_ov100_021451c4(a, (*(signed char*)(a + 0x144) != 0) ? data_ov100_02148904 : data_ov100_021488f4, b);
    }
    return 1;
}
