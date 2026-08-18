//cpp
/* dCapEnemy_c::GetCapState @ 0x02005fa0 (arm9, size 0xb4)   [mwccarm 1.2/base, 1.2/sp2, 1.2/sp2p3, 2004/b56]
 * Returns 2 if the cap flag is clear, 0 if the closest player already wears this
 * cap slot (or the release bit is clear), else consumes the cap: clears the flag,
 * latches field_f4 into field_b0, masks the slot bits, returns 1.
 *
 * Load-bearing spelling (do not "clean up"): the u64 no-op mask sits on the PLAIN
 * field_f4 READ, not on the RMW site. This is the deliberate inverse of the
 * matching-style.md RMW-launder rule: demoting the single-use load out of its
 * fold-onto-[r4,#0xf4] value-numbering class lets the RMW-pool chain lead the
 * tail interleave (A,B,A,B), which no spelling of the RMW side reaches. The
 * masked field_113 RMW address is the standard 6g launder.
 */
struct dCapEnemy_c {
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

extern int data_0209f2d8;
extern "C" int *_ZN8dActor_c13ClosestPlayerEv(void *self);

int dCapEnemy_c::GetCapState() {
    if (field_111 == 0) {
        return 2;
    }

    unsigned char val = *(unsigned char *)&data_0209f2d8;
    int check = (val == 1) ? 1 : 0;
    if (check == 0) {
        int *p = _ZN8dActor_c13ClosestPlayerEv(this);
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
    field_b0 = *(int *)((long long)(int)&field_f4);
    *(unsigned char *)((int)this + 0x113) &= 7;
    return 1;
}
