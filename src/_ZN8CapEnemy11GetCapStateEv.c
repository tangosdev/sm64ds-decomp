//cpp
// NONMATCHING: missing logic (ROM does more) (div=8). Logic verified correct vs ROM; not
// byte-matchable from C at mwccarm 1.2/sp2p3 (see notes/matching-style.md).
// Counts as decompiled, not matched.
struct CapEnemy {
    int GetCapState();
    char pad[0xb0];
    int field_b0;
    char pad2[0xf4 - 0xb0 - 4];
    int field_f4;
    char pad3[0x111 - 0xf4 - 4];
    unsigned char field_111;
    char pad4[0x113 - 0x111 - 1];
    unsigned char field_113;
    char pad5[0x17f - 0x113 - 1];
    unsigned char field_17f;
};

extern int CURRENT_GAMEMODE;
extern "C" int *_ZN5Actor13ClosestPlayerEv(void *self);

int CapEnemy::GetCapState() {
    if (field_111 == 0) {
        return 2;
    }

    unsigned char val = *(unsigned char *)&CURRENT_GAMEMODE;
    int check = (val == 1) ? 1 : 0;
    if (check == 0) {
        int *p = _ZN5Actor13ClosestPlayerEv(this);
        if (p != 0) {
            int s = field_113 & 7;
            int ps = p[2];
            if (s == ps) {
                return 0;
            }
        }
    }

    unsigned int b = field_17f;
    b = (b << 0x1e) >> 0x1f;
    if (b == 0) {
        return 0;
    }

    field_111 = 0;
    const int off = 0x113;
    int tmp = field_f4;
    unsigned char *p2 = (unsigned char *)this + off;
    field_b0 = tmp;
    *p2 = *p2 & 7;
    return 1;
}
